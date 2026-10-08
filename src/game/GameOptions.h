#pragma once

// The game's own Options (Title > Options: Vibration / Speaker / Sound Volume) handed to the app:
// the title menu's Options opens our menu at its Sound rows instead (a hook in overrides.cpp),
// and our settings are what the game uses. The game never saved its own; ours persist. The town's
// Start menu (Pause > Settings, the game's button setup) opens our menu the same way.

#include <string>

class PS2Runtime;

namespace rt::game
{
    // From the game thread (the hooks): the player chose Title > Options (our Options at its Sound
    // rows, which is all the game's had) or Pause > Settings (our Options from the top).
    enum class OptionsRequest { None, Sound, Top };
    void requestOptions(OptionsRequest where);
    // Once per host frame: what was asked for since the last call.
    OptionsRequest takeOptionsRequest();
    // Once per host frame: the game's vibration switch follows our setting ([options] in
    // config/game_state.toml: u8 0x33590E, 4 on, 0 off; the boot code sets 2).
    void applyGameOptions(PS2Runtime &runtime);

    // The town radio (the game's Pause > Radio page, which stays; [music] and [options] in
    // config/game_state.toml). The station is the progress block's byte +0x12DD (saved with the
    // game): 0 off, 1 PEACH FM (IOP play_tune 0), 2 E-RADIO (tune 1).
    enum RadioStation { kRadioOff = 0, kRadioPeachFm = 1, kRadioERadio = 2, kRadioStations = 3 };
    const char *radioStationName(int station); // "Off", "PEACH FM", "E-RADIO"
    // Whether there is a station to choose: an Adventure game is loaded (not the demo).
    bool radioAvailable(PS2Runtime &runtime);
    int radioStation(PS2Runtime &runtime); // the station chosen (as the game's Radio page shows it)
    // Any thread: choose a station, as the game's Radio page does. The byte is written at once;
    // while driving in a town the stream switches on the game thread at the town's next frame
    // (the game's own SNDMOD calls), and elsewhere the game starts it when the town (re)starts its
    // radio (out of the Pause menu, into a town). Returns false (nothing done) when unavailable.
    bool setRadioStation(PS2Runtime &runtime, int station);
    // Game thread (the town task's hook): a station chosen since the last call (-1 = none).
    int takeRadioRequest();
    // Test socket: {"cmd":"radio"} reports, {"cmd":"radio","station":N} switches. Reply:
    // {"ok":..,"available":..,"station":N,"name":..}.
    std::string radioCommand(PS2Runtime &runtime, const std::string &line);
}

