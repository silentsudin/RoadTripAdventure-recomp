#pragma once

#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>

namespace rt::native::gameplay::events {

// The town pre-control VM and the pad-pattern VM are different languages.
// No ELF evidence maps town bytes 0x8A/0x8B to radio/camera commands.
enum class Dialect { TownPrefix, Rumble };
enum class Type {
    Unknown, SmallOn, SmallOff, LargeValue, LargeOff, LargeInitial, Hold, End,
    // Explicit native triggers ONLY; these are not claimed ROM opcode mappings.
    Radio, Countdown, CameraPreset, Subtitle, FlagSet, FlagClear
};
struct Event {
    static constexpr std::size_t kMaxPayload = 256;
    Type type = Type::Unknown;
    std::uint8_t opcode = 0;
    std::uint8_t width = 0; // Includes opcode; zero means UNKNOWN/not a bytecode record.
    std::uint32_t address = 0;
    std::uint32_t value = 0;
    bool valueKnown = false; // 0x84's strength is external runtime state, UNKNOWN here.
    std::array<std::uint8_t,kMaxPayload> payload{};
    std::size_t payloadSize = 0;
};
enum class Status { Exhausted, End, QueueFull, Truncated, Unsupported, StepLimit };
struct PumpResult {
    Status status = Status::Exhausted;
    std::size_t records = 0;
    bool logOnce = false;
    Event record{}; // Faulting raw byte; width zero if not established.
};
struct Callbacks {
    std::function<void(const Event&)> onEvent;
    std::function<void(std::uint32_t)> onRadio;
    std::function<void(bool)> onCountdown;
    std::function<void(std::uint32_t)> onCameraPreset;
};

class EventBridge {
public:
    static constexpr std::size_t kCapacity = 64;
    // Cursor belongs to this stream instance: retain it between frames. Never
    // scan text for high bytes. Use Rumble ONLY for proven pad-pattern pointers.
    // Full queue/truncation/unsupported leave the faulting cursor unchanged.
    PumpResult pump(Dialect dialect, std::span<const std::uint8_t> stream,
                    std::size_t& cursor, std::uint32_t baseAddress = 0,
                    std::size_t maxRecords = kCapacity);
    // Native UI/game-state producers can queue owned subtitle bytes and explicit
    // radio/countdown/camera/flag triggers without pretending they came from ROM.
    bool enqueue(Type type, std::uint32_t value = 0,
                 std::span<const std::uint8_t> payload = {});
    // Pop BEFORE callbacks, dispatch once. Enqueues during callbacks are deferred
    // to the next drain; nested drains return zero. Callbacks must not throw.
    std::size_t drain(const Callbacks& callbacks);
    std::size_t size() const { return size_; }
private:
    bool push(const Event& event);
    std::array<Event,kCapacity> queue_{};
    std::size_t head_ = 0, size_ = 0;
    bool draining_ = false;
    // One diagnostic per reason/dialect/opcode over this bridge's lifetime.
    std::bitset<1024> diagnosed_{};
};

} // namespace rt::native::gameplay::events
