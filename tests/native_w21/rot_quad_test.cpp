// W8-04: rotated preview textured quad coverage and gauge-needle integration.
#include "preview/gauge_needle.h"
#include "preview/rot_quad.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <vector>

namespace {

constexpr double kPi = 3.14159265358979323846;
int checks = 0;
int failures = 0;

void check(bool ok, const char* name) {
    ++checks;
    if (!ok) {
        ++failures;
        std::printf("FAIL %s\n", name);
    }
}

rt::native::preview::PreviewTexture needleTexture() {
    rt::native::preview::PreviewTexture texture{8, 59, std::vector<std::uint8_t>(8 * 59 * 4)};
    for (std::uint32_t y = 0; y < texture.height; ++y) {
        for (std::uint32_t x = 0; x < texture.width; ++x) {
            const std::size_t at = (static_cast<std::size_t>(y) * texture.width + x) * 4;
            texture.rgba[at + 0] = static_cast<std::uint8_t>(x * 31);
            texture.rgba[at + 1] = static_cast<std::uint8_t>(y * 4);
            texture.rgba[at + 2] = 255;
            texture.rgba[at + 3] = 255;
        }
    }
    return texture;
}

rt::native::preview::RgbaSurface surface(std::vector<std::uint8_t>& pixels) {
    return {128, 128, pixels};
}

bool centroid(const std::vector<std::uint8_t>& pixels, double& x, double& y) {
    double sx = 0.0, sy = 0.0;
    int count = 0;
    for (int py = 0; py < 128; ++py) {
        for (int px = 0; px < 128; ++px) {
            if (pixels[(py * 128 + px) * 4 + 3] == 0) continue;
            sx += px + .5;
            sy += py + .5;
            ++count;
        }
    }
    if (!count) return false;
    x = sx / count;
    y = sy / count;
    return true;
}

void checkAngle(double theta, const rt::native::preview::PreviewTexture& texture) {
    namespace preview = rt::native::preview;
    namespace needle = preview::gauge_needle;
    constexpr int kOrigin = 0;
    constexpr double kPivot = 64.0;
    std::vector<std::uint8_t> pixels(128 * 128 * 4);
    const auto q = needle::needleQuad(theta, kOrigin + 10, kOrigin - 4);
    preview::TexturedQuad quad{};
    for (int i = 0; i < 4; ++i) {
        quad.x[i] = q.x[i];
        quad.y[i] = q.y[i];
        quad.u[i] = q.u[i];
        quad.v[i] = q.v[i];
    }
    // Move the gauge's declared pivot to the centre of this synthetic surface.
    for (float& x : quad.x) x += static_cast<float>(kPivot - (kOrigin + 10 + needle::kPivot.x));
    for (float& y : quad.y) y += static_cast<float>(kPivot - (kOrigin - 4 + needle::kPivot.y));
    check(preview::drawTexturedQuad(surface(pixels), quad, texture), "rotated quad draws");

    double actualX = 0.0, actualY = 0.0;
    const bool hasCentroid = centroid(pixels, actualX, actualY);
    check(hasCentroid, "rotated quad has lit pixels");
    const double expectedX = kPivot + 29.5 * std::sin(theta);
    const double expectedY = kPivot - 29.5 * std::cos(theta);
    const double error = std::hypot(actualX - expectedX, actualY - expectedY);
    std::printf("theta=%+.5f centroid=(%.2f,%.2f) expected=(%.2f,%.2f) error=%.3f px\n",
                theta, actualX, actualY, expectedX, expectedY, error);
    check(hasCentroid && error <= 1.25, "lit-pixel centroid follows needle ray");

    const auto expected = needle::needleQuad(theta, kOrigin + 10, kOrigin - 4);
    bool cornersMatch = true;
    for (int i = 0; i < 4; ++i)
        cornersMatch = cornersMatch && std::fabs(q.x[i] - expected.x[i]) < 1e-6 &&
                       std::fabs(q.y[i] - expected.y[i]) < 1e-6;
    check(cornersMatch, "textured quad uses gauge needle corners");
}

}  // namespace

int main() {
    namespace preview = rt::native::preview;
    const auto texture = needleTexture();
    for (double theta : {0.0, kPi / 4.0, -kPi / 4.0, kPi / 2.0, -kPi / 2.0, kPi})
        checkAngle(theta, texture);

    std::vector<std::uint8_t> low(128 * 128 * 4), high(128 * 128 * 4);
    check(preview::drawGaugeNeedle(surface(low), 0.f, texture, {0, 0}), "zero-rpm gauge needle draws");
    check(preview::drawGaugeNeedle(surface(high), 10000.f, texture, {0, 0}), "max-rpm gauge needle draws");
    double lowX = 0.0, lowY = 0.0, highX = 0.0, highY = 0.0;
    check(centroid(low, lowX, lowY) && centroid(high, highX, highY) &&
              (lowX - 70.0) * (highX - 70.0) + (lowY - 68.0) * (highY - 68.0) < 0.0,
          "zero and maximum rpm land on opposite pivot sides");

    std::vector<std::uint8_t> untouched(128 * 128 * 4);
    const preview::PreviewTexture invalid{};
    check(!preview::drawGaugeNeedle(surface(untouched), 100.f, invalid, {0, 0}), "invalid texture draws nothing");
    check(!preview::drawGaugeNeedle(surface(untouched), std::numeric_limits<float>::quiet_NaN(), texture, {0, 0}),
          "NaN rpm draws nothing");
    check(!preview::drawTexturedQuad(surface(untouched), {}, texture), "zero-length quad draws nothing");
    check(!centroid(untouched, lowX, lowY), "invalid inputs leave pixels untouched");

    std::printf("checks=%d failed=%d\n", checks, failures);
    return failures ? 1 : 0;
}
