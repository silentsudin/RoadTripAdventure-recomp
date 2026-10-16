// Minimal SDL3 + Vulkan host for the authored GameLoop.  The scene itself is
// rendered by previewRenderScene(), so this presentation layer deliberately
// contains no duplicate road/car/shader code.
#include "assets/field_converter.h"
#include "assets/field_tile_index.h"
#include "assets/native_model.h"
#include "audio/bgm_player.h"
#include "audio/engine.h"
#include "audio/vag_loader.h"
#include "gameplay/collectibles.h"
#include "gameplay/dialogue.h"
#include "gameplay/input.h"
#include "gameplay/menu.h"
#include "gameplay/session.h"
#include "gameplay/shop.h"
#include "preview/preview.h"
#include "preview/race_hud.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {
constexpr VkMemoryPropertyFlags kHostMemory = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

struct Presenter {
    VkInstance instance = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkPhysicalDevice physical = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;
    std::uint32_t queueFamily = 0;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkExtent2D extent{};
    std::vector<VkImage> images;
    VkCommandPool pool = VK_NULL_HANDLE;
    VkCommandBuffer command = VK_NULL_HANDLE;
    VkBuffer staging = VK_NULL_HANDLE;
    VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
    VkDeviceSize stagingSize = 0;
    VkSemaphore imageAvailable = VK_NULL_HANDLE;
    VkSemaphore renderDone = VK_NULL_HANDLE;
    VkFence frameDone = VK_NULL_HANDLE;
    VkPhysicalDeviceMemoryProperties memory{};
};

constexpr int kAudioSampleRate = 48000;
constexpr int kAudioChannels = 2;

struct AudioHost {
    rt::native::audio::SoundEngine engine;
    std::vector<std::int16_t> bgm;
    rt::native::audio::BgmPlayer tsqBgm;
    bool usingTsqBgm = false;
    SDL_AudioStream* stream = nullptr;
    std::atomic<bool> loggedFirstPeak = false;
    std::atomic<int> peakSinceLog = 0;
    std::atomic<int> framesSinceLog = 0;
};

void SDLCALL fillAudio(void* userdata, SDL_AudioStream* stream, int additionalBytes, int) {
    auto& audio = *static_cast<AudioHost*>(userdata);
    const int frames = additionalBytes / (kAudioChannels * static_cast<int>(sizeof(std::int16_t)));
    if (frames <= 0) return;

    rt::native::audio::OutputBuffer output;
    output.channels = kAudioChannels;
    output.samples.resize(static_cast<std::size_t>(frames) * output.channels);
    if (audio.usingTsqBgm) {
        audio.tsqBgm.pump(output.samples.data(), static_cast<std::size_t>(frames));
        // Keep the TSQ source at the same conservative combined BGM/master
        // gain used by the VAG SoundEngine path below.
        for (auto& sample : output.samples)
            sample = static_cast<std::int16_t>(static_cast<float>(sample) * (.25f * .7f));
    } else {
        audio.engine.tick(frames, output);
    }

    int peak = 0;
    for (const std::int16_t sample : output.samples) {
        const int magnitude = sample < 0 ? -static_cast<int>(sample) : static_cast<int>(sample);
        peak = std::max(peak, magnitude);
    }
    int observed = audio.peakSinceLog.load(std::memory_order_relaxed);
    while (observed < peak && !audio.peakSinceLog.compare_exchange_weak(
               observed, peak, std::memory_order_relaxed)) {}

    const int totalFrames = audio.framesSinceLog.fetch_add(frames, std::memory_order_relaxed) + frames;
    bool expected = false;
    if (audio.loggedFirstPeak.compare_exchange_strong(expected, true, std::memory_order_relaxed)) {
        std::fprintf(stderr, "rt-app audio callback frames=%d peak=%d\n", frames, peak);
    }
    if (totalFrames >= kAudioSampleRate) {
        const int intervalPeak = audio.peakSinceLog.exchange(0, std::memory_order_relaxed);
        audio.framesSinceLog.fetch_sub(kAudioSampleRate, std::memory_order_relaxed);
        std::fprintf(stderr, "rt-app audio peak=%d\n", intervalPeak);
    }
    (void)SDL_PutAudioStreamData(stream, output.samples.data(),
                                 static_cast<int>(output.samples.size() * sizeof(std::int16_t)));
}

bool initAudio(AudioHost& audio) {
    const char* bgmPath = std::getenv("RT_APP_BGM");
    const char* tsqSetting = std::getenv("RT_APP_BGM_TSQ");
    const bool tsqDisabled = tsqSetting && std::strcmp(tsqSetting, "0") == 0;
    const std::filesystem::path discRoot = [] {
        if (const char* root = std::getenv("RT_DISC")) return std::filesystem::path(root);
        if (const char* home = std::getenv("HOME"))
            return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
        return std::filesystem::path{};
    }();
    const std::string selectedTrack = bgmPath && *bgmPath ? bgmPath : "BGM_01";
    if (!tsqDisabled && audio.tsqBgm.load(discRoot, selectedTrack)) {
        audio.usingTsqBgm = true;
        std::cout << "rt-app BGM TSQ loaded: " << selectedTrack
                  << " from " << (discRoot / "SOUND") << "\n";
    } else {
        rt::native::audio::PcmBuffer loadedBgm;
        if (bgmPath && *bgmPath && rt::native::audio::loadVagFile(bgmPath, loadedBgm)) {
            audio.bgm = std::move(loadedBgm.samples);
            std::cout << "rt-app BGM loaded: " << bgmPath << " (" << audio.bgm.size() << " frames)\n";
        } else {
            if (bgmPath && *bgmPath)
                std::cerr << "rt-app BGM load failed, using synthetic tone: " << bgmPath << "\n";
            // A one-second synthetic saw wave keeps the output path usable when no
            // BGM file is selected. Its lifetime is the host's.
            audio.bgm.resize(kAudioSampleRate);
            for (int i = 0; i < kAudioSampleRate; ++i) {
                audio.bgm[static_cast<std::size_t>(i)] =
                    static_cast<std::int16_t>((i % 240) * 100 - 12000);
            }
        }
        audio.engine.setMasterVolume(.25f);
        audio.engine.playBgm(0, 0x0a1c0201u,
                             {audio.bgm.data(), audio.bgm.size(), true}, .7f);
    }

    const SDL_AudioSpec spec{SDL_AUDIO_S16, kAudioChannels, kAudioSampleRate};
    audio.stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                             &spec, fillAudio, &audio);
    if (!audio.stream) {
        std::cerr << "SDL audio disabled: " << SDL_GetError() << "\n";
        return false;
    }
    if (!SDL_ResumeAudioStreamDevice(audio.stream)) {
        std::cerr << "SDL audio disabled: " << SDL_GetError() << "\n";
        SDL_DestroyAudioStream(audio.stream);
        audio.stream = nullptr;
        return false;
    }
    std::cout << "rt-app audio device opened: 48000 Hz, S16, stereo\n";
    return true;
}

void destroyAudio(AudioHost& audio) {
    if (audio.stream) SDL_DestroyAudioStream(audio.stream);
    audio.stream = nullptr;
    audio.engine.stopAll();
}

bool vkOk(VkResult result, const char* what, std::string& error) {
    if (result == VK_SUCCESS) return true;
    error = std::string(what) + " (VkResult " + std::to_string(result) + ")";
    return false;
}

std::uint32_t memoryType(const Presenter& p, std::uint32_t bits, VkMemoryPropertyFlags flags) {
    for (std::uint32_t i = 0; i < p.memory.memoryTypeCount; ++i)
        if ((bits & (1u << i)) && (p.memory.memoryTypes[i].propertyFlags & flags) == flags) return i;
    return UINT32_MAX;
}

void destroySwapchain(Presenter& p) {
    if (p.staging) vkDestroyBuffer(p.device, p.staging, nullptr);
    if (p.stagingMemory) vkFreeMemory(p.device, p.stagingMemory, nullptr);
    p.staging = VK_NULL_HANDLE; p.stagingMemory = VK_NULL_HANDLE; p.stagingSize = 0;
    if (p.swapchain) vkDestroySwapchainKHR(p.device, p.swapchain, nullptr);
    p.swapchain = VK_NULL_HANDLE; p.images.clear(); p.extent = {};
}

void destroy(Presenter& p) {
    if (p.device) vkDeviceWaitIdle(p.device);
    destroySwapchain(p);
    if (p.frameDone) vkDestroyFence(p.device, p.frameDone, nullptr);
    if (p.renderDone) vkDestroySemaphore(p.device, p.renderDone, nullptr);
    if (p.imageAvailable) vkDestroySemaphore(p.device, p.imageAvailable, nullptr);
    if (p.pool) vkDestroyCommandPool(p.device, p.pool, nullptr);
    if (p.device) vkDestroyDevice(p.device, nullptr);
    if (p.surface) vkDestroySurfaceKHR(p.instance, p.surface, nullptr);
    if (p.instance) vkDestroyInstance(p.instance, nullptr);
    p = {};
}

bool makeSwapchain(Presenter& p, SDL_Window* window, std::string& error) {
    int pixelW = 0, pixelH = 0;
    SDL_GetWindowSizeInPixels(window, &pixelW, &pixelH);
    if (pixelW <= 0 || pixelH <= 0) return true; // Minimized; retry on the next size event.
    vkDeviceWaitIdle(p.device);
    destroySwapchain(p);
    VkSurfaceCapabilitiesKHR caps{};
    if (!vkOk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p.physical, p.surface, &caps), "surface capabilities", error)) return false;
    std::uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(p.physical, p.surface, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(p.physical, p.surface, &count, formats.data());
    if (formats.empty()) { error = "surface has no formats"; return false; }
    const auto preferred = std::find_if(formats.begin(), formats.end(), [](const auto& f) {
        return f.format == VK_FORMAT_B8G8R8A8_UNORM || f.format == VK_FORMAT_R8G8B8A8_UNORM;
    });
    p.format = (preferred == formats.end() ? formats.front() : *preferred).format;
    p.extent = caps.currentExtent.width != UINT32_MAX ? caps.currentExtent : VkExtent2D{
        std::clamp(static_cast<std::uint32_t>(pixelW), caps.minImageExtent.width, caps.maxImageExtent.width),
        std::clamp(static_cast<std::uint32_t>(pixelH), caps.minImageExtent.height, caps.maxImageExtent.height)};
    std::uint32_t modes = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(p.physical, p.surface, &modes, nullptr);
    std::vector<VkPresentModeKHR> presentModes(modes);
    vkGetPhysicalDeviceSurfacePresentModesKHR(p.physical, p.surface, &modes, presentModes.data());
    const bool mailbox = std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != presentModes.end();
    VkSwapchainCreateInfoKHR ci{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    ci.surface = p.surface; ci.minImageCount = std::max(2u, caps.minImageCount);
    if (caps.maxImageCount && ci.minImageCount > caps.maxImageCount) ci.minImageCount = caps.maxImageCount;
    ci.imageFormat = p.format; ci.imageColorSpace = (preferred == formats.end() ? formats.front() : *preferred).colorSpace;
    ci.imageExtent = p.extent; ci.imageArrayLayers = 1; ci.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; ci.preTransform = caps.currentTransform;
    ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; ci.presentMode = mailbox ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR;
    ci.clipped = VK_TRUE;
    if (!vkOk(vkCreateSwapchainKHR(p.device, &ci, nullptr, &p.swapchain), "vkCreateSwapchainKHR", error)) return false;
    vkGetSwapchainImagesKHR(p.device, p.swapchain, &count, nullptr); p.images.resize(count);
    vkGetSwapchainImagesKHR(p.device, p.swapchain, &count, p.images.data());
    const VkDeviceSize bytes = static_cast<VkDeviceSize>(p.extent.width) * p.extent.height * 4;
    VkBufferCreateInfo buffer{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO}; buffer.size = bytes; buffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT; buffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (!vkOk(vkCreateBuffer(p.device, &buffer, nullptr, &p.staging), "vkCreateBuffer(staging)", error)) return false;
    VkMemoryRequirements requirements{}; vkGetBufferMemoryRequirements(p.device, p.staging, &requirements);
    const std::uint32_t type = memoryType(p, requirements.memoryTypeBits, kHostMemory);
    if (type == UINT32_MAX) { error = "no host-visible staging memory"; return false; }
    VkMemoryAllocateInfo alloc{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO}; alloc.allocationSize = requirements.size; alloc.memoryTypeIndex = type;
    if (!vkOk(vkAllocateMemory(p.device, &alloc, nullptr, &p.stagingMemory), "vkAllocateMemory(staging)", error) ||
        !vkOk(vkBindBufferMemory(p.device, p.staging, p.stagingMemory, 0), "vkBindBufferMemory(staging)", error)) return false;
    p.stagingSize = bytes;
    return true;
}

bool init(Presenter& p, SDL_Window* window, std::string& error) {
    std::uint32_t extensionCount = 0;
    const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    if (!sdlExtensions) { error = SDL_GetError(); return false; }
    std::vector<const char*> extensions(sdlExtensions, sdlExtensions + extensionCount);
#ifdef __APPLE__
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif
    VkApplicationInfo app{VK_STRUCTURE_TYPE_APPLICATION_INFO}; app.pApplicationName = "rt-app"; app.apiVersion = VK_API_VERSION_1_1;
    VkInstanceCreateInfo instance{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO}; instance.pApplicationInfo = &app;
    instance.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size()); instance.ppEnabledExtensionNames = extensions.data();
#ifdef __APPLE__
    instance.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
    if (!vkOk(vkCreateInstance(&instance, nullptr, &p.instance), "vkCreateInstance", error) ||
        !SDL_Vulkan_CreateSurface(window, p.instance, nullptr, &p.surface)) { if (error.empty()) error = SDL_GetError(); return false; }
    std::uint32_t deviceCount = 0; vkEnumeratePhysicalDevices(p.instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount); vkEnumeratePhysicalDevices(p.instance, &deviceCount, devices.data());
    for (const auto physical : devices) {
        std::uint32_t queueCount = 0; vkGetPhysicalDeviceQueueFamilyProperties(physical, &queueCount, nullptr);
        std::vector<VkQueueFamilyProperties> queues(queueCount); vkGetPhysicalDeviceQueueFamilyProperties(physical, &queueCount, queues.data());
        for (std::uint32_t i = 0; i < queueCount; ++i) { VkBool32 present = VK_FALSE; vkGetPhysicalDeviceSurfaceSupportKHR(physical, i, p.surface, &present);
            if ((queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && present) { p.physical = physical; p.queueFamily = i; break; } }
        if (p.physical) break;
    }
    if (!p.physical) { error = "no graphics/present queue"; return false; }
    vkGetPhysicalDeviceMemoryProperties(p.physical, &p.memory);
    float priority = 1.f; VkDeviceQueueCreateInfo queue{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO}; queue.queueFamilyIndex = p.queueFamily; queue.queueCount = 1; queue.pQueuePriorities = &priority;
    std::vector<const char*> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#ifdef __APPLE__
    deviceExtensions.push_back("VK_KHR_portability_subset");
#endif
    VkDeviceCreateInfo device{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO}; device.queueCreateInfoCount = 1; device.pQueueCreateInfos = &queue;
    device.enabledExtensionCount = static_cast<std::uint32_t>(deviceExtensions.size()); device.ppEnabledExtensionNames = deviceExtensions.data();
    if (!vkOk(vkCreateDevice(p.physical, &device, nullptr, &p.device), "vkCreateDevice", error)) return false;
    vkGetDeviceQueue(p.device, p.queueFamily, 0, &p.queue);
    VkCommandPoolCreateInfo pool{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO}; pool.queueFamilyIndex = p.queueFamily; pool.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VkCommandBufferAllocateInfo command{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    if (!vkOk(vkCreateCommandPool(p.device, &pool, nullptr, &p.pool), "vkCreateCommandPool", error)) return false;
    command.commandPool = p.pool; command.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; command.commandBufferCount = 1;
    if (!vkOk(vkAllocateCommandBuffers(p.device, &command, &p.command), "vkAllocateCommandBuffers", error)) return false;
    VkSemaphoreCreateInfo semaphore{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO}; VkFenceCreateInfo fence{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO}; fence.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (!vkOk(vkCreateSemaphore(p.device, &semaphore, nullptr, &p.imageAvailable), "vkCreateSemaphore", error) ||
        !vkOk(vkCreateSemaphore(p.device, &semaphore, nullptr, &p.renderDone), "vkCreateSemaphore", error) ||
        !vkOk(vkCreateFence(p.device, &fence, nullptr, &p.frameDone), "vkCreateFence", error)) return false;
    return makeSwapchain(p, window, error);
}

bool present(Presenter& p, const std::vector<std::uint8_t>& rgba, bool& outOfDate, std::string& error) {
    outOfDate = false;
    if (!p.swapchain || rgba.size() != p.stagingSize) return true;
    vkWaitForFences(p.device, 1, &p.frameDone, VK_TRUE, UINT64_MAX);
    std::uint32_t image = 0; const VkResult acquired = vkAcquireNextImageKHR(p.device, p.swapchain, UINT64_MAX, p.imageAvailable, VK_NULL_HANDLE, &image);
    if (acquired == VK_ERROR_OUT_OF_DATE_KHR) { outOfDate = true; return true; }
    if (acquired != VK_SUCCESS && acquired != VK_SUBOPTIMAL_KHR && !vkOk(acquired, "vkAcquireNextImageKHR", error)) return false;
    void* mapped = nullptr; if (!vkOk(vkMapMemory(p.device, p.stagingMemory, 0, p.stagingSize, 0, &mapped), "vkMapMemory(staging)", error)) return false;
    auto* out = static_cast<std::uint8_t*>(mapped);
    if (p.format == VK_FORMAT_B8G8R8A8_UNORM || p.format == VK_FORMAT_B8G8R8A8_SRGB) for (std::size_t i = 0; i < rgba.size(); i += 4) { out[i] = rgba[i + 2]; out[i + 1] = rgba[i + 1]; out[i + 2] = rgba[i]; out[i + 3] = rgba[i + 3]; }
    else std::memcpy(out, rgba.data(), rgba.size());
    vkUnmapMemory(p.device, p.stagingMemory); vkResetFences(p.device, 1, &p.frameDone); vkResetCommandBuffer(p.command, 0);
    VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO}; vkBeginCommandBuffer(p.command, &begin);
    VkImageMemoryBarrier toTransfer{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER}; toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED; toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; toTransfer.image = p.images[image]; toTransfer.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    vkCmdPipelineBarrier(p.command, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &toTransfer);
    VkBufferImageCopy copy{}; copy.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1}; copy.imageExtent = {p.extent.width, p.extent.height, 1};
    vkCmdCopyBufferToImage(p.command, p.staging, p.images[image], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    VkImageMemoryBarrier toPresent = toTransfer; toPresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; toPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    toPresent.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; toPresent.dstAccessMask = 0;
    vkCmdPipelineBarrier(p.command, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &toPresent);
    vkEndCommandBuffer(p.command);
    const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_TRANSFER_BIT; VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO}; submit.waitSemaphoreCount = 1; submit.pWaitSemaphores = &p.imageAvailable; submit.pWaitDstStageMask = &waitStage; submit.commandBufferCount = 1; submit.pCommandBuffers = &p.command; submit.signalSemaphoreCount = 1; submit.pSignalSemaphores = &p.renderDone;
    if (!vkOk(vkQueueSubmit(p.queue, 1, &submit, p.frameDone), "vkQueueSubmit", error)) return false;
    VkPresentInfoKHR info{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR}; info.waitSemaphoreCount = 1; info.pWaitSemaphores = &p.renderDone; info.swapchainCount = 1; info.pSwapchains = &p.swapchain; info.pImageIndices = &image;
    const VkResult result = vkQueuePresentKHR(p.queue, &info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) { outOfDate = true; return true; }
    return result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR || (error = "vkQueuePresentKHR (VkResult " + std::to_string(result) + ")", false);
}

rt::native::assets::NativeModel demoCar() {
    using rt::native::assets::NativeModel; using rt::native::assets::Vertex;
    NativeModel car;
    const std::array<std::array<float, 3>, 8> points{{{-1.f, 0.f, -2.f}, {1.f, 0.f, -2.f}, {1.f, 0.f, 2.f}, {-1.f, 0.f, 2.f}, {-1.f, 1.f, -2.f}, {1.f, 1.f, -2.f}, {1.f, 1.f, 2.f}, {-1.f, 1.f, 2.f}}};
    for (const auto& p : points) car.vertices.push_back(Vertex{p[0], p[1], p[2]});
    car.indices = {0,2,1,0,3,2, 4,5,6,4,6,7, 0,1,5,0,5,4, 1,2,6,1,6,5, 2,3,7,2,7,6, 3,0,4,3,4,7};
    return car;
}
int envFrames() { const char* value = std::getenv("RT_APP_FRAMES"); return value ? std::max(0, std::atoi(value)) : 0; }

// RT_APP_TERRAIN=1 replaces the flat grass quad with real terrain extracted
// from shipped FLD tiles in the disc extract.  Default renders the full
// 64-tile world grid; RT_APP_TERRAIN_FILE=PATH overrides with one explicit
// tile.  Absent/malformed input falls back to the authored quad, never a
// failure.
rt::native::preview::TerrainMesh loadTerrainMesh() {
    rt::native::preview::TerrainMesh mesh;
    if (std::getenv("RT_APP_TERRAIN") == nullptr) return mesh;
    const std::filesystem::path root = [] {
        if (const char* value = std::getenv("RT_DISC")) return std::filesystem::path(value);
        if (const char* home = std::getenv("HOME"))
            return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
        return std::filesystem::path{};
    }();
    const char* named = std::getenv("RT_APP_TERRAIN_FILE");
    if (named != nullptr) {
        const std::filesystem::path path{named};
        std::ifstream input(path, std::ios::binary);
        if (!input) { std::cout << "rt-app terrain: SKIP (no tile at " << path << ")\n"; return mesh; }
        const std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)),
                                             std::istreambuf_iterator<char>());
        const auto catalog = rt::native::assets::catalogFieldTile(bytes);
        if (!catalog) { std::cout << "rt-app terrain: catalog failed for " << path << "\n"; return mesh; }
        auto terrain = rt::native::assets::buildFieldRenderTerrain(bytes, *catalog, 0.1f);
        mesh.positions = std::move(terrain.positions);
        mesh.indices = std::move(terrain.indices);
        mesh.triangleMaterials = std::move(terrain.triangleMaterials);
        std::cout << "rt-app terrain: loaded " << path.filename().string()
                  << " triangles=" << mesh.indices.size() / 3 << "\n";
        return mesh;
    }

    // Multi-tile path: resolve streaming ids via the ROM LBA descriptor table
    // + size matching (field_tile_index), place tile (col = id&0xF,
    // row = id>>4) at slot offsets — x step 80 render units (half-tile: the
    // lattice is a staggered brick grid), z step 160.
    const auto index = rt::native::assets::buildFieldTileIndex(root);
    if (!index) {
        std::cout << "rt-app terrain: SKIP (no tile index; err="
                  << static_cast<int>(index.error()) << ")\n";
        return mesh;
    }
    std::size_t tilesLoaded = 0;
    for (std::size_t id = 0; id < rt::native::assets::kFieldTileCount; ++id) {
        const std::string& name = (*index)[id].fileName;
        if (name.empty()) continue;
        std::ifstream input(root / "FLD" / name, std::ios::binary);
        if (!input) continue;
        const std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)),
                                             std::istreambuf_iterator<char>());
        const auto catalog = rt::native::assets::catalogFieldTile(bytes);
        if (!catalog) continue;
        auto terrain = rt::native::assets::buildFieldRenderTerrain(bytes, *catalog, 0.1f);
        const std::uint32_t vertexBase = static_cast<std::uint32_t>(mesh.positions.size());
        // Center the 16x4 slot grid on the origin so the spawn car starts on
        // solid ground (cols 0..15, rows 0..3; half-tile stagger steps of 80).
        const float xOffset = static_cast<float>(id & 0xFu) * 80.0f - 8.0f * 80.0f;
        const float zOffset = static_cast<float>(id >> 4) * 160.0f - 1.5f * 160.0f;
        mesh.positions.reserve(mesh.positions.size() + terrain.positions.size());
        for (auto& p : terrain.positions) {
            p[0] += xOffset;
            p[2] += zOffset;
            mesh.positions.push_back(p);
        }
        mesh.indices.reserve(mesh.indices.size() + terrain.indices.size());
        for (std::uint32_t i : terrain.indices) mesh.indices.push_back(vertexBase + i);
        mesh.triangleMaterials.insert(mesh.triangleMaterials.end(),
                                     terrain.triangleMaterials.begin(),
                                     terrain.triangleMaterials.end());
        ++tilesLoaded;
    }
    std::cout << "rt-app terrain: tiles=" << tilesLoaded
              << " triangles=" << mesh.indices.size() / 3 << "\n";
    return mesh;
}

enum class AppState { TITLE, MENU, PLAYING };

const char* appStateName(AppState state) {
    switch (state) {
    case AppState::TITLE: return "TITLE";
    case AppState::MENU: return "MENU";
    case AppState::PLAYING: return "PLAYING";
    }
    return "UNKNOWN";
}

const char* sessionStateName(rt::native::gameplay::SessionState state) {
    using rt::native::gameplay::SessionState;
    switch (state) {
    case SessionState::EXPLORE: return "EXPLORE";
    case SessionState::RACE: return "RACE";
    case SessionState::RESULTS: return "RESULTS";
    }
    return "UNKNOWN";
}

AppState initialAppState() {
    const char* value = std::getenv("RT_APP_STATE");
    if (value && std::strcmp(value, "MENU") == 0) return AppState::MENU;
    if (value && std::strcmp(value, "PLAYING") == 0) return AppState::PLAYING;
    return AppState::TITLE;
}

std::vector<rt::native::preview::ScreenQuad> menuQuads(
    AppState state, const rt::native::gameplay::MenuFlow& flow, int width, int height) {
    using Quad = rt::native::preview::ScreenQuad;
    const float scale = std::min(static_cast<float>(width) / 1024.f, static_cast<float>(height) / 720.f);
    const float offsetX = (width - 1024.f * scale) * .5f;
    const float offsetY = (height - 720.f * scale) * .5f;
    const auto rect = [=](float x, float y, float w, float h, std::array<float, 4> color) {
        return Quad{offsetX + x * scale, offsetY + y * scale, w * scale, h * scale, color};
    };
    const std::array<float, 4> panelColor{.08f, .12f, .22f, 1.f};
    const std::array<float, 4> rowColor{.22f, .32f, .52f, 1.f};
    const std::array<float, 4> selectedRowColor{.36f, .56f, .86f, 1.f};
    const std::array<float, 4> cursorColor{1.f, .72f, .16f, 1.f};
    std::vector<Quad> quads{rect(112, 72, 800, 576, panelColor)};
    if (state == AppState::TITLE) {
        quads.push_back(rect(202, 216, 620, 80, {.55f, .78f, 1.f, 1.f}));
        quads.push_back(rect(286, 386, 452, 30, {.88f, .93f, 1.f, 1.f}));
        return quads;
    }

    quads.push_back(rect(202, 120, 620, 34, {.55f, .78f, 1.f, 1.f}));
    constexpr float rowX = 234, rowY = 204, rowWidth = 556, rowHeight = 58, rowGap = 27;
    for (std::size_t row = 0; row < 4; ++row) {
        const float y = rowY + static_cast<float>(row) * (rowHeight + rowGap);
        if (row == flow.topLevelCursor) quads.push_back(rect(198, y - 8, 628, rowHeight + 16, cursorColor));
        quads.push_back(rect(rowX, y, rowWidth, rowHeight, row == flow.topLevelCursor ? selectedRowColor : rowColor));
        quads.push_back(rect(rowX + 28, y + 19, rowWidth - 56, 20, {.88f, .93f, 1.f, 1.f}));
    }
    return quads;
}

void compositeOver(std::vector<std::uint8_t>& scene, const std::vector<std::uint8_t>& overlay) {
    if (scene.size() != overlay.size()) return;
    for (std::size_t pixel = 0; pixel < scene.size(); pixel += 4) {
        const float alpha = static_cast<float>(overlay[pixel + 3]) / 255.f;
        if (alpha <= 0.f) continue;
        for (std::size_t channel = 0; channel < 3; ++channel) {
            const float source = static_cast<float>(overlay[pixel + channel]);
            const float destination = static_cast<float>(scene[pixel + channel]);
            scene[pixel + channel] = static_cast<std::uint8_t>(source * alpha + destination * (1.f - alpha) + .5f);
        }
    }
}
} // namespace

int main() {
    constexpr int kInitialWidth = 800, kInitialHeight = 600;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) { std::cerr << "SDL_Init: " << SDL_GetError() << "\n"; return 1; }
    SDL_Window* window = SDL_CreateWindow("Road Trip authored scene", kInitialWidth, kInitialHeight, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if (!window) { std::cerr << "SDL_CreateWindow: " << SDL_GetError() << "\n"; SDL_Quit(); return 1; }
    Presenter presenter; std::string error;
    if (!init(presenter, window, error)) { std::cerr << "Vulkan init: " << error << "\n"; destroy(presenter); SDL_DestroyWindow(window); SDL_Quit(); return 1; }
    int width = static_cast<int>(presenter.extent.width), height = static_cast<int>(presenter.extent.height);
    rt::native::preview::Renderer* renderer = rt::native::preview::previewInit(width, height, error, nullptr);
    if (!renderer) { std::cerr << "scene renderer init: " << error << "\n"; destroy(presenter); SDL_DestroyWindow(window); SDL_Quit(); return 1; }
    const bool noRender = std::getenv("RT_APP_NORENDER") != nullptr; const int maxFrames = envFrames();
    const rt::native::preview::TerrainMesh terrainMesh = loadTerrainMesh();
    AudioHost audio;
    if (std::getenv("RT_APP_NOAUDIO") == nullptr) (void)initAudio(audio);
    const auto recreateSwapchain = [&]() {
        int newW = 0, newH = 0; SDL_GetWindowSizeInPixels(window, &newW, &newH);
        if (newW <= 0 || newH <= 0) return true;
        if (!makeSwapchain(presenter, window, error)) { std::cerr << "swapchain recreate: " << error << "\n"; return false; }
        rt::native::preview::previewDestroy(renderer);
        renderer = rt::native::preview::previewInit(presenter.extent.width, presenter.extent.height, error, nullptr);
        if (!renderer) { std::cerr << "scene resize: " << error << "\n"; return false; }
        return true;
    };
    auto car = demoCar();
    // One lap lets the headless host exercise the complete authored session
    // loop promptly; GameSession enables race mode for its owned GameLoop.
    rt::native::gameplay::GameSession session(1);
    // These host-owned examples deliberately use synthetic names, IDs, and
    // positions; they do not load any guest game data.
    std::vector<rt::native::gameplay::Collectible> collectibles{
        {rt::native::gameplay::CollectibleKind::STAMP, {400.f, 400.f}, 30.f, 0},
        {rt::native::gameplay::CollectibleKind::COIN, {1200.f, 400.f}, 30.f, 0},
        {rt::native::gameplay::CollectibleKind::STAMP, {1200.f, 1200.f}, 30.f, 1},
    };
    rt::native::gameplay::Shop shop{{
        {1, "road map", 100, 1},
        {2, "fuel can", 250, 1},
        {3, "lucky charm", 500, 1},
    }};
    rt::native::gameplay::Dialogue dialogue;
    const std::vector<rt::native::gameplay::DialogueLine> dialogueLines{
        {"Guide", "Welcome to the scenic route."},
        {"Guide", "Collect stamps and coins as you drive."},
        {"Guide", "The shop has supplies when you need them."},
    };
    bool shopOpen = false;
    rt::native::gameplay::ControllerConfig controllerConfig;
    bool playerDriven = false, running = true; int frames = 0;
    std::uint64_t simulationFrame = 0;
    AppState state = initialAppState();
    // RT_APP_STATE=PLAYING is the host's direct-launch shortcut.  Log and
    // expose its initial EXPLORE state for one frame, then start the same
    // first AI race that older direct launches used.
    bool directStartRace = state == AppState::PLAYING;
    rt::native::gameplay::MenuFlow menuFlow;
    if (state == AppState::MENU) menuFlow.screen = rt::native::gameplay::MenuScreen::MODE_SELECT;
    bool loggedAppState = false, loggedSessionState = false;
    AppState lastLoggedAppState = state;
    rt::native::gameplay::SessionState lastLoggedSessionState = session.state();
    auto previous = std::chrono::steady_clock::now();
    while (running && (!maxFrames || frames < maxFrames)) {
        SDL_Event event; bool resize = false;
        rt::native::gameplay::MenuInput menuInput;
        bool titleAdvance = false, returnToMenu = false;
        bool startRace = false, openShop = false, openDialogue = false, advanceDialogue = false;
        int shopSelection = -1;
        while (SDL_PollEvent(&event)) { if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) running = false;
            if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
                if (state == AppState::TITLE) titleAdvance = true;
                if (state == AppState::MENU) {
                    menuInput.up = menuInput.up || event.key.scancode == SDL_SCANCODE_UP;
                    menuInput.down = menuInput.down || event.key.scancode == SDL_SCANCODE_DOWN;
                    menuInput.confirm = menuInput.confirm || event.key.scancode == SDL_SCANCODE_RETURN;
                    menuInput.cancel = menuInput.cancel || event.key.scancode == SDL_SCANCODE_ESCAPE;
                }
                if (state == AppState::PLAYING) {
                    if (event.key.scancode == SDL_SCANCODE_ESCAPE) returnToMenu = true;
                    if (event.key.scancode == SDL_SCANCODE_TAB &&
                        session.state() == rt::native::gameplay::SessionState::RACE)
                        playerDriven = !playerDriven;
                    if (session.state() == rt::native::gameplay::SessionState::EXPLORE) {
                        openShop = openShop || event.key.scancode == SDL_SCANCODE_S;
                        openDialogue = openDialogue || event.key.scancode == SDL_SCANCODE_D;
                        if (shopOpen && event.key.scancode >= SDL_SCANCODE_1 && event.key.scancode <= SDL_SCANCODE_3)
                            shopSelection = static_cast<int>(event.key.scancode - SDL_SCANCODE_1);
                        if (dialogue.isActive() && event.key.scancode == SDL_SCANCODE_RETURN)
                            advanceDialogue = true;
                        else if (!dialogue.isActive() &&
                                 (event.key.scancode == SDL_SCANCODE_RETURN || event.key.scancode == SDL_SCANCODE_R))
                            startRace = true;
                    }
                    if (session.state() == rt::native::gameplay::SessionState::RESULTS &&
                        event.key.scancode == SDL_SCANCODE_RETURN)
                        session.continueFromResults();
                }
            }
            if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) resize = true; }
        if (!running) break;
        if (resize && !recreateSwapchain()) break;
        const auto now = std::chrono::steady_clock::now(); const float dt = std::min(std::chrono::duration<float>(now - previous).count(), 0.1f); previous = now;
        if (state == AppState::TITLE && titleAdvance) {
            menuFlow = {};
            (void)rt::native::gameplay::tickMenuFlow(menuFlow, {.confirm = true}, dt);
            state = AppState::MENU;
        } else if (state == AppState::MENU) {
            const auto menuEvent = rt::native::gameplay::tickMenuFlow(menuFlow, menuInput, dt);
            if (menuEvent.type == rt::native::gameplay::MenuEventType::MODE_SELECTED) {
                state = AppState::PLAYING;
                // Preserve the old direct-to-race menu launch while leaving
                // EXPLORE available after results (and via direct PLAYING).
                session.startRace();
            }
            if (menuEvent.type == rt::native::gameplay::MenuEventType::CANCEL && menuFlow.screen == rt::native::gameplay::MenuScreen::TITLE)
                state = AppState::TITLE;
        } else if (state == AppState::PLAYING && returnToMenu) {
            menuFlow.screen = rt::native::gameplay::MenuScreen::MODE_SELECT;
            state = AppState::MENU;
        }
        const bool* keys = SDL_GetKeyboardState(nullptr);
        const auto held = [keys](SDL_Scancode key) { return keys && keys[key]; };
        rt::native::gameplay::PadState pad;
        rt::native::gameplay::AnalogAxes axes;
        axes.stickX = (held(SDL_SCANCODE_RIGHT) || held(SDL_SCANCODE_D) ? 1.f : 0.f) -
                      (held(SDL_SCANCODE_LEFT) || held(SDL_SCANCODE_A) ? 1.f : 0.f);
        if (held(SDL_SCANCODE_UP) || held(SDL_SCANCODE_W))
            rt::native::gameplay::setPressed(pad, controllerConfig.getButtonConfig(rt::native::gameplay::ControllerAction::ACCELERATE));
        if (held(SDL_SCANCODE_DOWN) || held(SDL_SCANCODE_S))
            rt::native::gameplay::setPressed(pad, controllerConfig.getButtonConfig(rt::native::gameplay::ControllerAction::BRAKE));
        if (held(SDL_SCANCODE_SPACE))
            rt::native::gameplay::setPressed(pad, controllerConfig.getButtonConfig(rt::native::gameplay::ControllerAction::REVERSE));
        const auto input = rt::native::gameplay::playerCarInput(pad, axes, controllerConfig);
        (void)input;
        rt::native::gameplay::FrameSnapshot snapshot;
        if (state == AppState::PLAYING) {
            // GameSession owns the race loop.  Its current public update path
            // is AI-driven; Tab remains the race-mode selection control for
            // the host UI and is logged below.
            session.update(menuInput);
            snapshot = session.lastSnapshot();
            simulationFrame = snapshot.frame;

            if (session.state() == rt::native::gameplay::SessionState::EXPLORE) {
                if (openShop && !shopOpen) {
                    shopOpen = true;
                    std::cout << "rt-app shop opened: 1=road map, 2=fuel can, 3=lucky charm\n";
                }
                if (openDialogue && !dialogue.isActive()) {
                    dialogue.open(dialogueLines);
                    const auto* line = dialogue.currentLine();
                    std::cout << "rt-app dialogue " << line->speaker << ": " << line->text << "\n";
                }
                if (shopOpen && shopSelection >= 0) {
                    // GameSession owns a mutable SaveData; its public accessor is
                    // read-only for callers that only display progress.
                    auto& progress = const_cast<rt::native::gameplay::SaveData&>(session.progress());
                    const std::uint32_t moneyBefore = progress.money;
                    const bool bought = shop.buy(progress, static_cast<std::size_t>(shopSelection));
                    const char* itemName = shopSelection < static_cast<int>(shop.items.size())
                        ? shop.items[static_cast<std::size_t>(shopSelection)].name.c_str() : "invalid item";
                    std::cout << "rt-app shop buy " << itemName << " result=" << (bought ? "ok" : "failed")
                              << " money=" << moneyBefore << "->" << progress.money << "\n";
                }
                if (advanceDialogue && dialogue.isActive()) {
                    const bool finished = dialogue.advance({.confirm = true});
                    if (finished)
                        std::cout << "rt-app dialogue closed\n";
                    else {
                        const auto* line = dialogue.currentLine();
                        std::cout << "rt-app dialogue " << line->speaker << ": " << line->text << "\n";
                    }
                }
                if (startRace) session.startRace();
            }

            // In EXPLORE, this uses the retained session snapshot.  The AI
            // drives during RACE, too, so checking that snapshot there makes
            // the synthetic pickup path observable in a headless launch.
            if (session.state() == rt::native::gameplay::SessionState::EXPLORE ||
                (session.state() == rt::native::gameplay::SessionState::RACE && !playerDriven)) {
                auto& progress = const_cast<rt::native::gameplay::SaveData&>(session.progress());
                for (auto& collectible : collectibles) {
                    if (!rt::native::gameplay::tryCollect(collectible, snapshot.carPosition, progress)) continue;
                    std::cout << "rt-app collected "
                              << (collectible.kind == rt::native::gameplay::CollectibleKind::STAMP ? "stamp " : "coin ")
                              << collectible.index << " total="
                              << rt::native::gameplay::collectedCount(collectibles) << "\n";
                }
            }
        }
        if (!loggedAppState || state != lastLoggedAppState) {
            std::cout << "rt-app state=" << appStateName(state) << "\n";
            lastLoggedAppState = state;
            loggedAppState = true;
        }
        if (state == AppState::PLAYING && (!loggedSessionState || session.state() != lastLoggedSessionState)) {
            std::cout << "rt-app session state=" << sessionStateName(session.state())
                      << " lap=" << snapshot.lap << " finished=" << snapshot.finished
                      << " money=" << session.progress().money << "\n";
            lastLoggedSessionState = session.state();
            loggedSessionState = true;
        }
        if (state == AppState::PLAYING && directStartRace) {
            session.startRace();
            directStartRace = false;
        }
        if (!noRender && presenter.extent.width && presenter.extent.height) {
            bool retry = false;
            do {
                std::vector<std::uint8_t> pixels;
                bool rendered = false;
                if (state == AppState::PLAYING) {
                    rendered = rt::native::preview::previewRenderScene(
                        renderer, car, snapshot, session.world(), pixels, error,
                        terrainMesh.indices.empty() ? nullptr : &terrainMesh);
                    if (rendered) {
                        std::vector<std::uint8_t> hudPixels;
                        rendered = rt::native::preview::previewRender2D(
                            renderer, rt::native::preview::raceHudQuads(snapshot, session.totalLaps(),
                                                                         static_cast<int>(presenter.extent.width), static_cast<int>(presenter.extent.height)),
                            {0.f, 0.f, 0.f, 0.f}, hudPixels, error);
                        if (rendered) compositeOver(pixels, hudPixels);
                    }
                } else {
                    rendered = rt::native::preview::previewRender2D(renderer, menuQuads(state, menuFlow,
                                                                        static_cast<int>(presenter.extent.width), static_cast<int>(presenter.extent.height)),
                                                                        {.025f, .035f, .075f, 1.f}, pixels, error);
                }
                if (!rendered || !present(presenter, pixels, retry, error)) {
                    std::cerr << "render: " << error << "\n"; running = false; break;
                }
                if (const char* snap = std::getenv("RT_APP_SNAPSHOT")) {
                    std::ofstream snapStream(snap, std::ios::binary);
                    snapStream << "P6\n" << presenter.extent.width << ' ' << presenter.extent.height
                             << "\n255\n";
                    for (std::size_t i = 0; i + 3 < pixels.size(); i += 4) {
                        snapStream.put(static_cast<char>(pixels[i]));
                        snapStream.put(static_cast<char>(pixels[i + 1]));
                        snapStream.put(static_cast<char>(pixels[i + 2]));
                    }
                    std::cout << "rt-app snapshot: " << snap << " (" << presenter.extent.width
                              << 'x' << presenter.extent.height << ")\n";
                }
                if (retry && !recreateSwapchain()) { running = false; break; }
            } while (retry && presenter.extent.width && presenter.extent.height);
        }
        ++frames;
    }
    std::cout << "rt-app frames=" << frames << " simulation_frame=" << simulationFrame << "\n";
    destroyAudio(audio);
    rt::native::preview::previewDestroy(renderer); destroy(presenter); SDL_DestroyWindow(window); SDL_Quit();
    return 0;
}
