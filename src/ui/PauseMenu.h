#pragma once

// The recomp's in-game menu, drawn in Road Trip's style (src/ui/Theme): Resume / Options /
// Controllers / Quit. Guide, Back+Start, Escape or F3 open and close it; it pauses the game while
// open. Options apply live (the game steps a couple of frames so the picture behind updates) and
// are saved to settings.toml. Render thread only.
//
// RT_MENU_SHOT=<png> [RT_MENU_PAGE=root|display|graphics|general|quit] [RT_MENU_AT=<vblank>]:
// open the menu at that vblank, save a picture of it and quit (for UI reviews without a person).

namespace rt::ui
{
    void updatePauseMenu();  // every frame, before drawing: input, open/close, pause
    bool pauseMenuWantsFrame(); // something to draw this frame (menu, controllers, first-run hint)
    void drawPauseMenu();    // inside the shared ImGui frame
    void menuShotAfterFrame(); // after the ImGui frame is rendered (RT_MENU_SHOT)
}
