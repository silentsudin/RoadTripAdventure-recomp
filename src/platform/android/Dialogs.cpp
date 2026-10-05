// Android dialogs through SDL (message boxes; the disc is picked with the storage access framework).

#include "platform/Dialogs.h"

#include <SDL3/SDL_messagebox.h>

namespace rt::dialogs
{
    std::optional<std::filesystem::path> pickRomImage()
    {
        return std::nullopt; // first-run disc picking: SAF via SDL_ShowOpenFileDialog (phase 2)
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
