#pragma once

// The player's save states: four slots in <data>/states/slot<N>.rtstate (the runtime's format,
// runtime/ps2_save_state.h), each with a picture of the moment (a PNG in the file's header), where
// it was taken and when. The in-game menu (src/ui/PauseMenu) saves and loads through here, and so
// do the test socket's state_slot commands (main.cpp), so tests drive the menu's own path.
//
// A save or load is asked for, then driven by update() (any thread, once per host frame or test
// step) until it is done: while the menu holds the game paused, update() lets vblanks through one
// at a time (behind the menu) until the scheduler reaches a point where the machine can be saved
// (at most kMaxVblanks: movies and memory card work block it), then the game holds again.

#include <cstdint>
#include <string>
#include <vector>

class PS2Runtime;

namespace rt::states
{
    constexpr int kSlots = 4;
    constexpr uint32_t kMaxVblanks = 120;

    struct Slot
    {
        int index = 0;             // 1..kSlots
        bool empty = true;         // no file
        bool readable = false;     // the header could be read (else `problem` says why)
        std::string problem;       // as the player reads it
        std::string place;         // "Peach Town", a course name, or ""
        std::string mode;          // "town", "race", "menu"
        std::string raceMode;      // "adventure", "quick", "2p" (races)
        std::string player;        // the Adventure's name
        int64_t savedUnixTime = 0;
        uint64_t stamp = 0;        // changes whenever the file does (textures, caches)
        // The picture (RGBA8, already decoded), empty without one.
        std::vector<uint8_t> thumbnail;
        int thumbWidth = 0, thumbHeight = 0;

        std::string title() const;  // "Peach Town", "Empty"
        std::string detail() const; // "Adventure, driving in town"
        std::string when() const;   // "Today, 14:32"
    };

    // The slots as the files are now (headers re-read when they changed; cheap).
    std::vector<Slot> slots();
    std::string slotPath(int index);

    // Whether the player can save and load now; `why` says why not (not shown in the menu: the
    // rows are simply not there). Needs the GS to support it (Capabilities::saveStates) and the
    // game in play: an Adventure or a race of the player's, no movie.
    bool available(PS2Runtime &runtime, std::string *why = nullptr);

    enum class Phase
    {
        Idle,
        Picture,  // save: waiting for the picture of the moment
        Checking, // load: reading and checking the file
        Waiting,  // waiting for a savable point (vblanks let through behind the menu)
        Writing,  // save: compressing and writing the file
        Settling, // load: a couple of frames so the picture shows the loaded moment
        Done,
        Failed,
    };

    struct Status
    {
        Phase phase = Phase::Idle;
        bool saving = false;  // else loading
        int slot = 0;
        bool refused = false; // load: the file was refused before anything changed
        std::string message;  // done/failed: as the player reads it
        std::string error;    // the runtime's own words (logs, tests)
        uint32_t vblanks = 0; // let through behind the menu so far
        uint64_t sequence = 0; // bumps when an operation ends
    };

    // Starts a save into / a load from slot `index` (false if one is already running). A load
    // reads and checks the file on a worker (`checkNow`: here, for the test socket).
    bool beginSave(int index);
    bool beginLoad(int index, bool checkNow = false);
    void update();
    Status status();
    bool busy();
    // Clears a finished operation's Done/Failed status.
    void acknowledge();

    // The picture to put in the next saved state's header (the metadata provider takes it).
    std::vector<std::pair<std::string, std::string>> takeSaveMetadata();

    // The loader's reason, as the player reads it.
    std::string friendlyLoadError(const std::string &error);
}
