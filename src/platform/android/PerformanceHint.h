#pragma once

// Android's performance hints (ADPF): the threads a frame depends on (VU1, GS, game, presenter)
// form one hint session with a 16.7 ms target, and each host frame reports how long the busiest
// of them worked in it. The CPU governor then keeps their cores fast enough for the frame instead
// of clocking down between bursts (which cost the Thor up to 6 fps a few minutes into a race),
// and no faster. Looked up at runtime (API 33; the app supports 31); RT_PERF_HINT=0 turns it off.
namespace rt::perfhint
{
    // Once per host frame, on the presenter thread.
    void tick();
}
