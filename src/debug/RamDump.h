#pragma once

class PS2Runtime;

namespace rt::debug
{
    // If RT_RAM_DUMP=<dir> is set, writes EE RAM to <dir>/ram_<n>.bin (and IOP RAM to iop_<n>.bin) every
    // RT_RAM_DUMP_SECONDS (default 10). Read without locking: good enough to inspect game state.
    void startRamDumpIfRequested(PS2Runtime &runtime);
}
