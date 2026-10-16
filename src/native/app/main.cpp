// Minimal SDL3 + Vulkan host for the authored GameLoop.  The scene itself is
// rendered by previewRenderScene(), so this presentation layer deliberately
// contains no duplicate road/car/shader code.
#include "assets/field_converter.h"
#include "assets/field_tile_index.h"
#include "assets/glyph_atlas.h"
#include "assets/native_model.h"
#include "assets/nav_routes.h"
#include "audio/bgm_player.h"
#include "audio/engine.h"
#include "audio/vag_loader.h"
#include "gameplay/collectibles.h"
#include "gameplay/dialogue.h"
#include "gameplay/input.h"
#include "gameplay/menu.h"
#include "gameplay/nav_drive.h"
#include "gameplay/session.h"
#include "gameplay/shop.h"
#include "gameplay/tile_streamer.h"
#include "gameplay/world_layout.h"
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
#include <optional>
#include <string>
#include <unordered_map>
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
// tile.  RT_APP_TEXTURES=1 additionally decodes each tile's GS uploads to
// RGBA8 (decodeFieldTextures) and streams per-vertex page-fraction UVs so
// the preview draws with the authentic textures; pages are deduplicated
// across tiles by content.  Absent/malformed input falls back to the
// authored quad (or the banded mesh), never a failure.

// Authentic tile placement (gameplay/world_layout; PROVEN formulas in
// docs/native-reference/world-geography.md): id -> (halfColumn 0..15,
// row 0..7) on the staggered lattice.  Render space is 0.1 game units,
// anchored so Peach Town has its origin at (0,0).  Peach Town is streaming
// id 43 (halfColumn 11, row 5, FLD/223.BIN) per the scripted scene table
// @0x2be438 (docs/native-reference/scene-tile-map.md) — the earlier id-11
// binding was My Garage / My City (same half-column, row 1: the arithmetic
// coincidence that seeded the mis-binding).  The graph-0 overlay is the
// COURSE/C00 race route, not Peach streets; the town street graph (scene
// T=1, outer dir @0x2d98a8) lands with the nav_routes townGraph API.
namespace {
constexpr std::int32_t kPeachHalfColumn = 11;
constexpr std::int32_t kPeachRow = 5;
constexpr int kPeachId = 43;

struct TileRenderOrigin { float x; float z; };
TileRenderOrigin tileRenderOrigin(int id) {
    const auto p = rt::native::gameplay::tilePlacement(id);
    return {static_cast<float>(p.halfColumn - kPeachHalfColumn) * 80.0f,
            static_cast<float>(p.row - kPeachRow) * 160.0f};
}

// Session car position (game units inside the anchored Peach tile, matching
// tileRenderOrigin) -> authentic fixed units on the world torus for the
// position-driven streamer demand.  Odd-row half-tile X shear applied once
// local Z passes half a tile (FUN_0021ba70 stored-position semantics, so
// gridCellId's FUN_0021c8d0 (-b) correction maps back to the same id).
rt::native::gameplay::FixedPos carFixedUnits(const rt::native::gameplay::Vec2& carGame) {
    const auto peach = rt::native::gameplay::tilePlacement(kPeachId);
    const double step = rt::native::gameplay::kFixedUnitsPerGameUnit;
    std::int32_t x = peach.x0 + static_cast<std::int32_t>(carGame.x * step);
    std::int32_t z = peach.z0 + static_cast<std::int32_t>(carGame.z * step);
    if ((peach.row & 1) != 0 &&
        carGame.z * step >= static_cast<double>(rt::native::gameplay::kHalfTileFixedUnits))
        x += rt::native::gameplay::kHalfTileFixedUnits;
    return {x, z};
}
} // namespace

namespace {
std::uint64_t textureContentHash(const rt::native::assets::FieldTexturePage& page) {
    std::uint64_t h = 1469598103934665603ull;
    const auto mix = [&h](const std::uint8_t* b, std::size_t n) {
        for (std::size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; }
    };
    const std::uint32_t dims[2] = {page.width, page.height};
    mix(reinterpret_cast<const std::uint8_t*>(dims), sizeof(dims));
    mix(page.rgba.data(), page.rgba.size());
    return h;
}

// Appends one tile's decoded pages to mesh.textures (deduplicated) and
// rewrites the patch's triangle page ids to the global indices.
void appendTexturedTile(rt::native::preview::TerrainMesh& mesh,
                       const rt::native::assets::FieldTexturedTerrain& tile,
                       float xOffset, float zOffset,
                       std::unordered_map<std::uint64_t, std::uint32_t>& pagesByHash) {
    const std::uint32_t vertexBase = static_cast<std::uint32_t>(mesh.positions.size());
    for (std::size_t i = 0; i < tile.patch.positions.size(); ++i) {
        auto p = tile.patch.positions[i];
        p[0] += xOffset;
        p[2] += zOffset;
        mesh.positions.push_back(p);
        mesh.colours.push_back(tile.patch.colours[i]);
        mesh.uvs.push_back(tile.patch.uvs[i]);
    }
    for (std::uint32_t i : tile.patch.indices) mesh.indices.push_back(vertexBase + i);
    mesh.triangleMaterials.reserve(mesh.triangleMaterials.size() + tile.patch.indices.size() / 3);
    mesh.triangleTexture.reserve(mesh.triangleTexture.size() + tile.patch.triangleTexture.size());
    for (std::size_t t = 0; t < tile.patch.triangleTexture.size(); ++t) {
        const std::int32_t local = tile.patch.triangleTexture[t];
        std::int32_t global = -1;
        if (local >= 0 && static_cast<std::size_t>(local) < tile.patch.pages.size()) {
            const auto& page = tile.patch.pages[static_cast<std::size_t>(local)];
            const auto hash = textureContentHash(page);
            auto it = pagesByHash.find(hash);
            if (it == pagesByHash.end()) {
                global = static_cast<std::int32_t>(mesh.textures.size());
                pagesByHash.emplace(hash, static_cast<std::uint32_t>(global));
                mesh.textures.push_back({page.width, page.height, page.rgba});
            } else {
                global = static_cast<std::int32_t>(it->second);
            }
        }
        mesh.triangleTexture.push_back(global);
        mesh.triangleMaterials.push_back(local < 0 ? 0u : static_cast<std::uint32_t>(local));
    }
}

std::filesystem::path discRoot() {
    if (const char* value = std::getenv("RT_DISC")) return std::filesystem::path(value);
    if (const char* home = std::getenv("HOME"))
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    return {};
}

// RT_APP_STREAM=1 (with RT_APP_TERRAIN=1) streams tiles through four
// hardware-style resident slots instead of the eager 64-tile load: slot
// index = id & 3 (the four 0x3A8000-byte fieldTileData buffers), resident
// set = a 4-column window of one id-grid row, one slot swapped per column
// step (docs/native-reference/field-topology-status.md).  The window follows
// a scripted serpentine sweep because the authentic camera-to-tile selection
// (nav tables, docs/native-reference/nav-tables.md) is not yet consumed.
// Rebuilds the terrain mesh only on swap frames; loads are synchronous (the
// host has no async I/O path yet).
struct StreamedTerrain {
    StreamedTerrain() : streamer(10, 1, 0) {}

    // Advances one host frame; returns true when a slot reloaded and the
    // caller should rebuild the mesh.
    bool refresh() {
        if (!dirty && !streamer.tick()) return false;
        dirty = false;
        bool reloaded = false;
        for (int slot = 0; slot < rt::native::gameplay::kFieldTileSlots; ++slot) {
            const int id = streamer.idInSlot(slot);
            if (slotId[static_cast<std::size_t>(slot)] == id) continue;
            if (loadSlot(slot, id)) reloaded = true;
            std::cout << "rt-app stream: slot " << slot << " <- id " << id << ' '
                      << index[static_cast<std::size_t>(id)].fileName
                      << " (window row " << streamer.row() << " col " << streamer.colStart() << ")\n";
        }
        return reloaded;
    }

    // Position-driven demand (RT_APP_STREAM_POS=1): follows the car with the
    // authentic 4-slot window via TileStreamer::advanceTo (Evict*->Keep*->
    // Load*, slot = id&3, LRU evictions, nearest-first loads). Returns true
    // when any slot changed and the mesh needs a rebuild.
    bool refreshAt(std::int32_t globalX, std::int32_t globalZ) {
        dirty = false;
        bool changed = false;
        for (const auto& cmd : streamer.advanceTo(globalX, globalZ)) {
            switch (cmd.action) {
                case rt::native::gameplay::TileStreamCommand::Action::Evict:
                    slotId[static_cast<std::size_t>(cmd.slot)] = -1;
                    slotBytes[static_cast<std::size_t>(cmd.slot)] = {};
                    slotTexturedValid[static_cast<std::size_t>(cmd.slot)] = false;
                    changed = true;
                    break;
                case rt::native::gameplay::TileStreamCommand::Action::Load:
                    if (loadSlot(cmd.slot, cmd.id)) {
                        changed = true;
                        std::cout << "rt-app stream: demand slot " << cmd.slot << " <- id "
                                  << cmd.id << ' ' << index[static_cast<std::size_t>(cmd.id)].fileName
                                  << " (demanded id " << streamer.lastDemandedId() << ")\n";
                    }
                    break;
                case rt::native::gameplay::TileStreamCommand::Action::Keep:
                    break;
            }
        }
        return changed;
    }

  private:
    bool loadSlot(int slot, int id) {
        const std::string& name = index[static_cast<std::size_t>(id)].fileName;
        const std::size_t slotIndex = static_cast<std::size_t>(slot);
        if (name.empty()) return false;
        std::ifstream input(root / "FLD" / name, std::ios::binary);
        if (!input) return false;
        slotBytes[slotIndex] =
            std::vector<std::uint8_t>((std::istreambuf_iterator<char>(input)),
                                     std::istreambuf_iterator<char>());
        slotId[slotIndex] = id;
        slotTexturedValid[slotIndex] = false;
        if (wantTextures) {
            auto textured = rt::native::assets::buildFieldTexturedTerrain(slotBytes[slotIndex], 0.1f);
            if (textured) {
                slotTextured[slotIndex] = std::move(*textured);
                slotTexturedValid[slotIndex] = true;
            } else {
                std::cout << "rt-app stream: textured build failed for " << name << "\n";
            }
        }
        ++loads;
        return true;
    }

  public:

    // Rebuilds the render mesh from the four resident slots with the same
    // staggered-grid placement as the eager loader (x step 80, z step 160).
    void buildMesh(rt::native::preview::TerrainMesh& mesh) const {
        mesh = rt::native::preview::TerrainMesh{};
        std::unordered_map<std::uint64_t, std::uint32_t> pagesByHash;
        for (int slot = 0; slot < rt::native::gameplay::kFieldTileSlots; ++slot) {
            const int id = slotId[static_cast<std::size_t>(slot)];
            if (id < 0) continue;
            const auto origin = tileRenderOrigin(id);
            const float xOffset = origin.x;
            const float zOffset = origin.z;
            const std::size_t slotIndex = static_cast<std::size_t>(slot);
            if (wantTextures) {
                if (slotTexturedValid[slotIndex])
                    appendTexturedTile(mesh, slotTextured[slotIndex], xOffset, zOffset, pagesByHash);
                continue;
            }
            const auto catalog = rt::native::assets::catalogFieldTile(slotBytes[slotIndex]);
            if (!catalog) continue;
            auto terrain = rt::native::assets::buildFieldRenderTerrain(slotBytes[slotIndex], *catalog, 0.1f);
            const std::uint32_t vertexBase = static_cast<std::uint32_t>(mesh.positions.size());
            for (auto& p : terrain.positions) {
                p[0] += xOffset;
                p[2] += zOffset;
                mesh.positions.push_back(p);
            }
            for (std::uint32_t i : terrain.indices) mesh.indices.push_back(vertexBase + i);
            mesh.triangleMaterials.insert(mesh.triangleMaterials.end(),
                                         terrain.triangleMaterials.begin(),
                                         terrain.triangleMaterials.end());
        }
    }

    rt::native::gameplay::TileStreamer streamer;
    rt::native::assets::FieldTileIndex index{};
    std::filesystem::path root;
    bool wantTextures = false;
    bool dirty = true;
    std::size_t loads = 0;
    std::array<int, 4> slotId{{-1, -1, -1, -1}};
    std::array<std::vector<std::uint8_t>, 4> slotBytes;
    std::array<rt::native::assets::FieldTexturedTerrain, 4> slotTextured;
    std::array<bool, 4> slotTexturedValid{{false, false, false, false}};
};
} // namespace

rt::native::preview::TerrainMesh loadTerrainMesh() {
    rt::native::preview::TerrainMesh mesh;
    if (std::getenv("RT_APP_TERRAIN") == nullptr) return mesh;
    const bool wantTextures = std::getenv("RT_APP_TEXTURES") != nullptr;
    std::unordered_map<std::uint64_t, std::uint32_t> pagesByHash;
    const std::filesystem::path root = discRoot();
    const char* named = std::getenv("RT_APP_TERRAIN_FILE");
    if (named != nullptr) {
        const std::filesystem::path path{named};
        std::ifstream input(path, std::ios::binary);
        if (!input) { std::cout << "rt-app terrain: SKIP (no tile at " << path << ")\n"; return mesh; }
        const std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)),
                                             std::istreambuf_iterator<char>());
        if (wantTextures) {
            auto textured = rt::native::assets::buildFieldTexturedTerrain(bytes, 0.1f);
            if (!textured) {
                std::cout << "rt-app terrain: textured build failed for " << path.filename().string()
                        << " (err=" << static_cast<int>(textured.error()) << ")\n";
                return mesh;
            }
            appendTexturedTile(mesh, *textured, 0.f, 0.f, pagesByHash);
            std::cout << "rt-app terrain: loaded " << path.filename().string()
                      << " triangles=" << mesh.indices.size() / 3
                      << " pages=" << mesh.textures.size()
                      << " clut hits/alias/miss=" << textured->clutHits << '/'
                      << textured->clutAliasHits << '/' << textured->clutMisses
                      << " unresolved=" << textured->unresolvedTriangles << "\n";
            return mesh;
        }
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
    // + size matching (field_tile_index), place each tile at its authentic
    // lattice origin (halfColumn/row from world_layout, x step 80 render units
    // = half-tile stagger, z step 160), anchored on Peach Town.
    const auto index = rt::native::assets::buildFieldTileIndex(root);
    if (!index) {
        std::cout << "rt-app terrain: SKIP (no tile index; err="
                  << static_cast<int>(index.error()) << ")\n";
        return mesh;
    }
    std::size_t tilesLoaded = 0;
    unsigned long long clutHits = 0, clutAlias = 0, clutMiss = 0, unresolved = 0;
    for (std::size_t id = 0; id < rt::native::assets::kFieldTileCount; ++id) {
        const std::string& name = (*index)[id].fileName;
        if (name.empty()) continue;
        std::ifstream input(root / "FLD" / name, std::ios::binary);
        if (!input) continue;
        const std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)),
                                             std::istreambuf_iterator<char>());
        // Authentic lattice origin, anchored so Peach Town (id 43) starts at
        // (0,0) and the spawn car sits on Peach terrain.
        const auto origin = tileRenderOrigin(static_cast<int>(id));
        const float xOffset = origin.x;
        const float zOffset = origin.z;
        if (wantTextures) {
            auto textured = rt::native::assets::buildFieldTexturedTerrain(bytes, 0.1f);
            if (!textured) continue;
            appendTexturedTile(mesh, *textured, xOffset, zOffset, pagesByHash);
            clutHits += textured->clutHits;
            clutAlias += textured->clutAliasHits;
            clutMiss += textured->clutMisses;
            unresolved += textured->unresolvedTriangles;
            ++tilesLoaded;
            continue;
        }
        const auto catalog = rt::native::assets::catalogFieldTile(bytes);
        if (!catalog) continue;
        auto terrain = rt::native::assets::buildFieldRenderTerrain(bytes, *catalog, 0.1f);
        const std::uint32_t vertexBase = static_cast<std::uint32_t>(mesh.positions.size());
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
              << " triangles=" << mesh.indices.size() / 3;
    if (wantTextures) {
        std::cout << " pages=" << mesh.textures.size()
                  << " clut hits/alias/miss=" << clutHits << '/' << clutAlias << '/' << clutMiss
                  << " unresolved=" << unresolved;
    }
    std::cout << "\n";
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
    // RT_APP_STREAM=1 (with RT_APP_TERRAIN=1) replaces the eager 64-tile
    // load with hardware-style 4-slot streaming (slot = id & 3).
    rt::native::preview::TerrainMesh terrainMesh;
    std::optional<StreamedTerrain> streamed;
    if (std::getenv("RT_APP_STREAM") != nullptr && std::getenv("RT_APP_TERRAIN") != nullptr) {
        if (std::getenv("RT_APP_TERRAIN_FILE") != nullptr)
            std::cout << "rt-app stream: RT_APP_TERRAIN_FILE ignored under stream mode\n";
        auto index = rt::native::assets::buildFieldTileIndex(discRoot());
        if (!index) {
            std::cout << "rt-app stream: SKIP (no tile index; err="
                      << static_cast<int>(index.error()) << ")\n";
        } else {
            StreamedTerrain st;
            st.root = discRoot();
            st.index = *index;
            st.wantTextures = std::getenv("RT_APP_TEXTURES") != nullptr;
            const char* stepValue = std::getenv("RT_APP_STREAM_STEP_FRAMES");
            const char* rowValue = std::getenv("RT_APP_STREAM_ROW");
            const int step = stepValue ? std::atoi(stepValue) : 10;
            const int startRow = rowValue ? std::atoi(rowValue) : 1;
            st.streamer = rt::native::gameplay::TileStreamer(step, startRow, 0);
            streamed = std::move(st);
            std::cout << "rt-app stream: 4-slot streaming textured=" << (streamed->wantTextures ? 1 : 0)
                      << " step=" << step << " startRow=" << startRow << "\n";
        }
    } else {
        terrainMesh = loadTerrainMesh();
    }
    // RT_APP_NAV=1 overlays a ROM nav-graph's centerline segments (static
    // graph 0 = the COURSE/C00 race-route pack graph, per
    // docs/native-reference/scene-tile-map.md — not Peach streets) as bright
    // yellow strips.  RT_APP_NAV_TOWN=1 instead draws the AUTHENTIC Peach Town
    // street graph (scene T=1, outer dir @0x2d98a8 -> car0 pair
    // 0x2c5ff0/0x2c67d0, 84 segments) — tile-local [0,1600] game units, so
    // scaled 0.1 both overlays land inside the anchored Peach tile at (0,0).
    std::vector<rt::native::preview::RouteLine> navLines;
    if (std::getenv("RT_APP_NAV") != nullptr) {
        auto tables = rt::native::assets::NavRouteTables::open(discRoot() / "SLUS_203.98");
        if (!tables) {
            std::cout << "rt-app nav: SKIP (ELF unavailable, err=" << static_cast<int>(tables.error()) << ")\n";
        } else {
            const bool navTown = std::getenv("RT_APP_NAV_TOWN") != nullptr;
            auto graph = navTown ? tables->townGraph(1) : tables->staticGraph(0);
            if (!graph) {
                std::cout << "rt-app nav: SKIP (" << (navTown ? "town graph 1 absent" : "static graph 0 absent")
                          << ")\n";
            } else {
                for (const auto& seg : graph->segments)
                    navLines.push_back({seg.ax * .1f, seg.az * .1f, seg.bx * .1f, seg.bz * .1f,
                                       1.f, .85f, .15f});
                if (navTown)
                    std::cout << "rt-app nav: town graph 1 segments=" << navLines.size()
                              << " vaddr=0x" << std::hex << graph->segmentVaddr << std::dec << "\n";
                else
                    std::cout << "rt-app nav: static graph 0 segments=" << navLines.size()
                              << " vaddr=0x" << std::hex << graph->segmentVaddr << std::dec << "\n";
            }
        }
    }
    // RT_APP_ROUTE=1 drives the render-visible demo car along the static
    // graph 0 centerline (COURSE/C00 race route per scene-tile-map.md) with
    // a deterministic arc-length follower (gameplay/nav_drive).  Session
    // authoring keeps its own car state; only the snapshot handed to the
    // scene renderer is overridden.
    std::optional<rt::native::gameplay::NavDrive> navDrive;
    constexpr float kNavRouteSpeedUnitsPerFrame = 30.f;
    int navRouteLap = 0;
    if (std::getenv("RT_APP_ROUTE") != nullptr) {
        auto tables = rt::native::assets::NavRouteTables::open(discRoot() / "SLUS_203.98");
        if (!tables) {
            std::cout << "rt-app route: SKIP (ELF unavailable, err=" << static_cast<int>(tables.error()) << ")\n";
        } else {
            const bool routeTown = std::getenv("RT_APP_ROUTE_TOWN") != nullptr;
            auto graph = routeTown ? tables->townGraph(1) : tables->staticGraph(0);
            if (!graph) {
                std::cout << "rt-app route: SKIP (" << (routeTown ? "town graph 1 absent" : "static graph 0 absent")
                          << ")\n";
            } else {
                const rt::native::gameplay::NavPath path = rt::native::gameplay::buildNavPath(*graph);
                if (path.centerline.size() < 2) {
                    std::cout << "rt-app route: SKIP (centerline has no segments)\n";
                } else {
                    navDrive.emplace(path);
                    std::cout << "rt-app route: " << (routeTown ? "town" : "nav")
                              << " lap drive segments="
                              << navDrive->segmentCount()
                              << " length=" << navDrive->totalLength() << "\n";
                }
            }
        }
    }
    // RT_APP_HUDTEXT=1 presents HUD text through the textured pass: decode
    // the shipped FONTE glyph atlas once (SYS/FONTE.GSL) and place race
    // lap/time + clock strings at the ROM-proven elements
    // (docs/native-reference/hud-layout.md; glyph grid per
    // docs/native-reference/sys-gsl-packs.md §4 corrected 2026-10-16).
    bool hudTextEnabled = std::getenv("RT_APP_HUDTEXT") != nullptr;
    bool hudTextWarned = false;
    rt::native::assets::GlyphAtlas fontAtlas;
    if (hudTextEnabled) {
        std::ifstream input(discRoot() / "SYS" / "FONTE.GSL", std::ios::binary);
        if (!input) {
            std::cout << "rt-app hudtext: SKIP (FONTE.GSL missing)\n";
            hudTextEnabled = false;
        } else {
            std::vector<std::uint8_t> gsl((std::istreambuf_iterator<char>(input)),
                                         std::istreambuf_iterator<char>());
            auto decoded = rt::native::assets::decodeGlyphAtlas(gsl);
            if (decoded.value.width == 0) {
                std::cout << "rt-app hudtext: SKIP (decode err=" << static_cast<int>(decoded.err)
                          << " offset=" << decoded.errorOffset << ")\n";
                hudTextEnabled = false;
            } else {
                fontAtlas = std::move(decoded.value);
                unsigned glyphCount = 0;
                for (bool occupied : fontAtlas.slotNonEmpty) glyphCount += occupied ? 1u : 0u;
                std::cout << "rt-app hudtext: FONTE decoded " << fontAtlas.width << "x"
                          << fontAtlas.height << " non-empty slots=" << glyphCount << "\n";
            }
        }
    }
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
            if (navDrive) {
                // Advance the render-only follower once per authored frame and
                // announce each completed closed lap.
                const rt::native::gameplay::NavPose pose = navDrive->step(kNavRouteSpeedUnitsPerFrame);
                if (pose.lap > navRouteLap) {
                    navRouteLap = pose.lap;
                    std::cout << "rt-app route: lap " << pose.lap << " complete\n";
                }
            }

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
        if (streamed) {
            // RT_APP_STREAM_POS=1: authentic position-driven demand (the car's
            // fixed-unit world position drives the 4-slot window); default
            // keeps the scripted serpentine sweep for the deterministic matrix.
            const bool changed = std::getenv("RT_APP_STREAM_POS") != nullptr
                ? streamed->refreshAt(carFixedUnits(snapshot.carPosition).x,
                                     carFixedUnits(snapshot.carPosition).z)
                : streamed->refresh();
            if (changed) streamed->buildMesh(terrainMesh);
        }
        if (!noRender && presenter.extent.width && presenter.extent.height) {
            bool retry = false;
            // Render-local copy so the route follower can move the visible car
            // without disturbing session authoring state (lap/collectible logic
            // above still reads the authored snapshot).
            rt::native::gameplay::FrameSnapshot renderSnapshot = snapshot;
            if (navDrive) {
                const rt::native::gameplay::NavPose pose = navDrive->pose();
                // carPosition is consumed in game units (preview scales .1).
                renderSnapshot.carPosition = {pose.position.x, pose.position.z};
                renderSnapshot.heading = pose.heading;
            }
            do {
                std::vector<std::uint8_t> pixels;
                bool rendered = false;
                if (state == AppState::PLAYING) {
                    rendered = rt::native::preview::previewRenderScene(
                        renderer, car, renderSnapshot, session.world(), pixels, error,
                        terrainMesh.indices.empty() ? nullptr : &terrainMesh,
                        navLines.empty() ? nullptr : &navLines);
                    if (rendered) {
                        std::vector<std::uint8_t> hudPixels;
                        rendered = rt::native::preview::previewRender2D(
                            renderer, rt::native::preview::raceHudQuads(snapshot, session.totalLaps(),
                                                                         static_cast<int>(presenter.extent.width), static_cast<int>(presenter.extent.height)),
                            {0.f, 0.f, 0.f, 0.f}, hudPixels, error);
                        if (rendered) compositeOver(pixels, hudPixels);
                        if (hudTextEnabled && rendered) {
                            // Idempotent per renderer: re-uploads only after a
                            // renderer recreate (resize path), no-op otherwise.
                            const rt::native::preview::PreviewTexture atlasTex{
                                fontAtlas.width, fontAtlas.height, fontAtlas.pixels};
                            std::string texError;
                            if (rt::native::preview::previewUploadFontAtlas(renderer, atlasTex, texError)) {
                                const int w = static_cast<int>(presenter.extent.width);
                                const int h = static_cast<int>(presenter.extent.height);
                                std::vector<rt::native::preview::HudQuad> textQuads =
                                    session.state() == rt::native::gameplay::SessionState::RACE
                                        ? rt::native::preview::raceHudTextQuads(snapshot,
                                                                               session.totalLaps(), w, h)
                                        : rt::native::preview::townClockTextQuads(snapshot.clock, w, h);
                                std::vector<std::uint8_t> textPixels;
                                if (!textQuads.empty() &&
                                    rt::native::preview::previewRender2DTextured(
                                        renderer, textQuads, {0.f, 0.f, 0.f, 0.f}, textPixels, error))
                                    compositeOver(pixels, textPixels);
                            } else if (!hudTextWarned) {
                                hudTextWarned = true;
                                std::cout << "rt-app hudtext: upload failed: " << texError << "\n";
                            }
                        }
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
    if (streamed) {
        std::cout << "rt-app stream: loads=" << streamed->loads
                  << " final window row=" << streamed->streamer.row()
                  << " col=" << streamed->streamer.colStart() << "\n";
    }
    destroyAudio(audio);
    rt::native::preview::previewDestroy(renderer); destroy(presenter); SDL_DestroyWindow(window); SDL_Quit();
    return 0;
}
