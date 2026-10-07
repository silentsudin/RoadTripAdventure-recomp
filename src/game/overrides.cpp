// Game-specific runtime hooks for Road Trip (USA), SLUS-20398 v1.02.
// Bind addresses to runtime handlers here as boot issues are triaged, e.g.
//   ps2_game_overrides::bindAddressHandler(runtime, 0x00123456, "ret0");

#include "game/GameOptions.h"
#include "game/GameStats.h"
#include "states/StateSlots.h"
#include "game_overrides.h"
#include "ps2_runtime.h"
#include "ps2_runtime_macros.h"
#include "ps2x/state_archive.h"
#include "runtime/ps2_save_state.h"

#include <cstdlib>
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
        bool jitter = false;
        float jx = 0.0f, jy = 0.0f; // sub-pixel camera jitter for temporal AA, frame-buffer pixels
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

    // Shifts everything the camera draws by (jx, jy) screen pixels: the viewport's centre (used by
    // the microprogram's clip path) and the screen matrix (x += jx * w).
    void jitterCamera(uint8_t *rdram, uint32_t cam, float jx, float jy)
    {
        storeF(rdram, cam + 0xB0, loadF(rdram, cam + 0xB0) + jx);
        storeF(rdram, cam + 0xB4, loadF(rdram, cam + 0xB4) + jy);
        for (uint32_t j = 0; j < 4; ++j)
        {
            const uint32_t col = cam + 16 * j;
            const float w = loadF(rdram, col + 0xC);
            storeF(rdram, col, loadF(rdram, col) + jx * w);
            storeF(rdram, col + 4, loadF(rdram, col + 4) + jy * w);
        }
    }

    void finishIfDone(uint8_t *rdram, R5900Context *ctx)
    {
        if (g_build.active && ctx->pc == g_build.ra)
        {
            if (g_build.k < 0.999f)
                narrowCamera(rdram, g_build.cam, g_build.k);
            if (g_build.jitter)
                jitterCamera(rdram, g_build.cam, g_build.jx, g_build.jy);
            g_build.active = false;
        }
    }

    void buildCameraWide(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t cam = GPR_U32(ctx, 4);
        const bool player = cam == kPlayerCameras[0] || cam == kPlayerCameras[1];
        float k = 1.0f, jx = 0.0f, jy = 0.0f;
        bool jitter = false;
        if (player)
        {
            GS &gs = runtime->gsUnsynced();
            if (gs.wideDriving())
                k = gs.wideHorizontalScale();
            jitter = gs.cameraJitter(jx, jy);
        }
        setFrustumScale(rdram, k);
        g_build = {k < 0.999f || jitter, cam, GPR_U32(ctx, 31), k, jitter, jx, jy};
        g_buildCamera(rdram, ctx, runtime);
        finishIfDone(rdram, ctx);
    }

    // The builder resuming after a pause (entered at one of its internal call return addresses).
    void resumeCameraWide(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        g_buildCamera(rdram, ctx, runtime);
        finishIfDone(rdram, ctx);
    }

    // ---------------------------------------------------------------- progressive fields
    // The game renders 640x224 fields (sceGsResetGraph(0, INTERLACE, NTSC, FIELD)) and, for an
    // interlaced TV, draws every other one half a line lower: each frame the main loop passes the
    // field parity from sceGsSyncV (0x335909) as `halfoff` to libgraph's sceGsSetHalfOffset
    // (0x26F340, draw env 1) and its draw env 2 twin (0x26F3C8), which add 8 (0.5 px) to XYOFFSET's
    // OFY. With progressive fields on (GS::progressiveFields) halfoff is forced to 0, so every
    // field is the same picture and the scanout shows it as is. A full 448-line frame does not
    // fit: textures start right after the two 224-line frame buffers and Z (block 0x1A40).
    constexpr uint32_t kSetHalfOffset[2] = {0x0026F340u, 0x0026F3C8u};
    PS2Runtime::RecompiledFunction g_setHalfOffset[2] = {};

    template <int I>
    void setHalfOffset(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        if (runtime->gsUnsynced().progressiveFields())
            SET_GPR_U64(ctx, 7, 0); // a3 = halfoff
        g_setHalfOffset[I](rdram, ctx, runtime);
    }

    // ---------------------------------------------------------------- Options
    // Title > Options starts task 0x2092D8 (a0 = task, a1 = the system block). It is replaced:
    // the app's menu opens at its Sound rows instead, and the task ends at once the way its own
    // Exit does (the parent's child-returned pulse +0x24 and result +4 set, then the task
    // killed, 0x204DE8), so the title menu carries on with its cursor on Options.
    constexpr uint32_t kOptionsTask = 0x002092D8u, kKillTask = 0x00204DE8u;
    PS2Runtime::RecompiledFunction g_killTask = nullptr;

    void optionsTask(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t task = GPR_U32(ctx, 4);
        uint32_t parent;
        std::memcpy(&parent, rdram + ((task + 0x18) & PS2_RAM_MASK), 4);
        if (parent)
        {
            rdram[(parent + 0x24) & PS2_RAM_MASK] = 1;
            const uint32_t one = 1;
            std::memcpy(rdram + ((parent + 4) & PS2_RAM_MASK), &one, 4);
        }
        rt::game::requestOptions();
        std::cout << "[roadtrip] Title > Options: the app's menu (Sound)" << std::endl;
        g_killTask(rdram, ctx, runtime); // a0 is still the task; returns to our caller
    }

    // Save states: where the game is, for a slot's label (menus list slots from file headers).
    void registerStateMetadata(PS2Runtime &runtime)
    {
        static bool registered = false;
        if (registered)
            return;
        registered = true;
        ps2_save_state::setMetadataProvider([&runtime]
                                            {
                                                std::vector<std::pair<std::string, std::string>> out;
                                                const rt::game::Stats st = rt::game::readStats(runtime);
                                                if (st.racing)
                                                {
                                                    out.emplace_back("mode", "race");
                                                    out.emplace_back("place", st.course);
                                                    out.emplace_back("race_mode", st.mode == rt::game::Stats::Mode::QuickRace ? "quick"
                                                                                  : st.mode == rt::game::Stats::Mode::TwoPlayer ? "2p"
                                                                                                                                 : "adventure");
                                                }
                                                else if (st.inTown && !st.demo)
                                                {
                                                    out.emplace_back("mode", "town");
                                                    out.emplace_back("place", st.townLabel);
                                                }
                                                else
                                                    out.emplace_back("mode", st.demo ? "demo" : "menu");
                                                if (st.adventure)
                                                {
                                                    out.emplace_back("player", st.playerName);
                                                    out.emplace_back("money", std::to_string(st.money));
                                                    out.emplace_back("stamps", std::to_string(st.stamps.count()));
                                                }
                                                // The menu's picture of the moment (rt::states).
                                                for (auto &kv : rt::states::takeSaveMetadata())
                                                    out.push_back(std::move(kv));
                                                return out;
                                            });
    }

    void applyRoadTrip(PS2Runtime &runtime)
    {
        registerStateMetadata(runtime);
        std::cout << "[roadtrip] applying SLUS-20398 overrides\n";
        g_setHalfOffset[0] = runtime.lookupFunction(kSetHalfOffset[0]);
        g_setHalfOffset[1] = runtime.lookupFunction(kSetHalfOffset[1]);
        if (g_setHalfOffset[0] && g_setHalfOffset[1])
        {
            runtime.replaceFunction(kSetHalfOffset[0], setHalfOffset<0>);
            runtime.replaceFunction(kSetHalfOffset[1], setHalfOffset<1>);
        }
        else
            std::cerr << "[roadtrip] sceGsSetHalfOffset not found: progressive fields unavailable\n";
        g_killTask = runtime.lookupFunction(kKillTask);
        // RT_GAME_OPTIONS=1 keeps the game's own Options screens (the regression tests of them).
        const char *own = std::getenv("RT_GAME_OPTIONS");
        if (own && *own == '1')
            std::cout << "[roadtrip] RT_GAME_OPTIONS=1: the game's own Options menu\n";
        else if (g_killTask && runtime.lookupFunction(kOptionsTask))
            runtime.replaceFunction(kOptionsTask, optionsTask);
        else
            std::cerr << "[roadtrip] Options task not found: the game's own Options menu stays\n";
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
            // Save states: a camera build can be paused at a scheduler checkpoint.
            static bool registered = false;
            if (!registered)
            {
                registered = true;
                ps2_save_state::registerSection(ps2x::fourcc("GAME"), [](ps2x::StateArchive &ar)
                                                {
                                                    ar & g_build.active & g_build.cam & g_build.ra & g_build.k;
                                                    ar & g_build.jitter & g_build.jx & g_build.jy;
                                                    ar & g_frustum & g_haveFrustum & g_frustumK;
                                                });
            }
        }
        else
            std::cerr << "[roadtrip] camera builder 0x21F698 not found: widescreen 3D unavailable\n";
    }
}

PS2_REGISTER_GAME_OVERRIDE("Road Trip (USA)", "SLUS_203.98", 0x00200008u, 0x5A49851Du, applyRoadTrip)
