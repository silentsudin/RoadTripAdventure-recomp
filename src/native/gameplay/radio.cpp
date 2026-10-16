// Implementation of the authored radio-station model. See radio.h for the
// evidence ledger and docs/native-reference/radio-stations.md for the
// module contract and the audio-host glue notes.
#include "gameplay/radio.h"

namespace rt::native::gameplay {

bool isValidRadioByte(std::uint8_t raw) {
    return raw <= kRadioStationMax;
}

RadioStation stationFromSaveByte(std::uint8_t raw, bool* clamped) {
    if (clamped) *clamped = raw > kRadioStationMax;
    // Clamp to the maximum documented value, mirroring the save model's
    // migrateToNative policy (save.cpp clampU8(radioStation, 2)): byte 0x03
    // .. 0xFF -> 2 (E-RADIO). The original apply path tests the byte signed
    // (`blez` in ApplySoundVolumes 0x2590B0), and the original writer only
    // produces 0..2 (options handler 0x2137B0), so the save-layout migration
    // rule ("radio 0x12DD in 0..2") is the native policy modeled here.
    if (raw > kRadioStationMax) raw = kRadioStationMax;
    return static_cast<RadioStation>(raw);
}

std::uint8_t stationToSaveByte(RadioStation station) {
    return static_cast<std::uint8_t>(station);
}

std::optional<RadioVagPair> stationToVagPair(RadioStation station) {
    switch (station) {
        case RadioStation::PeachFm:
            // Boot registers the 1CH pair on driver slot 0 from
            // gs_music_file_table rows 4/5 (startLBN 228654/192175;
            // SOUND/1CH_L.VAG + SOUND/1CH_R.VAG). Station 1 = PEACH FM.
            return RadioVagPair{"SOUND/1CH_L.VAG", "SOUND/1CH_R.VAG"};
        case RadioStation::ERadio:
            // Boot registers the 3CH pair on driver slot 1 from rows 8/9
            // (startLBN 204239/216475; SOUND/3CH_L.VAG + SOUND/3CH_R.VAG).
            // Station 2 = E-RADIO.
            return RadioVagPair{"SOUND/3CH_L.VAG", "SOUND/3CH_R.VAG"};
        case RadioStation::Off:
            // Off stops the stream request: no pair. (Options handler commit
            // for station 0 sends cmd 0x35 stop; ApplySoundVolumes sends no
            // stream request.)
            return std::nullopt;
    }
    return std::nullopt;  // unreachable for the documented enum range
}

RadioState::RadioState(std::uint8_t saveByte)
    : setting_(stationFromSaveByte(saveByte)), pending_(setting_ != RadioStation::Off) {}

void RadioState::requestStation(RadioStation station) {
    setting_ = station;
    pending_ = true;
}

void RadioState::requestSaveByte(std::uint8_t raw) {
    requestStation(stationFromSaveByte(raw));
}

RadioApply RadioState::onTownFrame() {
    RadioApply apply;
    apply.station = applied_;
    if (!pending_) return apply;  // no change at this town-frame boundary

    pending_ = false;
    apply.station = setting_;
    apply.changed = setting_ != applied_;
    applied_ = setting_;

    if (apply.changed) {
        if (applied_ == RadioStation::Off) {
            apply.action = RadioStreamAction::Stop;
        } else {
            apply.action = RadioStreamAction::Start;
            apply.pair = stationToVagPair(applied_);
        }
    }
    return apply;
}

std::optional<RadioStation> RadioState::pendingStation() const {
    if (!pending_) return std::nullopt;
    return setting_;
}

}  // namespace rt::native::gameplay
