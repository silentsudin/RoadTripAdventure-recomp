// Pins W11-30: vec4 position helpers (ToRelativeVec4 0x21c728, FixedPosToFloatVec4
// 0x21c7f0, StoreIndexedPosition 0x21c928), the camera-preset table scan with the
// unsigned-byte index fix (CycleCameraPresetState 0x21e328: pseudocode reads
// *(byte*) for the table index — an int8_t carry would change the slot math), and
// the Print family end-to-end through uiDebugText8's queue accounting.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t ToRelativeVec4(uint32_t q0 = 0, uint32_t q1 = 0, uint32_t q2 = 0, uint32_t q3 = 0, ...);
uint32_t FixedPosToFloatVec4(uint32_t q0 = 0, uint32_t q1 = 0, ...);
uint32_t StoreIndexedPosition(uint32_t q0 = 0, uint32_t q1 = 0, uint32_t q2 = 0, uint32_t q3 = 0,
                              uint32_t q4 = 0, ...);
uint32_t CycleCameraPresetState(uint32_t q0 = 0, uint32_t q1 = 0, ...);
void PrintVec3Norm(uint32_t q0 = 0, int q1 = 0, ...);
}  // namespace guest

static void setf(uint32_t a, float f) { *(float*)rtguest::G(a) = f; }
static uint32_t rd32(uint32_t a) { return *(uint32_t*)rtguest::G(a); }
static void wr32(uint32_t a, uint32_t v) { *(uint32_t*)rtguest::G(a) = v; }
static void wr16(uint32_t a, uint32_t v) { *(uint16_t*)rtguest::G(a) = (uint16_t)v; }

int main() {
  std::memset(rtguest::kRam, 0, sizeof(rtguest::kRam));

  // --- ToRelativeVec4: 27-bit wrap + DAT_00332c1c scale ---------------------
  setf(0x00332c1c, 4.0f);
  wr32(0x00400000, 0x00123456);  // pos.x
  wr32(0x00400004, 0x0000ABCD);  // pos.y
  wr32(0x00400008, 0x080000FF);  // pos.z: bit27 set -> sign-extend via |0xf8000000
  guest::ToRelativeVec4(0x00400100, 0x00400000, 0x10, 0);
  assert(rd32(0x00400100) == 0x4891A230u);  // (0x123456-0x10)/4 = 298257.5
  assert(rd32(0x00400104) == 0x462BCD00u);  // 0xABCD/4
  assert(rd32(0x00400108) == 0xCBFFFFE0u);  // (int)0xF80000FF /4 = -33554368.25
  assert(rd32(0x0040010C) == 0x3F800000u);  // w = 1.0

  // --- FixedPosToFloatVec4: 25-bit position with 0x2000000 carry bit --------
  setf(0x00332c20, 8.0f);
  wr32(0x00400200, 0x00123450);
  wr32(0x00400204, 0x00000055);
  wr32(0x00400208, 0x02345678);
  guest::FixedPosToFloatVec4(0x00400300, 0x00400200);
  assert(rd32(0x00400300) == 0x4A091A28u);  // (0x123450+0x1000000)&0x1ffffff / 8
  assert(rd32(0x00400304) == 0x412A0000u);  // 0x55/8
  assert(rd32(0x00400308) == 0x48D159E0u);  // (0x2345678&0x1ffffff)/8
  assert(rd32(0x0040030C) == 0x3F800000u);

  // --- StoreIndexedPosition: slot 2 -> car block offset 0x4e0 ---------------
  setf(0x00332c24, 16.0f);
  guest::StoreIndexedPosition(0x3FC00000u, 0x40200000u, 0xC0500000u, 2, 0x77);
  assert(rd32(0x00177ACF0 + 0x4E0) == 0x18u);   // 1.5*16
  assert(rd32(0x00177ACF4 + 0x4E0) == 0x28u);   // 2.5*16
  assert(rd32(0x00177ACF8 + 0x4E0) == 0xFFFFFFCCu);  // (int)(-3.25*16) = -52
  assert(rd32(0x00177ACE0 + 0x4E0) == 0x3FC00000u);  // gs_race_cars_address raw bits
  assert(rd32(0x00177ACE4 + 0x4E0) == 0x40200000u);
  assert(rd32(0x00177ACE8 + 0x4E0) == 0xC0500000u);
  assert(*(uint16_t*)rtguest::G(0x00177AE24 + 0x4E0) == 0x77u);

  // --- CycleCameraPresetState: +0x27 = 0x80 must index unsigned -------------
  // early-return guard reads ushort at 0x177ade8 + (0x80*0x28-0x80)*8*2 = +0x13800
  // (zeroed) -> proceeds. state +0x25 = 1 -> next preset 2, uVar6 = 0x20;
  // block tags: block0=0x111, block3 (+0x750) = 0x20 -> match at iVar7 = 3.
  wr32(0x00400010, 0x00200000);  // task owner ptr for InitializeTask
  *(uint8_t*)rtguest::G(0x00400025) = 1;
  *(uint8_t*)rtguest::G(0x00400027) = 0x80;
  wr32(0x0040001C, 0);
  wr16(0x00177ADE8, 0x111);
  wr16(0x00177ADE8 + 0x750, 0x20);
  guest::CycleCameraPresetState(0x00400000, 0x00401000);
  assert(*(uint8_t*)rtguest::G(0x00400025) == 2);
  assert(*(uint8_t*)rtguest::G(0x00400027) == 3);

  // --- PrintVec3Norm (z==0 branch) through uiDebugText8 accounting ----------
  std::memset(rtguest::G(0x0032FC08), 0, 16);
  std::strcpy((char*)rtguest::G(0x0032FC08), "%f,%f,%f");
  wr32(0x00402000, 0x3F800000u);  // 1.0
  wr32(0x00402004, 0x40000000u);  // 2.0
  wr32(0x00402008, 0);            // 0 -> plain branch
  wr32(0x00335680, 0x00900000);   // debug text queue base
  wr32(0x00335684, 0);            // queue cursor
  wr32(0x00335688, 0);            // attr latch
  guest::PrintVec3Norm(0x00402000, 0);
  assert(rd32(0x00335688) == 0xFu);
  // "1.000000,2.000000,0.000000" = 26 chars: header (+1) then +5 per glyph.
  assert(rd32(0x00335684) == 1 + 5 * 26);

  return 0;
}
