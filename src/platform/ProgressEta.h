#pragma once

// Time left in a TaskProgress phase, from this machine's measured throughput: the work done over
// the last 40 seconds (or since the phase started). Unknown until the phase has run 10 seconds and
// done 5% of its work (parallel compiles finish in lumps); the shown value changes once a second
// so it can be read.

#include "platform/TaskProgress.h"

#include <deque>
#include <string>
#include <utility>

namespace rt
{
    class ProgressEta
    {
    public:
        // Seconds left in the current phase, or a negative value while unknown. `now` in seconds.
        double update(const TaskProgress &p, double now)
        {
            const int step = p.step.load();
            const uint64_t done = p.done.load(), total = p.total.load();
            if (step != m_step || total != m_total)
            {
                m_step = step;
                m_total = total;
                m_samples.clear();
                m_phaseStart = now;
                m_shown = -1.0;
            }
            if (total == 0)
                return -1.0;
            // Parallel units (compiling): their own model, steadier than the overall rate.
            if (now - m_shownAt >= 1.0 || m_shown < 0.0)
            {
                const double units = p.unitSecondsLeft();
                if (units >= 0.0)
                {
                    m_shownAt = now;
                    return m_shown = units;
                }
                if (p.hasUnits())
                    return m_shown = -1.0; // estimating until enough units have finished
            }
            m_samples.emplace_back(now, done);
            while (m_samples.size() > 2 && now - m_samples.front().first > 40.0)
                m_samples.pop_front();
            if (now - m_shownAt < 1.0 && m_shown >= 0.0)
                return m_shown;
            m_shownAt = now;
            const auto &[t0, d0] = m_samples.front();
            if (now - m_phaseStart < 10.0 || done * 20 < total || now - t0 < 1.0 || done <= d0)
                return m_shown = -1.0;
            const double rate = static_cast<double>(done - d0) / (now - t0);
            return m_shown = static_cast<double>(total - done) / rate;
        }

        double phaseSeconds(double now) const { return now - m_phaseStart; }

        // "About 3 minutes left", "About 40 seconds left", "Almost done", or "" while unknown.
        static std::string describe(double seconds)
        {
            if (seconds < 0.0)
                return {};
            if (seconds < 10.0)
                return "Almost done";
            if (seconds < 55.0)
                return "About " + std::to_string((static_cast<int>(seconds) + 5) / 10 * 10) + " seconds left";
            const int minutes = static_cast<int>(seconds / 60.0 + 0.5);
            return minutes <= 1 ? "About a minute left" : "About " + std::to_string(minutes) + " minutes left";
        }

    private:
        int m_step = -1;
        uint64_t m_total = 0;
        std::deque<std::pair<double, uint64_t>> m_samples;
        double m_phaseStart = 0.0, m_shown = -1.0, m_shownAt = -10.0;
    };
}
