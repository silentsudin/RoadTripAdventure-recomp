#pragma once

// The host window, clock and keyboard through SDL3, whichever presenter shows the game (raylib's
// SDL window or the Vulkan presenter's). Render thread only.

#include <string>

class PS2Runtime;
struct SDL_Window;

namespace rt::host
{
    void setRuntime(PS2Runtime *runtime);
    PS2Runtime *runtime();
    // Once per host frame, before reading keys.
    void beginFrame();

    double now(); // seconds since start

    // SDL scancodes. Pressed: went down since the last frame; repeat: held, every 50 ms after 400 ms.
    bool keyPressed(int scancode);
    bool keyPressedRepeat(int scancode);

    SDL_Window *window();
    bool windowFocused();
    // Window size in points, and the usable area of its display.
    void windowSize(int &width, int &height);
    void displayUsableSize(int &width, int &height);
    int displayRefreshRate(); // Hz, 0 if unknown

    // Saves the next presented frame (with the UI) as a PNG.
    bool screenshot(const std::string &pngPath);
    // Which presenter shows the game ("raylib", "vulkan").
    const char *presenterName();
}
