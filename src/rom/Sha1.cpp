#include "Sha1.h"

#include <cstdio>
#include <cstring>

namespace rt
{
    namespace
    {
        inline uint32_t rol(uint32_t v, int n) { return (v << n) | (v >> (32 - n)); }
    }

    Sha1::Sha1() : m_h{0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u} {}

    void Sha1::block(const uint8_t *p)
    {
        uint32_t w[80];
        for (int i = 0; i < 16; ++i)
            w[i] = (uint32_t(p[i * 4]) << 24) | (uint32_t(p[i * 4 + 1]) << 16) | (uint32_t(p[i * 4 + 2]) << 8) | p[i * 4 + 3];
        for (int i = 16; i < 80; ++i)
            w[i] = rol(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);

        uint32_t a = m_h[0], b = m_h[1], c = m_h[2], d = m_h[3], e = m_h[4];
        for (int i = 0; i < 80; ++i)
        {
            uint32_t f, k;
            if (i < 20)
                f = (b & c) | (~b & d), k = 0x5A827999u;
            else if (i < 40)
                f = b ^ c ^ d, k = 0x6ED9EBA1u;
            else if (i < 60)
                f = (b & c) | (b & d) | (c & d), k = 0x8F1BBCDCu;
            else
                f = b ^ c ^ d, k = 0xCA62C1D6u;
            const uint32_t t = rol(a, 5) + f + e + k + w[i];
            e = d, d = c, c = rol(b, 30), b = a, a = t;
        }
        m_h[0] += a, m_h[1] += b, m_h[2] += c, m_h[3] += d, m_h[4] += e;
    }

    void Sha1::update(const uint8_t *data, size_t len)
    {
        m_total += len;
        while (len > 0)
        {
            const size_t n = std::min(len, sizeof(m_buf) - m_bufLen);
            std::memcpy(m_buf + m_bufLen, data, n);
            m_bufLen += n, data += n, len -= n;
            if (m_bufLen == sizeof(m_buf))
            {
                block(m_buf);
                m_bufLen = 0;
            }
        }
    }

    std::string Sha1::hexdigest()
    {
        const uint64_t bits = m_total * 8;
        const uint8_t pad = 0x80;
        update(&pad, 1);
        const uint8_t zero = 0;
        while (m_bufLen != 56)
            update(&zero, 1);
        uint8_t len[8];
        for (int i = 0; i < 8; ++i)
            len[i] = static_cast<uint8_t>(bits >> (56 - 8 * i));
        update(len, 8);

        char out[41];
        for (int i = 0; i < 5; ++i)
            std::snprintf(out + i * 8, 9, "%08x", m_h[i]);
        return std::string(out, 40);
    }
}
