#pragma once

// The app going to the background and coming back (Android: another activity in front, home, the
// screen off). Away, the game is paused and the audio device closed, so the app uses no CPU and
// plays nothing (and Android has little reason to kill it); back, both resume, unless the player
// had paused the game with the in-game menu. Desktop windows keep running when minimized.
namespace rt::lifecycle
{
    // Watches SDL's lifecycle events (they arrive on Android's UI thread while the app's own loop is
    // blocked by SDL). Call once SDL is initialised.
    void install();
    // Whether the app is in the background now.
    bool away();
}
