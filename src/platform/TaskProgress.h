#pragma once

// Progress shared between a worker thread (installer, game builder) and the progress window.

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace rt
{
    struct TaskProgress
    {
        // Work in the current phase (bytes, or another weight); total 0 = not measurable.
        std::atomic<uint64_t> done{0};
        std::atomic<uint64_t> total{0};
        // Which phase of how many (each setPhase is the next one; steps is set by whoever knows the plan).
        std::atomic<int> step{0};
        std::atomic<int> steps{0};
        std::atomic<bool> finished{false};
        std::atomic<bool> failed{false};

        void setPhase(std::string phase, std::string detail = {})
        {
            {
                std::lock_guard lock(m_mutex);
                m_phase = std::move(phase);
                m_detail = std::move(detail);
            }
            done = 0;
            total = 0;
            ++step;
        }
        void setDetail(std::string detail)
        {
            std::lock_guard lock(m_mutex);
            m_detail = std::move(detail);
        }
        std::string phase() const
        {
            std::lock_guard lock(m_mutex);
            return m_phase;
        }
        std::string detail() const
        {
            std::lock_guard lock(m_mutex);
            return m_detail;
        }

        bool fail(std::string message)
        {
            {
                std::lock_guard lock(m_mutex);
                m_error = std::move(message);
            }
            failed = true;
            finished = true;
            return false;
        }
        bool succeed()
        {
            finished = true;
            return true;
        }
        std::string error() const
        {
            std::lock_guard lock(m_mutex);
            return m_error;
        }

        // Parallel work in units of known weight (compiling): `jobs` workers, each reporting the unit
        // it starts and finishes. The time left then comes from how fast a worker gets through
        // weight (completed weight / the seconds it took), counting the units in flight by their
        // elapsed time, which is steady from the first finished unit on, however lumpy the finishes.
        void beginUnits(int jobs)
        {
            std::lock_guard lock(m_mutex);
            m_slots.assign(static_cast<size_t>(std::max(jobs, 1)), Slot{});
            m_unitWeight = 0;
            m_unitSeconds = 0.0;
            m_unitsDone = 0;
        }
        void beginUnit(int slot, uint64_t weight)
        {
            std::lock_guard lock(m_mutex);
            if (static_cast<size_t>(slot) < m_slots.size())
                m_slots[slot] = {clock(), weight, true};
        }
        void endUnit(int slot)
        {
            std::lock_guard lock(m_mutex);
            if (static_cast<size_t>(slot) >= m_slots.size() || !m_slots[slot].active)
                return;
            Slot &u = m_slots[slot];
            u.active = false;
            m_unitWeight += u.weight;
            m_unitSeconds += clock() - u.start;
            ++m_unitsDone;
        }
        bool hasUnits() const
        {
            std::lock_guard lock(m_mutex);
            return !m_slots.empty();
        }
        // Seconds left by the unit model, or a negative value (no units, or fewer than three finished:
        // single units vary too much to go by).
        double unitSecondsLeft() const
        {
            std::lock_guard lock(m_mutex);
            if (m_slots.empty() || m_unitsDone < 3 || m_unitSeconds <= 0.0 || m_unitWeight == 0)
                return -1.0;
            const double perSecond = static_cast<double>(m_unitWeight) / m_unitSeconds; // one worker
            const double now = clock();
            const uint64_t d = done.load(), t = total.load();
            double left = t > d ? static_cast<double>(t - d) : 0.0, longest = 0.0;
            for (const Slot &u : m_slots)
                if (u.active)
                {
                    // A unit in flight: what it should still take, at least a little.
                    const double rest = std::max(static_cast<double>(u.weight) * 0.05,
                                                 static_cast<double>(u.weight) - (now - u.start) * perSecond);
                    left -= static_cast<double>(u.weight) - rest;
                    longest = std::max(longest, rest / perSecond);
                }
            return std::max(std::max(left, 0.0) / (perSecond * static_cast<double>(m_slots.size())), longest);
        }

    private:
        static double clock()
        {
            return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
        }
        struct Slot
        {
            double start = 0.0;
            uint64_t weight = 0;
            bool active = false;
        };
        std::vector<Slot> m_slots;
        uint64_t m_unitWeight = 0;
        double m_unitSeconds = 0.0;
        int m_unitsDone = 0;

        mutable std::mutex m_mutex;
        std::string m_phase;
        std::string m_detail;
        std::string m_error;
    };
}
