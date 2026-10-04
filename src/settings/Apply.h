#pragma once

// Puts the current settings into effect (render thread). What cannot change while running is
// read at startup instead (see Settings.h).

namespace ps2x::gs
{
    class PgsControl;
}

namespace rt::settings
{
    // The Vulkan GS's live controls (nullptr with the CPU GS). Also reads the GPU for Capabilities.
    void setGsControl(ps2x::gs::PgsControl *control);
    ps2x::gs::PgsControl *gsControl();

    void applyWindow();   // window mode and size
    void applyGraphics(); // supersampling, sharp textures
    void applyAspect();
    void applyAll();
    // Startup: the supersampling level for the GS, unless RT_GS_SSAA overrides it.
    void exportGsEnvironment();
}
