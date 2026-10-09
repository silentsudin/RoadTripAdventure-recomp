// DLSS and XeSS plugin for Road Trip Recomp (Windows, Vulkan). Built apart from the app with MSVC,
// against NVIDIA's NGX SDK (static library) and Intel's XeSS SDK (import library): neither SDK's
// licence allows linking it into a GPL program, so they live here behind the C interface in
// runtime/gs/rt_upscaler_api.h and ship as an optional add-on. Each half is compiled in only when its
// SDK is given to CMake (RT_HAVE_DLSS, RT_HAVE_XESS).

#define RTU_EXPORT __declspec(dllexport)
#include "runtime/gs/rt_upscaler_api.h"

#include <vulkan/vulkan.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#if defined(RT_HAVE_DLSS)
#include "nvsdk_ngx_helpers_vk.h"
#include "nvsdk_ngx_vk.h"
#endif
#if defined(RT_HAVE_XESS)
#include "xess/xess_vk.h"
#endif

namespace
{
    RtuInit g_init = {};
    PFN_vkCmdPipelineBarrier g_cmdBarrier = nullptr;

    std::wstring widen(const std::string &s)
    {
        if (s.empty())
            return {};
        const int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
        std::wstring w(static_cast<size_t>(n), wchar_t(0));
        MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), w.data(), n);
        return w;
    }

    // The directory this DLL was loaded from (the vendor's DLLs are beside it).
    std::wstring moduleDir()
    {
        HMODULE self = nullptr;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&moduleDir), &self);
        std::wstring path(32768, wchar_t(0));
        path.resize(GetModuleFileNameW(self, path.data(), static_cast<DWORD>(path.size())));
        const size_t slash = path.find_last_of(L"/\\");
        return slash == std::wstring::npos ? std::wstring(L".") : path.substr(0, slash);
    }

    // The libraries want the output writable (GENERAL) and the inputs sampled (SHADER_READ_ONLY):
    // our contract keeps everything in SHADER_READ_ONLY between calls, so the output is moved here.
    void outputLayout(VkCommandBuffer cmd, uint64_t image, VkImageLayout from, VkImageLayout to)
    {
        if (!g_cmdBarrier)
            return;
        VkImageMemoryBarrier b = {};
        b.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        b.srcAccessMask = from == VK_IMAGE_LAYOUT_GENERAL ? VK_ACCESS_SHADER_WRITE_BIT : 0;
        b.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        b.oldLayout = from;
        b.newLayout = to;
        b.srcQueueFamilyIndex = b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        b.image = reinterpret_cast<VkImage>(image);
        b.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        g_cmdBarrier(cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &b);
    }

    VkImageSubresourceRange colorRange() { return {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}; }

    template <class T>
    const char *const *namesOf(const std::vector<T> &names, std::vector<const char *> &ptrs)
    {
        ptrs.clear();
        for (const auto &n : names)
            ptrs.push_back(n.c_str());
        return ptrs.data();
    }

    // ------------------------------------------------------------------------- DLSS
#if defined(RT_HAVE_DLSS)
    namespace dlss
    {
        // An id of our own, for NGX's "project" form (no application id from NVIDIA needed).
        constexpr const char *kProjectId = "6f1c1f5e-3b7d-4a0a-9a52-5d2f0a7b8c31";
        bool g_ok = false;
        NVSDK_NGX_Parameter *g_caps = nullptr;
        std::wstring g_dataDir, g_pluginDir;
        const wchar_t *g_pathList[1] = {nullptr};
        NVSDK_NGX_FeatureCommonInfo g_common = {};

        NVSDK_NGX_FeatureDiscoveryInfo discovery()
        {
            NVSDK_NGX_FeatureDiscoveryInfo d = {};
            d.SDKVersion = NVSDK_NGX_Version_API;
            d.FeatureID = NVSDK_NGX_Feature_SuperSampling;
            d.Identifier.IdentifierType = NVSDK_NGX_Application_Identifier_Type_Project_Id;
            d.Identifier.v.ProjectDesc.ProjectId = kProjectId;
            d.Identifier.v.ProjectDesc.EngineType = NVSDK_NGX_ENGINE_TYPE_CUSTOM;
            d.Identifier.v.ProjectDesc.EngineVersion = "1.0";
            d.ApplicationDataPath = g_dataDir.c_str();
            d.FeatureInfo = &g_common;
            return d;
        }

        void setupPaths()
        {
            if (g_pluginDir.empty())
                g_pluginDir = moduleDir();
            if (g_dataDir.empty())
                g_dataDir = widen(g_init.app_data_dir ? g_init.app_data_dir : ".");
            g_pathList[0] = g_pluginDir.c_str();
            g_common.PathListInfo.Path = g_pathList;
            g_common.PathListInfo.Length = 1;
        }

        std::vector<std::string> g_instNames, g_devNames;
        std::vector<const char *> g_instPtrs, g_devPtrs;

        int instanceExtensions(const char *const **names, uint32_t *count)
        {
            g_dataDir = L".";
            setupPaths();
            const NVSDK_NGX_FeatureDiscoveryInfo d = discovery();
            uint32_t n = 0;
            VkExtensionProperties *props = nullptr;
            g_instNames.clear();
            if (NVSDK_NGX_SUCCEED(NVSDK_NGX_VULKAN_GetFeatureInstanceExtensionRequirements(&d, &n, &props)))
                for (uint32_t i = 0; i < n; ++i)
                    g_instNames.emplace_back(props[i].extensionName);
            *names = namesOf(g_instNames, g_instPtrs);
            *count = static_cast<uint32_t>(g_instNames.size());
            return 0;
        }

        int deviceExtensions(void *instance, void *physical, const char *const **names, uint32_t *count)
        {
            setupPaths();
            const NVSDK_NGX_FeatureDiscoveryInfo d = discovery();
            uint32_t n = 0;
            VkExtensionProperties *props = nullptr;
            g_devNames.clear();
            if (NVSDK_NGX_SUCCEED(NVSDK_NGX_VULKAN_GetFeatureDeviceExtensionRequirements(
                    static_cast<VkInstance>(instance), static_cast<VkPhysicalDevice>(physical), &d, &n, &props)))
                for (uint32_t i = 0; i < n; ++i)
                    g_devNames.emplace_back(props[i].extensionName);
            *names = namesOf(g_devNames, g_devPtrs);
            *count = static_cast<uint32_t>(g_devNames.size());
            return 0;
        }

        bool init()
        {
            if (g_init.vendor_id != 0x10DE)
                return false; // NVIDIA RTX GPUs only
            g_dataDir = widen(g_init.app_data_dir ? g_init.app_data_dir : ".");
            setupPaths();
            NVSDK_NGX_Result r = NVSDK_NGX_VULKAN_Init_with_ProjectID(
                kProjectId, NVSDK_NGX_ENGINE_TYPE_CUSTOM, "1.0", g_dataDir.c_str(), static_cast<VkInstance>(g_init.instance),
                static_cast<VkPhysicalDevice>(g_init.physical_device), static_cast<VkDevice>(g_init.device),
                reinterpret_cast<PFN_vkGetInstanceProcAddr>(g_init.get_instance_proc_addr),
                reinterpret_cast<PFN_vkGetDeviceProcAddr>(g_init.get_device_proc_addr), &g_common);
            if (NVSDK_NGX_FAILED(r))
            {
                std::fprintf(stderr, "[dlss] NGX init failed (0x%x)\n", static_cast<unsigned>(r));
                return false;
            }
            r = NVSDK_NGX_VULKAN_GetCapabilityParameters(&g_caps);
            int available = 0, needsDriver = 0;
            if (NVSDK_NGX_SUCCEED(r) && g_caps)
            {
                NVSDK_NGX_Parameter_GetI(g_caps, NVSDK_NGX_Parameter_SuperSampling_NeedsUpdatedDriver, &needsDriver);
                NVSDK_NGX_Parameter_GetI(g_caps, NVSDK_NGX_Parameter_SuperSampling_Available, &available);
            }
            if (!available)
            {
                std::fprintf(stderr, "[dlss] not available on this GPU/driver%s\n", needsDriver ? " (the driver is too old)" : "");
                NVSDK_NGX_VULKAN_Shutdown1(static_cast<VkDevice>(g_init.device));
                return false;
            }
            g_ok = true;
            return true;
        }

        struct Context
        {
            uint32_t rw, rh, dw, dh;
            NVSDK_NGX_Handle *handle = nullptr;
            NVSDK_NGX_Parameter *params = nullptr;
        };

        NVSDK_NGX_PerfQuality_Value qualityFor(float ratio)
        {
            if (ratio >= 2.9f)
                return NVSDK_NGX_PerfQuality_Value_UltraPerformance;
            if (ratio >= 1.95f)
                return NVSDK_NGX_PerfQuality_Value_MaxPerf;
            if (ratio >= 1.65f)
                return NVSDK_NGX_PerfQuality_Value_Balanced;
            if (ratio >= 1.4f)
                return NVSDK_NGX_PerfQuality_Value_MaxQuality;
            if (ratio >= 1.15f)
                return NVSDK_NGX_PerfQuality_Value_UltraQuality;
            return NVSDK_NGX_PerfQuality_Value_DLAA;
        }

        int dispatch(Context *c, const RtuDispatch *in)
        {
            VkCommandBuffer cmd = static_cast<VkCommandBuffer>(in->command_buffer);
            if (!c->handle)
            {
                // The feature is made on a command buffer, which create() hasn't got: here.
                if (NVSDK_NGX_FAILED(NVSDK_NGX_VULKAN_AllocateParameters(&c->params)))
                    return 1;
                NVSDK_NGX_DLSS_Create_Params cp = {};
                cp.Feature.InWidth = c->rw;
                cp.Feature.InHeight = c->rh;
                cp.Feature.InTargetWidth = c->dw;
                cp.Feature.InTargetHeight = c->dh;
                cp.Feature.InPerfQualityValue = qualityFor(static_cast<float>(c->dw) / static_cast<float>(c->rw));
                // Low-res (render size) motion that includes the camera's jitter, reverse Z, LDR; no
                // exposure texture: the library measures it.
                cp.InFeatureCreateFlags = NVSDK_NGX_DLSS_Feature_Flags_MVLowRes | NVSDK_NGX_DLSS_Feature_Flags_MVJittered |
                                          NVSDK_NGX_DLSS_Feature_Flags_DepthInverted | NVSDK_NGX_DLSS_Feature_Flags_AutoExposure;
                const NVSDK_NGX_Result r = NGX_VULKAN_CREATE_DLSS_EXT1(static_cast<VkDevice>(g_init.device), cmd, 1, 1, &c->handle,
                                                                       c->params, &cp);
                if (NVSDK_NGX_FAILED(r))
                {
                    std::fprintf(stderr, "[dlss] feature creation failed (0x%x) at %ux%u -> %ux%u\n", static_cast<unsigned>(r), c->rw,
                                 c->rh, c->dw, c->dh);
                    c->handle = nullptr;
                    return 1;
                }
            }
            auto res = [](const RtuImage &i, bool write) {
                return NVSDK_NGX_Create_ImageView_Resource_VK(reinterpret_cast<VkImageView>(i.view), reinterpret_cast<VkImage>(i.image),
                                                              colorRange(), static_cast<VkFormat>(i.format), i.width, i.height, write);
            };
            NVSDK_NGX_Resource_VK color = res(in->color, false), depth = res(in->depth, false), motion = res(in->motion, false),
                                  output = res(in->output, true);
            NVSDK_NGX_VK_DLSS_Eval_Params ep = {};
            ep.Feature.pInColor = &color;
            ep.Feature.pInOutput = &output;
            ep.Feature.InSharpness = 0.0f; // sharpening is the host's (RCAS)
            ep.pInDepth = &depth;
            ep.pInMotionVectors = &motion;
            ep.InJitterOffsetX = in->jitter_x;
            ep.InJitterOffsetY = in->jitter_y;
            ep.InRenderSubrectDimensions = {in->render_width, in->render_height};
            ep.InReset = in->reset ? 1 : 0;
            ep.InMVScaleX = in->mv_scale_x;
            ep.InMVScaleY = in->mv_scale_y;
            ep.InPreExposure = 1.0f;
            ep.InExposureScale = 1.0f;
            ep.InFrameTimeDeltaInMsec = in->frame_time_ms;
            outputLayout(cmd, in->output.image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
            const NVSDK_NGX_Result r = NGX_VULKAN_EVALUATE_DLSS_EXT(cmd, c->handle, c->params, &ep);
            outputLayout(cmd, in->output.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            if (NVSDK_NGX_FAILED(r))
            {
                std::fprintf(stderr, "[dlss] evaluate failed (0x%x)\n", static_cast<unsigned>(r));
                return 1;
            }
            return 0;
        }

        void destroy(Context *c)
        {
            if (c->handle)
                NVSDK_NGX_VULKAN_ReleaseFeature(c->handle);
            if (c->params)
                NVSDK_NGX_VULKAN_DestroyParameters(c->params);
        }

        void shutdown()
        {
            if (g_ok)
                NVSDK_NGX_VULKAN_Shutdown1(static_cast<VkDevice>(g_init.device));
            g_ok = false;
        }
    }
#endif

    // ------------------------------------------------------------------------- XeSS
#if defined(RT_HAVE_XESS)
    namespace xess
    {
        bool g_ok = false;
        std::string g_version = "Intel XeSS";
        std::vector<std::string> g_instNames, g_devNames;
        std::vector<const char *> g_instPtrs, g_devPtrs;

        int instanceExtensions(const char *const **names, uint32_t *count)
        {
            uint32_t n = 0, minApi = 0;
            const char *const *list = nullptr;
            g_instNames.clear();
            if (xessVKGetRequiredInstanceExtensions(&n, &list, &minApi) == XESS_RESULT_SUCCESS)
                for (uint32_t i = 0; i < n; ++i)
                    g_instNames.emplace_back(list[i]);
            *names = namesOf(g_instNames, g_instPtrs);
            *count = static_cast<uint32_t>(g_instNames.size());
            return 0;
        }

        int deviceExtensions(void *instance, void *physical, const char *const **names, uint32_t *count)
        {
            uint32_t n = 0;
            const char *const *list = nullptr;
            g_devNames.clear();
            if (xessVKGetRequiredDeviceExtensions(static_cast<VkInstance>(instance), static_cast<VkPhysicalDevice>(physical), &n, &list) ==
                XESS_RESULT_SUCCESS)
                for (uint32_t i = 0; i < n; ++i)
                    g_devNames.emplace_back(list[i]);
            *names = namesOf(g_devNames, g_devPtrs);
            *count = static_cast<uint32_t>(g_devNames.size());
            return 0;
        }

        bool init()
        {
            xess_context_handle_t ctx = nullptr;
            if (xessVKCreateContext(static_cast<VkInstance>(g_init.instance), static_cast<VkPhysicalDevice>(g_init.physical_device),
                                    static_cast<VkDevice>(g_init.device), &ctx) != XESS_RESULT_SUCCESS)
            {
                std::fprintf(stderr, "[xess] this GPU/driver can't create an XeSS context\n");
                return false;
            }
            xess_version_t v = {};
            if (xessGetVersion(&v) == XESS_RESULT_SUCCESS)
                g_version = "Intel XeSS " + std::to_string(v.major) + "." + std::to_string(v.minor) + "." + std::to_string(v.patch);
            xessDestroyContext(ctx);
            g_ok = true;
            return true;
        }

        struct Context
        {
            xess_context_handle_t handle = nullptr;
            uint32_t rw, rh, dw, dh;
        };

        // The quality setting whose input range holds our render size (XeSS takes the input in a range
        // around the optimal size for each), nearest in ratio.
        bool pickQuality(xess_context_handle_t ctx, const xess_2d_t &out, uint32_t rw, uint32_t rh, xess_quality_settings_t *quality)
        {
            static const xess_quality_settings_t all[] = {XESS_QUALITY_SETTING_ULTRA_PERFORMANCE, XESS_QUALITY_SETTING_PERFORMANCE,
                                                          XESS_QUALITY_SETTING_BALANCED,          XESS_QUALITY_SETTING_QUALITY,
                                                          XESS_QUALITY_SETTING_ULTRA_QUALITY_PLUS, XESS_QUALITY_SETTING_ULTRA_QUALITY,
                                                          XESS_QUALITY_SETTING_AA};
            double best = 1e30;
            bool found = false;
            for (auto q : all)
            {
                xess_2d_t opt = {}, lo = {}, hi = {};
                if (xessGetOptimalInputResolution(ctx, &out, q, &opt, &lo, &hi) != XESS_RESULT_SUCCESS)
                    continue;
                if (rw < lo.x || rw > hi.x || rh < lo.y || rh > hi.y)
                    continue;
                const double d = std::abs(static_cast<double>(rw) - opt.x);
                if (d < best)
                {
                    best = d;
                    *quality = q;
                    found = true;
                }
            }
            return found;
        }

        Context *create(const RtuCreateInfo *info)
        {
            auto *c = new Context();
            c->rw = info->render_width, c->rh = info->render_height, c->dw = info->display_width, c->dh = info->display_height;
            if (xessVKCreateContext(static_cast<VkInstance>(g_init.instance), static_cast<VkPhysicalDevice>(g_init.physical_device),
                                    static_cast<VkDevice>(g_init.device), &c->handle) != XESS_RESULT_SUCCESS)
            {
                delete c;
                return nullptr;
            }
            const xess_2d_t out = {c->dw, c->dh};
            xess_vk_init_params_t p = {};
            p.outputResolution = out;
            if (!pickQuality(c->handle, out, c->rw, c->rh, &p.qualitySetting))
            {
                std::fprintf(stderr, "[xess] no quality setting takes %ux%u -> %ux%u\n", c->rw, c->rh, c->dw, c->dh);
                xessDestroyContext(c->handle);
                delete c;
                return nullptr;
            }
            // LDR colour, reverse Z, low-res motion that carries the camera's jitter.
            p.initFlags = XESS_INIT_FLAG_LDR_INPUT_COLOR | XESS_INIT_FLAG_INVERTED_DEPTH | XESS_INIT_FLAG_JITTERED_MV;
            const xess_result_t r = xessVKInit(c->handle, &p);
            if (r != XESS_RESULT_SUCCESS)
            {
                std::fprintf(stderr, "[xess] xessVKInit failed (%d)\n", static_cast<int>(r));
                xessDestroyContext(c->handle);
                delete c;
                return nullptr;
            }
            return c;
        }

        int dispatch(Context *c, const RtuDispatch *in)
        {
            VkCommandBuffer cmd = static_cast<VkCommandBuffer>(in->command_buffer);
            auto view = [](const RtuImage &i) {
                xess_vk_image_view_info v = {};
                v.imageView = reinterpret_cast<VkImageView>(i.view);
                v.image = reinterpret_cast<VkImage>(i.image);
                v.subresourceRange = colorRange();
                v.format = static_cast<VkFormat>(i.format);
                v.width = i.width;
                v.height = i.height;
                return v;
            };
            xessSetJitterScale(c->handle, 1.0f, 1.0f);
            xessSetVelocityScale(c->handle, in->mv_scale_x, in->mv_scale_y);
            xess_vk_execute_params_t e = {};
            e.colorTexture = view(in->color);
            e.velocityTexture = view(in->motion);
            e.depthTexture = view(in->depth);
            e.outputTexture = view(in->output);
            e.jitterOffsetX = in->jitter_x;
            e.jitterOffsetY = in->jitter_y;
            e.exposureScale = 1.0f;
            e.resetHistory = in->reset ? 1u : 0u;
            e.inputWidth = in->render_width;
            e.inputHeight = in->render_height;
            outputLayout(cmd, in->output.image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
            const xess_result_t r = xessVKExecute(c->handle, cmd, &e);
            outputLayout(cmd, in->output.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            if (r != XESS_RESULT_SUCCESS)
            {
                std::fprintf(stderr, "[xess] xessVKExecute failed (%d)\n", static_cast<int>(r));
                return 1;
            }
            return 0;
        }

        void destroy(Context *c)
        {
            if (c->handle)
                xessDestroyContext(c->handle);
        }
    }
#endif

    // ------------------------------------------------------------------------- the interface
    struct Ctx
    {
        uint32_t kind = 0;
        void *impl = nullptr;
    };

    int instanceExtensions(const char *const **names, uint32_t *count)
    {
        static std::vector<std::string> all;
        static std::vector<const char *> ptrs;
        all.clear();
        auto add = [&](int (*fn)(const char *const **, uint32_t *)) {
            const char *const *n = nullptr;
            uint32_t c = 0;
            if (fn(&n, &c) == 0)
                for (uint32_t i = 0; i < c; ++i)
                    all.emplace_back(n[i]);
        };
#if defined(RT_HAVE_DLSS)
        add(dlss::instanceExtensions);
#endif
#if defined(RT_HAVE_XESS)
        add(xess::instanceExtensions);
#endif
        (void)add;
        *names = namesOf(all, ptrs);
        *count = static_cast<uint32_t>(all.size());
        return 0;
    }

    int deviceExtensions(void *instance, void *physical, void *, const char *const **names, uint32_t *count)
    {
        static std::vector<std::string> all;
        static std::vector<const char *> ptrs;
        all.clear();
        auto add = [&](int (*fn)(void *, void *, const char *const **, uint32_t *)) {
            const char *const *n = nullptr;
            uint32_t c = 0;
            if (fn(instance, physical, &n, &c) == 0)
                for (uint32_t i = 0; i < c; ++i)
                    all.emplace_back(n[i]);
        };
#if defined(RT_HAVE_DLSS)
        add(dlss::deviceExtensions);
#endif
#if defined(RT_HAVE_XESS)
        add(xess::deviceExtensions);
#endif
        (void)add;
        *names = namesOf(all, ptrs);
        *count = static_cast<uint32_t>(all.size());
        return 0;
    }

    uint32_t init(const RtuInit *in)
    {
        g_init = *in;
        auto gdpa = reinterpret_cast<PFN_vkGetDeviceProcAddr>(in->get_device_proc_addr);
        g_cmdBarrier = reinterpret_cast<PFN_vkCmdPipelineBarrier>(gdpa(static_cast<VkDevice>(in->device), "vkCmdPipelineBarrier"));
        uint32_t kinds = 0;
#if defined(RT_HAVE_DLSS)
        if (dlss::init())
            kinds |= RTU_KIND_DLSS;
#endif
#if defined(RT_HAVE_XESS)
        if (xess::init())
            kinds |= RTU_KIND_XESS;
#endif
        return kinds;
    }

    float maxScale(uint32_t kind) { return kind == RTU_KIND_DLSS || kind == RTU_KIND_XESS ? 3.0f : 1.0f; }

    void *create(const RtuCreateInfo *info)
    {
        auto *c = new Ctx();
        c->kind = info->kind;
#if defined(RT_HAVE_DLSS)
        if (info->kind == RTU_KIND_DLSS && dlss::g_ok)
        {
            auto *d = new dlss::Context();
            d->rw = info->render_width, d->rh = info->render_height, d->dw = info->display_width, d->dh = info->display_height;
            c->impl = d;
            return c;
        }
#endif
#if defined(RT_HAVE_XESS)
        if (info->kind == RTU_KIND_XESS && xess::g_ok)
        {
            c->impl = xess::create(info);
            if (c->impl)
                return c;
        }
#endif
        delete c;
        return nullptr;
    }

    int dispatch(void *context, const RtuDispatch *in)
    {
        auto *c = static_cast<Ctx *>(context);
#if defined(RT_HAVE_DLSS)
        if (c->kind == RTU_KIND_DLSS)
            return dlss::dispatch(static_cast<dlss::Context *>(c->impl), in);
#endif
#if defined(RT_HAVE_XESS)
        if (c->kind == RTU_KIND_XESS)
            return xess::dispatch(static_cast<xess::Context *>(c->impl), in);
#endif
        (void)in;
        return 1;
    }

    void destroy(void *context)
    {
        auto *c = static_cast<Ctx *>(context);
        if (!c)
            return;
#if defined(RT_HAVE_DLSS)
        if (c->kind == RTU_KIND_DLSS && c->impl)
        {
            dlss::destroy(static_cast<dlss::Context *>(c->impl));
            delete static_cast<dlss::Context *>(c->impl);
        }
#endif
#if defined(RT_HAVE_XESS)
        if (c->kind == RTU_KIND_XESS && c->impl)
        {
            xess::destroy(static_cast<xess::Context *>(c->impl));
            delete static_cast<xess::Context *>(c->impl);
        }
#endif
        delete c;
    }

    void shutdown()
    {
#if defined(RT_HAVE_DLSS)
        dlss::shutdown();
#endif
    }

    const char *describe(uint32_t kind)
    {
#if defined(RT_HAVE_XESS)
        if (kind == RTU_KIND_XESS)
            return xess::g_version.c_str();
#endif
        return kind == RTU_KIND_DLSS ? "NVIDIA DLSS (NGX)" : "";
    }

    const RtuApi g_api = {RTU_API_VERSION, instanceExtensions, deviceExtensions, init, maxScale, create, dispatch,
                          destroy, shutdown, describe};
}

extern "C" RTU_EXPORT const RtuApi *rtu_get_api(uint32_t version)
{
    return version == RTU_API_VERSION ? &g_api : nullptr;
}
