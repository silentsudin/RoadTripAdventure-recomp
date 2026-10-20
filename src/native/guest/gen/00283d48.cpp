// guest:0x00283d48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _pow5mult(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  uint uVar1;
uint32_t puVar2;
uint32_t puVar3;
  undefined8 uVar4;
  
  if ((param_3 & 3) != 0) {
    param_2 = _multadd(param_1,param_2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0033241c + (param_3 & 3) * 4))),0);
  }
  param_3 = (int)param_3 >> 2;
  if (param_3 != 0) {
    puVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x48)));
    uVar4 = param_2;
    if (puVar2 == (uint32_t)0x0) {
      puVar2 = (uint32_t)_i2b(param_1,0x271);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x48))) = puVar2;
      (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
    }
    while( true ) {
      puVar3 = puVar2;
      uVar1 = param_3 & 1;
      param_3 = (int)param_3 >> 1;
      param_2 = uVar4;
      if (uVar1 != 0) {
        param_2 = _multiply(param_1,uVar4,puVar3);
        _Bfree(param_1,uVar4);
      }
      if (param_3 == 0) break;
      uVar4 = param_2;
      puVar2 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar3));
      if ((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar3)) == (uint32_t)0x0) {
        puVar2 = (uint32_t)_multiply(param_1,puVar3,puVar3);
        (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = puVar2;
        (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
      }
    }
  }
  return param_2;
}


RT_GUEST_FN(0x00283d48, guest::_pow5mult)

}  // namespace guest
