#pragma once

// The performance overlay (Options → Performance overlay): the frame rate, or also the longest
// frame, CPU and GPU load and temperature, in one row along the bottom edge from the left, clear of
// the race HUD. Inside the shared ImGui frame.

#include "settings/Settings.h"

namespace rt::ui
{
    void drawPerfOverlay(rt::settings::PerfOverlay level);
}
