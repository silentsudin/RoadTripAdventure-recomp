// Pins computed from build/decomp/fn/00271c50.c, 00271c60.c and 002720c8.c.
#include "../../src/native/guest/guest_shim.h"

#include <cassert>

namespace guest {
uint32_t xorHighBitPair(uint32_t, ...);
uint32_t charCodeToTileCoord(uint32_t, ...);
uint32_t charCodeToGlyphIndex(uint32_t, ...);
}

int main() {
  // ushort argument/result: upper input bits do not survive the helper.
  assert(guest::xorHighBitPair(0x1234) == 0x92b4);
  assert(guest::xorHighBitPair(0xabcd1234) == 0x92b4);

  // High-byte normal range, high-byte >= 0xa0, and low-byte carry paths.
  assert(guest::charCodeToTileCoord(0x719f) == 0x0221);
  assert(guest::charCodeToTileCoord(0xa09e) == 0xdf7e);
  assert(guest::charCodeToTileCoord(0x719e) == 0x017e);

  // ASCII stays low-byte; valid and invalid double-byte glyphs follow the two branches.
  assert(guest::charCodeToGlyphIndex(0x0041) == 0x41);
  assert(guest::charCodeToGlyphIndex(0x2421) == 0x80);
  assert(guest::charCodeToGlyphIndex(0x2474) == 0x7f);
}
