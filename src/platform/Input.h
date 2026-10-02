#pragma once

// Host input -> DualShock 2. Call update() once per host frame on the render thread: it reads the
// keyboard and every connected gamepad (merged), applies the mapping from input.toml in the data
// directory (written with defaults on first run), and hands one snapshot to the runtime's pad
// emulation. The game thread never touches raylib input, so there is no data race and short
// taps are not missed.

namespace rt::input
{
    // Loads (or creates) input.toml and stops Escape from closing the window.
    void initialize();
    void update();
}
