// guest:0x0021c928 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t StoreIndexedPosition(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(16);
  float fVar1;
  float fVar2;
  
  fVar2 = param_3 * (*(float*)rtguest::G(0x332c24));
  fVar1 = param_2 * (*(float*)rtguest::G(0x332c24));
  (*(int32_t*)rtguest::G((uint32_t)0x177acf0 + (uint32_t)(param_4 * 0x9c) * 4)) = (int)(param_1 * (*(float*)rtguest::G(0x332c24)));
  (*(uint32_t*)rtguest::G((uint32_t)0x177acf4 + (uint32_t)(param_4 * 0x9c) * 4)) = (int)fVar1;
  (*(uint32_t*)rtguest::G((uint32_t)0x177ae24 + (uint32_t)(param_4 * 0x138) * 4)) = param_5;
  (*(uint32_t*)rtguest::G((uint32_t)0x177acf8 + (uint32_t)(param_4 * 0x9c) * 4)) = (int)fVar2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177ace0 + (uint32_t)(param_4 * 0x9c) * 4)) = param_1;
  (*(uint32_t*)rtguest::G((uint32_t)0x177ace4 + (uint32_t)(param_4 * 0x9c) * 4)) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)0x177ace8 + (uint32_t)(param_4 * 0x9c) * 4)) = param_3;
  return 0;
}


RT_GUEST_FN(0x0021c928, guest::StoreIndexedPosition)

}  // namespace guest
