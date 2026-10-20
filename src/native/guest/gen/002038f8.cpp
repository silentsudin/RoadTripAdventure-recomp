// guest:0x002038f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiBatchInit(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  (*(uint32_t*)rtguest::G(0x335688)) = 0xffffffff;
  (*(uint32_t*)rtguest::G(0x335680)) = 0x3362e0;
  (*(uint32_t*)rtguest::G(0x335684)) = 0;
  gifBeginSpriteBatch(0x3362e0,0x2013780e65321a40);
  gifSetSpriteCount(0x336340,0);
  gifBeginSpriteBatch(0x340350,0x2013780e65321a40);
  gifSetSpriteCount(0x3403b0,0);
  return 0;
}


RT_GUEST_FN(0x002038f8, guest::uiBatchInit)

}  // namespace guest
