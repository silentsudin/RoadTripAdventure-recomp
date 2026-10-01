#pragma once

// Small self-contained SHA-1 used only to identify the user's disc build.

#include <cstddef>
#include <cstdint>
#include <string>

namespace rt
{
    class Sha1
    {
    public:
        Sha1();
        void update(const uint8_t *data, size_t len);
        std::string hexdigest(); // finalizes

    private:
        void block(const uint8_t *p);
        uint32_t m_h[5];
        uint8_t m_buf[64];
        size_t m_bufLen = 0;
        uint64_t m_total = 0;
    };
}
