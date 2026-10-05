#pragma once

// Progress shared between a worker thread (installer, game builder) and the progress window.

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>

namespace rt
{
    struct TaskProgress
    {
        std::atomic<uint64_t> done{0};
        std::atomic<uint64_t> total{0};
        std::atomic<bool> finished{false};
        std::atomic<bool> failed{false};

        void setPhase(std::string phase, std::string detail = {})
        {
            std::lock_guard lock(m_mutex);
            m_phase = std::move(phase);
            m_detail = std::move(detail);
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

    private:
        mutable std::mutex m_mutex;
        std::string m_phase;
        std::string m_detail;
        std::string m_error;
    };
}
