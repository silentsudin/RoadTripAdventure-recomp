// Desktop/native regression control socket.  This intentionally implements the
// JSON-lines vocabulary used by tests/regression/rtharness, not PS2 guest RAM.
#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace rt::native::runtime {

struct TestPad {
    std::uint16_t buttons = 0xffff; // active-low DualShock words
    int lx = 128, ly = 128, rx = 128, ry = 128;
    bool connected = true;
};

class TestSocket {
public:
    using Command = std::function<std::string(const std::string&, const std::string&)>;
    TestSocket();
    ~TestSocket();
    TestSocket(const TestSocket&) = delete;
    TestSocket& operator=(const TestSocket&) = delete;

    // RT_TEST_SOCKET=tcp:<port>; Android also accepts @roadtrip-test.
    bool startIfRequested(Command command);
    void stop();
    [[nodiscard]] bool enabled() const;
    [[nodiscard]] bool lockstep() const;
    // Called by the host loop. Returns true for exactly one requested 60-Hz tick.
    bool takeStep();
    void completeStep();
    [[nodiscard]] TestPad pad() const;
    [[nodiscard]] std::uint64_t vblank() const;
private:
    struct Impl;
    Impl* impl_ = nullptr;
};

} // namespace rt::native::runtime
