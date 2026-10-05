#include "ThreadDump.h"

#include "ps2_runtime.h"
#include "runtime/ee_scheduler.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>

namespace rt::debug
{
    namespace
    {
        const char *statusName(EeThreadStatus s)
        {
            switch (s)
            {
            case EeThreadStatus::Running: return "RUN";
            case EeThreadStatus::Ready: return "READY";
            case EeThreadStatus::Waiting: return "WAIT";
            case EeThreadStatus::WaitingSuspended: return "WAITSUSP";
            case EeThreadStatus::Suspended: return "SUSP";
            case EeThreadStatus::Dormant: return "DORMANT";
            }
            return "?";
        }

        const char *waitName(EeWaitReason r)
        {
            switch (r)
            {
            case EeWaitReason::None: return "-";
            case EeWaitReason::Sleep: return "sleep";
            case EeWaitReason::Semaphore: return "sema";
            case EeWaitReason::EventFlag: return "evflag";
            case EeWaitReason::VSync: return "vsync";
            case EeWaitReason::External: return "external";
            case EeWaitReason::Mpeg: return "mpeg";
            }
            return "?";
        }

        void dump(const EeKernelSnapshot &snap)
        {
            std::fprintf(stderr, "[threads] seq=%llu eeCycle=%llu running=%d\n",
                         static_cast<unsigned long long>(snap.sequence),
                         static_cast<unsigned long long>(snap.eeCycle), snap.runningThreadId);
            for (const auto &t : snap.threads)
            {
                std::fprintf(stderr,
                             "  tid=%-3d %-8s wait=%-8s id=%-3d prio=%d/%d pc=%08x ra=%08x sp=%08x entry=%08x wakeups=%u\n",
                             t.id, statusName(t.status), waitName(t.waitReason), t.waitId, t.currentPriority,
                             t.initialPriority, t.pc, t.ra, t.sp, t.entry, t.wakeupCount);
            }
            for (const auto &s : snap.semaphores)
                std::fprintf(stderr, "  sema id=%-3d count=%d/%d waiters=%u\n", s.id, s.count, s.maxCount, s.waiters);
            for (const auto &f : snap.eventFlags)
                std::fprintf(stderr, "  evflag id=%-3d bits=%08x waiters=%u\n", f.id, f.bits, f.waiters);
        }
    }

    void startThreadDumpIfRequested(PS2Runtime &runtime)
    {
        const char *env = std::getenv("RT_THREAD_DUMP");
        if (!env || !*env)
            return;
        const double seconds = std::atof(env) > 0 ? std::atof(env) : 2.0;
        std::thread([&runtime, seconds]
                    {
                        for (;;)
                        {
                            std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
                            dump(runtime.eeScheduler().snapshot());
                        } })
            .detach();
    }
}
