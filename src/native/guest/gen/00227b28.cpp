// guest:0x00227b28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitPlayerSlotStrings(uint32_t q0, ...) {
  (*(uint8_t*)rtguest::G((uint32_t)0x0177fd8c + 0)) = 'P';
  (*(uint8_t*)rtguest::G((uint32_t)0x0177fd8c + 1)) = 'L';
  (*(uint8_t*)rtguest::G((uint32_t)0x0177fd8c + 2)) = 'A';
  (*(uint8_t*)rtguest::G((uint32_t)0x0177fd8c + 3)) = 'Y';
  (*(uint32_t*)rtguest::G(0x177fd90)) = 'E';
  (*(uint8_t*)rtguest::G((uint32_t)0x0177fd8c + 4)) = 'R';
  (*(uint8_t*)rtguest::G((uint32_t)0x0177fd8c + 5)) = ' ';
  (*(uint8_t*)rtguest::G((uint32_t)0x0177fd8c + 6)) = '1';
  (*(uint32_t*)rtguest::G(0x177fd94)) = '\0';
  (*(uint8_t*)rtguest::G((uint32_t)0x017831d4 + 0)) = 'P';
  (*(uint8_t*)rtguest::G((uint32_t)0x017831d4 + 1)) = 'L';
  (*(uint8_t*)rtguest::G((uint32_t)0x017831d4 + 2)) = 'A';
  (*(uint8_t*)rtguest::G((uint32_t)0x017831d4 + 3)) = 'Y';
  (*(uint8_t*)rtguest::G((uint32_t)0x017831d4 + 4)) = 'E';
  (*(uint8_t*)rtguest::G((uint32_t)0x017831d4 + 5)) = 'R';
  (*(uint8_t*)rtguest::G((uint32_t)0x017831d4 + 6)) = ' ';
  (*(uint8_t*)rtguest::G((uint32_t)0x017831d4 + 7)) = '2';
  (*(uint32_t*)rtguest::G(0x17831dc)) = '\0';
  (*(uint32_t*)rtguest::G(0x178321f)) = 0x1a;
  (*(uint32_t*)rtguest::G((uint32_t)0x177f760 + 0)) = 0xd00e;
  (*(uint32_t*)rtguest::G((uint32_t)0x0177f778 + 4)) = 0xc00d;
  (*(uint32_t*)rtguest::G((uint32_t)0x0177f798 + 0)) = 0x0010d00e;
  (*(uint32_t*)rtguest::G((uint32_t)0x01782ba8 + 0)) = 0x00f30f30;
  (*(uint32_t*)rtguest::G((uint32_t)0x01782bc0 + 4)) = 0x00d20d20;
  (*(uint32_t*)rtguest::G((uint32_t)0x01782be0 + 0)) = 0x00f42f42;
  (*(uint8_t*)rtguest::G((uint32_t)0x177f760 + 4)) = 0x2d;
  (*(uint8_t*)rtguest::G((uint32_t)0x0177f780 + 0)) = 0x49;
  (*(uint8_t*)rtguest::G((uint32_t)0x0177f798 + 4)) = 0x68;
  (*(int8_t*)rtguest::G(0x177fdd4)) = 1;
  (*(uint32_t*)rtguest::G(0x177fdd5)) = 0x21;
  (*(uint32_t*)rtguest::G(0x177fdd7)) = 0xb;
  (*(uint8_t*)rtguest::G((uint32_t)0x01782bc8 + 0)) = 0x69;
  (*(uint8_t*)rtguest::G((uint32_t)0x01782be0 + 4)) = 0x8d;
  (*(uint32_t*)rtguest::G(0x178321c)) = 7;
  (*(uint32_t*)rtguest::G(0x178321e)) = 4;
  (*(int8_t*)rtguest::G(0x177fdd6)) = 7;
  (*(uint32_t*)rtguest::G(0x178321d)) = 0x1f;
  (*(uint8_t*)rtguest::G((uint32_t)0x01782ba8 + 4)) = 0;
  return 0;
}


RT_GUEST_FN(0x00227b28, guest::InitPlayerSlotStrings)

}  // namespace guest
