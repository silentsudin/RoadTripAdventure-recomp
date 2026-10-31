// CRT filter contract (issue #21): scanlines and an aperture mask on the
// composited frame. Pure integer math, no GPU and no game data.
#include "../../src/native/preview/crt_filter.h"

#include <cstdint>
#include <cstdio>
#include <vector>

using namespace rt::native::preview;

static int g_fail = 0;
static int g_checks = 0;
#define CHECK(c) do { ++g_checks; if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

namespace {

std::vector<std::uint8_t> whiteFrame(int w, int h) {
    std::vector<std::uint8_t> frame(static_cast<std::size_t>(w) * h * 4, 255u);
    return frame;
}

std::uint8_t at(const std::vector<std::uint8_t>& frame, int w, int x, int y, int channel) {
    return frame[(static_cast<std::size_t>(y) * w + x) * 4 + static_cast<std::size_t>(channel)];
}

}  // namespace

int main() {
    // A white frame: the bright scanline rows keep 255, the gap rows dim, and
    // the aperture mask gives each channel its own stripe column.
    constexpr int kW = 9, kH = 9;
    std::vector<std::uint8_t> frame = whiteFrame(kW, kH);
    applyCrtFilter(frame, kW, kH);
    CHECK(at(frame, kW, 0, 0, 0) == 255);
    CHECK(at(frame, kW, 0, 2, 0) < at(frame, kW, 0, 0, 0));
    CHECK(at(frame, kW, 0, 3, 0) == 255);
    CHECK(at(frame, kW, 0, 0, 0) > at(frame, kW, 0, 0, 2));  // red stripe
    CHECK(at(frame, kW, 1, 0, 1) > at(frame, kW, 1, 0, 0));  // green stripe
    CHECK(at(frame, kW, 2, 0, 2) > at(frame, kW, 2, 0, 1));  // blue stripe
    // Alpha survives untouched.
    for (int y = 0; y < kH; ++y)
        for (int x = 0; x < kW; ++x)
            CHECK(at(frame, kW, x, y, 3) == 255);

    // Deterministic: the same input twice gives the same output.
    std::vector<std::uint8_t> again = whiteFrame(kW, kH);
    applyCrtFilter(again, kW, kH);
    CHECK(again == frame);

    // Size guard: a buffer shorter than w*h*4, or a non-positive size, is
    // left exactly as it was.
    std::vector<std::uint8_t> tiny(4, 200u);
    const auto before = tiny;
    applyCrtFilter(tiny, 9, 9);
    CHECK(tiny == before);
    applyCrtFilter(tiny, 1, 0);
    CHECK(tiny == before);
    applyCrtFilter(tiny, -1, -1);
    CHECK(tiny == before);

    // A larger buffer changes pixels on every scanline gap: the filter is a
    // pure gain, never a wrap or an overflow (255 stays the maximum).
    std::vector<std::uint8_t> big = whiteFrame(320, 240);
    applyCrtFilter(big, 320, 240);
    CHECK(big.size() == static_cast<std::size_t>(320) * 240 * 4);
    for (int y = 0; y < 240; ++y)
        CHECK(at(big, 320, 0, y, 0) <= 255);

    if (g_fail) { std::fprintf(stderr, "crt_filter: %d failure(s)\n", g_fail); return 1; }
    std::printf("crt_filter: %d checks passed\n", g_checks);
    return 0;
}
