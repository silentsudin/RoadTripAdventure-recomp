// W3-02: renderer-independent player-centred race outline.
// CarAiInit 0x24FD24..0x24FE04 selects staticGraph(task+0x22), with
// 11 -> 6 and 16 -> 15; types -1/34 use courseGraph instead.
#pragma once
#include "assets/nav_routes.h"
#include "gameplay/minimap.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <set>
#include <utility>
#include <vector>

namespace rt::native::gameplay::minimap {

// FIELD pixels: (102,183) becomes UI (102,366), via fieldToUi.
struct TrackSegment { FieldPoint a, b; };
struct TrackPolyline { std::vector<TrackSegment> segments; };

// Authored native zoom schedule supplied by the brief, NOT the ROM's two
// town zoom entries.  Baseline is W2-11 s=50; smaller levels magnify.
inline constexpr std::array<float, 5> kTrackZoomLevels{1020.f, 765.f, 612.f, 459.f, 306.f};
inline float trackZoomScale(int level) {
    return 50.f * (kTrackZoomLevels[0] /
        kTrackZoomLevels[static_cast<std::size_t>(std::clamp(level, 0, 4))]);
}
inline FieldPoint trackProject(float px, float pz, float wx, float wz, int level) {
    const float s = trackZoomScale(level);
    // W2-11 law with x0=102+4s, y0=183-1.88s to hold the player fixed.
    return {102.f + (wx - px) * s / kWorldDivisor,
            183.f - kYSquash * s * (wz - pz) / kWorldDivisor};
}

// Fixed-order Liang-Barsky, inclusive pixel-centre bounds. The original race
// placement handler sends NO scissor packet; this native player-centred view
// deliberately reuses the NTSC town minimap window, not a claimed ROM race clip.
inline bool clipTrackSegment(TrackSegment& line, ScissorRect rect = kScissorTownNtsc) {
    const float dx = line.b.x - line.a.x, dy = line.b.y - line.a.y;
    if (!std::isfinite(line.a.x) || !std::isfinite(line.a.y) ||
        !std::isfinite(line.b.x) || !std::isfinite(line.b.y) ||
        !std::isfinite(dx) || !std::isfinite(dy)) return false;
    float enter = 0.f, leave = 1.f;
    const std::array<float, 4> p{-dx, dx, -dy, dy};
    const std::array<float, 4> q{line.a.x - static_cast<float>(rect.x0),
        static_cast<float>(rect.x1) - line.a.x, line.a.y - static_cast<float>(rect.y0),
        static_cast<float>(rect.y1) - line.a.y};
    for (std::size_t i = 0; i < p.size(); ++i) {
        if (p[i] == 0.f) { if (q[i] < 0.f) return false; continue; }
        const float t = q[i] / p[i];
        if (p[i] < 0.f) enter = std::max(enter, t);
        else leave = std::min(leave, t);
        if (enter > leave) return false;
    }
    const FieldPoint origin = line.a;
    auto at = [&](float t) -> FieldPoint {
        return {std::clamp(origin.x + dx * t, static_cast<float>(rect.x0), static_cast<float>(rect.x1)),
                std::clamp(origin.y + dy * t, static_cast<float>(rect.y0), static_cast<float>(rect.y1))};
    };
    line.a = at(enter); line.b = at(leave);
    return true;
}

inline FieldPoint trackNode(const assets::NavSegment& segment) {
    return {(segment.ax + segment.bx) * .5f, (segment.az + segment.bz) * .5f};
}
inline TrackPolyline buildTrackPolyline(const assets::NavGraph& graph, float px, float pz, int zoomLevel) {
    TrackPolyline out;
    out.segments.reserve(graph.transitions.size());
    std::set<std::pair<std::size_t, std::size_t>> seen;
    // Nav segments are cross-sections. Transition A/B connect their midpoints;
    // joining raw array neighbours would invent edges at branches (nav-tables.md).
    for (const auto& transition : graph.transitions) {
        const std::size_t a = transition.segmentA, b = transition.segmentB;
        if (a >= graph.segments.size() || b >= graph.segments.size() || a == b) continue;
        if (!seen.emplace(std::min(a, b), std::max(a, b)).second) continue;
        const auto na = trackNode(graph.segments[a]), nb = trackNode(graph.segments[b]);
        TrackSegment line{trackProject(px, pz, na.x, na.y, zoomLevel),
                          trackProject(px, pz, nb.x, nb.y, zoomLevel)};
        if (clipTrackSegment(line)) out.segments.push_back(line);
    }
    return out;
}

struct TrackArrow {
    FieldPoint position;
    float yaw = 0.f; // native radians: forward = (sin(yaw), cos(yaw))
    float scale = .62f; // empirical oracle correction, not ROM model data
    std::array<FieldPoint, 3> vertices;
};
inline TrackArrow buildTrackArrow(float px, float pz, float yaw, int zoomLevel, float naiveRadius = 5.f) {
    TrackArrow arrow;
    arrow.position = trackProject(px, pz, px, pz, zoomLevel);
    arrow.yaw = yaw;
    const float sn = std::sin(yaw), cs = std::cos(yaw), r = naiveRadius * arrow.scale;
    const std::array<FieldPoint, 3> local{{{0.f, 1.f}, {-.6f, -.6f}, {.6f, -.6f}}};
    for (std::size_t i = 0; i < local.size(); ++i)
        arrow.vertices[i] = {arrow.position.x + r * (local[i].x * cs + local[i].y * sn),
            arrow.position.y - kYSquash * r * (-local[i].x * sn + local[i].y * cs)};
    return arrow;
}

} // namespace rt::native::gameplay::minimap
