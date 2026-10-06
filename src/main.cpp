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
#include "platform/Host.h"
#include "platform/Input.h"
#include "platform/Lifecycle.h"
#include "debug/ThreadDump.h"
#include "game/GameBuilder.h"
#include "platform/Dialogs.h"
#include "platform/Paths.h"
#if defined(__ANDROID__)
#include "platform/android/Assets.h"
#include "platform/android/PerformanceHint.h"
#endif
#include "platform/ProgressEta.h"
#include "platform/TaskProgress.h"
#include "rom/RomInstaller.h"
#include "settings/Apply.h"
#include "settings/Capabilities.h"
#include "settings/Settings.h"

#include "ps2_runtime.h"
#include <fstream>
#include "runtime/ps2_test_harness.h"
#include "runtime/gs/gs_frontend.h"
#include "runtime/gs/gs_hw_backend.h"
#include "runtime/gs/gs_pgs_backend.h"
#include "Stubs/CD.h"
#if defined(PS2X_ENABLE_DEBUG_UI)
#include "imgui.h"
#include "ps2_debug_panel.h"
#include "rlImGui.h"
#include "ui/PauseMenu.h"
#include "ui/SetupScreen.h"
#include "ui/Theme.h"
#endif

#include "raylib.h"
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_scancode.h>
#if defined(__ANDROID__)
#include <SDL3/SDL_main.h> // main() becomes SDL_main, called by SDLActivity
#endif

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
                std::cout << "usage: RoadTrip [--rom <image.cue|.bin|.iso|.chd>] [--reinstall] [--rebuild-game]\n";
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
        // RT_REBUILD_GAME=1: as --rebuild-game (Android, where files/env.txt stands in for arguments).
        if (const char *env = std::getenv("RT_REBUILD_GAME"); env && *env == '1')
            o.rebuild = true;
        return o;
    }

    // Android: the presenter opened for the first-run setup screen, handed to the runtime later.
    std::unique_ptr<ps2x::HostPresenter> g_setupPresenter;
    ps2x::HostPresenter *setupPresenter();

    // Runs `task` on a worker thread while showing a small progress window. `finale`: the last
    // setup task (Android: a rumble and a fade into the game when it succeeds).
    bool runWithProgress(const std::function<bool(rt::TaskProgress &)> &task, bool finale = false)
    {
        rt::TaskProgress progress;
        std::thread worker([&]
                           { task(progress) ? progress.succeed() : (progress.finished.load() || progress.fail("failed")); });

        const auto start = std::chrono::steady_clock::now();
        auto seconds = [&] { return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count(); };
#if defined(__ANDROID__)
        // One window per activity on Android: the Vulkan presenter opens now and draws the setup
        // screen (the runtime takes it over afterwards), about 20 times a second.
#if defined(PS2X_ENABLE_DEBUG_UI)
        ps2x::HostPresenter *screen = setupPresenter();
#else
        ps2x::HostPresenter *screen = nullptr;
#endif
        std::string lastPhase;
        auto drawScreen = [&]
        {
            int action = 0; // rt::ui::SetupAction: 0 none, 1 retry, 2 close
            screen->frameUi([&]
                            {
                                screen->uiBegin();
#if defined(PS2X_ENABLE_DEBUG_UI)
                                action = static_cast<int>(rt::ui::drawSetupScreen(progress, seconds()));
#endif
                                screen->uiEnd(); });
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            return action;
        };
        while (!progress.finished)
        {
            if (progress.phase() != lastPhase)
                std::cout << "[setup] " << (lastPhase = progress.phase()) << "\n";
            if (screen)
                drawScreen();
            else
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        worker.join();
        if (progress.failed && screen)
        {
            std::cerr << "[setup] " << progress.error() << "\n";
            int action = 0;
            while (!(action = drawScreen()) && !screen->closeRequested())
            {
            }
            // "Try again": the whole task again (each step starts over cleanly).
            return action == 1 ? runWithProgress(task, finale) : false;
        }
#if defined(PS2X_ENABLE_DEBUG_UI)
        if (finale && screen && !progress.failed)
        {
            // Done: a short rumble on the controller, and half a second's fade to black; the
            // title screen comes up out of it.
            int count = 0;
            SDL_Gamepad *pad = nullptr; // closed after the fade (closing stops the rumble)
            if (SDL_JoystickID *pads = SDL_GetGamepads(&count))
            {
                if (count > 0 && (pad = SDL_OpenGamepad(pads[0])))
                    SDL_RumbleGamepad(pad, 0x5000, 0x9000, 180);
                SDL_free(pads);
            }
            const double fadeStart = seconds();
            for (double t = 0.0; t < 0.5; t = seconds() - fadeStart)
            {
                screen->frameUi([&]
                                {
                                    screen->uiBegin();
                                    rt::ui::drawSetupScreen(progress, seconds());
                                    rt::ui::drawFadeOut(static_cast<float>(t / 0.5));
                                    screen->uiEnd(); });
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
            if (pad)
                SDL_CloseGamepad(pad); // the input layer opens it again
        }
#endif
#else
        SetTraceLogLevel(LOG_WARNING);
        InitWindow(720, 200, "Road Trip - first-time setup");
        SetTargetFPS(30);
        rt::ProgressEta eta;
        while (!progress.finished)
        {
            if (WindowShouldClose())
                break; // can't safely cancel mid-step; keep working without drawing
            const double total = static_cast<double>(progress.total.load());
            const double frac = total > 0 ? std::min(1.0, progress.done.load() / total) : 0.0;
            const double left = eta.update(progress, seconds());
            std::string phase = progress.phase();
            if (progress.steps > 1)
                phase = "Step " + std::to_string(std::max(1, progress.step.load())) + " of " + std::to_string(progress.steps.load()) +
                        ": " + phase;
            std::string status = total > 0 ? std::to_string(static_cast<int>(frac * 100.0)) + "%" : std::string();
            if (const std::string e = rt::ProgressEta::describe(left); !e.empty())
                status += "   " + e;
            BeginDrawing();
            ClearBackground(Color{24, 28, 36, 255});
            DrawText(phase.c_str(), 24, 36, 20, RAYWHITE);
            DrawRectangle(24, 80, 672, 24, Color{60, 66, 80, 255});
            DrawRectangle(24, 80, static_cast<int>(672 * frac), 24, Color{90, 170, 250, 255});
            DrawText(status.c_str(), 24, 116, 18, RAYWHITE);
            DrawText(progress.detail().c_str(), 24, 150, 18, LIGHTGRAY);
            EndDrawing();
        }
        worker.join();
        if (IsWindowReady())
            CloseWindow();
#endif

        if (progress.failed)
        {
            rt::dialogs::message("Setup failed", progress.error());
            return false;
        }
        return true;
    }

    // Whether the image was checked this run: the check is then step 1 of the setup's steps.
    bool g_imageChecked = false;

    // Hashes the whole image against the good dump, with the progress screen (step 1 of 5: the
    // check, extracting, translating, compiling, linking).
    rt::ImageCheck verifyChosen(const fs::path &image, std::string &detail)
    {
        rt::ImageCheck result = rt::ImageCheck::Unreadable;
        g_imageChecked = true;
        runWithProgress([&](rt::TaskProgress &progress)
                        {
                            progress.steps = 5;
                            result = rt::verifyImage(image, progress, detail);
                            return true; });
        std::cout << "[setup] disc image check: " << detail << "\n";
        return result;
    }

    std::optional<fs::path> chooseRom(const Options &opts)
    {
        std::optional<fs::path> rom = opts.rom;
#if defined(__ANDROID__) && defined(PS2X_ENABLE_DEBUG_UI)
        // A welcome screen first, then the system's document picker; back to the welcome screen
        // if nothing usable was picked.
        if (!rom)
        {
            ps2x::HostPresenter *screen = setupPresenter();
            std::string note;
            std::optional<fs::path> doubtful; // checked, didn't verify: the warning offers "Use it anyway"
            rt::ui::ImageWarning warning = rt::ui::ImageWarning::Mismatch;
            while (screen && !screen->closeRequested())
            {
                rt::ui::WelcomeAction action = rt::ui::WelcomeAction::None;
                screen->frameUi([&]
                                {
                                    screen->uiBegin();
                                    action = doubtful ? rt::ui::drawImageWarning(warning) : rt::ui::drawWelcomeScreen(note);
                                    screen->uiEnd(); });
                if (action == rt::ui::WelcomeAction::UseAnyway && doubtful)
                    return doubtful;
                if (action != rt::ui::WelcomeAction::Choose)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                    continue;
                }
                doubtful.reset();
                std::optional<fs::path> picked = rt::dialogs::pickRomImage();
                if (!picked)
                {
                    note = "No file was chosen.";
                    continue;
                }
                std::string detail;
                const rt::RomCheck check = rt::checkRom(*picked, detail);
                if (check == rt::RomCheck::Ok)
                {
                    const rt::ImageCheck image = verifyChosen(*picked, detail);
                    if (image == rt::ImageCheck::Verified)
                        return picked;
                    if (image == rt::ImageCheck::Unreadable)
                    {
                        note = "That file couldn't be read to the end. Choose it again, or another copy.";
                        continue;
                    }
                    warning = image == rt::ImageCheck::Damaged      ? rt::ui::ImageWarning::Damaged
                              : image == rt::ImageCheck::Unverified ? rt::ui::ImageWarning::Unverified
                                                                    : rt::ui::ImageWarning::Mismatch;
                    doubtful = picked;
                    continue;
                }
                std::cout << "[setup] disc image rejected: " << detail << "\n";
                if (check == rt::RomCheck::UnknownBuild)
                {
                    note = "That's another version of Road Trip. This app needs Road Trip (USA).";
                    continue;
                }
                note = check == rt::RomCheck::WrongGame ? "That isn't a Road Trip disc image." : "That file can't be read as a disc image.";
            }
            return std::nullopt;
        }
#endif
        if (!rom)
            rom = rt::dialogs::pickRomImage();
        if (!rom)
            return std::nullopt;

        std::string detail;
        switch (rt::checkRom(*rom, detail))
        {
        case rt::RomCheck::Ok:
        {
            const rt::ImageCheck image = verifyChosen(*rom, detail);
            // Given on the command line (scripts, tests): the check is only logged.
            if (image == rt::ImageCheck::Verified || opts.rom)
                return rom;
            if (image == rt::ImageCheck::Unreadable)
            {
                rt::dialogs::message("Can't use this disc image", detail);
                return std::nullopt;
            }
            if (rt::dialogs::message(image == rt::ImageCheck::Unverified ? "Disc image not verified" : "Disc image doesn't match",
                                     detail + (image == rt::ImageCheck::Mismatch ? " It may be damaged or altered." : "") +
                                         "\n\nUse it anyway?",
                                     true))
                return rom;
            return std::nullopt;
        }
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

    // The MoltenVK bundled in RoadTrip.app/Contents/Frameworks, or "" for the system loader.
    std::string vulkanLibrary()
    {
        const fs::path bundled = rt::paths::bundleResources().parent_path() / "Frameworks" / "libMoltenVK.dylib";
        std::error_code ec;
        return fs::exists(bundled, ec) ? bundled.string() : std::string();
    }

    // The Vulkan presenter, opened before the runtime exists for the setup screen (Android has one
    // window per activity, so setup can't have a window of its own). Null if unavailable.
    ps2x::HostPresenter *setupPresenter()
    {
#if defined(PS2X_ENABLE_DEBUG_UI)
        if (!g_setupPresenter)
        {
            ps2x::gs::PgsPresenterOptions options;
            options.vulkanLibrary = vulkanLibrary();
            options.vsync = rt::settings::current().vsync;
            std::string error;
            auto presenter = ps2x::gs::createPgsPresenter(options, error);
            if (!presenter || !presenter->open("Road Trip Adventure", 1280, 720))
            {
                std::cerr << "[setup] no presenter for the setup screen (" << error << ")\n";
                return nullptr;
            }
            presenter->uiInit();
            SDL_InitSubSystem(SDL_INIT_GAMEPAD); // the setup screens take a controller's confirm button (through ImGui)
            rt::ui::theme::initialize();
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_NavEnableKeyboard;
            g_setupPresenter = std::move(presenter);
        }
        return g_setupPresenter.get();
#else
        return nullptr;
#endif
    }

    // An SDL3 window with a Vulkan swapchain on the GS's device (the picture stays on the GPU), unless
    // RT_PRESENTER=raylib or the CPU GS is chosen: then the runtime's raylib/OpenGL window.
    void selectPresenter(PS2Runtime &runtime)
    {
        const char *choice = std::getenv("RT_PRESENTER");
        const char *gs = std::getenv("RT_GS_BACKEND");
        if ((choice && std::string(choice) == "raylib") || (gs && std::string(gs) == "cpu" && !choice))
            return;
        if (g_setupPresenter)
        {
            runtime.setPresenter(std::move(g_setupPresenter)); // already open (the setup screen)
            return;
        }
        ps2x::gs::PgsPresenterOptions options;
        options.vulkanLibrary = vulkanLibrary();
        options.vsync = rt::settings::current().vsync;
        std::string error;
        if (auto presenter = ps2x::gs::createPgsPresenter(options, error))
            runtime.setPresenter(std::move(presenter));
        else
            std::cerr << "[presenter] Vulkan presenter unavailable (" << error << "); using raylib\n";
    }

    // GPU GS (paraLLEl-GS on Vulkan/MoltenVK, or the hardware GS) unless RT_GS_BACKEND=cpu; falls back to
    // the CPU GS.
    void selectGsBackend(PS2Runtime &runtime)
    {
        const char *choice = std::getenv("RT_GS_BACKEND");
        if (choice && std::string(choice) == "cpu")
        {
            std::cout << "[gs] using CPU backend (RT_GS_BACKEND=cpu)\n";
            return;
        }

        // The hardware-rasterizer GS on the presenter's device: RT_GS_BACKEND=hw, and the default on
        // Android (paraLLEl-GS's compute rasterizer is several times heavier on phone GPUs;
        // RT_GS_BACKEND=pgs keeps it).
#if defined(__ANDROID__)
        const bool hardware = !choice || std::string(choice) != "pgs";
#else
        const bool hardware = choice && std::string(choice) == "hw";
#endif
        if (hardware)
        {
            ps2x::gs::HwOptions hw;
            hw.presenter = runtime.presenter();
            hw.vulkanLibrary = vulkanLibrary();
            std::string error;
            ps2x::gs::PgsControl *control = nullptr;
            if (auto backend = ps2x::gs::createHwBackend(hw, error, &control))
            {
                runtime.gs().setRasterBackend(std::move(backend));
                rt::settings::setGsControl(control);
                rt::settings::capabilities().frameGeneration = false; // no shadow frames (yet)
                return;
            }
            std::cerr << "[gs] hardware GS unavailable (" << error << "); using paraLLEl-GS\n";
        }

        ps2x::gs::PgsOptions options;
        options.vulkanLibrary = vulkanLibrary();
        options.pipelineCacheDir = (rt::paths::dataRoot() / "cache").string();
        options.presenter = runtime.presenter(); // shares its device when it is the Vulkan presenter

        std::string error;
        ps2x::gs::PgsControl *control = nullptr;
        if (auto backend = ps2x::gs::createPgsBackend(options, error, &control))
        {
            runtime.gs().setRasterBackend(std::move(backend));
            rt::settings::setGsControl(control);
            rt::settings::capabilities().frameGeneration = true;
        }
        else
            std::cerr << "[gs] Vulkan GS unavailable (" << error << "); using CPU backend\n";
    }

    // Test socket {"cmd":"sdlpad","type":"xbox|ps|switch","buttons":"start,back,..."}: a virtual SDL
    // gamepad with these buttons held (the rest released), so tests reach the app's own input
    // path (menus, rebinding, button icons) the way a real controller does. "type":"none" unplugs it.
    std::string testSdlPad(const std::string &line)
    {
        static SDL_JoystickID id = 0;
        static SDL_Joystick *joy = nullptr;
        static std::string current;
        std::string type = ps2_test::jsonField(line, "type");
        if (type.empty())
            type = current.empty() ? "xbox" : current;
        if (joy && type != current)
        {
            SDL_CloseJoystick(joy);
            SDL_DetachVirtualJoystick(id);
            joy = nullptr;
        }
        if (type == "none")
            return "{\"ok\":true}";
        if (!joy)
        {
            SDL_VirtualJoystickDesc desc;
            SDL_INIT_INTERFACE(&desc);
            desc.type = SDL_JOYSTICK_TYPE_GAMEPAD;
            desc.naxes = SDL_GAMEPAD_AXIS_COUNT;
            desc.nbuttons = SDL_GAMEPAD_BUTTON_COUNT;
            // Every button and axis present, in SDL's gamepad order: SDL derives the mapping from
            // these (not from the database entry of the device the ids name).
            desc.button_mask = (1u << SDL_GAMEPAD_BUTTON_COUNT) - 1;
            desc.axis_mask = (1u << SDL_GAMEPAD_AXIS_COUNT) - 1;
            // Real devices' USB ids, so SDL reports their gamepad type and button labels.
            if (type == "ps")
                desc.vendor_id = 0x054C, desc.product_id = 0x0CE6, desc.name = "Test DualSense";
            else if (type == "switch")
                desc.vendor_id = 0x057E, desc.product_id = 0x2009, desc.name = "Test Switch Pro";
            else
                desc.vendor_id = 0x045E, desc.product_id = 0x0B12, desc.name = "Test Xbox";
            id = SDL_AttachVirtualJoystick(&desc);
            joy = id ? SDL_OpenJoystick(id) : nullptr;
            if (!joy)
                return std::string("{\"ok\":false,\"error\":\"") + SDL_GetError() + "\"}";
            current = type;
        }
        static const std::pair<const char *, SDL_GamepadButton> names[] = {
            {"a", SDL_GAMEPAD_BUTTON_SOUTH}, {"b", SDL_GAMEPAD_BUTTON_EAST}, {"x", SDL_GAMEPAD_BUTTON_WEST},
            {"y", SDL_GAMEPAD_BUTTON_NORTH}, {"back", SDL_GAMEPAD_BUTTON_BACK}, {"guide", SDL_GAMEPAD_BUTTON_GUIDE},
            {"start", SDL_GAMEPAD_BUTTON_START}, {"l3", SDL_GAMEPAD_BUTTON_LEFT_STICK},
            {"r3", SDL_GAMEPAD_BUTTON_RIGHT_STICK}, {"lb", SDL_GAMEPAD_BUTTON_LEFT_SHOULDER},
            {"rb", SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER}, {"up", SDL_GAMEPAD_BUTTON_DPAD_UP},
            {"down", SDL_GAMEPAD_BUTTON_DPAD_DOWN}, {"left", SDL_GAMEPAD_BUTTON_DPAD_LEFT},
            {"right", SDL_GAMEPAD_BUTTON_DPAD_RIGHT}};
        const std::string held = "," + ps2_test::jsonField(line, "buttons") + ",";
        for (const auto &[name, button] : names)
            SDL_SetJoystickVirtualButton(joy, button, held.find("," + std::string(name) + ",") != std::string::npos);
        for (const char *trigger : {"lt", "rt"})
            SDL_SetJoystickVirtualAxis(joy, trigger[0] == 'l' ? SDL_GAMEPAD_AXIS_LEFT_TRIGGER : SDL_GAMEPAD_AXIS_RIGHT_TRIGGER,
                                       held.find("," + std::string(trigger) + ",") != std::string::npos ? SDL_JOYSTICK_AXIS_MAX : 0);
        return "{\"ok\":true}";
    }

    // Extracts the disc and builds the game library as needed.
    bool ensureSetUp(const Options &opts)
    {
#if defined(__ANDROID__)
        // The compiler's headers and libraries ship in the APK; unpack them first.
        if (std::string kitError; !rt::android::ensureBuildKit(kitError))
        {
            rt::dialogs::message("Setup failed", kitError);
            return false;
        }
#endif
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
                                   // Extracting is one step; translating, compiling and linking three.
                                   progress.steps = (g_imageChecked ? 1 : 0) + (needInstall ? 1 : 0) + (needBuild ? 3 : 0);
                                   progress.step = g_imageChecked ? 1 : 0; // the check was step 1
                                   // RT_SETUP_TEST_FAIL=1 (UI reviews): fail at once, with the last build log.
                                   if (const char *f = std::getenv("RT_SETUP_TEST_FAIL"); f && *f == '1')
                                   {
                                       progress.setPhase("Compiling the game for this device");
                                       return progress.fail("Compiling the game failed.\n\nDetails: " +
                                                            (rt::paths::gameDir() / "build.log").string());
                                   }
                                   if (needInstall && !rt::installFromRom(*rom, progress))
                                       return false;
                                   return !needBuild ||
                                          rt::game::build(rt::paths::discDir() / rt::kBootElfName, progress); },
                               true);
    }
}

int main(int argc, char *argv[])
{
#if defined(__ANDROID__)
    // Apps get no environment: files/env.txt (KEY=VALUE lines) sets RT_* switches for debugging,
    // e.g. `adb shell run-as <pkg> sh -c 'echo RT_GS_BACKEND=cpu > files/env.txt'`.
    {
        std::ifstream env(rt::paths::dataRoot() / "env.txt");
        for (std::string line; std::getline(env, line);)
            if (const size_t eq = line.find('='); eq != std::string::npos && line[0] != '#')
                setenv(line.substr(0, eq).c_str(), line.substr(eq + 1).c_str(), 1);
    }
#endif
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
        rt::host::setRuntime(&runtime); // settings (aspect) reach the GS also in headless runs
        // The debug-UI hooks run on the render thread each frame; also used for frame dumps.
        struct UiHooks
        {
#if defined(PS2X_ENABLE_DEBUG_UI)
            PS2DebugPanel panel;
#endif
        } hooks;
        runtime.setDebugUiCallbacks(
            [](PS2Runtime &rt, void *user)
            {
                rt::host::setRuntime(&rt);
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
#if defined(__ANDROID__)
                rt::perfhint::tick();        // ADPF: the frame's work, so the governor keeps up
#endif
                rt::debug::maybeDumpFrame(); // the presenter saves this frame as shown (with any menu or overlay)
                rt::debug::drawFpsOverlay(rt);
#if defined(PS2X_ENABLE_DEBUG_UI)
                // One ImGui frame for the debug panel (F1) and the in-game menu.
                auto &panel = static_cast<UiHooks *>(user)->panel;
                rt::ui::updatePauseMenu(); // Guide, Back+Start, Esc or F3: the in-game menu (pauses the game)
                if (rt::host::keyPressed(SDL_SCANCODE_F1))
                    panel.toggleVisible();
                if (panel.isVisible() || rt::ui::pauseMenuWantsFrame())
                {
                    rt.presenter()->uiBegin();
                    panel.drawWindow(rt);
                    rt::ui::drawPauseMenu();
                    rt.presenter()->uiEnd();
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
        if (const char *h = std::getenv("RT_HEADLESS"); !(h && *h == '1'))
            selectPresenter(runtime);
        if (!runtime.initialize("Road Trip Adventure"))
        {
            std::cerr << "Failed to initialize PS2 runtime\n";
            return 1;
        }
        selectGsBackend(runtime);
        rt::settings::applyAll();
        rt::lifecycle::install(); // Android: pause the game and close audio while the app is away
        // Test-socket commands of the app's own:
        //   {"cmd":"texture_pack","path":DIR}  use that pack ("" = none; no "path": keep it)
        //                                      -> {"pack_images","replaced"} of the active pack
        ps2_test::setCommandHandler([](const std::string &cmd, const std::string &line) -> std::string {
            if (cmd == "texture_pack")
            {
                if (line.find("\"path\"") != std::string::npos)
                    rt::settings::overrideTexturePack(ps2_test::jsonField(line, "path"));
                ps2x::gs::PgsControl *gs = rt::settings::gsControl();
                const auto stats = gs ? gs->texturePackStats() : ps2x::gs::PgsControl::TexturePackStats{};
                return "{\"ok\":true,\"pack_images\":" + std::to_string(stats.packImages) +
                       ",\"replaced\":" + std::to_string(stats.replaced) + "}";
            }
            if (cmd == "sdlpad")
                return testSdlPad(line);
            return {};
        });

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
        rt::debug::startFpsLogIfHeadless(runtime);
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
