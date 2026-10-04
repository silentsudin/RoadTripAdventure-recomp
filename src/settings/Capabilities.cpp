#include "Capabilities.h"

namespace rt::settings
{
    namespace
    {
        constexpr uint32_t kNvidia = 0x10DE, kAmd = 0x1002, kArm = 0x13B5;

        Availability no(std::string why) { return {false, std::move(why)}; }
        Availability later() { return no("Coming in a later update"); }

        Os buildOs()
        {
#if defined(__ANDROID__)
            return Os::Android;
#elif defined(__APPLE__)
            return Os::MacOS;
#elif defined(_WIN32)
            return Os::Windows;
#else
            return Os::Linux;
#endif
        }
    }

    Capabilities &capabilities()
    {
        static Capabilities c = [] {
            Capabilities k;
            k.os = buildOs();
            return k;
        }();
        return c;
    }

    void setGpu(const std::string &name, uint32_t vendor)
    {
        capabilities().gpuName = name;
        capabilities().gpuVendor = vendor;
    }

    Availability availability(const Capabilities &c, Upscaler u)
    {
        const bool desktop = c.os == Os::Windows || c.os == Os::Linux;
        switch (u)
        {
        case Upscaler::None:
            return {};
        case Upscaler::MetalFxSpatial:
        case Upscaler::MetalFxTemporal:
            if (c.os != Os::MacOS)
                return no("MetalFX is Apple's: macOS only");
            return later();
        case Upscaler::SnapdragonGsr1:
        case Upscaler::SnapdragonGsr2:
            if (c.os == Os::MacOS)
                return no("Snapdragon GSR is for Android and PC GPUs");
            return later();
        case Upscaler::ArmNss:
            if (c.os != Os::Android || c.gpuVendor != kArm || !c.armNeuralAccel)
                return no("Needs an Arm Mali GPU with neural accelerators");
            return later();
        case Upscaler::Dlss:
            if (!desktop || c.gpuVendor != kNvidia)
                return no("Needs an NVIDIA RTX GPU on Windows or Linux");
            return later();
        case Upscaler::Fsr4:
            if (!desktop || c.gpuVendor != kAmd)
                return no("Needs an AMD Radeon RX 9000 GPU on Windows or Linux");
            return later();
        case Upscaler::Xess:
            if (c.os == Os::MacOS)
                return no("XeSS has no macOS version");
            return later();
        case Upscaler::Fsr1:
            return c.postProcess ? Availability{} : no("Needs the Vulkan presenter");
        case Upscaler::ArmAsr:
        case Upscaler::Fsr3:
        default:
            return later();
        }
    }

    Availability availability(const Capabilities &c, AntiAliasing a)
    {
        if (a == AntiAliasing::None)
            return {};
        if (a == AntiAliasing::Fxaa)
            return c.postProcess ? Availability{} : no("Needs the Vulkan presenter");
        return later();
    }

    Availability refreshAvailability(const Capabilities &c, int hz)
    {
        if (hz == 60)
            return {};
        if (hz > c.displayRefresh)
            return no("Higher than this display's " + std::to_string(c.displayRefresh) + " Hz");
        return later();
    }

    Availability aspectAvailability(Aspect) { return {}; }
    Availability anisotropyAvailability(int level) { return level <= 1 ? Availability{} : later(); }
    Availability texturePackAvailability() { return later(); }
}
