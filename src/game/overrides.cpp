// Game-specific runtime hooks for Road Trip (USA), SLUS-20398 v1.02.
// Bind addresses to runtime handlers here as boot issues are triaged, e.g.
//   ps2_game_overrides::bindAddressHandler(runtime, 0x00123456, "ret0");

#include "game_overrides.h"
#include "ps2_runtime.h"

#include <iostream>

namespace
{
    void applyRoadTrip(PS2Runtime &runtime)
    {
        (void)runtime;
        std::cout << "[roadtrip] applying SLUS-20398 overrides\n";
    }
}

PS2_REGISTER_GAME_OVERRIDE("Road Trip (USA)", "SLUS_203.98", 0x00200008u, 0x5A49851Du, applyRoadTrip)
