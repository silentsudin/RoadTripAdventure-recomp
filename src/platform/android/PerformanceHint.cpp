// Android performance hints (see PerformanceHint.h).

#include "platform/android/PerformanceHint.h"

#include "platform/Lifecycle.h"

#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace rt::perfhint
{
    namespace
    {
        // The NDK's APerformanceHint API (android/performance_hint.h), resolved at runtime.
        using GetManager = void *(*)();
        using CreateSession = void *(*)(void *, const int32_t *, size_t, int64_t);
        using ReportActual = int (*)(void *, int64_t);

        struct Thread
        {
            pid_t tid;
            int fd; // /proc/self/task/<tid>/schedstat, kept open (pread re-reads it)
            uint64_t lastNs;
        };

        bool g_tried = false;
        void *g_session = nullptr;
        ReportActual g_report = nullptr;
        std::vector<Thread> g_threads;
        std::chrono::steady_clock::time_point g_start = std::chrono::steady_clock::now();

        uint64_t runNs(int fd)
        {
            char buf[96];
            const ssize_t n = pread(fd, buf, sizeof(buf) - 1, 0);
            if (n <= 0)
                return 0;
            buf[n] = 0;
            return std::strtoull(buf, nullptr, 10); // first field: time on the CPU, ns
        }

        void createSession()
        {
            g_tried = true;
            if (const char *e = std::getenv("RT_PERF_HINT"); e && *e == '0')
                return;
            void *lib = dlopen("libandroid.so", RTLD_NOW);
            auto getManager = lib ? reinterpret_cast<GetManager>(dlsym(lib, "APerformanceHint_getManager")) : nullptr;
            auto create = lib ? reinterpret_cast<CreateSession>(dlsym(lib, "APerformanceHint_createSession")) : nullptr;
            g_report = lib ? reinterpret_cast<ReportActual>(dlsym(lib, "APerformanceHint_reportActualWorkDuration")) : nullptr;
            if (!getManager || !create || !g_report)
            {
                std::fprintf(stderr, "[perfhint] not available (API 33+)\n");
                return;
            }
            // The threads a frame waits on, by name.
            static const char *const names[] = {"GifVif1Worker", "GsThread", "GameThread", "SDLThread"};
            std::vector<int32_t> tids;
            if (DIR *dir = opendir("/proc/self/task"))
            {
                while (dirent *e = readdir(dir))
                {
                    if (e->d_name[0] == '.')
                        continue;
                    const std::string base = std::string("/proc/self/task/") + e->d_name;
                    char comm[32] = {};
                    if (FILE *f = std::fopen((base + "/comm").c_str(), "r"))
                    {
                        if (std::fgets(comm, sizeof(comm), f))
                            comm[std::strcspn(comm, "\n")] = 0;
                        std::fclose(f);
                    }
                    if (std::none_of(std::begin(names), std::end(names), [&](const char *n) { return std::strcmp(n, comm) == 0; }))
                        continue;
                    const int fd = open((base + "/schedstat").c_str(), O_RDONLY | O_CLOEXEC);
                    if (fd < 0)
                        continue;
                    const pid_t tid = static_cast<pid_t>(std::atoi(e->d_name));
                    tids.push_back(tid);
                    g_threads.push_back({tid, fd, runNs(fd)});
                }
                closedir(dir);
            }
            void *manager = getManager();
            if (!manager || tids.empty() ||
                !(g_session = create(manager, tids.data(), tids.size(), 16'666'667)))
            {
                std::fprintf(stderr, "[perfhint] no session (%zu threads)\n", tids.size());
                return;
            }
            std::fprintf(stderr, "[perfhint] session for %zu threads, 16.7 ms target\n", tids.size());
        }
    }

    void tick()
    {
        // The runtime's threads exist a moment after it starts.
        if (!g_tried)
        {
            if (std::chrono::steady_clock::now() - g_start < std::chrono::seconds(3))
                return;
            createSession();
        }
        if (!g_session || rt::lifecycle::away())
            return;
        // The busiest thread's CPU time since the last frame: how long the frame's work took.
        uint64_t busiest = 0;
        for (Thread &t : g_threads)
        {
            const uint64_t now = runNs(t.fd);
            if (now > t.lastNs)
                busiest = std::max<uint64_t>(busiest, now - t.lastNs);
            t.lastNs = now;
        }
        if (busiest > 0)
            g_report(g_session, static_cast<int64_t>(busiest));
    }
}
