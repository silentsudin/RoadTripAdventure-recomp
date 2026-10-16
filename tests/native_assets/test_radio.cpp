// Contract tests for the authored radio-station model (radio.{h,cpp}).
// Deterministic and synthetic: no game data, no audio output. The
// station -> VAG pair expectations are copied from
// docs/native-reference/audio-map.md SS2/SS5 (boot-registered stereo pairs,
// saved-station verdict), NOT read from the disc.
#include "../../src/native/gameplay/radio.h"
#include "../../src/native/gameplay/save.h"

#include <cstdio>
#include <vector>

using namespace rt::native::gameplay;

static int g_fail = 0;
static int g_checks = 0;
#define CHECK(c) do { ++g_checks; if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

// Expectation table copied from audio-map.md (SS5 "radio is the streaming VAG
// pairs" + SS2 boot registration: slot 0 = 1CH, slot 1 = 3CH, slot 2 = 2CH but
// unreachable by a station byte). Off has no pair: it stops the stream.
namespace {
struct ExpectedPair {
    RadioStation station;
    const char* left;
    const char* right;
};
constexpr ExpectedPair kExpectedPairs[] = {
    {RadioStation::PeachFm, "SOUND/1CH_L.VAG", "SOUND/1CH_R.VAG"},
    {RadioStation::ERadio, "SOUND/3CH_L.VAG", "SOUND/3CH_R.VAG"},
};
}  // namespace

static void testByteValidationAndClamp() {
    // Documented bytes: 0 off / 1 PEACH FM / 2 E-RADIO (save-layout.md).
    bool clamped = true;
    CHECK(stationFromSaveByte(0, &clamped) == RadioStation::Off);
    CHECK(!clamped);
    CHECK(stationFromSaveByte(1, &clamped) == RadioStation::PeachFm);
    CHECK(!clamped);
    CHECK(stationFromSaveByte(2, &clamped) == RadioStation::ERadio);
    CHECK(!clamped);
    CHECK(isValidRadioByte(0) && isValidRadioByte(1) && isValidRadioByte(2));
    CHECK(stationToSaveByte(RadioStation::Off) == 0);
    CHECK(stationToSaveByte(RadioStation::PeachFm) == 1);
    CHECK(stationToSaveByte(RadioStation::ERadio) == 2);

    // Out-of-range bytes clamp to 2 (E-RADIO), exactly like the save model's
    // migrateToNative clampU8(radioStation, 2) - see save.cpp.
    for (unsigned raw = 3; raw <= 255; ++raw) {
        CHECK(stationFromSaveByte(static_cast<std::uint8_t>(raw), &clamped) == RadioStation::ERadio);
        CHECK(clamped);
        CHECK(!isValidRadioByte(static_cast<std::uint8_t>(raw)));
    }

    // Byte -> enum -> byte round-trips for every documented value.
    for (unsigned raw = 0; raw <= 2; ++raw) {
        const RadioStation station = stationFromSaveByte(static_cast<std::uint8_t>(raw));
        CHECK(stationToSaveByte(station) == raw);
    }
}

static void testClampAgreesWithSaveModel() {
    // An out-of-range byte travels through the save model's own parse ->
    // migrateToNative normalization; radio::stationFromSaveByte must land on
    // the same station for the two models to stay consistent.
    std::vector<std::uint8_t> raw(kSaveSize, 0);
    raw[save_offset::kRadioStation] = 5;
    SaveResult parsed = parseSave(raw);
    CHECK(parsed.ok);
    CHECK(parsed.data.radioStation == 5);  // parse alone does not clamp
    std::vector<std::string> changes = migrateToNative(parsed.data);
    CHECK(parsed.data.radioStation == 2);
    CHECK(stationToSaveByte(stationFromSaveByte(5)) == parsed.data.radioStation);
    CHECK(!changes.empty());  // the radio clamp is one of the reported changes

    // The save field table documents the byte's offset and label.
    bool found = false;
    for (const SaveFieldRegion& region : saveFieldRegions()) {
        if (region.offset == save_offset::kRadioStation) {
            found = true;
            CHECK(region.size == 1);
        }
    }
    CHECK(found);
}

static void testStationToVagPairMatchesAudioMap() {
    for (const ExpectedPair& expected : kExpectedPairs) {
        std::optional<RadioVagPair> pair = stationToVagPair(expected.station);
        CHECK(pair.has_value());
        CHECK(pair->left == expected.left);
        CHECK(pair->right == expected.right);
    }
    // Off carries no stream: pair is absent and stops a stream request.
    CHECK(!stationToVagPair(RadioStation::Off).has_value());

    // Slot 2 (the 2CH pair) is registered at boot but no station byte reaches
    // it (audio-map.md SS5: "no station value reaches it -> unused/hidden on
    // this disc"): no station may expose a 2CH path.
    for (const ExpectedPair& expected : kExpectedPairs) {
        const std::optional<RadioVagPair> pair = stationToVagPair(expected.station);
        CHECK(pair->left.string().find("2CH") == std::string::npos);
        CHECK(pair->right.string().find("2CH") == std::string::npos);
    }

    // Disc-root-relative form, matching the disc inventory names
    // (SOUND/<name>.VAG) so the host joins them onto its disc root.
    for (const ExpectedPair& expected : kExpectedPairs) {
        const std::optional<RadioVagPair> pair = stationToVagPair(expected.station);
        CHECK(pair->left.parent_path() == "SOUND");
    }
}

static void testDeferredApplyUntilTownFrame() {
    // Save byte 1 (PEACH FM): setting is PeachFm, but nothing is audible until
    // the first town frame (StartTownModeTask/UpdateTownExplorationTask are
    // the ApplySoundVolumes callers - the radio only runs in town).
    RadioState radio(1);
    CHECK(radio.currentStation() == RadioStation::Off);
    CHECK(radio.pendingStation().has_value() && *radio.pendingStation() == RadioStation::PeachFm);
    CHECK(!radio.pathsForCurrent().has_value());
    CHECK(radio.saveByte() == 1);

    // A town frame with no stream running yet starts the save station's pair.
    const RadioApply first = radio.onTownFrame();
    CHECK(first.station == RadioStation::PeachFm);
    CHECK(first.changed);
    CHECK(first.action == RadioStreamAction::Start);
    CHECK(first.pair.has_value());
    CHECK(first.pair->left == "SOUND/1CH_L.VAG" && first.pair->right == "SOUND/1CH_R.VAG");
    CHECK(radio.currentStation() == RadioStation::PeachFm);
    CHECK(!radio.pendingStation().has_value());
    CHECK(radio.pathsForCurrent().has_value());

    // Station switch: pending immediately, audible only at the next boundary.
    radio.requestStation(RadioStation::ERadio);
    CHECK(radio.saveByte() == 2);  // the byte commits at request time
    CHECK(radio.pendingStation().has_value() && *radio.pendingStation() == RadioStation::ERadio);
    CHECK(radio.currentStation() == RadioStation::PeachFm);  // still playing
    const std::optional<RadioVagPair> stillPlaying = radio.pathsForCurrent();
    CHECK(stillPlaying->left == "SOUND/1CH_L.VAG");
    const RadioApply next = radio.onTownFrame();
    CHECK(next.changed);
    CHECK(next.action == RadioStreamAction::Start);
    CHECK(next.station == RadioStation::ERadio);
    CHECK(next.pair->left == "SOUND/3CH_L.VAG" && next.pair->right == "SOUND/3CH_R.VAG");
    CHECK(radio.currentStation() == RadioStation::ERadio);
    CHECK(!radio.pendingStation().has_value());
    CHECK(radio.pathsForCurrent()->left == "SOUND/3CH_L.VAG");

    // No pending request: town frames change nothing (idempotent once
    // applied; the original's per-frame re-apply is not observable here).
    const RadioApply idle = radio.onTownFrame();
    CHECK(idle.station == RadioStation::ERadio);
    CHECK(!idle.changed);
    CHECK(idle.action == RadioStreamAction::None);
    CHECK(!idle.pair.has_value());

    // Requesting the station already playing is a no-op apply, not a restart.
    radio.requestStation(RadioStation::ERadio);
    CHECK(radio.pendingStation().has_value());
    const RadioApply same = radio.onTownFrame();
    CHECK(same.station == RadioStation::ERadio);
    CHECK(!same.changed);
    CHECK(same.action == RadioStreamAction::None);
    CHECK(!same.pair.has_value());
    CHECK(radio.pathsForCurrent()->left == "SOUND/3CH_L.VAG");

    // Same-tick switch arbitration (last-write-wins): with ERadio audible,
    // two requests in one tick must land on the FINAL request - the
    // intermediate ERadio request is superseded by the later PeachFm one,
    // which applies as a real stream change.
    radio.requestStation(RadioStation::ERadio);
    radio.requestStation(RadioStation::PeachFm);
    CHECK(radio.saveByte() == 1);  // the byte holds the final request's value
    const RadioApply lastWins = radio.onTownFrame();
    CHECK(lastWins.station == RadioStation::PeachFm && lastWins.changed &&
          lastWins.action == RadioStreamAction::Start);
    CHECK(lastWins.pair->left == "SOUND/1CH_L.VAG" && lastWins.pair->right == "SOUND/1CH_R.VAG");
    CHECK(radio.pathsForCurrent()->left == "SOUND/1CH_L.VAG");

    // ...and the reverse same-tick order flips the winner: [PeachFm, ERadio]
    // while PeachFm is audible -> ERadio wins with a real stream change, not
    // the first candidate. (Regression guard: an early-return on the first
    // request, or any first-write-wins arbitration, fails here.)
    radio.requestStation(RadioStation::PeachFm);
    radio.requestStation(RadioStation::ERadio);
    CHECK(radio.saveByte() == 2);
    const RadioApply lastWinsSwitch = radio.onTownFrame();
    CHECK(lastWinsSwitch.station == RadioStation::ERadio && lastWinsSwitch.changed &&
          lastWinsSwitch.action == RadioStreamAction::Start);
    CHECK(lastWinsSwitch.pair->left == "SOUND/3CH_L.VAG" && lastWinsSwitch.pair->right == "SOUND/3CH_R.VAG");
    CHECK(radio.pathsForCurrent()->left == "SOUND/3CH_L.VAG");

    // A same-tick pair whose final request equals the applied station is a
    // no-op: the byte only ever held one value at a time, and it is the final
    // request's - ERadio (already the applied station) re-applies unchanged.
    radio.requestStation(RadioStation::PeachFm);
    radio.requestStation(RadioStation::ERadio);
    CHECK(radio.saveByte() == 2);
    const RadioApply lastWinsNoOp = radio.onTownFrame();
    CHECK(lastWinsNoOp.station == RadioStation::ERadio);
    CHECK(!lastWinsNoOp.changed);
    CHECK(lastWinsNoOp.action == RadioStreamAction::None);
    CHECK(radio.pathsForCurrent()->left == "SOUND/3CH_L.VAG");

    // A single deferred request still switches the stream at the boundary.
    radio.requestStation(RadioStation::PeachFm);
    const RadioApply singleSwitch = radio.onTownFrame();
    CHECK(singleSwitch.station == RadioStation::PeachFm && singleSwitch.changed &&
          singleSwitch.action == RadioStreamAction::Start);
    CHECK(singleSwitch.pair->left == "SOUND/1CH_L.VAG");
}

static void testOffSemantics() {
    // Off at construction: nothing pending, nothing to start.
    RadioState radio(0);
    CHECK(radio.currentStation() == RadioStation::Off);
    CHECK(!radio.pendingStation().has_value());
    const RadioApply offStart = radio.onTownFrame();
    CHECK(!offStart.changed && offStart.action == RadioStreamAction::None);
    CHECK(!radio.pathsForCurrent().has_value());

    // Off stops the stream request, but only at the town-frame boundary: the
    // pair keeps playing until the apply (options-handler commit for station 0
    // sends cmd 0x35 stop in the original).
    radio.requestSaveByte(1);
    CHECK(radio.onTownFrame().action == RadioStreamAction::Start);
    radio.requestSaveByte(0);
    CHECK(radio.pendingStation().has_value() && *radio.pendingStation() == RadioStation::Off);
    CHECK(radio.currentStation() == RadioStation::PeachFm);
    CHECK(radio.pathsForCurrent()->left == "SOUND/1CH_L.VAG");  // audible until apply
    const RadioApply stop = radio.onTownFrame();
    CHECK(stop.changed);
    CHECK(stop.action == RadioStreamAction::Stop);
    CHECK(stop.station == RadioStation::Off);
    CHECK(!stop.pair.has_value());
    CHECK(radio.currentStation() == RadioStation::Off);
    CHECK(!radio.pathsForCurrent().has_value());
    CHECK(radio.saveByte() == 0);

    // Off applied to an already-off radio: no observable change.
    radio.requestSaveByte(0);
    const RadioApply alreadyOff = radio.onTownFrame();
    CHECK(!alreadyOff.changed && alreadyOff.action == RadioStreamAction::None);

    // Off after E-RADIO also stops, with no new pair.
    RadioState eradio(2);
    const RadioApply start = eradio.onTownFrame();
    CHECK(start.action == RadioStreamAction::Start);
    CHECK(start.pair->left == "SOUND/3CH_L.VAG");
    eradio.requestStation(RadioStation::Off);
    const RadioApply stop2 = eradio.onTownFrame();
    CHECK(stop2.action == RadioStreamAction::Stop && stop2.changed);

    // A suppressed out-of-range byte clamps to E-RADIO, never to Off.
    RadioState clampedByte(9);
    CHECK(clampedByte.pendingStation().has_value());
    CHECK(*clampedByte.pendingStation() == RadioStation::ERadio);
}

static void testSaveModelRoundTrip() {
    // SaveData::radioStation <-> radio model round trip, both directions, via
    // the model's own serialize/parse cycle.
    SaveData save;
    save.radioStation = stationToSaveByte(RadioStation::PeachFm);
    CHECK(save.radioStation == 1);
    std::vector<std::uint8_t> bytes = serializeSave(save);
    CHECK(bytes.size() == kSaveSize);
    CHECK(bytes[save_offset::kRadioStation] == 1);
    SaveResult reloaded = parseSave(bytes);
    CHECK(reloaded.ok);
    CHECK(reloaded.data.radioStation == stationToSaveByte(RadioStation::PeachFm));
    CHECK(stationFromSaveByte(reloaded.data.radioStation) == RadioStation::PeachFm);

    // Full path: save byte -> RadioState -> town apply -> save back after an
    // in-session station change.
    RadioState radio(reloaded.data.radioStation);
    const RadioApply applied = radio.onTownFrame();
    CHECK(applied.station == RadioStation::PeachFm);
    CHECK(applied.pair->left == "SOUND/1CH_L.VAG");

    radio.requestStation(RadioStation::ERadio);
    SaveData savedBack;
    savedBack.radioStation = radio.saveByte();  // the setting commits at request
    CHECK(savedBack.radioStation == 2);
    std::vector<std::uint8_t> bytes2 = serializeSave(savedBack);
    CHECK(bytes2[save_offset::kRadioStation] == 2);
    SaveResult roundTripped = parseSave(bytes2);
    CHECK(roundTripped.data.radioStation == 2);
    CHECK(stationFromSaveByte(roundTripped.data.radioStation) == RadioStation::ERadio);
    RadioState reloadedRadio(roundTripped.data.radioStation);
    const RadioApply reapplied = reloadedRadio.onTownFrame();
    CHECK(reapplied.station == RadioStation::ERadio);
    CHECK(reapplied.pair->left == "SOUND/3CH_L.VAG" && reapplied.pair->right == "SOUND/3CH_R.VAG");

    // Off round-trips too.
    SaveData offSave;
    offSave.radioStation = 0;
    SaveResult offParsed = parseSave(serializeSave(offSave));
    CHECK(offParsed.ok);
    CHECK(stationFromSaveByte(offParsed.data.radioStation) == RadioStation::Off);
    RadioState offRadio(offParsed.data.radioStation);
    CHECK(offRadio.currentStation() == RadioStation::Off);
    CHECK(offRadio.onTownFrame().action == RadioStreamAction::None);
}

int main() {
    testByteValidationAndClamp();
    testClampAgreesWithSaveModel();
    testStationToVagPairMatchesAudioMap();
    testDeferredApplyUntilTownFrame();
    testOffSemantics();
    testSaveModelRoundTrip();
    if (g_fail) { std::fprintf(stderr, "%d failure(s)\n", g_fail); return 1; }
    std::printf("radio: all %d checks passed\n", g_checks);
    return 0;
}
