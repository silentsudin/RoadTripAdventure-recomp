// guest:0x0020cdc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

// W13 hand port: Ghidra lost the jump table at 0x20ce1c (`(&PTR_LAB_002ea690)[loadState]()`;
// the regex pass double-dereferenced it into an unregistered rtCall -> 0, so loadStep's state
// climbed forever and the 0x30000 town load never finished). Table 0x2ea690 = {20ce28, 20ce28,
// 20ce58, 20cea0, 20d0e8, 20cf58, 20cf98, 20d0e8}; the case bodies 0x20ce28..0x20d0f0 below are
// read off the disasm (Ghidra mis-split 0x20d008 out of case 6 as "HandleInputEvent").
namespace guest {

static void rebaseTexWalk(uint32_t p, uint64_t keep, int32_t tbpAdd, uint32_t faultRa) {
  while ((*(uint8_t*)rtguest::G(p + 3)) != 0x70) {
    if ((*(uint8_t*)rtguest::G(p + 3)) != 0x10) RT_CALLRA(faultRa, guest::fatal(0x2ea490));
    uint64_t v = (*(uint64_t*)rtguest::G(p + 0x20));
    uint64_t t = (uint64_t)(uint32_t)(((int32_t)((uint32_t)(v >> 32) & 0x3fff) + tbpAdd) & 0x3fff);
    (*(uint64_t*)rtguest::G(p + 0x20)) = (v & keep) | (t << 32);
    p = p + (uint32_t)(*(uint16_t*)rtguest::G(p)) * 0x10 + 0x10;
  }
}

uint64_t DispatchInputMapping(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
  }
  uint32_t& loadState = (*(uint32_t*)rtguest::G(0x3356c0));

  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) == '\0') && (loadState == 0)) {
    loadState = 5;
  }
  if (7 < loadState) {
    return 0;
  }
  switch (loadState) {
  case 0:
  case 1: {  // 0x20ce28
    uint32_t car = (*(uint32_t*)rtguest::G(0x177ea5c));
    uint32_t r = RT_CALLRA(0x20ce3c, guest::loadCarModel(0x497b80, 3, (*(uint8_t*)rtguest::G(car + 4))));
    return r == 0 ? 1 : 0;
  }
  case 2: {  // 0x20ce58
    int32_t idx = (*(int8_t*)rtguest::G(param_2 + 0x23));
    uint32_t part = (uint32_t)(*(uint8_t*)rtguest::G(param_2 + 0x21)) * 0x3f000;
    RT_CALLRA(0x20ce8c, guest::cdReadEntryPart(0x29b290 + (uint32_t)idx * 0xc, 0x100000, part, 0x3f000));
    loadState = loadState | 0x8000;
    return 0;
  }
  case 3: {  // 0x20cea0
    rebaseTexWalk(0x100000, 0xffffc000ffffffffull, 0x1f2e, 0x20cee0);
    uint32_t ch = RT_CALLRA(0x20cf30, guest::sceDmaGetChan(1));
    RT_CALLRA(0x20cf40, guest::sceDmaSync(ch, 0, 0));
    RT_CALLRA(0x20cf48, guest::sendTexturePackets(0x100000));
    return 0;
  }
  case 5: {  // 0x20cf58
    uint32_t part = ((*(uint32_t*)rtguest::G(param_1 + 0x1c)) & 0xf) * 0x53000;
    RT_CALLRA(0x20cf80, guest::cdReadEntryPart(0x29b290, 0x100000, part, 0x53000));
    loadState = loadState | 0x8000;
    return 0;
  }
  case 6: {  // 0x20cf98
    uint32_t sz = (*(uint32_t*)rtguest::G(0x100004));
    RT_CALLRA(0x20cfa8, guest::printf(0x2ea650, sz));
    if ((int32_t)0x34000 < (int32_t)sz) RT_CALLRA(0x20cfcc, guest::fatal(0x2ea678, sz));
    RT_CALLRA(0x20cfe8, guest::memmove(0x497b80, 0x100000, (*(uint32_t*)rtguest::G(0x100004))));
    (*(uint32_t*)rtguest::G(0x177f358)) = 0x497b80 + (*(uint32_t*)rtguest::G(0x497b80));
    uint32_t tex = 0x100000 + (*(uint32_t*)rtguest::G(0x100004));
    uint32_t tail = 0x497b80 + (*(uint32_t*)rtguest::G(0x497b84));
    rebaseTexWalk(tex, ~0ull, 0, 0x20d078);  // mask 0xffffc000ffffffff | 0x3fff<<32: TBP kept
    uint32_t ch = RT_CALLRA(0x20d0b4, guest::sceDmaGetChan(1));
    RT_CALLRA(0x20d0c4, guest::sceDmaSync(ch, 0, 0));
    RT_CALLRA(0x20d0cc, guest::sendTexturePackets(tex));
    return tail == 0 ? 1 : 0;
  }
  default:  // 4, 7: 0x20d0e8
    loadState = 0xffffffff;
    return 0;
  }
}


RT_GUEST_FN(0x0020cdc0, guest::DispatchInputMapping)

}  // namespace guest
