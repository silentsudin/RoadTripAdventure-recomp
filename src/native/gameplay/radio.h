// Authored radio-station model for Road Trip Adventure (SLUS-20398).
// Pure C++, no SDL/audio output, no PS2/guest types; guest addresses appear in
// comments only. Everything the audio host needs to make save byte
// `radio_station` audible: enum, save-byte validation, the authentic stereo
// VAG pair per station, and the town-frame lifecycle.
//
// Evidence (see docs/native-reference/radio-stations.md for the full ledger):
//   CONFIRMED (direct ELF read, SLUS_203.98, fileOffset = vaddr - 0x1FF000):
//   - Station byte at progress+0x12DD == EE 0x01780A3D: read
//     `lb v0, 0x12dd(0x0177F760)` + signed `blez` in ApplySoundVolumes
//     0x2590B0 (0x2590F8..0x259100); written `sb` by the options/radio handler
//     FUN_002137B0 at 0x21390C when the menu commits a station.
//   - ApplySoundVolumes 0x2590B0: pushes master volume (cmd 0x31, amp at
//     progress+0x12DE), then station > 0 -> RPC cmd 0x33 with `station - 1`
//     (SoundRpcCmd33PlaySe 0x258A48, delay slot `addiu a0,a0,-1` at 0x259100)
//     followed by cmd 0x34 commit (0x258AF0); station <= 0 returns without
//     sending a stream request.
//   - ApplySoundVolumes callers: StartTownModeTask 0x210430 (jal 0x210488),
//     UpdateTownExplorationTask 0x2105B8 (jal 0x2106C0, 0x2107C8), and the
//     options/radio handler 0x2137B0 (jal 0x213860) - the radio only becomes
//     audible through town code (audio-map.md:249-253).
//   - Boot registers exactly three stereo VAG pairs via cmd 0x32 on driver
//     slots {0, 2, 1} in code order, from gs_music_file_table rows 4..9:
//     slot 0 = 1CH_L/R (LBN 228654/192175), slot 2 = 2CH_L/R (240904/179788),
//     slot 1 = 3CH_L/R (204239/216475) - all six LBNs match
//     disc/.lbn_map.tsv (audio-map.md:84-93, CONFIRMED at 0x257004/14/24 with
//     s2 = 0x29AE20 = table row 4).
//   INFERRED (doc-grounded, driver-internal and not confirmable from the ELF):
//   - station 1 (PEACH FM) -> cmd 0x33 index 0 -> registered slot 0 = 1CH pair;
//     station 2 (E-RADIO) -> index 1 -> slot 1 = 3CH pair (audio-map.md:257-262,
//     8 item 5). The index->slot consumption happens inside SNDMOD.
//   - slot 2 (2CH pair) is registered but no station byte (documented 0..2)
//     reaches it: unused/hidden on this disc. No station maps to it here.
//   AUTHORED (host policy, labeled as such in the module doc):
//   - The "deferred apply": a station request becomes audible at the next
//     town-frame boundary (the original re-reads the byte in town code;
//     menu-commit also applies immediately at 0x213920/0x213A30).
//   - Pair paths are disc-root-relative ("SOUND/1CH_L.VAG"), matching
//     disc/.lbn_map.tsv names; the host joins them onto its disc root.
#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>

namespace rt::native::gameplay {

// Radio station enum: the exact progress+0x12DD byte values. Oracle match:
// rt::game::RadioStation (GameOptions.h:27) { kRadioOff = 0, kRadioPeachFm = 1,
// kRadioERadio = 2, kRadioStations = 3 } and radioStationName()
// (GameOptions.cpp:59-67) -> 0 "Off", 1 "PEACH FM", 2 "E-RADIO". The
// save-layout.md field map agrees (0 off, 1 Peach FM, 2 E-Radio, PROVEN). The
// save model stores the same values in SaveData::radioStation (u8);
// stationToSaveByte() round-trips the two representations.
enum class RadioStation : std::uint8_t {
    Off = 0,      // "Off"     - no radio stream (stop request on commit)
    PeachFm = 1,  // "PEACH FM" - 1CH stereo pair, driver slot 0
    ERadio = 2,   // "E-RADIO"  - 3CH stereo pair, driver slot 1
};

// Highest documented station byte (save-layout.md migration spec:
// "radio 0x12DD in 0..2"). The host's station cycler stays inside this domain
// too: (station + d + kRadioStations) % kRadioStations over 0..2
// (PauseMenu.cpp:538-541), with setRadioStation rejecting anything outside
// 0..kRadioStations-1 (GameOptions.cpp:85) - 0..2 is the complete value list.
inline constexpr std::uint8_t kRadioStationMax = 2;

// One authentic stereo radio stream: the L/R VAG pair boot-registers for a
// station's driver slot (audio-map.md SS2). Paths are disc-root-relative;
// the audio host joins them onto its disc root (`RT_DISC` /
// ~/Library/Application Support/RoadTripRecomp/disc), like the TSQ loader.
struct RadioVagPair {
    std::filesystem::path left;
    std::filesystem::path right;
};

// True when `raw` is a documented station byte (0..2).
bool isValidRadioByte(std::uint8_t raw);

// Validates/clamps a raw progress+0x12DD byte into the enum. Out-of-range
// values clamp to kRadioStationMax, mirroring the save model's
// migrateToNative normalization (save.cpp: `clampU8(radioStation, 2, ...)`),
// so stationFromSaveByte and the save model agree byte-for-byte. `clamped`
// (optional out) reports whether clamping happened.
RadioStation stationFromSaveByte(std::uint8_t raw, bool* clamped = nullptr);

// Trivial enum -> save byte (SaveData::radioStation) conversion.
std::uint8_t stationToSaveByte(RadioStation station);

// Station -> authentic stereo VAG pair (disc-root-relative paths):
//   PeachFm -> {SOUND/1CH_L.VAG, SOUND/1CH_R.VAG}   (driver slot 0)
//   ERadio  -> {SOUND/3CH_L.VAG, SOUND/3CH_R.VAG}   (driver slot 1)
//   Off     -> std::nullopt (no stream; Off stops any stream request)
// File names verified against audio-map.md SS2/SS3 and present in
// disc/.lbn_map.tsv. The station->pair binding is INFERRED
// (audio-map.md SS5/SS8.5); the pair's existence, slot order, and disc identity
// are CONFIRMED/PROVEN.
std::optional<RadioVagPair> stationToVagPair(RadioStation station);

// What one town-frame apply tells the audio host to do (audio-map.md SS5:
// ApplySoundVolumes 0x2590B0 sends cmd 0x33 with station-1 + cmd 0x34 commit
// for station > 0, and the stop request FUN_00258b30 / cmd 0x35 for 0).
enum class RadioStreamAction {
    None,   // nothing to change (no pending station, or same station re-applied)
    Start,  // (re)start the stream for `pair`, replacing any current radio stream
            // (original: RPC cmd 0x33 (station-1) + cmd 0x34 commit)
    Stop,   // stop the radio stream (Off took effect; original stop request)
};

// Result of one town-frame apply (RadioState::onTownFrame).
struct RadioApply {
    RadioStation station = RadioStation::Off;      // effective station after apply
    RadioStreamAction action = RadioStreamAction::None;
    bool changed = false;                          // station changed at this boundary
    std::optional<RadioVagPair> pair;              // set when action == Start
};

// Radio lifecycle state machine. Lifetime of a station value:
//   1. A save byte or station request sets the *setting* (the persisted byte,
//      as the original's options handler writes it at commit time).
//   2. Nothing is audible until the next town-frame boundary, when
//      onTownFrame() applies the setting (the original only consults the byte
//      from town code: StartTownModeTask/UpdateTownExplorationTask ->
//      ApplySoundVolumes 0x2590B0).
//   3. Applying Off stops the stream request; applying a station > 0 starts
//      (or restarts, on a station change) that station's pair.
class RadioState {
public:
    // Loads the setting from a raw progress+0x12DD byte (typically
    // SaveData::radioStation). Nothing is applied yet: a save byte of 1/2
    // starts its pair at the first town frame, 0 starts nothing - matching
    // "the radio only runs in town".
    explicit RadioState(std::uint8_t saveByte = 0);

    // Requests a station. It becomes pending (deferred): audible at the next
    // town-frame boundary. Last request wins.
    void requestStation(RadioStation station);

    // Same as requestStation(stationFromSaveByte(raw)); the save/options UI
    // path, clamping exactly like the save model.
    void requestSaveByte(std::uint8_t raw);

    // One town frame: applies a pending station and reports what the audio
    // host must do. With no pending station this is a no-op (the original
    // re-sends the stream request from town code, but nothing observable
    // changes when the station has not).
    RadioApply onTownFrame();

    // ---- query API consumed by the audio host ----
    // Currently effective (audible) station.
    [[nodiscard]] RadioStation currentStation() const { return applied_; }
    // Pending station if a request has not been applied yet (nullopt = none).
    // This is what should still take effect at the next town frame.
    [[nodiscard]] std::optional<RadioStation> pendingStation() const;
    // Authentic VAG pair for the effective station (nullopt = Off / no
    // stream). The audio host (re)starts the SoundEngine on this pair.
    [[nodiscard]] std::optional<RadioVagPair> pathsForCurrent() const {
        return stationToVagPair(applied_);
    }
    // The setting byte as the save model stores it (progress+0x12DD): the
    // requested station, written back to SaveData::radioStation.
    [[nodiscard]] std::uint8_t saveByte() const { return stationToSaveByte(setting_); }

private:
    RadioStation setting_ = RadioStation::Off;   // persisted byte value
    RadioStation applied_ = RadioStation::Off;  // effective/audible station
    bool pending_ = false;                       // setting_ not yet applied at a town frame
};

}  // namespace rt::native::gameplay
