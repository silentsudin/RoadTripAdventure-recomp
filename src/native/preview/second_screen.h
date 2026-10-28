#pragma once

// Native second-screen surface (Map / Notebook / Radio), the twin of the recomp's
// src/ui/SecondScreen.cpp (ui-parity.md "Second screen"). Data-only like ui_menu:
// no SDL, no Vulkan; the caller composites `quads` then `text`.
//
// Page choice and the tap-override rule live in gameplay::SecondScreenPages
// (map_view.h); this module turns one page + the game state into quads.
// Coordinates are window pixels (the second display is its own output, not the
// 640x448 frame). S = uiDesignScale(height) like every other recomp panel.
//
// Deviations from the recomp (none changes a label): stamp cards have no art (the
// stamp icons come from SYS/STAMP.GSL, not extracted natively yet; the card keeps
// its frame and shows the caption "Stamp N"), and the map shows our reconstructed
// roads/markers instead of a diverted GS layer.

#include "gameplay/map_view.h"
#include "gameplay/radio.h"
#include "gameplay/save.h"
#include "preview/ui_menu.h"

#include <array>
#include <bitset>
#include <cstdint>
#include <string>
#include <vector>

namespace rt::native::preview {

struct SecondScreenData {
    // Context.
    bool racing = false;
    bool mapUp = false;
    bool inTown = false;
    bool canDrive = false;
    bool adventure = false;
    bool mapHere = true;  // the map router sent the map to this screen

    // Race column.
    std::string course;
    int countdown = 0;  // 2..5 "Get ready" ..., 6 = GO (recomp's values); 0 = none
    int lap = 0, laps = 0, place = 0;
    bool finished = false;
    std::uint32_t lastLapFrames = 0, bestBeforeLast = 0, bestLapFrames = 0;  // 60 Hz frames, 0 = none
    bool newBestLap = false;  // a new best flashes "New best lap!" for a moment

    // Town column.
    std::string townLabel;
    int hour = 0, minute = 0, housesLeft = 0;
    int townPhotos = 0, townPhotosTaken = 0;  // Quick-Pic photos of this town

    // Notebook.
    std::string playerName, currency;
    std::uint8_t licence = 0;
    std::uint32_t money = 0, mileageM = 0;
    int coins = 0, townsVisited = 0, photos = 0;
    std::bitset<100> stamps;
    bool stampsGot = true;  // Got (else To get)
    int newStamp = 0;       // stamp number earned a moment ago (1..100), 0 = none
    float newStampSince = 0.f;  // seconds since it arrived
    float scroll = 0.f;     // pixels, caller-owned

    // Radio.
    bool radioAvailable = false;
    gameplay::RadioStation station = gameplay::RadioStation::Off;
    bool nowPlaying = false;
    std::string title, artist;
    int elapsedSec = 0, lengthSec = 0;

    // Map: projected draw list in the unit square, null = no map to draw.
    const gameplay::MapProjectedFrame* map = nullptr;

    bool menuOpen = false;  // the pill reads "Close"
    std::string stateWords;  // diagnostic tail of the debug line
};

// Fills the notebook / radio / town fields from a save (Adventure assumed).
void fillSecondScreenFromSave(SecondScreenData& data, const gameplay::SaveData& save);

// Quick-Pic photo numbers that belong to the current town -> "Photos here".
void setSecondScreenTownPhotos(SecondScreenData& data, const gameplay::SaveData& save, const std::vector<int>& photoNumbers);

// The unit label the recomp prints: lap time "1'23"45", "--'--"--" when none.
std::string secondScreenRaceTime(std::uint32_t frames);
// Delta note for the last lap against the best before it: "+0"02" / "New best -0"02".
std::string secondScreenLapNote(std::uint32_t lastLapFrames, std::uint32_t bestBeforeLast, bool* newBest);

UiScreen buildSecondScreen(int w, int h, gameplay::SecondScreenPage page, const SecondScreenData& data);

// "racing 1 town 0 ..." (the recomp's [second] line) for the diagnostic log.
std::string secondScreenDebugLine(const SecondScreenData& data, gameplay::SecondScreenPage page, int autoPage,
                                  bool overridden, int chosenPage, bool haveMap);
// The line as printed ("[second] ..."), without the trailing newline.
std::string secondScreenDebugPrint(const std::string& line);
// The recomp's diagnostic switch (RT_SECOND_DEBUG=1), owned here with the line.
bool secondScreenDebugEnabled();

// Tracks lap times from race snapshots: feed each logic step.
class SecondScreenLapTracker {
public:
    // `now` is any monotonic seconds clock; a best lap faster than the previous one flashes for 2.5 s.
    void note(int lap, std::uint32_t elapsedFrames, std::uint32_t bestLap, double now = 0);
    bool newBestFlash(double now) const { return now - bestAt_ < 2.5; }
    std::uint32_t last() const { return last_; }
    std::uint32_t bestBeforeLast() const { return bestBefore_; }
    void reset() { *this = SecondScreenLapTracker{}; }
private:
    int lap_ = 0;
    std::uint32_t lapStart_ = 0, last_ = 0, bestBefore_ = 0, best_ = 0;
    double bestAt_ = -10;
};

// Notices the stamp earned while we watch: exactly one new bit (loading a save changes many).
// It holds the screen for 3.5 s (SecondScreen.cpp:674-695).
class SecondScreenStampWatch {
public:
    void observe(const std::bitset<100>& stamps, double now);
    void reset() { valid_ = false; recent_ = 0; }
    int recent(double now) const { return recent_ && now - at_ < 3.5 ? recent_ : 0; }
    float since(double now) const { return static_cast<float>(now - at_); }
private:
    std::bitset<100> seen_;
    bool valid_ = false;
    int recent_ = 0;
    double at_ = -10;
};

// At most 30 pictures a second, at once for a touch (SecondScreen.cpp:848-857).
class SecondScreenThrottle {
public:
    bool due(double now, bool touch);
private:
    double last_ = 0;
};

}  // namespace rt::native::preview
