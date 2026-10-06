#pragma once

// What this device can run, for greying out options with a reason. Two kinds of "no": the
// platform or GPU cannot run it (DLSS without an NVIDIA GPU), or it is not built yet.

#include "Settings.h"

#include <cstdint>
#include <string>

namespace rt::settings
{
    enum class Os { MacOS, Android, Windows, Linux };

    struct Capabilities
    {
        Os os = Os::MacOS;
        std::string gpuName;   // from the Vulkan device ("" until the GS backend is up)
        uint32_t gpuVendor = 0;
        int displayRefresh = 60; // the current display's refresh rate
        bool armNeuralAccel = false; // Arm Mali with neural accelerators (Arm NSS)
        bool postProcess = false;    // the presenter runs post-processing passes (the Vulkan one)
        int maxSuperSampling = 16;   // samples per pixel the GS supports on this GPU (4, 8 or 16)
        bool gpuGs = false;          // a GPU GS runs (paraLLEl-GS or the hardware GS; texture dumps and packs need it)
        bool secondDisplay = false;   // a second display the app can draw on (Android presentation display)
        bool frameGeneration = false; // the GS renders generated frames (paraLLEl-GS's shadow instances; not the hardware GS)
        bool temporalInputs = false;  // the GS gives depth and motion vectors (TAA, MetalFX temporal: paraLLEl-GS)
    };

    struct Availability
    {
        bool ok = true;
        std::string reason; // why not, when !ok
    };

    Capabilities &capabilities();
    // Fills gpu fields (vendor id: 0x10DE NVIDIA, 0x1002 AMD, 0x8086 Intel, 0x5143 Qualcomm, 0x13B5 Arm, 0x106B Apple).
    void setGpu(const std::string &name, uint32_t vendor);

    Availability availability(const Capabilities &c, Upscaler u);
    Availability availability(const Capabilities &c, AntiAliasing a);
    Availability refreshAvailability(const Capabilities &c, int hz);
    Availability aspectAvailability(Aspect a);
    Availability anisotropyAvailability(int level);
    Availability texturePackAvailability();
}
