// Explicit compatibility facade for selected regression guest addresses.
#pragma once
#include "gameplay/game_loop.h"
#include "gameplay/session.h"
#include <cstdint>
#include <functional>
#include <string>
#include <vector>
namespace rt::native::runtime {
class GuestFacade {
public:
    // Addresses are compatibility names, not claims that this app has EE RAM.
    static constexpr std::uint32_t kCars = 0x0177ACE0, kRacePlace = 0x015B0088;
    static constexpr std::uint32_t kClock = 0x0021BD20 + 0x14;
    static constexpr std::uint32_t kGravity = 0x0021BD20 - 0x59;
    static constexpr std::uint32_t kVibration = 0x0033590E;
    GuestFacade(gameplay::GameSession& session, gameplay::FrameSnapshot& snapshot,
                std::function<void(int)> warp);
    std::vector<std::uint8_t> read(std::uint32_t address, std::size_t length, std::string& error) const;
    bool write(std::uint32_t address, const std::vector<std::uint8_t>& data, std::string& error);
    // JSON-lines adapter used by TestSocket for read/write/map/state commands.
    std::string command(const std::string& command, const std::string& line);
    int tile() const { return tile_; }
    void setTile(int id);
private:
    gameplay::GameSession& session_; gameplay::FrameSnapshot& snapshot_;
    std::function<void(int)> warp_; int tile_ = 43; std::uint8_t rumble_ = 4;
};
} // namespace rt::native::runtime
