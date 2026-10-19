// Pins W11-35: CalculateTachometerValue (0x249520) clamp/scale incl. negative
// short and the (p2||p3) clamp gate, and CalculateSpeedometerValue (0x249458)
// F15-vector magnitude -> 216x -> clamp 0xba -> scaled by p2 (VU0 shim path).
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t CalculateTachometerValue(uint32_t q0 = 0, uint32_t q1 = 0, uint32_t q2 = 0, ...);
uint32_t CalculateSpeedometerValue(uint32_t q0 = 0, uint32_t q1 = 0, uint32_t q2 = 0, ...);
}  // namespace guest

int main() {
  std::memset(rtguest::kRam, 0, sizeof(rtguest::kRam));

  // tach: rpm short at +0x1d0
  *(int16_t*)rtguest::G(0x004001d0) = 20000;
  assert(guest::CalculateTachometerValue(0x00400000, 0, 0) == 20000);   // no clamp gate
  assert(guest::CalculateTachometerValue(0x00400000, 0, 1) == 10000);   // p3 opens gate, no scale
  assert(guest::CalculateTachometerValue(0x00400000, 2, 0) == 2);       // (10000*2+9999)/10000
  *(int16_t*)rtguest::G(0x004001d0) = -1000;
  assert(guest::CalculateTachometerValue(0x00400000, 2, 0) == 0);       // 7999/10000 trunc

  // speedo: F15 vector raw 0x8000 in x -> 1.0 -> sqrt 1 * 216 = 216.0
  *(uint32_t*)rtguest::G(0x004000f0) = 0x8000;
  *(float*)rtguest::G(0x00332f44) = 1.0f;
  assert(guest::CalculateSpeedometerValue(0x00400000, 0, 0) == 216);    // gate closed: unclamped
  assert(guest::CalculateSpeedometerValue(0x00400000, 0, 1) == 0xba);   // clamped, unscaled
  assert(guest::CalculateSpeedometerValue(0x00400000, 2, 0) == 2);      // (186*2+185)/186
  return 0;
}
