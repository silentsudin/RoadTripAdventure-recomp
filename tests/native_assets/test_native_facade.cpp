#include "runtime/guest_facade.h"
#include <cassert>
#include <cstring>

int main() {
    using namespace rt::native;
    gameplay::GameSession session;
    gameplay::FrameSnapshot snapshot;
    int warped = 0;
    runtime::GuestFacade facade(session, snapshot, [&](int id) { warped = id; });
    std::string error;
    auto clock = facade.read(runtime::GuestFacade::kClock, 4, error);
    assert(error.empty() && clock.size() == 4);
    auto miss = facade.read(0xdeadbeef, 4, error);
    assert(!error.empty() && miss == std::vector<std::uint8_t>(4));
    const std::uint32_t garage = 11;
    std::vector<std::uint8_t> bytes(4); std::memcpy(bytes.data(), &garage, 4);
    assert(facade.write(0x00335910, bytes, error));
    assert(facade.tile() == 11 && warped == 11);
    const std::uint32_t peach = 43;
    std::memcpy(bytes.data(), &peach, 4);
    assert(facade.write(0x00335910, bytes, error));
    assert(facade.tile() == 43 && warped == 43);
}
