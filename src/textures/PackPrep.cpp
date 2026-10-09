#include "textures/PackPrep.h"

#include "runtime/gs/gs_texture_pack_cache.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <memory>
#include <mutex>
#include <thread>

namespace rt::textures::packprep
{
    namespace
    {
        struct Job
        {
            std::string pack;
            ps2x::gs::packcache::Progress progress;
            std::atomic<bool> checking{true}, finished{false}, completed{false};
            std::chrono::steady_clock::time_point started = std::chrono::steady_clock::now();
            std::thread thread;
            ~Job() // (also at exit, with the menu open: a running thread must not outlive it)
            {
                progress.cancel = true;
                if (thread.joinable())
                    thread.join();
            }
        };
        std::mutex g_mutex;
        std::unique_ptr<Job> g_job;

        void stopLocked()
        {
            if (!g_job)
                return;
            g_job.reset(); // (stops and joins)
        }
    }

    void want(const std::string &packDir)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_job && g_job->pack == packDir)
            return;
        stopLocked();
        if (packDir.empty() || !ps2x::gs::packcache::available())
            return;
        g_job = std::make_unique<Job>();
        g_job->pack = packDir;
        Job *job = g_job.get();
        job->thread = std::thread([job] {
            const auto missing = ps2x::gs::packcache::missing(job->pack);
            const bool todo = missing != 0;
            // (a pack that converts again and again: this says how many images it thinks are missing)
            std::fprintf(stderr, "[textures] pack check %s: %zu image(s) to convert\n", job->pack.c_str(), static_cast<size_t>(missing));
            job->checking = false;
            if (todo && !job->progress.cancel)
                ps2x::gs::packcache::prepare(job->pack, job->progress);
            job->completed = todo && !job->progress.cancel;
            job->finished = true;
        });
    }

    void stop()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        stopLocked();
    }

    Status status()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        Status s;
        if (!g_job)
            return s;
        s.checking = g_job->checking;
        s.running = !g_job->finished && !g_job->checking;
        s.finished = g_job->completed;
        s.done = g_job->progress.done;
        s.total = g_job->progress.total;
        const double t = std::chrono::duration<double>(std::chrono::steady_clock::now() - g_job->started).count();
        if (s.running && s.done >= 3u && t > 2.0 && s.total > s.done)
            s.secondsLeft = t / s.done * (s.total - s.done);
        return s;
    }
}
