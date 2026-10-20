// guest:0x00207d98 translated from Ghidra pseudocode and VU0 disassembly.
#include "../guest_vu0.h"
#include "guest_decls.inc"

namespace guest {
uint32_t eeQuadBuild(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(272);
  using namespace rtguest::vu0;
  S.vf[1] = lqc2(param_2); S.vf[2] = lqc2(param_2 + 16);
  vadd(15, 1, 1, 2);
  S.vf[1] = S.vf[0]; vaddbc(8, 3, 1, 0, 0);
  S.vf[2] = S.vf[0]; vaddbc(4, 3, 2, 0, 0);
  S.vf[3] = S.vf[0]; vaddbc(2, 3, 3, 0, 0);
  if (*static_cast<float*>(rtguest::G(param_2 + 16)) != 0.f) {
    if (*static_cast<float*>(rtguest::G(param_2 + 20)) == 0.f) S.vf[1] = S.vf[2];
    else S.vf[1] = S.vf[3];
  }
  uint32_t frame = rtguest::allocFrame(256);
  // The stack layout below is the packed quad layout Ghidra reconstructs.
  auto w = [&](uint32_t o, uint32_t v) { *static_cast<uint32_t*>(rtguest::G(frame + o)) = v; };
  auto r = [&](uint32_t o) { return *static_cast<uint32_t*>(rtguest::G(param_2 + o)); };
  auto sum = S.vf[1]; uint32_t s[4]; std::memcpy(s, &sum, 16);
  w(0x00, r(0)); w(0x04, r(4)); w(0x08, r(8)); w(0x0c, r(12));
  w(0x10, s[0]); w(0x14, s[1]); w(0x18, s[2]); w(0x1c, 0x3f800000);
  w(0x20, r(32)); w(0x24, r(36)); w(0x28, r(40)); w(0x2c, r(44));
  w(0x30, r(48)); w(0x34, r(52)); w(0x38, r(56)); w(0x3c, r(60));
  eeQuadTransform(param_1, frame);
  return 0;
}
RT_GUEST_FN(0x00207d98, guest::eeQuadBuild)
}  // namespace guest
