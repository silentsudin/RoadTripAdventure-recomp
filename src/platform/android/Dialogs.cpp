// Android dialogs through SDL (message boxes; the disc is picked with the storage access framework).

#include "platform/Dialogs.h"

#include <SDL3/SDL_dialog.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_messagebox.h>

#include <atomic>
#include <cctype>
#include <chrono>
#include <string>
#include <thread>

namespace rt::dialogs
{
    // The storage access framework's document picker (SDL_ShowOpenFileDialog). Returns a content://
    // URI, which IsoReader reads through the content resolver. A .cue can't lead to its .bin there
    // (no access to siblings), so the image itself is picked; when several files are picked (a
    // .cue with its .bin) the first that isn't a cue sheet is used.
    std::optional<std::filesystem::path> pickRomImage()
    {
        struct Result
        {
            std::atomic<bool> done{false};
            std::string uri;
        } result;
        SDL_ShowOpenFileDialog(
            [](void *user, const char *const *files, int)
            {
                auto *r = static_cast<Result *>(user);
                for (; files && *files; ++files)
                {
                    std::string f = *files;
                    std::string lower = f;
                    for (char &c : lower)
                        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                    if (lower.size() >= 4 && lower.compare(lower.size() - 4, 4, ".cue") == 0)
                        continue;
                    r->uri = f;
                    break;
                }
                r->done = true;
            },
            &result, nullptr, nullptr, 0, nullptr, true);
        // The answer comes from the activity; keep SDL's event queue moving meanwhile.
        while (!result.done)
        {
            SDL_PumpEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        if (result.uri.empty())
            return std::nullopt;
        return std::filesystem::path(result.uri);
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
