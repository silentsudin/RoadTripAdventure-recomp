// W7-01: tachometer needle quad geometry + ROM golden tip directions.
#include "preview/gauge_needle.h"

#include <array>
#include <cmath>
#include <cstdio>

namespace {

constexpr double kPi = 3.14159265358979323846;

int checks = 0;
int failures = 0;
void check(bool ok, const char* what) {
    ++checks;
    if (!ok) {
        ++failures;
        std::printf("FAIL %s\n", what);
    }
}

double needleTheta(int rpm) {
    const int clamped = rpm < 0 ? 0 : rpm > 10000 ? 10000 : rpm;
    return kPi * (static_cast<double>(clamped) / 10000.0 - .7);
}

double degrees(double radians) { return radians * 180.0 / kPi; }

double tipDirectionDegrees(const rt::native::preview::gauge_needle::NeedleQuad& quad,
                           double pivotX, double pivotY) {
    const double tipX = (quad.x[0] + quad.x[1]) * .5;
    const double tipY = (quad.y[0] + quad.y[1]) * .5;
    // The source screenshots use x right / y up: negate screen-space y before atan2.
    return degrees(std::atan2(-(tipY - pivotY), tipX - pivotX));
}

double distance(double ax, double ay, double bx, double by) {
    return std::hypot(ax - bx, ay - by);
}

bool near(double actual, double expected, double tolerance) {
    return std::fabs(actual - expected) <= tolerance;
}

}  // namespace

int main() {
    namespace needle = rt::native::preview::gauge_needle;
    constexpr int kOriginX = 484;
    constexpr int kOriginY = 294;
    constexpr double kPivotX = kOriginX + needle::kPivot.x;
    constexpr double kPivotY = kOriginY + needle::kPivot.y;

    const auto coast = needle::needleQuad(needleTheta(117), kOriginX, kOriginY);
    check(near(tipDirectionDegrees(coast, kPivotX, kPivotY), -146.11, 2.0),
          "coast golden -146.11 deg");
    const auto cruise = needle::needleQuad(needleTheta(6217), kOriginX, kOriginY);
    check(near(tipDirectionDegrees(cruise, kPivotX, kPivotY), 104.09, 2.0),
          "cruise golden +104.09 deg");

    check(near(distance((coast.x[0] + coast.x[1]) * .5, (coast.y[0] + coast.y[1]) * .5,
                       kPivotX, kPivotY),
               needle::kNeedleAtlasCell.h, 1e-5),
          "pivot-to-tip length equals ROM sprite height");

    const auto reference = needle::needleQuad(0.0, kOriginX, kOriginY);
    const std::array<double, 4> referenceEdges{{
        distance(reference.x[0], reference.y[0], reference.x[1], reference.y[1]),
        distance(reference.x[1], reference.y[1], reference.x[3], reference.y[3]),
        distance(reference.x[3], reference.y[3], reference.x[2], reference.y[2]),
        distance(reference.x[2], reference.y[2], reference.x[0], reference.y[0]),
    }};
    for (int i = 0; i < 8; ++i) {
        const auto quad = needle::needleQuad(kPi * i / 8.0, kOriginX, kOriginY);
        const std::array<double, 4> edges{{
            distance(quad.x[0], quad.y[0], quad.x[1], quad.y[1]),
            distance(quad.x[1], quad.y[1], quad.x[3], quad.y[3]),
            distance(quad.x[3], quad.y[3], quad.x[2], quad.y[2]),
            distance(quad.x[2], quad.y[2], quad.x[0], quad.y[0]),
        }};
        bool rigid = true;
        for (int edge = 0; edge < 4; ++edge) rigid = rigid && near(edges[edge], referenceEdges[edge], 1e-4);
        check(rigid, "rigid quad at theta step");
    }

    const bool shiftedPasses = near(tipDirectionDegrees(
                                        needle::needleQuad(needleTheta(117) + 15.0 * kPi / 180.0, kOriginX, kOriginY),
                                        kPivotX, kPivotY),
                                    -146.11, 2.0);
    check(!shiftedPasses, "negative control: +15 deg must miss coast golden");
    if (!shiftedPasses)
        std::puts("CAUGHT: +15 degree needle-angle perturbation misses the coast golden");
    std::printf("checks=%d failed=%d\n", checks, failures);
    return failures ? 1 : 0;
}
