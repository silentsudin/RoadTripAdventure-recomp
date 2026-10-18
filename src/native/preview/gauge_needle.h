#pragma once

#include <cmath>

namespace rt::native::preview::gauge_needle {

// The 1P gauge selects texture-table entry 1 with CSA 12 at 0x00249E04..0x00249E0C.
// That is METER0.GSL's second image (128x120), not its 256x128 dial-face image.
enum class AtlasPage { Meter0SubCsa12 };

struct AtlasCell {
    AtlasPage page;
    int u;
    int v;
    int w;
    int h;
    int pageWidth;
    int pageHeight;
};

struct Pivot {
    int x;
    int y;
};

// The record assembled at 0x00249E10..0x00249EDC selects this 8x59 texel cell.
inline constexpr AtlasCell kNeedleAtlasCell{AtlasPage::Meter0SubCsa12, 3, 53, 8, 59, 128, 120};
// The same record's screen anchor is the 1P gauge origin plus (70,68).
inline constexpr Pivot kPivot{70, 68};

struct NeedleQuad {
    float x[4];
    float y[4];
    float u[4];
    float v[4];
};

// Builds the four strip vertices in design-space pixels.  thetaRad is the
// emitter's Z turn in radians: zero keeps the needle upright and positive
// turns it clockwise because screen y grows down.  The vertex shape comes
// from 0x00249E78..0x00249EAC and the rotation path is uiArenaRotQuads at
// 0x0024EB10.  UVs are normalized for the 128x120 CSA-12 page so they feed
// the existing HUD texture path directly.
inline NeedleQuad needleQuad(double thetaRad, int originX, int originY) {
    constexpr float localX[4] = {-4.f, 4.f, -4.f, 4.f};
    constexpr float localY[4] = {-59.f, -59.f, 0.f, 0.f};
    const double c = std::cos(thetaRad);
    const double s = std::sin(thetaRad);
    NeedleQuad quad{};
    for (int i = 0; i < 4; ++i) {
        quad.x[i] = static_cast<float>(originX + kPivot.x + localX[i] * c - localY[i] * s);
        quad.y[i] = static_cast<float>(originY + kPivot.y + localX[i] * s + localY[i] * c);
    }
    const float u0 = static_cast<float>(kNeedleAtlasCell.u) / kNeedleAtlasCell.pageWidth;
    const float u1 = static_cast<float>(kNeedleAtlasCell.u + kNeedleAtlasCell.w) / kNeedleAtlasCell.pageWidth;
    const float v0 = static_cast<float>(kNeedleAtlasCell.v) / kNeedleAtlasCell.pageHeight;
    const float v1 = static_cast<float>(kNeedleAtlasCell.v + kNeedleAtlasCell.h) / kNeedleAtlasCell.pageHeight;
    quad.u[0] = quad.u[2] = u0;
    quad.u[1] = quad.u[3] = u1;
    quad.v[0] = quad.v[1] = v0;
    quad.v[2] = quad.v[3] = v1;
    return quad;
}

}  // namespace rt::native::preview::gauge_needle
