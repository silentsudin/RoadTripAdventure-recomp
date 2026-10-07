#pragma once

// The recomp's in-game menu, drawn in Road Trip's style (src/ui/Theme): Resume / Options /
// Controllers / Quit. Guide, Back+Start, Escape or F3 open and close it; it pauses the game while
// open. Options apply live (the game steps a couple of frames so the picture behind updates) and
// are saved to settings.toml. Render thread only.
//
// Save state / Load state (in a game of the player's, where the GS can save): four slots with a
// picture of the moment (src/states/StateSlots).
//
// RT_MENU_SHOT=<png> [RT_MENU_PAGE=root|display|graphics|controllers|device|buttons|quit|reset|
// save|load|save_confirm|load_confirm|state_message] [RT_MENU_AT=<vblank>]: open the menu at that
// vblank, save a picture of it and quit (for UI reviews without a person). RT_MENU_LOAD=<slot>
// loads that save state first (the menu opens 30 vblanks after); RT_MENU_SLOT=<n> focuses a slot;
// RT_MENU_SAVE=1 (page save) saves into it from the open menu before the picture.

namespace rt::ui
{
    void updatePauseMenu();  // every frame, before drawing: input, open/close, pause
    bool pauseMenuWantsFrame(); // something to draw this frame (menu, controllers, first-run hint)
    void drawPauseMenu();    // inside the shared ImGui frame
    void menuShotAfterFrame(); // after the ImGui frame is rendered (RT_MENU_SHOT)
    bool pauseMenuOpen();
    void togglePauseMenu();    // as Guide does (the second screen's Menu button)
    void openOptions();        // the Options page from the top (the game's Pause > Settings)
    void openSoundOptions();   // the Options page at its Sound rows (the game's Title > Options)
}
