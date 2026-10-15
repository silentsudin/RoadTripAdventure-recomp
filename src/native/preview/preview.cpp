#include "preview.h"
#include "preview_shaders.h"

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace rt::native::preview {

ModelStats computeStats(const assets::NativeModel& m) {
    ModelStats s;
    s.triangles = m.indices.size() / 3;
    constexpr float inf = std::numeric_limits<float>::infinity();
    s.bmin = {inf, inf, inf};
    s.bmax = {-inf, -inf, -inf};
    for (const auto& v : m.vertices) {
        const float p[3] = {v.px, v.py, v.pz};
        for (int i = 0; i < 3; ++i) {
            s.bmin[i] = std::min(s.bmin[i], p[i]);
            s.bmax[i] = std::max(s.bmax[i], p[i]);
        }
    }
    if (m.vertices.empty()) {
        s.bmin = s.bmax = {0, 0, 0};
    }
    return s;
}

// ------------------------------------------------------------------ PNG
namespace {
std::uint32_t crc32(const std::uint8_t* d, std::size_t n, std::uint32_t crc = 0) {
    static std::uint32_t table[256];
    static bool init = false;
    if (!init) {
        for (std::uint32_t i = 0; i < 256; ++i) {
            std::uint32_t c = i;
            for (int k = 0; k < 8; ++k) c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
            table[i] = c;
        }
        init = true;
    }
    crc = ~crc;
    for (std::size_t i = 0; i < n; ++i) crc = table[(crc ^ d[i]) & 0xFF] ^ (crc >> 8);
    return ~crc;
}
void be32(std::vector<std::uint8_t>& o, std::uint32_t v) {
    for (int s = 24; s >= 0; s -= 8) o.push_back(static_cast<std::uint8_t>(v >> s));
}
void chunk(std::vector<std::uint8_t>& o, const char* type, const std::vector<std::uint8_t>& data) {
    be32(o, static_cast<std::uint32_t>(data.size()));
    std::vector<std::uint8_t> td(type, type + 4);
    td.insert(td.end(), data.begin(), data.end());
    o.insert(o.end(), td.begin(), td.end());
    be32(o, crc32(td.data(), td.size()));
}
} // namespace

std::vector<std::uint8_t> encodePng(std::uint32_t w, std::uint32_t h,
                                    const std::vector<std::uint8_t>& rgba) {
    std::vector<std::uint8_t> raw;
    raw.reserve((static_cast<std::size_t>(w) * 4 + 1) * h);
    for (std::uint32_t y = 0; y < h; ++y) {
        raw.push_back(0);
        raw.insert(raw.end(), rgba.begin() + static_cast<std::size_t>(y) * w * 4,
                   rgba.begin() + static_cast<std::size_t>(y + 1) * w * 4);
    }
    std::vector<std::uint8_t> z{0x78, 0x01};
    std::size_t pos = 0;
    do {
        const std::size_t n = std::min<std::size_t>(65535, raw.size() - pos);
        z.push_back(pos + n >= raw.size() ? 1 : 0);
        z.push_back(n & 0xFF); z.push_back(n >> 8);
        z.push_back(~n & 0xFF); z.push_back((~n >> 8) & 0xFF);
        z.insert(z.end(), raw.begin() + pos, raw.begin() + pos + n);
        pos += n;
    } while (pos < raw.size());
    std::uint32_t a = 1, b = 0;
    for (std::uint8_t c : raw) { a = (a + c) % 65521; b = (b + a) % 65521; }
    be32(z, (b << 16) | a);

    std::vector<std::uint8_t> out{0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    std::vector<std::uint8_t> ihdr;
    be32(ihdr, w); be32(ihdr, h);
    ihdr.insert(ihdr.end(), {8, 6, 0, 0, 0});
    chunk(out, "IHDR", ihdr);
    chunk(out, "IDAT", z);
    chunk(out, "IEND", {});
    return out;
}

// ------------------------------------------------------------------ matrices (column-major)
namespace {
using Mat = std::array<float, 16>;
Mat mul(const Mat& a, const Mat& b) {
    Mat r{};
    for (int c = 0; c < 4; ++c)
        for (int rI = 0; rI < 4; ++rI) {
            float s = 0;
            for (int k = 0; k < 4; ++k) s += a[k * 4 + rI] * b[c * 4 + k];
            r[c * 4 + rI] = s;
        }
    return r;
}
Mat identity() { Mat m{}; m[0] = m[5] = m[10] = m[15] = 1; return m; }

#define VK_TRY(expr, msg)                                  \
    do {                                                   \
        VkResult r_ = (expr);                              \
        if (r_ != VK_SUCCESS) {                            \
            error = std::string(msg) + " (VkResult " + std::to_string(r_) + ")"; \
            return false;                                  \
        }                                                  \
    } while (0)

std::uint32_t findMem(const VkPhysicalDeviceMemoryProperties& p, std::uint32_t bits, VkMemoryPropertyFlags f) {
    for (std::uint32_t i = 0; i < p.memoryTypeCount; ++i)
        if ((bits & (1u << i)) && (p.memoryTypes[i].propertyFlags & f) == f) return i;
    return 0xFFFFFFFFu;
}
} // namespace

struct Renderer {
    VkInstance inst = VK_NULL_HANDLE;
    VkPhysicalDevice pd = VK_NULL_HANDLE;
    VkDevice dev = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;
    std::uint32_t qfi = 0;
    std::uint32_t W = 0, H = 0;
    VkPhysicalDeviceMemoryProperties mp{};
    std::string deviceName;
    VkImage colImg = VK_NULL_HANDLE, depImg = VK_NULL_HANDLE;
    VkDeviceMemory colMem = VK_NULL_HANDLE, depMem = VK_NULL_HANDLE;
    VkImageView colView = VK_NULL_HANDLE, depView = VK_NULL_HANDLE;
    VkRenderPass rp = VK_NULL_HANDLE;
    VkFramebuffer fb = VK_NULL_HANDLE;
    VkShaderModule vs = VK_NULL_HANDLE, fs = VK_NULL_HANDLE;
    VkPipelineLayout pl = VK_NULL_HANDLE;
    VkPipeline pipe = VK_NULL_HANDLE;
    VkShaderModule uiVs = VK_NULL_HANDLE, uiFs = VK_NULL_HANDLE;
    VkPipelineLayout uiPl = VK_NULL_HANDLE;
    VkPipeline uiPipe = VK_NULL_HANDLE;
    VkBuffer rb = VK_NULL_HANDLE; VkDeviceMemory rbm = VK_NULL_HANDLE;
    VkCommandPool pool = VK_NULL_HANDLE;
    VkCommandBuffer cmd = VK_NULL_HANDLE;
    // Geometry buffers: grown on demand, re-uploaded every frame.
    VkBuffer vb = VK_NULL_HANDLE, ib = VK_NULL_HANDLE;
    VkDeviceMemory vbm = VK_NULL_HANDLE, ibm = VK_NULL_HANDLE;
    VkDeviceSize vbCap = 0, ibCap = 0;
    VkBuffer uiVb = VK_NULL_HANDLE;
    VkDeviceMemory uiVbm = VK_NULL_HANDLE;
    VkDeviceSize uiVbCap = 0;
};

static const VkMemoryPropertyFlags kHostMem = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

static bool makeBuffer(Renderer& R, std::string& error, VkDeviceSize size, VkBufferUsageFlags usage,
                       VkBuffer& buf, VkDeviceMemory& mem) {
    VkBufferCreateInfo bi{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bi.size = size; bi.usage = usage; bi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VK_TRY(vkCreateBuffer(R.dev, &bi, nullptr, &buf), "vkCreateBuffer");
    VkMemoryRequirements mr; vkGetBufferMemoryRequirements(R.dev, buf, &mr);
    VkMemoryAllocateInfo ai{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    ai.allocationSize = mr.size; ai.memoryTypeIndex = findMem(R.mp, mr.memoryTypeBits, kHostMem);
    if (ai.memoryTypeIndex == 0xFFFFFFFFu) { error = "no memory type"; return false; }
    VK_TRY(vkAllocateMemory(R.dev, &ai, nullptr, &mem), "vkAllocateMemory");
    VK_TRY(vkBindBufferMemory(R.dev, buf, mem, 0), "vkBindBufferMemory");
    return true;
}

static bool makeImage(Renderer& R, std::string& error, VkFormat fmt, VkImageUsageFlags usage,
                      VkImageAspectFlags aspect, VkImage& img, VkDeviceMemory& mem, VkImageView& view) {
    VkImageCreateInfo ii{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    ii.imageType = VK_IMAGE_TYPE_2D; ii.format = fmt; ii.extent = {R.W, R.H, 1};
    ii.mipLevels = 1; ii.arrayLayers = 1; ii.samples = VK_SAMPLE_COUNT_1_BIT;
    ii.tiling = VK_IMAGE_TILING_OPTIMAL; ii.usage = usage;
    ii.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VK_TRY(vkCreateImage(R.dev, &ii, nullptr, &img), "vkCreateImage");
    VkMemoryRequirements mr; vkGetImageMemoryRequirements(R.dev, img, &mr);
    VkMemoryAllocateInfo ai{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    ai.allocationSize = mr.size;
    ai.memoryTypeIndex = findMem(R.mp, mr.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (ai.memoryTypeIndex == 0xFFFFFFFFu) { error = "no image memory type"; return false; }
    VK_TRY(vkAllocateMemory(R.dev, &ai, nullptr, &mem), "vkAllocateMemory(image)");
    VK_TRY(vkBindImageMemory(R.dev, img, mem, 0), "vkBindImageMemory");
    VkImageViewCreateInfo vi{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    vi.image = img; vi.viewType = VK_IMAGE_VIEW_TYPE_2D; vi.format = fmt;
    vi.subresourceRange = {aspect, 0, 1, 0, 1};
    VK_TRY(vkCreateImageView(R.dev, &vi, nullptr, &view), "vkCreateImageView");
    return true;
}

void previewDestroy(Renderer* R) {
    if (!R) return;
    if (R->dev) {
        vkDeviceWaitIdle(R->dev);
        if (R->pool) vkDestroyCommandPool(R->dev, R->pool, nullptr);  // frees cmd
        if (R->vb) vkDestroyBuffer(R->dev, R->vb, nullptr);
        if (R->vbm) vkFreeMemory(R->dev, R->vbm, nullptr);
        if (R->ib) vkDestroyBuffer(R->dev, R->ib, nullptr);
        if (R->ibm) vkFreeMemory(R->dev, R->ibm, nullptr);
        if (R->uiVb) vkDestroyBuffer(R->dev, R->uiVb, nullptr);
        if (R->uiVbm) vkFreeMemory(R->dev, R->uiVbm, nullptr);
        if (R->rb) vkDestroyBuffer(R->dev, R->rb, nullptr);
        if (R->rbm) vkFreeMemory(R->dev, R->rbm, nullptr);
        if (R->pipe) vkDestroyPipeline(R->dev, R->pipe, nullptr);
        if (R->pl) vkDestroyPipelineLayout(R->dev, R->pl, nullptr);
        if (R->vs) vkDestroyShaderModule(R->dev, R->vs, nullptr);
        if (R->fs) vkDestroyShaderModule(R->dev, R->fs, nullptr);
        if (R->uiPipe) vkDestroyPipeline(R->dev, R->uiPipe, nullptr);
        if (R->uiPl) vkDestroyPipelineLayout(R->dev, R->uiPl, nullptr);
        if (R->uiVs) vkDestroyShaderModule(R->dev, R->uiVs, nullptr);
        if (R->uiFs) vkDestroyShaderModule(R->dev, R->uiFs, nullptr);
        if (R->fb) vkDestroyFramebuffer(R->dev, R->fb, nullptr);
        if (R->rp) vkDestroyRenderPass(R->dev, R->rp, nullptr);
        if (R->colView) vkDestroyImageView(R->dev, R->colView, nullptr);
        if (R->depView) vkDestroyImageView(R->dev, R->depView, nullptr);
        if (R->colImg) vkDestroyImage(R->dev, R->colImg, nullptr);
        if (R->depImg) vkDestroyImage(R->dev, R->depImg, nullptr);
        if (R->colMem) vkFreeMemory(R->dev, R->colMem, nullptr);
        if (R->depMem) vkFreeMemory(R->dev, R->depMem, nullptr);
        vkDestroyDevice(R->dev, nullptr);
    }
    if (R->inst) vkDestroyInstance(R->inst, nullptr);
    delete R;
}

static bool initImpl(Renderer& R, std::uint32_t W, std::uint32_t H, std::string& error) {
    R.W = W; R.H = H;
    // only the instance/device are destroyed on the success path.
    
    VkApplicationInfo app{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app.apiVersion = VK_API_VERSION_1_1;
    app.pApplicationName = "rt-asset-preview";
    VkInstanceCreateInfo ici{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    ici.pApplicationInfo = &app;
    std::vector<const char*> exts;
#ifdef __APPLE__
    exts.push_back("VK_KHR_portability_enumeration");
    exts.push_back("VK_KHR_get_physical_device_properties2");
    ici.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
    ici.enabledExtensionCount = static_cast<std::uint32_t>(exts.size());
    ici.ppEnabledExtensionNames = exts.data();
    VK_TRY(vkCreateInstance(&ici, nullptr, &R.inst), "vkCreateInstance (is a Vulkan ICD/MoltenVK installed? try VK_ICD_FILENAMES)");

    std::uint32_t n = 0;
    vkEnumeratePhysicalDevices(R.inst, &n, nullptr);
    if (!n) { error = "no Vulkan physical device"; return false; }
    std::vector<VkPhysicalDevice> pds(n);
    vkEnumeratePhysicalDevices(R.inst, &n, pds.data());
    VkPhysicalDevice pd = R.pd = pds[0];
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(pd, &props);
    R.deviceName = props.deviceName;
    vkGetPhysicalDeviceMemoryProperties(pd, &R.mp);

    std::uint32_t qn = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(pd, &qn, nullptr);
    std::vector<VkQueueFamilyProperties> qf(qn);
    vkGetPhysicalDeviceQueueFamilyProperties(pd, &qn, qf.data());
    std::uint32_t qfi = 0xFFFFFFFFu;
    for (std::uint32_t i = 0; i < qn; ++i)
        if (qf[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) { qfi = i; break; }
    if (qfi == 0xFFFFFFFFu) { error = "no graphics queue"; return false; }

    float prio = 1.0f;
    VkDeviceQueueCreateInfo qci{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    qci.queueFamilyIndex = qfi; qci.queueCount = 1; qci.pQueuePriorities = &prio;
    std::vector<const char*> dexts;
#ifdef __APPLE__
    dexts.push_back("VK_KHR_portability_subset");
#endif
    VkDeviceCreateInfo dci{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    dci.queueCreateInfoCount = 1; dci.pQueueCreateInfos = &qci;
    dci.enabledExtensionCount = static_cast<std::uint32_t>(dexts.size());
    dci.ppEnabledExtensionNames = dexts.data();
    VK_TRY(vkCreateDevice(pd, &dci, nullptr, &R.dev), "vkCreateDevice");
    R.qfi = qfi;
    vkGetDeviceQueue(R.dev, qfi, 0, &R.queue);
    VkDevice dev = R.dev;

    if (!makeImage(R, error, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                   VK_IMAGE_ASPECT_COLOR_BIT, R.colImg, R.colMem, R.colView)) return false;
    if (!makeImage(R, error, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                   VK_IMAGE_ASPECT_DEPTH_BIT, R.depImg, R.depMem, R.depView)) return false;

    // Render pass.
    VkAttachmentDescription att[2]{};
    att[0].format = VK_FORMAT_R8G8B8A8_UNORM; att[0].samples = VK_SAMPLE_COUNT_1_BIT;
    att[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; att[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    att[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; att[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    att[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; att[0].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    att[1] = att[0];
    att[1].format = VK_FORMAT_D32_SFLOAT; att[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    att[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    VkAttachmentReference cref{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkAttachmentReference dref{1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
    VkSubpassDescription sub{};
    sub.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    sub.colorAttachmentCount = 1; sub.pColorAttachments = &cref; sub.pDepthStencilAttachment = &dref;
    VkRenderPassCreateInfo rpi{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    rpi.attachmentCount = 2; rpi.pAttachments = att; rpi.subpassCount = 1; rpi.pSubpasses = &sub;
    VkRenderPass& rp = R.rp;
    VK_TRY(vkCreateRenderPass(dev, &rpi, nullptr, &rp), "vkCreateRenderPass");
    VkImageView views[2] = {R.colView, R.depView};
    VkFramebufferCreateInfo fbi{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    fbi.renderPass = rp; fbi.attachmentCount = 2; fbi.pAttachments = views;
    fbi.width = W; fbi.height = H; fbi.layers = 1;
    VkFramebuffer& fb = R.fb;
    VK_TRY(vkCreateFramebuffer(dev, &fbi, nullptr, &fb), "vkCreateFramebuffer");
    // Pipeline.
    auto makeShader = [&](const unsigned int* code, std::size_t bytes, VkShaderModule& m) -> bool {
        VkShaderModuleCreateInfo si{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
        si.codeSize = bytes; si.pCode = code;
        VK_TRY(vkCreateShaderModule(dev, &si, nullptr, &m), "vkCreateShaderModule");
        return true;
    };
    VkShaderModule& vs = R.vs; VkShaderModule& fs = R.fs;
    if (!makeShader(kPreviewVertSpv, sizeof(kPreviewVertSpv), vs)) return false;
    if (!makeShader(kPreviewFragSpv, sizeof(kPreviewFragSpv), fs)) return false;
    VkPipelineShaderStageCreateInfo stages[2]{};
    stages[0] = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_VERTEX_BIT, vs, "main", nullptr};
    stages[1] = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_FRAGMENT_BIT, fs, "main", nullptr};
    VkVertexInputBindingDescription bind{0, 12, VK_VERTEX_INPUT_RATE_VERTEX};
    VkVertexInputAttributeDescription attrs[2] = {
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, 12},
    };
    bind.stride = 24;
    VkPipelineVertexInputStateCreateInfo vin{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vin.vertexBindingDescriptionCount = 1; vin.pVertexBindingDescriptions = &bind;
    vin.vertexAttributeDescriptionCount = 2; vin.pVertexAttributeDescriptions = attrs;
    VkPipelineInputAssemblyStateCreateInfo ia{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkViewport vp{0, 0, static_cast<float>(W), static_cast<float>(H), 0, 1};
    VkRect2D sc{{0, 0}, {W, H}};
    VkPipelineViewportStateCreateInfo vps{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    vps.viewportCount = 1; vps.pViewports = &vp; vps.scissorCount = 1; vps.pScissors = &sc;
    VkPipelineRasterizationStateCreateInfo rs{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rs.polygonMode = VK_POLYGON_MODE_FILL; rs.cullMode = VK_CULL_MODE_NONE; rs.lineWidth = 1.0f;
    VkPipelineMultisampleStateCreateInfo ms{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    VkPipelineDepthStencilStateCreateInfo ds{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    ds.depthTestEnable = VK_TRUE; ds.depthWriteEnable = VK_TRUE; ds.depthCompareOp = VK_COMPARE_OP_LESS;
    VkPipelineColorBlendAttachmentState cba{};
    cba.colorWriteMask = 0xF;
    VkPipelineColorBlendStateCreateInfo cb{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    cb.attachmentCount = 1; cb.pAttachments = &cba;
    VkPushConstantRange pcr{VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, 144};
    VkPipelineLayoutCreateInfo pli{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pli.pushConstantRangeCount = 1; pli.pPushConstantRanges = &pcr;
    VkPipelineLayout& pl = R.pl;
    VK_TRY(vkCreatePipelineLayout(dev, &pli, nullptr, &pl), "vkCreatePipelineLayout");
    VkGraphicsPipelineCreateInfo gpi{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    gpi.stageCount = 2; gpi.pStages = stages; gpi.pVertexInputState = &vin; gpi.pInputAssemblyState = &ia;
    gpi.pViewportState = &vps; gpi.pRasterizationState = &rs; gpi.pMultisampleState = &ms;
    gpi.pDepthStencilState = &ds; gpi.pColorBlendState = &cb; gpi.layout = pl; gpi.renderPass = rp;
    VkPipeline& pipe = R.pipe;
    VK_TRY(vkCreateGraphicsPipelines(dev, VK_NULL_HANDLE, 1, &gpi, nullptr, &pipe), "vkCreateGraphicsPipelines");

    if (!makeShader(kPreviewUiVertSpv, sizeof(kPreviewUiVertSpv), R.uiVs)) return false;
    if (!makeShader(kPreviewUiFragSpv, sizeof(kPreviewUiFragSpv), R.uiFs)) return false;
    VkPipelineShaderStageCreateInfo uiStages[2]{};
    uiStages[0] = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_VERTEX_BIT, R.uiVs, "main", nullptr};
    uiStages[1] = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_FRAGMENT_BIT, R.uiFs, "main", nullptr};
    VkVertexInputBindingDescription uiBind{0, 24, VK_VERTEX_INPUT_RATE_VERTEX};
    VkVertexInputAttributeDescription uiAttrs[2] = {
        {0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
        {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 8},
    };
    VkPipelineVertexInputStateCreateInfo uiVin{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    uiVin.vertexBindingDescriptionCount = 1; uiVin.pVertexBindingDescriptions = &uiBind;
    uiVin.vertexAttributeDescriptionCount = 2; uiVin.pVertexAttributeDescriptions = uiAttrs;
    VkPipelineDepthStencilStateCreateInfo uiDs{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    VkPipelineLayoutCreateInfo uiPli{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    VK_TRY(vkCreatePipelineLayout(dev, &uiPli, nullptr, &R.uiPl), "vkCreatePipelineLayout(ui)");
    VkGraphicsPipelineCreateInfo uiGpi = gpi;
    uiGpi.stageCount = 2; uiGpi.pStages = uiStages; uiGpi.pVertexInputState = &uiVin;
    uiGpi.pDepthStencilState = &uiDs; uiGpi.layout = R.uiPl;
    VK_TRY(vkCreateGraphicsPipelines(dev, VK_NULL_HANDLE, 1, &uiGpi, nullptr, &R.uiPipe), "vkCreateGraphicsPipelines(ui)");

    const VkDeviceSize rbSize = static_cast<VkDeviceSize>(W) * H * 4;
    if (!makeBuffer(R, error, rbSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, R.rb, R.rbm)) return false;
    VkCommandPoolCreateInfo cpi{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    cpi.queueFamilyIndex = qfi;
    cpi.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_TRY(vkCreateCommandPool(dev, &cpi, nullptr, &R.pool), "vkCreateCommandPool");
    VkCommandBufferAllocateInfo cai{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    cai.commandPool = R.pool; cai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; cai.commandBufferCount = 1;
    VK_TRY(vkAllocateCommandBuffers(dev, &cai, &R.cmd), "vkAllocateCommandBuffers");
    return true;
}

Renderer* previewInit(std::uint32_t W, std::uint32_t H, std::string& error, std::string* deviceName) {
    if (!W || !H) { error = "zero-sized target"; return nullptr; }
    auto* R = new Renderer();
    if (!initImpl(*R, W, H, error)) { previewDestroy(R); return nullptr; }
    if (deviceName) *deviceName = R->deviceName;
    return R;
}

static bool growBuffer(Renderer& R, std::string& error, VkDeviceSize need, VkBufferUsageFlags usage,
                       VkBuffer& buf, VkDeviceMemory& mem, VkDeviceSize& cap) {
    if (need <= cap) return true;
    vkDeviceWaitIdle(R.dev);
    if (buf) vkDestroyBuffer(R.dev, buf, nullptr);
    if (mem) vkFreeMemory(R.dev, mem, nullptr);
    buf = VK_NULL_HANDLE; mem = VK_NULL_HANDLE; cap = 0;
    const VkDeviceSize newCap = std::max<VkDeviceSize>(need, 1 << 16);
    if (!makeBuffer(R, error, newCap, usage, buf, mem)) return false;
    cap = newCap;
    return true;
}

struct SceneVertex { float x, y, z, r, g, b; };

static bool renderGeometry(Renderer* Rp, const std::vector<SceneVertex>& vertices,
                           const std::vector<std::uint32_t>& indices, const float* viewIn, const float* projIn,
                           const float clear[4], const float fog[4], std::vector<std::uint8_t>& rgbaOut,
                           std::string& error) {
    if (!Rp) { error = "null renderer"; return false; }
    Renderer& R = *Rp;
    const std::uint32_t W = R.W, H = R.H;
    VkDevice dev = R.dev;
    if (indices.empty() || vertices.empty()) { error = "empty geometry"; return false; }
    const VkDeviceSize vbSize = vertices.size() * sizeof(SceneVertex);
    const VkDeviceSize ibSize = indices.size() * sizeof(std::uint32_t);
    if (!growBuffer(R, error, vbSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, R.vb, R.vbm, R.vbCap)) return false;
    if (!growBuffer(R, error, ibSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, R.ib, R.ibm, R.ibCap)) return false;
    void* p;
    VK_TRY(vkMapMemory(dev, R.vbm, 0, vbSize, 0, &p), "vkMapMemory"); std::memcpy(p, vertices.data(), vbSize); vkUnmapMemory(dev, R.vbm);
    VK_TRY(vkMapMemory(dev, R.ibm, 0, ibSize, 0, &p), "vkMapMemory"); std::memcpy(p, indices.data(), ibSize); vkUnmapMemory(dev, R.ibm);

    Mat view, proj{};
    if (viewIn && projIn) {
        std::memcpy(view.data(), viewIn, 64);
        std::memcpy(proj.data(), projIn, 64);
    } else {
        // Camera: fit the bounding sphere, orthographic, 3/4 view.
        ModelStats st;
        constexpr float inf = std::numeric_limits<float>::infinity();
        st.bmin = {inf, inf, inf}; st.bmax = {-inf, -inf, -inf};
        for (const auto& v : vertices) for (int i = 0; i < 3; ++i) {
            const float p0[3] = {v.x, v.y, v.z};
            st.bmin[i] = std::min(st.bmin[i], p0[i]); st.bmax[i] = std::max(st.bmax[i], p0[i]);
        }
        float c[3], r = 0;
        for (int i = 0; i < 3; ++i) c[i] = 0.5f * (st.bmin[i] + st.bmax[i]);
        for (int i = 0; i < 3; ++i) { const float e = 0.5f * (st.bmax[i] - st.bmin[i]); r += e * e; }
        r = std::max(std::sqrt(r), 1e-3f);
        const float yaw = 0.6f, pitch = 0.4f;
        const float cy = std::cos(yaw), sy = std::sin(yaw), cp = std::cos(pitch), sp = std::sin(pitch);
        Mat T = identity(); T[12] = -c[0]; T[13] = -c[1]; T[14] = -c[2];
        Mat Ry = identity(); Ry[0] = cy; Ry[2] = -sy; Ry[8] = sy; Ry[10] = cy;
        Mat Rx = identity(); Rx[5] = cp; Rx[6] = sp; Rx[9] = -sp; Rx[10] = cp;
        view = mul(Rx, mul(Ry, T));
        const float aspect = static_cast<float>(W) / static_cast<float>(H);
        const float hx = r * 1.05f * std::max(aspect, 1.0f), hy = r * 1.05f * std::max(1.0f / aspect, 1.0f);
        proj[0] = 1.0f / hx; proj[5] = -1.0f / hy;  // flip Y: Vulkan clip space is y-down
        proj[10] = 1.0f / (2.0f * r); proj[14] = 0.5f; proj[15] = 1.0f;  // view z in [-r, r] -> [0,1]
    }
    Mat mvp = mul(proj, view);
    float pc[36];
    std::memcpy(pc, mvp.data(), 64); std::memcpy(pc + 16, view.data(), 64);
    std::memcpy(pc + 32, fog, 16);

    const VkDeviceSize rbSize = static_cast<VkDeviceSize>(W) * H * 4;
    VkCommandBuffer cmd = R.cmd;
    vkResetCommandBuffer(cmd, 0);
    VkCommandBufferBeginInfo bbi{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(cmd, &bbi);
    VkClearValue clears[2]{};
    clears[0].color = {{clear[0], clear[1], clear[2], clear[3]}};
    clears[1].depthStencil = {1.0f, 0};
    VkRenderPassBeginInfo rbi{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rbi.renderPass = R.rp; rbi.framebuffer = R.fb; rbi.renderArea = {{0, 0}, {W, H}};
    rbi.clearValueCount = 2; rbi.pClearValues = clears;
    vkCmdBeginRenderPass(cmd, &rbi, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, R.pipe);
    VkDeviceSize off = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &R.vb, &off);
    vkCmdBindIndexBuffer(cmd, R.ib, 0, VK_INDEX_TYPE_UINT32);
    vkCmdPushConstants(cmd, R.pl, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, 144, pc);
    vkCmdDrawIndexed(cmd, static_cast<std::uint32_t>(indices.size()), 1, 0, 0, 0);
    vkCmdEndRenderPass(cmd);
    VkBufferImageCopy copy{};
    copy.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    copy.imageExtent = {W, H, 1};
    vkCmdCopyImageToBuffer(cmd, R.colImg, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, R.rb, 1, &copy);
    vkEndCommandBuffer(cmd);
    VkSubmitInfo si{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    si.commandBufferCount = 1; si.pCommandBuffers = &cmd;
    VK_TRY(vkQueueSubmit(R.queue, 1, &si, VK_NULL_HANDLE), "vkQueueSubmit");
    VK_TRY(vkQueueWaitIdle(R.queue), "vkQueueWaitIdle");

    rgbaOut.resize(rbSize);
    VK_TRY(vkMapMemory(dev, R.rbm, 0, rbSize, 0, &p), "vkMapMemory(readback)");
    std::memcpy(rgbaOut.data(), p, rbSize);
    vkUnmapMemory(dev, R.rbm);
    return true;
}

bool previewRender(Renderer* r, const assets::NativeModel& model, const float* view, const float* proj,
                   std::vector<std::uint8_t>& rgbaOut, std::string& error) {
    std::vector<SceneVertex> vertices;
    vertices.reserve(model.vertices.size());
    for (const auto& v : model.vertices) {
        const bool ground = std::abs(v.py) < 1e-4f;
        vertices.push_back({v.px, v.py, v.pz, ground ? .25f : .9f, ground ? .45f : .8f, ground ? .3f : .6f});
    }
    const float clear[4] = {.08f, .09f, .12f, 1.f};
    const float fog[4] = {0.f, 0.f, 0.f, 1e30f};
    return renderGeometry(r, vertices, model.indices, view, proj, clear, fog, rgbaOut, error);
}

static void addQuad(std::vector<SceneVertex>& vertices, std::vector<std::uint32_t>& indices,
                    const gameplay::Vec2& a, const gameplay::Vec2& b, float halfWidth, const float color[3]) {
    const gameplay::Vec2 d = b - a;
    const float len = std::max(std::sqrt(d.x * d.x + d.z * d.z), 1e-4f);
    const float nx = -d.z / len * halfWidth, nz = d.x / len * halfWidth;
    const std::uint32_t base = static_cast<std::uint32_t>(vertices.size());
    for (const auto& p : {gameplay::Vec2{a.x + nx, a.z + nz}, {b.x + nx, b.z + nz},
                          {b.x - nx, b.z - nz}, {a.x - nx, a.z - nz}})
        vertices.push_back({p.x, 0.01f, p.z, color[0], color[1], color[2]});
    for (std::uint32_t i : {0u, 2u, 1u, 0u, 3u, 2u}) indices.push_back(base + i);
}

static void argbToRgb(std::uint32_t argb, float out[3]) {
    out[0] = static_cast<float>((argb >> 16) & 0xff) / 255.f;
    out[1] = static_cast<float>((argb >> 8) & 0xff) / 255.f;
    out[2] = static_cast<float>(argb & 0xff) / 255.f;
}

bool previewRenderScene(Renderer* r, const assets::NativeModel& car, const gameplay::FrameSnapshot& s,
                        const gameplay::World& world, std::vector<std::uint8_t>& rgbaOut, std::string& error,
                        const TerrainMesh* terrain) {
    std::vector<SceneVertex> vertices;
    std::vector<std::uint32_t> indices;
    const float grass[3] = {.16f, .42f, .18f}, asphalt[3] = {.10f, .11f, .12f};
    // Field ground in render units: World is 1600 units, snapshots use a 0.1 scale.
    const std::uint32_t groundBase = 0;
    const bool useTerrain = terrain != nullptr && !terrain->positions.empty() &&
                            terrain->indices.size() >= 3 && terrain->indices.size() % 3 == 0;
    if (!useTerrain) {
        for (const auto& p : {gameplay::Vec2{0, 0}, {160, 0}, {160, 160}, {0, 160}})
            vertices.push_back({p.x, 0.f, p.z, grass[0], grass[1], grass[2]});
        for (std::uint32_t i : {0u, 2u, 1u, 0u, 3u, 2u}) indices.push_back(groundBase + i);
    } else {
        // Underlay so tile-edge cracks never show sky; slight drop keeps it
        // below the terrain's lowest heights under the default depth test.
        for (const auto& p : {gameplay::Vec2{-4, -4}, {164, -4}, {164, 164}, {-4, 164}})
            vertices.push_back({p.x, -.4f, p.z, grass[0] * .5f, grass[1] * .5f, grass[2] * .5f});
        for (std::uint32_t i : {0u, 2u, 1u, 0u, 3u, 2u}) indices.push_back(groundBase + i);
        const std::uint32_t terrainBase = static_cast<std::uint32_t>(vertices.size());
        for (std::size_t i = 0; i < terrain->positions.size(); ++i) {
            const auto& p = terrain->positions[i];
            // Height banding: low ground grass, mid dirt, high rock; material
            // id adds deterministic per-patch tint until textures bind.
            const float h = p[1];
            float col[3];
            if (h < 1.0f) {
                col[0] = grass[0]; col[1] = grass[1]; col[2] = grass[2];
            } else if (h < 3.0f) {
                col[0] = .38f; col[1] = .32f; col[2] = .20f;
            } else {
                col[0] = .45f; col[1] = .45f; col[2] = .47f;
            }
            const std::uint32_t material = i / 3 < terrain->triangleMaterials.size()
                                             ? terrain->triangleMaterials[i / 3] : 0;
            const float tint = 1.f + static_cast<float>(material % 16u) * 0.012f;
            vertices.push_back({p[0], p[1], p[2], col[0] * tint, col[1] * tint, col[2] * tint});
        }
        indices.reserve(indices.size() + terrain->indices.size());
        for (const std::uint32_t index : terrain->indices) {
            indices.push_back(terrainBase + index);
        }
    }
    const auto& path = world.roadWaypoints;
    const std::size_t count = path.size();
    const std::size_t segments = count > 1 ? (world.roadClosed ? count : count - 1) : 0;
    for (std::size_t i = 0; i < segments; ++i) {
        const auto& a = path[i]; const auto& b = path[(i + 1) % count];
        addQuad(vertices, indices, {a.x * .1f, a.z * .1f}, {b.x * .1f, b.z * .1f}, world.roadHalfWidth * .1f, asphalt);
    }
    const ModelStats st = computeStats(car);
    const float cx = .5f * (st.bmin[0] + st.bmax[0]), cz = .5f * (st.bmin[2] + st.bmax[2]);
    const float scale = 4.4f / std::max(st.bmax[2] - st.bmin[2], 1e-3f);
    const float sh = std::sin(s.heading), ch = std::cos(s.heading);
    const std::uint32_t carBase = static_cast<std::uint32_t>(vertices.size());
    for (const auto& v : car.vertices) {
        const float x = (v.px - cx) * scale, z = (v.pz - cz) * scale;
        vertices.push_back({s.carPosition.x * .1f + x * ch + z * sh, (v.py - st.bmin[1]) * scale,
                            s.carPosition.z * .1f - x * sh + z * ch, .9f, .8f, .6f});
    }
    for (std::uint32_t i : car.indices) indices.push_back(carBase + i);
    float sky[3], fogColor[3];
    argbToRgb(s.lighting.skyColour, sky); argbToRgb(s.lighting.fogColourRGBA, fogColor);
    const float clear[4] = {sky[0], sky[1], sky[2], 1.f};
    // GameLoop's world is converted to render units (0.1) before the camera
    // matrices are made, so fog range must use the same scale as vView.
    const float fog[4] = {fogColor[0], fogColor[1], fogColor[2], s.lighting.fogFar * .1f};
    return renderGeometry(r, vertices, indices, s.view, s.proj, clear, fog, rgbaOut, error);
}

struct UiVertex { float x, y, r, g, b, a; };

bool previewRender2D(Renderer* Rp, const std::vector<ScreenQuad>& quads,
                     const std::array<float, 4>& clearColor,
                     std::vector<std::uint8_t>& rgbaOut, std::string& error) {
    if (!Rp) { error = "null renderer"; return false; }
    Renderer& R = *Rp;
    std::vector<UiVertex> vertices;
    vertices.reserve(quads.size() * 6);
    const auto ndcX = [&](float x) { return x * 2.0f / static_cast<float>(R.W) - 1.0f; };
    const auto ndcY = [&](float y) { return y * 2.0f / static_cast<float>(R.H) - 1.0f; };
    for (const ScreenQuad& q : quads) {
        if (q.width <= 0 || q.height <= 0) continue;
        const UiVertex a{ndcX(q.x), ndcY(q.y), q.color[0], q.color[1], q.color[2], q.color[3]};
        const UiVertex b{ndcX(q.x + q.width), ndcY(q.y), q.color[0], q.color[1], q.color[2], q.color[3]};
        const UiVertex c{ndcX(q.x + q.width), ndcY(q.y + q.height), q.color[0], q.color[1], q.color[2], q.color[3]};
        const UiVertex d{ndcX(q.x), ndcY(q.y + q.height), q.color[0], q.color[1], q.color[2], q.color[3]};
        vertices.insert(vertices.end(), {a, c, b, a, d, c});
    }
    if (vertices.empty()) { error = "no non-empty screen quads"; return false; }
    const VkDeviceSize vbSize = vertices.size() * sizeof(UiVertex);
    if (!growBuffer(R, error, vbSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, R.uiVb, R.uiVbm, R.uiVbCap)) return false;
    void* p;
    VK_TRY(vkMapMemory(R.dev, R.uiVbm, 0, vbSize, 0, &p), "vkMapMemory(ui)");
    std::memcpy(p, vertices.data(), vbSize); vkUnmapMemory(R.dev, R.uiVbm);

    VkCommandBuffer cmd = R.cmd;
    vkResetCommandBuffer(cmd, 0);
    VkCommandBufferBeginInfo bbi{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(cmd, &bbi);
    VkClearValue clears[2]{};
    clears[0].color = {{clearColor[0], clearColor[1], clearColor[2], clearColor[3]}};
    clears[1].depthStencil = {1.0f, 0};
    VkRenderPassBeginInfo rbi{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rbi.renderPass = R.rp; rbi.framebuffer = R.fb; rbi.renderArea = {{0, 0}, {R.W, R.H}};
    rbi.clearValueCount = 2; rbi.pClearValues = clears;
    vkCmdBeginRenderPass(cmd, &rbi, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, R.uiPipe);
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &R.uiVb, &offset);
    vkCmdDraw(cmd, static_cast<std::uint32_t>(vertices.size()), 1, 0, 0);
    vkCmdEndRenderPass(cmd);
    VkBufferImageCopy copy{};
    copy.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    copy.imageExtent = {R.W, R.H, 1};
    vkCmdCopyImageToBuffer(cmd, R.colImg, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, R.rb, 1, &copy);
    vkEndCommandBuffer(cmd);
    VkSubmitInfo si{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    si.commandBufferCount = 1; si.pCommandBuffers = &cmd;
    VK_TRY(vkQueueSubmit(R.queue, 1, &si, VK_NULL_HANDLE), "vkQueueSubmit(ui)");
    VK_TRY(vkQueueWaitIdle(R.queue), "vkQueueWaitIdle(ui)");
    const VkDeviceSize rbSize = static_cast<VkDeviceSize>(R.W) * R.H * 4;
    rgbaOut.resize(rbSize);
    VK_TRY(vkMapMemory(R.dev, R.rbm, 0, rbSize, 0, &p), "vkMapMemory(readback ui)");
    std::memcpy(rgbaOut.data(), p, rbSize);
    vkUnmapMemory(R.dev, R.rbm);
    return true;
}

bool renderModel(const assets::NativeModel& model, std::uint32_t W, std::uint32_t H,
                 std::vector<std::uint8_t>& rgbaOut, std::string& error, std::string* deviceName) {
    Renderer* R = previewInit(W, H, error, deviceName);
    if (!R) return false;
    const bool ok = previewRender(R, model, nullptr, nullptr, rgbaOut, error);
    previewDestroy(R);
    return ok;
}

bool renderFrame(const assets::NativeModel& model, const float* view, const float* proj,
                 std::uint32_t W, std::uint32_t H, std::vector<std::uint8_t>& rgbaOut,
                 std::string& error, std::string* deviceName) {
    Renderer* R = previewInit(W, H, error, deviceName);
    if (!R) return false;
    const bool ok = previewRender(R, model, view, proj, rgbaOut, error);
    previewDestroy(R);
    return ok;
}

} // namespace rt::native::preview
