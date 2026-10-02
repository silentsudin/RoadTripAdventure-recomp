#include "RamDump.h"

#include "ps2_runtime.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

namespace rt::debug
{
    void startRamDumpIfRequested(PS2Runtime &runtime)
    {
        const char *dir = std::getenv("RT_RAM_DUMP");
        if (!dir || !*dir)
            return;
        const char *secondsEnv = std::getenv("RT_RAM_DUMP_SECONDS");
        const double seconds = secondsEnv && std::atof(secondsEnv) > 0 ? std::atof(secondsEnv) : 10.0;
        std::filesystem::create_directories(dir);
        std::thread([&runtime, out = std::string(dir), seconds]
                    {
                        for (int n = 0;; ++n)
                        {
                            std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
                            char name[32];
                            std::snprintf(name, sizeof(name), "/ram_%04d.bin", n);
                            std::ofstream f(out + name, std::ios::binary);
                            f.write(reinterpret_cast<const char *>(runtime.memory().getRDRAM()), PS2_RAM_SIZE);
                        } })
            .detach();
    }
}
