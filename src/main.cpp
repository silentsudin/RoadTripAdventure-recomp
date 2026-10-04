// Road Trip Adventure recomp entry point.
//
// The app ships no game code or data. On first launch it:
//   1. asks for the user's disc image and extracts it to
//      ~/Library/Application Support/RoadTripRecomp/disc,
//   2. recompiles SLUS_203.98 from that disc and compiles it with the system clang into
//      .../game/libroadtrip_game.dylib (src/game/GameBuilder.cpp),
// then loads the library and boots it through the PS2Recomp runtime, with cdrom0: mapped onto
// the extracted tree and mc0: onto the saves directory.

#include "debug/FpsOverlay.h"
#include "debug/FrameDump.h"
#include "debug/RamDump.h"
#include "platform/Input.h"
#include "debug/ThreadDump.h"
#include "game/GameBuilder.h"
#include "platform/Dialogs.h"
#include "platform/Paths.h"
#include "platform/TaskProgress.h"
#include "rom/RomInstaller.h"
#include "settings/Apply.h"
#include "settings/Settings.h"

#include "ps2_runtime.h"
#include "runtime/ps2_test_harness.h"
#include "runtime/gs/gs_frontend.h"
#include "runtime/gs/gs_pgs_backend.h"
#include "Stubs/CD.h"
#if defined(PS2X_ENABLE_DEBUG_UI)
#include "imgui.h"
#include "ps2_debug_panel.h"
#include "rlImGui.h"
#include "ui/PauseMenu.h"
#include "ui/Theme.h"
#endif

#include "raylib.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <thread>

namespace fs = std::filesystem;

namespace
{
    struct Options
    {
        std::optional<fs::path> rom;
        bool reinstall = false;
        bool rebuild = false;
    };

    Options parseArgs(int argc, char *argv[])
    {
        Options o;
        for (int i = 1; i < argc; ++i)
        {
            const std::string a = argv[i];
            if (a == "--rom" && i + 1 < argc)
                o.rom = argv[++i];
            else if (a == "--reinstall")
                o.reinstall = o.rebuild = true;
            else if (a == "--rebuild-game")
                o.rebuild = true;
            else if (a == "--help" || a == "-h")
            {
                std::cout << "usage: RoadTrip [--rom <image.cue|.bin|.iso>] [--reinstall] [--rebuild-game]\n";
                std::exit(0);
            }
            else if (a.rfind("-psn_", 0) == 0)
                continue; // Finder launch argument
            else
                std::cerr << "ignoring unknown argument: " << a << "\n";
        }
        if (!o.rom)
        {
            if (const char *env = std::getenv("RT_ROM"); env && *env)
                o.rom = env;
        }
        return o;
    }

    // Runs `task` on a worker thread while showing a small progress window.
    bool runWithProgress(const std::function<bool(rt::TaskProgress &)> &task)
    {
        rt::TaskProgress progress;
        std::thread worker([&]
                           { task(progress) ? progress.succeed() : (progress.finished.load() || progress.fail("failed")); });

        SetTraceLogLevel(LOG_WARNING);
        InitWindow(720, 200, "Road Trip - first-time setup");
        SetTargetFPS(30);
        while (!progress.finished)
        {
            if (WindowShouldClose())
                break; // can't safely cancel mid-step; keep working without drawing
            const double total = static_cast<double>(progress.total.load());
            const double frac = total > 0 ? std::min(1.0, progress.done.load() / total) : 0.0;
            BeginDrawing();
            ClearBackground(Color{24, 28, 36, 255});
            DrawText(progress.phase().c_str(), 24, 36, 20, RAYWHITE);
            DrawRectangle(24, 90, 672, 24, Color{60, 66, 80, 255});
            DrawRectangle(24, 90, static_cast<int>(672 * frac), 24, Color{90, 170, 250, 255});
            DrawText(progress.detail().c_str(), 24, 130, 18, LIGHTGRAY);
            EndDrawing();
        }
        worker.join();
        if (IsWindowReady())
            CloseWindow();

        if (progress.failed)
        {
            rt::dialogs::message("Setup failed", progress.error());
            return false;
        }
        return true;
    }

    std::optional<fs::path> chooseRom(const Options &opts)
    {
        std::optional<fs::path> rom = opts.rom;
        if (!rom)
            rom = rt::dialogs::pickRomImage();
        if (!rom)
            return std::nullopt;

        std::string detail;
        switch (rt::checkRom(*rom, detail))
        {
        case rt::RomCheck::Ok:
            return rom;
        case rt::RomCheck::UnknownBuild:
            if (rt::dialogs::message("Unrecognised version",
                                     "This disc doesn't match Road Trip (USA) SLUS-20398 v1.02, the only version "
                                     "this app supports. It will very likely not work.\n\n" +
                                         detail + "\n\nContinue anyway?",
                                     true))
                return rom;
            return std::nullopt;
        case rt::RomCheck::WrongGame:
        case rt::RomCheck::Unreadable:
            rt::dialogs::message("Can't use this disc image", detail);
            return std::nullopt;
        }
        return std::nullopt;
    }

    // GPU GS (paraLLEl-GS on Vulkan/MoltenVK) unless RT_GS_BACKEND=cpu; falls back to the CPU GS.
    void selectGsBackend(PS2Runtime &runtime)
    {
        const char *choice = std::getenv("RT_GS_BACKEND");
        if (choice && std::string(choice) == "cpu")
        {
            std::cout << "[gs] using CPU backend (RT_GS_BACKEND=cpu)\n";
            return;
        }

        ps2x::gs::PgsOptions options;
        // Prefer the MoltenVK bundled in RoadTrip.app/Contents/Frameworks; otherwise the system loader.
        const fs::path bundled = rt::paths::bundleResources().parent_path() / "Frameworks" / "libMoltenVK.dylib";
        std::error_code ec;
        if (fs::exists(bundled, ec))
            options.vulkanLibrary = bundled.string();
        options.pipelineCacheDir = (rt::paths::dataRoot() / "cache").string();

        std::string error;
        ps2x::gs::PgsControl *control = nullptr;
        if (auto backend = ps2x::gs::createPgsBackend(options, error, &control))
        {
            runtime.gs().setRasterBackend(std::move(backend));
            rt::settings::setGsControl(control);
        }
        else
            std::cerr << "[gs] Vulkan GS unavailable (" << error << "); using CPU backend\n";
    }

    // Extracts the disc and builds the game library as needed.
    bool ensureSetUp(const Options &opts)
    {
        const bool needInstall = opts.reinstall || !rt::isInstalled();
        const bool needBuild = needInstall || opts.rebuild || !rt::game::isBuilt();
        if (!needInstall && !needBuild)
            return true;

        if (needBuild && !rt::game::findCompiler())
        {
            rt::game::requestCommandLineTools();
            rt::dialogs::message("Developer tools needed",
                                 "Road Trip builds the game from your own disc the first time it runs, which needs "
                                 "Apple's Command Line Tools.\n\nmacOS will now offer to install them. When that "
                                 "finishes, open Road Trip again.");
            return false;
        }

        std::optional<fs::path> rom;
        if (needInstall)
        {
            rom = chooseRom(opts);
            if (!rom)
                return false;
        }

        return runWithProgress([&](rt::TaskProgress &progress)
                               {
                                   if (needInstall && !rt::installFromRom(*rom, progress))
                                       return false;
                                   return !needBuild ||
                                          rt::game::build(rt::paths::discDir() / rt::kBootElfName, progress); });
    }
}

int main(int argc, char *argv[])
{
    const Options opts = parseArgs(argc, argv);

    try
    {
        if (!ensureSetUp(opts))
            return 1;
        const fs::path elfPath = rt::paths::discDir() / rt::kBootElfName;

        std::string loadError;
        if (!rt::game::load(loadError))
        {
            rt::dialogs::message("Can't load the game",
                                 loadError + "\n\nRun Road Trip with --rebuild-game to build it again.");
            return 1;
        }

        PS2Runtime runtime;
        // The debug-UI hooks run on the render thread each frame; also used for frame dumps.
        struct UiHooks
        {
#if defined(PS2X_ENABLE_DEBUG_UI)
            PS2DebugPanel panel;
#endif
        } hooks;
        runtime.setDebugUiCallbacks(
            [](PS2Runtime &, void *user)
            {
                rt::input::initialize();
#if defined(PS2X_ENABLE_DEBUG_UI)
                // Hidden for players; F1 toggles it, RT_DEBUG_UI=1 shows it at startup.
                auto &panel = static_cast<UiHooks *>(user)->panel;
                panel.initialize();
                // The Controllers window can be used with a controller; the menu reads pads itself.
                ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_NavEnableKeyboard;
                rt::ui::theme::initialize(); // the game's look: fonts and colours
                // Window positions live with the other settings, not in the working directory.
                static const std::string ini = (rt::paths::dataRoot() / "imgui.ini").string();
                ImGui::GetIO().IniFilename = ini.c_str();
                const char *env = std::getenv("RT_DEBUG_UI");
                panel.setVisible(env && *env && *env != '0');
#endif
            },
            [](PS2Runtime &rt, void *user)
            {
                rt::input::update();         // one input snapshot per host frame, on this thread
                rt::debug::maybeDumpFrame(); // before the overlay, so dumps show only the game
                rt::debug::drawFpsOverlay(rt);
#if defined(PS2X_ENABLE_DEBUG_UI)
                // One ImGui frame for the debug panel (F1) and the in-game menu.
                auto &panel = static_cast<UiHooks *>(user)->panel;
                rt::ui::updatePauseMenu(); // Guide, Back+Start, Esc or F3: the in-game menu (pauses the game)
                if (IsKeyPressed(KEY_F1))
                    panel.toggleVisible();
                if (panel.isVisible() || rt::ui::pauseMenuWantsFrame())
                {
                    rlImGuiBegin();
                    panel.drawWindow(rt);
                    rt::ui::drawPauseMenu();
                    rlImGuiEnd();
                    rt::ui::menuShotAfterFrame();
                }
#endif
            },
            [](PS2Runtime &, void *user)
            {
                rt::input::shutdown(); // stops vibration, closes the controllers
#if defined(PS2X_ENABLE_DEBUG_UI)
                static_cast<UiHooks *>(user)->panel.shutdown();
#endif
            },
            &hooks);
        rt::settings::exportGsEnvironment(); // before the GS starts
        if (!runtime.initialize("Road Trip Adventure"))
        {
            std::cerr << "Failed to initialize PS2 runtime\n";
            return 1;
        }
        selectGsBackend(runtime);
        rt::settings::applyAll();

        // Recompiled VU1 microcode (3D geometry) unless RT_VU1_MODE=interp.
        {
            uint64_t imageHash = 0;
            void *entry = rt::game::vu1NativeEntry(imageHash);
            const char *mode = std::getenv("RT_VU1_MODE");
            if (entry && !(mode && std::string(mode) == "interp"))
            {
                runtime.setVu1Native(reinterpret_cast<PS2Runtime::Vu1NativeEntry>(entry), imageHash);
                std::cout << "[vu1] using recompiled microcode\n";
            }
        }
        // Saves live outside the disc tree. Must be set before loadELF: it resets the IOP, which
        // initialises the memory card (needs patches/0002 so loadELF keeps this root).
        {
            PS2Runtime::IoPaths io = PS2Runtime::getIoPaths();
            io.mcRoot = rt::paths::savesDir() / "mc0";
            std::error_code ec;
            fs::create_directories(io.mcRoot, ec);
            PS2Runtime::setIoPaths(io);
        }
        if (!runtime.loadELF(elfPath.string()))
        {
            std::cerr << "Failed to load ELF: " << elfPath << "\n";
            return 1;
        }

        // The game reads sectors by LBN; expose each extracted file at its original disc location.
        {
            size_t mapped = 0;
            for (const auto &e : rt::readLbnMap())
            {
                std::string ps2Path = "cdrom0:\\" + e.path + ";1";
                std::replace(ps2Path.begin(), ps2Path.end(), '/', '\\');
                mapped += ps2_stubs::registerCdFileAtLbn(ps2Path, rt::paths::discDir() / e.path, e.lba, e.size);
            }
            std::cout << "[roadtrip] mapped " << mapped << " disc files by LBN\n";
        }

        rt::debug::startThreadDumpIfRequested(runtime);
        rt::debug::startRamDumpIfRequested(runtime);
        runtime.run();
        ps2_test::finishRecording(); // close a movie recorded with RT_MOVIE_RECORD
        std::cout.flush();
        std::cerr.flush();
        std::_Exit(0);
    }
    catch (const std::exception &e)
    {
        std::cerr << "[main] fatal exception: " << e.what() << "\n";
    }
    std::_Exit(1);
}
