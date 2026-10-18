#include "gameplay/event_bridge.h"

#include <algorithm>

namespace rt::native::gameplay::events {

bool EventBridge::push(const Event& event) {
    if (size_ == kCapacity) return false;
    queue_[(head_ + size_) % kCapacity] = event;
    ++size_;
    return true;
}

bool EventBridge::enqueue(Type type, std::uint32_t value,
                          std::span<const std::uint8_t> payload) {
    if (payload.size() > Event::kMaxPayload) return false;
    Event e;
    e.type = type;
    e.value = value;
    e.valueKnown = true;
    e.payloadSize = payload.size();
    std::copy(payload.begin(),payload.end(),e.payload.begin());
    return push(e);
}

PumpResult EventBridge::pump(Dialect dialect, std::span<const std::uint8_t> stream,
                             std::size_t& cursor, std::uint32_t baseAddress,
                             std::size_t maxRecords) {
    PumpResult r;
    while (cursor < stream.size()) {
        Event e;
        e.opcode = stream[cursor];
        e.address = baseAddress + static_cast<std::uint32_t>(cursor);
        auto fault = [&](Status status) {
            r.status = status;
            r.record = e;
            const auto reason = status == Status::Truncated ? 512u : 0u;
            const auto key = reason + (dialect == Dialect::Rumble ? 256u : 0u) + e.opcode;
            r.logOnce = !diagnosed_[key];
            diagnosed_.set(key);
            return r;
        };
        // InterpretScriptCommands @23B140 returns at ANY byte >=20. It is
        // unsafe to infer an event width here: these may be SJIS glyph bytes.
        if (dialect != Dialect::Rumble) return fault(Status::Unsupported);
        if (r.records == maxRecords) { r.status = Status::StepLimit; return r; }
        // Pad-pattern interpreter @20A708..20A7F8. The default arm writes
        // the byte to the hold counter; even 85..8B have proven width ONE.
        e.width = e.opcode == 0x82 ? 2 : 1;
        switch (e.opcode) {
            case 0x80: e.type = Type::SmallOn; break;
            case 0x81: e.type = Type::SmallOff; break;
            case 0x82: e.type = Type::LargeValue; break;
            case 0x83: e.type = Type::LargeOff; break;
            case 0x84: e.type = Type::LargeInitial; break;
            case 0xff: e.type = Type::End; break;
            default: e.type = Type::Hold; e.value = e.opcode; e.valueKnown = true; break;
        }
        if (stream.size() - cursor < e.width) return fault(Status::Truncated);
        if (e.width == 2) {
            e.payloadSize = 1;
            e.payload[0] = stream[cursor + 1];
            e.value = e.payload[0];
            e.valueKnown = true;
        }
        if (!push(e)) { r.status = Status::QueueFull; return r; }
        cursor += e.width;
        ++r.records;
        if (e.type == Type::End) { r.status = Status::End; return r; }
    }
    // A caller cursor beyond the supplied buffer is malformed, not an empty
    // valid stream. No dereference or cursor arithmetic is attempted.
    r.status = cursor == stream.size() ? Status::Exhausted : Status::Truncated;
    return r;
}

std::size_t EventBridge::drain(const Callbacks& cb) {
    if (draining_) return 0;
    draining_ = true;
    struct Reset { bool& b; ~Reset() { b = false; } } reset{draining_};
    const std::size_t count = size_;
    for (std::size_t i=0;i<count;++i) {
        const Event e = queue_[head_];
        head_ = (head_ + 1) % kCapacity;
        --size_;
        if (cb.onEvent) cb.onEvent(e);
        switch (e.type) {
            case Type::Radio: if (cb.onRadio) cb.onRadio(e.value); break;
            case Type::Countdown: if (cb.onCountdown) cb.onCountdown(e.value != 0); break;
            case Type::CameraPreset: if (cb.onCameraPreset) cb.onCameraPreset(e.value); break;
            default: break;
        }
    }
    return count;
}

} // namespace rt::native::gameplay::events
