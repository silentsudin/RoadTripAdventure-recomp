// guest:0x00258ec8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_music_play_bgm(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  RT_CALLRA(0x258ee4, guest::MusicRpcCmd15(0));
  RT_CALLRA(0x258ef8, guest::MusicRpcPlayBgm(0,param_1));
  SendMusicPlayRpc((int)param_1 >> 8 & 0xff,
                   0x29adf0 + ((int)param_1 >> 0x10 & 0xffU) * 0xc);  // W13: fn/00258ec8.c:11 `&gs_music_file_table + idx*3`, 12-byte {name*,a,b} entries (SendMusicPlayRpc reads [0..2])
  return 0;
}


RT_GUEST_FN(0x00258ec8, guest::gs_music_play_bgm)

}  // namespace guest
