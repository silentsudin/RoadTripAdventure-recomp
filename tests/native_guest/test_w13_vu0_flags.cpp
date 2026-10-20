// W13-DIV class 2 pins: VU0 macro FMAC ops set MAC/status like the oracle
// (ps2xRecomp vu_translator.cpp:22-38 fmacFlagTarget: add/sub/mul/madd/opm forms
// on fd or ACC; max/mini do not), and the 0021d470 vf0 clip test still works.
#undef NDEBUG  // asserts must stay live in Release
#include "../../src/native/guest/guest_vu0.h"
#include <cassert>
#include <cstring>

using namespace rtguest::vu0;

static void reset() { S = State{}; S.vf[0] = {{0.f, 0.f, 0.f, 1.f}}; }

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);
  // vmaddbc: ACC + fs*ft.x -> fd; x=0 (Z), y<0 (S), z,w positive.
  reset(); S.acc = {{0.f, -2.f, 1.f, 1.f}}; S.vf[1] = {{0.f, 1.f, 1.f, 1.f}}; S.vf[2] = {{1.f, 0, 0, 0}};
  vmaddbc(0b1111, 0, 3, 1, 2);
  assert(S.mac == ((1u << 3) | (1u << (2 + 4))) && S.status == (0x001u | 0x040u | 0x002u | 0x080u));
  // ACC-target ops flag the ACC result; dest mask selects lanes (.y only here).
  reset(); S.vf[1] = {{-1.f, -1.f, 1.f, 1.f}}; S.vf[2] = {{1.f, 1.f, 1.f, 1.f}};
  vmulabc(0b0100, 0, 1, 2);
  assert(S.mac == (1u << (2 + 4)) && (S.status & 0x0ffu) == 0x082u);
  // sticky bits survive a later flag-setting op that clears the live ones.
  vadd(0b1000, 4, 2, 2);
  assert(S.mac == 0u && S.status == 0x080u);
  // max/mini never touch flags.
  reset(); S.vf[1] = {{-1.f, 0.f, 0.f, 0.f}};
  vmaxbc(0b1111, 0, 5, 1, 1); vminibc(0b1111, 0, 5, 1, 1);
  assert(S.mac == 0u && S.status == 0u);
  // the rest of the FMAC set flags: vmul, vmulbc, vaddbc, vmulq, vaddq, vmaddabc, vopmula, vopmsub
  auto flagsZx = [](auto op) { reset(); op(); return (S.mac & 0x8u) != 0u; };
  assert(flagsZx([] { vmul(0b1000, 3, 0, 0); }));
  assert(flagsZx([] { vmulbc(0b1000, 0, 3, 0, 0); }));
  assert(flagsZx([] { vaddbc(0b1000, 0, 3, 0, 0); }));
  assert(flagsZx([] { vmulq(0b1000, 3, 0); }));
  assert(flagsZx([] { vaddq(0b1000, 3, 0); }));
  assert(flagsZx([] { vmaddabc(0b1000, 0, 0, 0); }));
  assert(flagsZx([] { vopmula(0b1000, 0, 0); }));
  assert(flagsZx([] { vopmsub(0b1000, 3, 0, 0); }));
  // 0021d470 clip test shape: ctc2 status 0 then vsub into vf0; vf0 restored, sticky S read.
  reset(); S.vf[12] = {{-5.f, 1.f, 1.f, 1.f}}; S.status = 0x3c0u; ctc2(16, 0);
  { const Vu4 vf0 = S.vf[0]; vsub(0b1101, 0, 12, 0); S.vf[0] = vf0; }
  assert((cfc2(16) & 0xc0u) == 0xc0u && S.vf[0].v[3] == 1.f);
  return 0;
}
