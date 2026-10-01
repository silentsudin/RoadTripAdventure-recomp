// Never linked into anything. Compiled with the same settings the recompiled game needs, so
// scripts/bundle_sdk.py can lift its exact flags/includes from compile_commands.json.
// Include every runtime header generated code uses, so missing include dirs fail here first.

#include "ps2_runtime_macros.h"
#include "ps2_runtime.h"
#include "ps2_stubs.h"
#include "ps2_syscalls.h"

#ifdef PS2_FUNCTION_LOG_TRACKER
#include "ps2_log.h"
#endif

int rt_game_sdk_probe() { return 0; }
