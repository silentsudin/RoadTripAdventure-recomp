// Game-specific runtime hooks for Road Trip (USA), SLUS-20398 v1.02.
// Bind addresses to runtime handlers here as boot issues are triaged, e.g.
//   ps2_game_overrides::bindAddressHandler(runtime, 0x00123456, "ret0");

#include "game_overrides.h"
#include "ps2_runtime.h"
#include "ps2_runtime_macros.h"

#include <cstring>
#include <iostream>

namespace
{
    // ---------------------------------------------------------------- widescreen camera
    // 0x21F698(a0 = camera, a1 = params, a2 = view) builds a 3D camera: the screen matrix at
    // +0x00 (projection x view, screen X = focal * x / z + cx), the clip matrices at +0x40 and
    // +0xC0 (x, y scaled by focal / 512) and the viewport at +0x80 (cx at +0xB0). VU1 reads them
    // from the camera by reference, so narrowing their X after the build widens the field of view
    // for everything drawn with that camera; the presenter then stretches the 640-wide frame to
    // the wide window. Only the two player cameras are touched (HUD and menu 3D use others). The
    // EE culls ground tiles against frustum points it builds from the table at 0x29CF40 inside the
    // call, so those are widened while widescreen driving is on.
    constexpr uint32_t kBuildCamera = 0x0021F698u;
    constexpr uint32_t kPlayerCameras[2] = {0x0177E6D0u, 0x0177E860u};
    constexpr uint32_t kBuildCameraEnd = 0x0021F920u; // the next function (the HUD camera builder)
    constexpr uint32_t kFrustumTable = 0x0029CF40u; // 7 x vec4 (x px, y px, depth)

    PS2Runtime::RecompiledFunction functionAt(uint32_t address)
    {
        const uint32_t slot = (address - g_ps2RecompiledFunctionTableBase) / 4u;
        return slot < g_ps2RecompiledFunctionTableSlotCount ? g_ps2RecompiledFunctionTable[slot] : nullptr;
    }

    PS2Runtime::RecompiledFunction g_buildCamera = nullptr;

    // A build in progress. The builder can pause at a scheduler checkpoint inside one of its calls
    // and resume later at the address after that call (its resume points are wrapped too), so the
    // camera is narrowed only once the build has returned to its caller.
    struct Build
    {
        bool active = false;
        uint32_t cam = 0, ra = 0;
        float k = 1.0f;
    } g_build;
    float g_frustum[7];
    bool g_haveFrustum = false;
    float g_frustumK = 1.0f; // the scale the table in guest memory is widened by

    float loadF(const uint8_t *rdram, uint32_t addr)
    {
        float f;
        std::memcpy(&f, rdram + (addr & PS2_RAM_MASK), 4);
        return f;
    }

    void storeF(uint8_t *rdram, uint32_t addr, float f) { std::memcpy(rdram + (addr & PS2_RAM_MASK), &f, 4); }

    // The tile-cull frustum points stay widened while driving in widescreen (only the camera
    // builder reads them).
    void setFrustumScale(uint8_t *rdram, float k)
    {
        if (!g_haveFrustum)
        {
            for (int i = 0; i < 7; ++i)
                g_frustum[i] = loadF(rdram, kFrustumTable + i * 16);
            g_haveFrustum = true;
        }
        if (k == g_frustumK)
            return;
        for (int i = 0; i < 7; ++i)
            storeF(rdram, kFrustumTable + i * 16, g_frustum[i] / k);
        g_frustumK = k;
    }

    void narrowCamera(uint8_t *rdram, uint32_t cam, float k)
    {
        const float cx = loadF(rdram, cam + 0xB0);
        for (uint32_t j = 0; j < 4; ++j)
        {
            const uint32_t col = cam + 16 * j;
            const float x = loadF(rdram, col), w = loadF(rdram, col + 0xC);
            storeF(rdram, col, cx * w + k * (x - cx * w)); // narrow around the screen centre
            storeF(rdram, cam + 0x40 + 16 * j, loadF(rdram, cam + 0x40 + 16 * j) * k);
            storeF(rdram, cam + 0xC0 + 16 * j, loadF(rdram, cam + 0xC0 + 16 * j) * k);
        }
    }

    void finishIfDone(uint8_t *rdram, R5900Context *ctx)
    {
        if (g_build.active && ctx->pc == g_build.ra)
        {
            narrowCamera(rdram, g_build.cam, g_build.k);
            g_build.active = false;
        }
    }

    void buildCameraWide(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t cam = GPR_U32(ctx, 4);
        const bool player = cam == kPlayerCameras[0] || cam == kPlayerCameras[1];
        float k = 1.0f;
        if (player)
        {
            GS &gs = runtime->gsUnsynced();
            if (gs.wideDriving())
                k = gs.wideHorizontalScale();
        }
        setFrustumScale(rdram, k);
        g_build = {k < 0.999f, cam, GPR_U32(ctx, 31), k};
        g_buildCamera(rdram, ctx, runtime);
        finishIfDone(rdram, ctx);
    }

    // The builder resuming after a pause (entered at one of its internal call return addresses).
    void resumeCameraWide(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        g_buildCamera(rdram, ctx, runtime);
        finishIfDone(rdram, ctx);
    }

    void applyRoadTrip(PS2Runtime &runtime)
    {
        std::cout << "[roadtrip] applying SLUS-20398 overrides\n";
        g_buildCamera = runtime.lookupFunction(kBuildCamera);
        if (g_buildCamera)
        {
            runtime.replaceFunction(kBuildCamera, buildCameraWide);
            // Its resume points: table slots inside the function that enter the same code.
            int resumes = 0;
            for (uint32_t a = kBuildCamera + 4; a < kBuildCameraEnd; a += 4)
                if (runtime.hasFunction(a) && functionAt(a) == g_buildCamera)
                {
                    runtime.replaceFunction(a, resumeCameraWide);
                    ++resumes;
                }
            std::cout << "[roadtrip] widescreen camera hook (" << resumes << " resume points)\n";
        }
        else
            std::cerr << "[roadtrip] camera builder 0x21F698 not found: widescreen 3D unavailable\n";
    }
}

PS2_REGISTER_GAME_OVERRIDE("Road Trip (USA)", "SLUS_203.98", 0x00200008u, 0x5A49851Du, applyRoadTrip)
