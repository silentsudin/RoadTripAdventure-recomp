// Windows dialogs through SDL (the native file picker and message boxes).

#include "platform/Dialogs.h"

#include <SDL3/SDL_dialog.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_messagebox.h>

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

namespace rt::dialogs
{
    std::optional<std::filesystem::path> pickRomImage()
    {
        struct Result
        {
            std::atomic<bool> done{false};
            std::string path;
        } result;
        static const SDL_DialogFileFilter filters[] = {
            {"Disc images", "cue;bin;iso;chd"},
            {"All files", "*"},
        };
        SDL_ShowOpenFileDialog(
            [](void *user, const char *const *files, int)
            {
                auto *r = static_cast<Result *>(user);
                if (files && *files)
                    r->path = *files;
                r->done = true;
            },
            &result, nullptr, filters, 2, nullptr, false);
        // The dialog answers from its own thread; keep SDL's event queue moving meanwhile.
        while (!result.done)
        {
            SDL_PumpEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        if (result.path.empty())
            return std::nullopt;
        return std::filesystem::u8path(result.path);
    }

    bool message(const std::string &title, const std::string &text, bool withCancel)
    {
        const SDL_MessageBoxButtonData buttons[] = {
            {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "OK"},
            {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Cancel"},
        };
        const SDL_MessageBoxData data = {SDL_MESSAGEBOX_INFORMATION, nullptr, title.c_str(), text.c_str(),
                                         withCancel ? 2 : 1, buttons, nullptr};
        int pressed = 0;
        return SDL_ShowMessageBox(&data, &pressed) && pressed == 1;
    }
}
