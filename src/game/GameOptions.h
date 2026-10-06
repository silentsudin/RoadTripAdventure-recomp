#pragma once

// The game's own Options (Title > Options: Vibration / Speaker / Sound Volume) handed to the app:
// the title menu's Options opens our menu at its Sound rows instead (a hook in overrides.cpp),
// and our settings are what the game uses. The game never saved its own; ours persist.

class PS2Runtime;

namespace rt::game
{
    // From the game thread (the hook): the player chose Options on the title menu.
    void requestOptions();
    // Once per host frame: whether that happened since the last call.
    bool takeOptionsRequest();
    // Once per host frame: the game's vibration switch follows our setting ([options] in
    // config/game_state.toml: u8 0x33590E, 4 on, 0 off; the boot code sets 2).
    void applyGameOptions(PS2Runtime &runtime);
}
