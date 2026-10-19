#include "../../src/native/guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t gifChainEndTag(uint32_t, uint32_t, ...);
uint32_t gifPkBufferInit(uint32_t, uint32_t, ...);
uint32_t gifPkBufferReset(uint32_t, ...);
uint32_t gifPkAlign16(uint32_t, ...);
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);

  constexpr uint32_t tag = 0x1000;
  guest::gifChainEndTag(tag, 0x1234);
  assert(*static_cast<uint16_t*>(rtguest::G(tag)) == 0x1234);
  assert(*static_cast<uint8_t*>(rtguest::G(tag + 3)) == 0x70);
  assert(*static_cast<uint32_t*>(rtguest::G(tag + 4)) == 0);

  constexpr uint32_t packet = 0x1100;
  guest::gifPkBufferInit(packet, 0x1204);
  assert(*static_cast<uint32_t*>(rtguest::G(packet)) == 0x1204);
  assert(*static_cast<uint32_t*>(rtguest::G(packet + 4)) == 0x1204);
  assert(*static_cast<uint32_t*>(rtguest::G(packet + 8)) == 0);
  guest::gifPkBufferReset(packet);
  assert(*static_cast<uint32_t*>(rtguest::G(packet)) == 0x1204);

  *static_cast<uint32_t*>(rtguest::G(packet)) = 0x1204;
  *static_cast<uint32_t*>(rtguest::G(packet + 8)) = 0x1200;
  *static_cast<int32_t*>(rtguest::G(0x1200)) = 0x100;
  assert(guest::gifPkAlign16(packet) == 0x1210);
  assert(*static_cast<uint32_t*>(rtguest::G(packet)) == 0x1210);
  assert(*static_cast<uint32_t*>(rtguest::G(packet + 8)) == 0);
  assert(*static_cast<int32_t*>(rtguest::G(0x1200)) == 0x100);
}
