// FSR 3 upscaler plugin for Road Trip Recomp (Windows, Vulkan): AMD FidelityFX SDK 1.1.x "ffx-api"
// (MIT), through the signed amd_fidelityfx_vk.dll that sits next to this DLL. The interface is
// runtime/gs/rt_upscaler_api.h; the conventions of the inputs are described there.

#define RTU_EXPORT __declspec(dllexport)
#include "runtime/gs/rt_upscaler_api.h"

#include <vulkan/vulkan.h>

#include "ffx_api/ffx_api.h"
#include "ffx_api/ffx_api_types.h"
#include "ffx_api/ffx_upscale.h"
#include "ffx_api/vk/ffx_api_vk.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <cfloat>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace
{
    PfnFfxCreateContext g_create = nullptr;
    PfnFfxDestroyContext g_destroy = nullptr;
    PfnFfxConfigure g_configure = nullptr;
    PfnFfxDispatch g_dispatch = nullptr;
    PfnFfxQuery g_query = nullptr;
    HMODULE g_ffx = nullptr;
    RtuInit g_init = {};
    std::string g_description = "AMD FSR 3 (FidelityFX ffx-api)";

    struct Context
    {
        ffxContext handle = nullptr;
        uint32_t displayW = 0, displayH = 0;
    };

    void message(uint32_t type, const wchar_t *text)
    {
        std::fprintf(stderr, "[fsr3] %s: %ls\n", type == FFX_API_MESSAGE_TYPE_ERROR ? "error" : "warning", text);
    }

    // The directory this DLL was loaded from (the signed FidelityFX DLL is beside it).
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

    FfxApiResource resource(const RtuImage &image, uint32_t format, uint32_t usage)
    {
        FfxApiResourceDescription d = {};
        d.type = FFX_API_RESOURCE_TYPE_TEXTURE2D;
        d.format = format;
        d.width = image.width;
        d.height = image.height;
        d.depth = 1;
        d.mipCount = 1;
        d.flags = FFX_API_RESOURCE_FLAGS_NONE;
        d.usage = usage;
        return ffxApiGetResourceVK(reinterpret_cast<void *>(image.image), d, FFX_API_RESOURCE_STATE_PIXEL_COMPUTE_READ);
    }

    int instanceExtensions(const char *const **names, uint32_t *count)
    {
        *names = nullptr;
        *count = 0;
        return 0;
    }

    int deviceExtensions(void *, void *, void *, const char *const **names, uint32_t *count)
    {
        // The Vulkan backend loads vkGetBufferMemoryRequirements2KHR by that name, which the loader
        // only resolves with the extension enabled (core in 1.1, but the alias needs it); and takes
        // dedicated allocations when the driver has VK_KHR_dedicated_allocation. The host keeps the
        // ones this GPU has.
        static const char *const wanted[] = {"VK_KHR_get_memory_requirements2", "VK_KHR_dedicated_allocation"};
        *names = wanted;
        *count = sizeof(wanted) / sizeof(wanted[0]);
        return 0;
    }

    // RT_UPSCALER_DEBUG=1: say where a crash happened (the module and offset of the faulting
    // address and of the return address on the stack), since the library has no symbols for us.
    LONG CALLBACK crashReport(EXCEPTION_POINTERS *e)
    {
        if (e->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
            return EXCEPTION_CONTINUE_SEARCH;
        auto where = [](const void *address) {
            HMODULE module = nullptr;
            char name[MAX_PATH] = "?";
            if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   static_cast<LPCSTR>(address), &module))
                GetModuleFileNameA(module, name, sizeof(name));
            return std::string(name) + "+0x" + [&] {
                char buf[32];
                std::snprintf(buf, sizeof(buf), "%llx", static_cast<unsigned long long>(
                    reinterpret_cast<uintptr_t>(address) - reinterpret_cast<uintptr_t>(module)));
                return std::string(buf);
            }();
        };
        const CONTEXT *c = e->ContextRecord;
        std::fprintf(stderr, "[fsr3] access violation: rip=%s\n", where(reinterpret_cast<void *>(c->Rip)).c_str());
        const void *const *stack = reinterpret_cast<const void *const *>(c->Rsp);
        for (int i = 0; i < 6; ++i)
            std::fprintf(stderr, "[fsr3]   [rsp+%d] %s\n", i * 8, where(stack[i]).c_str());
        std::fflush(stderr);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    uint32_t init(const RtuInit *in)
    {
        if (const char *dbg = std::getenv("RT_UPSCALER_DEBUG"); dbg && *dbg == '1')
            AddVectoredExceptionHandler(1, crashReport);
        g_init = *in;
        if (!g_ffx)
        {
            const std::wstring dll = moduleDir() + L"\\amd_fidelityfx_vk.dll";
            g_ffx = LoadLibraryExW(dll.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
            if (!g_ffx)
            {
                std::fprintf(stderr, "[fsr3] amd_fidelityfx_vk.dll not found next to the plugin (error %lu)\n", GetLastError());
                return 0;
            }
            auto sym = [](const char *name) { return reinterpret_cast<void *>(GetProcAddress(g_ffx, name)); };
            g_create = reinterpret_cast<PfnFfxCreateContext>(sym("ffxCreateContext"));
            g_destroy = reinterpret_cast<PfnFfxDestroyContext>(sym("ffxDestroyContext"));
            g_configure = reinterpret_cast<PfnFfxConfigure>(sym("ffxConfigure"));
            g_dispatch = reinterpret_cast<PfnFfxDispatch>(sym("ffxDispatch"));
            g_query = reinterpret_cast<PfnFfxQuery>(sym("ffxQuery"));
            if (!g_create || !g_destroy || !g_dispatch)
            {
                std::fprintf(stderr, "[fsr3] amd_fidelityfx_vk.dll lacks the ffx-api entry points\n");
                FreeLibrary(g_ffx);
                g_ffx = nullptr;
                return 0;
            }
            if (g_configure)
            {
                ffxConfigureDescGlobalDebug1 dbg = {};
                dbg.header.type = FFX_API_CONFIGURE_DESC_TYPE_GLOBALDEBUG1;
                dbg.fpMessage = message;
                dbg.debugLevel = FFX_API_CONFIGURE_GLOBALDEBUG_LEVEL_WARNINGS;
                g_configure(nullptr, &dbg.header);
            }
        }
        return RTU_KIND_FSR3;
    }

    float maxScale(uint32_t) { return 3.0f; } // Ultra Performance, FSR 3's widest ratio

    void *create(const RtuCreateInfo *info)
    {
        if (info->kind != RTU_KIND_FSR3 || !g_create)
            return nullptr;
        ffxCreateBackendVKDesc backend = {};
        backend.header.type = FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_VK;
        backend.vkDevice = static_cast<VkDevice>(g_init.device);
        backend.vkPhysicalDevice = static_cast<VkPhysicalDevice>(g_init.physical_device);
        backend.vkDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(g_init.get_device_proc_addr);

        ffxCreateContextDescUpscale up = {};
        up.header.type = FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE;
        up.header.pNext = &backend.header;
        // Inputs as described in rt_upscaler_api.h: reverse Z with an infinite far plane, motion with
        // the camera's jitter in it (the library cancels it), LDR colour.
        up.flags = FFX_UPSCALE_ENABLE_DEPTH_INVERTED | FFX_UPSCALE_ENABLE_DEPTH_INFINITE |
                   FFX_UPSCALE_ENABLE_MOTION_VECTORS_JITTER_CANCELLATION;
        if (const char *dbg = std::getenv("RT_UPSCALER_DEBUG"); dbg && *dbg == '1')
            up.flags |= FFX_UPSCALE_ENABLE_DEBUG_CHECKING;
        up.maxRenderSize = {info->render_width, info->render_height};
        up.maxUpscaleSize = {info->display_width, info->display_height};
        up.fpMessage = message;

        auto *c = new Context();
        c->displayW = info->display_width;
        c->displayH = info->display_height;
        const ffxReturnCode_t rc = g_create(&c->handle, &up.header, nullptr);
        if (rc != FFX_API_RETURN_OK)
        {
            std::fprintf(stderr, "[fsr3] ffxCreateContext failed (%u) at %ux%u -> %ux%u\n", rc, info->render_width,
                         info->render_height, info->display_width, info->display_height);
            delete c;
            return nullptr;
        }
        if (g_query)
        {
            ffxQueryGetProviderVersion version = {};
            version.header.type = FFX_API_QUERY_DESC_TYPE_GET_PROVIDER_VERSION;
            if (g_query(&c->handle, &version.header) == FFX_API_RETURN_OK && version.versionName)
                g_description = std::string("AMD FSR 3, ") + version.versionName;
        }
        return c;
    }

    int dispatch(void *context, const RtuDispatch *in)
    {
        auto *c = static_cast<Context *>(context);
        ffxDispatchDescUpscale d = {};
        d.header.type = FFX_API_DISPATCH_DESC_TYPE_UPSCALE;
        d.commandList = in->command_buffer;
        d.color = resource(in->color, FFX_API_SURFACE_FORMAT_R8G8B8A8_UNORM, FFX_API_RESOURCE_USAGE_READ_ONLY);
        d.depth = resource(in->depth, FFX_API_SURFACE_FORMAT_R32_FLOAT, FFX_API_RESOURCE_USAGE_READ_ONLY);
        d.motionVectors = resource(in->motion, FFX_API_SURFACE_FORMAT_R16G16_FLOAT, FFX_API_RESOURCE_USAGE_READ_ONLY);
        d.output = resource(in->output, FFX_API_SURFACE_FORMAT_R8G8B8A8_UNORM,
                            FFX_API_RESOURCE_USAGE_RENDERTARGET | FFX_API_RESOURCE_USAGE_UAV);
        d.jitterOffset = {in->jitter_x, in->jitter_y};
        d.motionVectorScale = {in->mv_scale_x, in->mv_scale_y};
        d.renderSize = {in->render_width, in->render_height};
        d.upscaleSize = {c->displayW, c->displayH};
        d.enableSharpening = in->sharpness > 0.0f;
        d.sharpness = in->sharpness;
        d.frameTimeDelta = in->frame_time_ms;
        d.preExposure = 1.0f;
        d.reset = in->reset != 0;
        // Reverse Z with an infinite far plane: the library wants near = FLT_MAX and far = the real
        // near plane's distance (see its debug messages about cameraNear / cameraFar).
        d.cameraNear = FLT_MAX;
        d.cameraFar = in->camera_near;
        d.cameraFovAngleVertical = in->camera_fov_vertical;
        d.viewSpaceToMetersFactor = 1.0f;
        const ffxReturnCode_t rc = g_dispatch(&c->handle, &d.header);
        if (rc != FFX_API_RETURN_OK)
            std::fprintf(stderr, "[fsr3] ffxDispatch failed (%u)\n", rc);
        return rc == FFX_API_RETURN_OK ? 0 : 1;
    }

    void destroy(void *context)
    {
        auto *c = static_cast<Context *>(context);
        if (c && c->handle && g_destroy)
            g_destroy(&c->handle, nullptr);
        delete c;
    }

    void shutdown() {}

    const char *describe(uint32_t) { return g_description.c_str(); }

    const RtuApi g_api = {RTU_API_VERSION, instanceExtensions, deviceExtensions, init, maxScale, create, dispatch,
                          destroy, shutdown, describe};
}

extern "C" RTU_EXPORT const RtuApi *rtu_get_api(uint32_t version)
{
    return version == RTU_API_VERSION ? &g_api : nullptr;
}
