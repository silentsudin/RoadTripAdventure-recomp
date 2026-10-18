// ROM-derived 1P METER0 tachometer-needle primitive (handler 0 @0x2496C8).
//
// The coordinator must switch race_hud_layout.h::kRaceHudNeedleLawResolved
// separately when its renderer is ready to consume this primitive; this header
// intentionally does not mutate that integration gate.
#pragma once

#include "gameplay/hud_live.h"

#include <algorithm>
#include <cstdint>

namespace rt::native::gameplay::gauge_needle {

// 0x249DD0 calls CalculateTachometerValue(car, 0, 1), which reads the signed
// halfword at car+0x1D0 and caps only values above 10000 (0x249520..0x24953C).
inline constexpr int kCarRpmOffset = 0x1d0;
inline constexpr int kRpmCeiling = 10000;
inline constexpr float kPi = 3.1415927410125732f;  // DAT_00332F84
inline constexpr float kRpmBias = -0.699999988079071f;  // DAT_00332F54

// The rotation record stores a half-turn value.  RotQuads multiplies it by pi:
// theta = pi * (min(rpmWord, 10000) / 10000 - 0.7).  Physics maintains the
// ordinary runtime domain [0, 10000]; preserve the helper's high-only clamp.
[[nodiscard]] inline float angleRadForRpm(std::int16_t rpmWord) {
    const int rpm = std::min(static_cast<int>(rpmWord), kRpmCeiling);
    return (static_cast<float>(rpm) / static_cast<float>(kRpmCeiling) + kRpmBias) * kPi;
}

// Inline equivalent of the 0x249E10 record written for uiArenaRotQuads.
[[nodiscard]] inline hud_live::DrawQuad buildNeedleQuad(hud_live::Anchor dialOrigin,
                                                         std::int16_t rpmWord) {
    hud_live::DrawQuad q;
    q.table = hud_live::Table::Drive;
    q.texEntry = 1;
    q.csa = 12;
    q.page = hud_live::Page::Meter0;
    q.u = 3;
    q.v = 53;
    q.w = 8;
    q.h = 59;
    q.x = static_cast<std::int16_t>(dialOrigin.x + 70);
    q.y = static_cast<std::int16_t>(dialOrigin.y + 68);
    q.tag = 0;
    q.rotated = true;
    q.angleRad = angleRadForRpm(rpmWord);
    q.verts = {-4.f, -59.f, 4.f, -59.f, -4.f, 0.f, 4.f, 0.f};
    return q;
}

}  // namespace rt::native::gameplay::gauge_needle
