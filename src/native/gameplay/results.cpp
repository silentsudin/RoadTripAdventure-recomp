#include "gameplay/results.h"

#include <algorithm>
#include <limits>

namespace rt::native::gameplay {
namespace {

bool beatsRecord(std::uint32_t time, std::uint32_t record) {
    return time != kBestTimeNone && (record == kBestTimeNone || time < record);
}

}  // namespace

RaceResult buildRaceResult(const RaceTracker& tracker, int courseIndex) {
    const bool validCourse = courseIndex >= 0 &&
                             static_cast<std::size_t>(courseIndex) < kBestTimeEntries;
    const std::uint32_t totalFrames = tracker.totalFrames();
    // The tracker takes these notifications, so inspect a copy to preserve the
    // original completed-race snapshot for other result consumers.
    RaceTracker snapshot = tracker;

    // RaceTracker compares current lap/total times against the records loaded
    // from SaveData. Its notifications therefore encode the strict comparison
    // required for a new record, including rejecting tied times.
    return {
        getRaceTimeRank(totalFrames),
        totalFrames,
        tracker.bestLap(),
        validCourse && snapshot.takeNewBestLap(),
        validCourse && snapshot.takeNewBestTotal(),
    };
}

void recordRaceResult(SaveData& save, int courseIndex, const RaceResult& result) {
    if (courseIndex < 0) return;
    const auto index = static_cast<std::size_t>(courseIndex);

    if (index < kRaceResultCount) {
        const int rank = rankFromResult(result);
        save.raceResults[index] = static_cast<std::uint8_t>(
            std::clamp(rank, 0, static_cast<int>(std::numeric_limits<std::uint8_t>::max() - 1)));
    }
    if (index < kBestTimeEntries) {
        if (beatsRecord(result.bestLap, save.bestLap[index])) save.bestLap[index] = result.bestLap;
        if (beatsRecord(result.totalFrames, save.bestTotal[index])) save.bestTotal[index] = result.totalFrames;
    }
}

int rankFromResult(const RaceResult& result) {
    return result.rank;
}

}  // namespace rt::native::gameplay
