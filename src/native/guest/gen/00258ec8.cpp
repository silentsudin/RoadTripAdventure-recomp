// guest:0x00258ec8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_music_play_bgm(uint32_t param_1, ...) {
  MusicRpcCmd15(0);
  MusicRpcPlayBgm(0,param_1);
  SendMusicPlayRpc((int)param_1 >> 8 & 0xff,
                   0x29adf0 + ((int)param_1 >> 0x10 & 0xffU) * 3);
  return 0;
}


RT_GUEST_FN(0x00258ec8, guest::gs_music_play_bgm)

}  // namespace guest
