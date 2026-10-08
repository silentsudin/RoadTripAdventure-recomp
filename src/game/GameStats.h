#pragma once

// Road Trip's progress and live race state, read from EE RAM for the second screen. Addresses
// mirror config/game_state.toml (the reference, with the evidence); reads are unsynchronised with
// the game thread, which is fine for display.

#include <bitset>
#include <cstdint>
#include <string>
#include <vector>

class PS2Runtime;

namespace rt::game
{
    struct Stats
    {
        // Adventure (the progress block; valid once a save is loaded or a game started).
        bool adventure = false;
        std::string playerName, currency;
        uint32_t money = 0;
        uint32_t mileageMetres = 0;
        int licence = 0; // 0 none, 1 B, 2 A, 3 Super A
        int location = 0; // town of the save point (config towns.names)
        uint32_t skiJumpBest = 0;
        std::bitset<100> stamps;
        uint8_t raceResults[35] = {}; // place - 1 per race-table entry, 0xFF = not raced
        int coins = 0;          // Choro Q coins found, of 100
        int townsVisited = 0;   // of 10 (My Garage .. My City)
        int mail = 0, unreadMail = 0;
        // Driving in a town (live): which, the time of day, houses there not visited yet.
        bool demo = false;   // the attract demo (or the title after it): nothing of the player's
        bool inTown = false; // or the field areas between towns
        bool canDrive = false; // the car takes input (drive_flags 0x04): not in a building, a dialogue or a menu
        int town = 0;        // location: 1..9 towns, 10..21 field areas
        int tile = 0;        // the map tile the car is in (FLD tile code)
        std::string townLabel;
        int hour = 0, minute = 0;
        int housesLeft = 0;
        int photos = 0;                     // Quick-Pic Shop photos taken, of 100
        int townPhotos = 0, townPhotosTaken = 0;
        float carX = 0, carZ = 0;           // our car in the town's coordinates
        int mapZoom = 50;                   // the minimap's scale (50 near, 25 far)
        // A race (any mode): our place, the field, laps and times (frames at 60 per second).
        bool racing = false;
        enum class Mode : uint8_t { Adventure, QuickRace, TwoPlayer } mode = Mode::QuickRace;
        int place = 0, entrants = 0; // place 1-based
        int lap = 0, laps = 0;       // the lap being driven (1-based, capped at laps)
        bool finished = false;
        int countdown = 0;           // 2..5 lights, 6 GO, 0 after
        uint32_t totalFrames = 0, lapFrames = 0, bestLapFrames = 0; // best: of the laps done (0 = none yet)
        uint32_t lastLapFrames = 0, bestBeforeLast = 0; // the lap just finished, and the best before it
        std::string course;
    };

    Stats readStats(PS2Runtime &runtime);

    // The town radio (Pause > Radio): what's on now. Read from the sound driver (SNDMOD) in IOP
    // RAM; songs are named from our own tracklist (titles and artists only, [music] in
    // config/game_state.toml), empty where unknown.
    struct NowPlaying
    {
        bool playing = false;
        std::string station; // PEACH FM, E-RADIO
        std::string title, artist;
        float elapsed = 0, length = 0; // seconds into the song, and its length (0 = unknown)
    };
    NowPlaying nowPlaying(PS2Runtime &runtime);

    // Stamp n's caption (1..100) as the game's notebook words it, or "" when unknown.
    std::string stampCaption(PS2Runtime &runtime, int n);

    // Stamp n's picture (1..100) as 64x64 RGBA8 from the user's disc (SYS/STAMP.GSL), or empty.
    std::vector<uint8_t> stampIcon(int n);

    // The buildings of a town (from the game's door and resident tables in RAM), for the map.
    struct Place
    {
        enum class Kind : uint8_t { Factory, Shop, PhotoBooth, House, Other } kind;
        std::string name;  // the resident or shop ("Quick-Pic Shop No.5", "Kevin's mom")
        char letter = 0;   // shops: P (parts), B (body), C (colour: paint)
        int photo = 0;     // photo booths: the photo number
        float x = 0, z = 0;   // in the current map tile's coordinates (as the car's)
        bool done = false;    // photo taken / house visited
        bool here = false;    // of the location we're in (the counts); else a neighbour's
    };
    // The places around us: the location we're in and the neighbouring map tiles' (the field areas
    // between towns are tiles of their own, with their own doors).
    std::vector<Place> placesAround(PS2Runtime &runtime, const Stats &st);
    // Where a world point lands on the town minimap (640x224 field pixels, before widescreen).
    void mapPoint(const Stats &st, float x, float z, float &fx, float &fy);

    // m'ss"hh as the game's HUD shows times.
    std::string raceTime(uint32_t frames);

    const char *townName(int index);
    const char *licenceName(int licence);
}
