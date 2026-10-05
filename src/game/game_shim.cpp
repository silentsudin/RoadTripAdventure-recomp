// Compiled on the user's machine into libroadtrip_game.dylib, next to the code ps2_recomp
// generated from their disc. Not part of the app binary.
//
// The generated register_functions.cpp defines its own dense function table; the game is built
// with -Dg_ps2RecompiledFunctionTable*=rt_game* so those names don't clash with the app's table.
// rt_game_register() copies every entry into the app's full-range table.

#include "ps2_runtime.h"

#include <cstdint>

#ifndef RT_GAME_BUILD_ID
#define RT_GAME_BUILD_ID "unknown"
#endif

extern "C" __attribute__((visibility("default"))) const char *rt_game_build_id()
{
    return RT_GAME_BUILD_ID;
}

// Returns the number of entries copied, or -1 if an entry falls outside the destination table.
extern "C" __attribute__((visibility("default"))) int rt_game_register(PS2Runtime::RecompiledFunction *dst,
                                                                       uint32_t dstBase, uint32_t dstSlots)
{
    int copied = 0;
    for (uint32_t i = 0; i < g_ps2RecompiledFunctionTableSlotCount; ++i)
    {
        PS2Runtime::RecompiledFunction fn = g_ps2RecompiledFunctionTable[i];
        if (!fn)
            continue;
        const uint32_t address = g_ps2RecompiledFunctionTableBase + i * 4u;
        if (address < dstBase || ((address - dstBase) >> 2) >= dstSlots)
            return -1;
        dst[(address - dstBase) >> 2] = fn;
        ++copied;
    }
    return copied;
}
