#include "gameplay/nav_walkable.h"

#include <cmath>
#include <vector>

namespace rt::native::gameplay {
namespace {

struct Leg {
    Pt start;
    Pt end;
    float length = 0.f;
};

float distance(Pt a, Pt b) {
    const float dx = b.x - a.x;
    const float dz = b.z - a.z;
    return std::sqrt(dx * dx + dz * dz);
}

std::vector<Leg> legsFor(const NavPath& path) {
    std::vector<Leg> legs;
    const auto append = [&legs](Pt start, Pt end) {
        const float length = distance(start, end);
        if (length > 0.f) legs.push_back({start, end, length});
    };
    for (std::size_t i = 1; i < path.centerline.size(); ++i)
        append(path.centerline[i - 1], path.centerline[i]);
    if (path.closed && path.centerline.size() > 1)
        append(path.centerline.back(), path.centerline.front());
    return legs;
}

Pt pointAt(const std::vector<Leg>& legs, float offset) {
    float remaining = offset;
    for (const Leg& leg : legs) {
        if (remaining <= leg.length) {
            const float t = remaining / leg.length;
            return {leg.start.x + (leg.end.x - leg.start.x) * t,
                    leg.start.z + (leg.end.z - leg.start.z) * t};
        }
        remaining -= leg.length;
    }
    return legs.back().end;
}

Pt transitionMidpoint(const assets::NavGraph& graph, const assets::NavTransition& node) {
    const assets::NavSegment& a = graph.segments[node.segmentA];
    const assets::NavSegment& b = graph.segments[node.segmentB];
    return {(a.ax + a.bx + b.ax + b.bx) * 0.25f,
            (a.az + a.bz + b.az + b.bz) * 0.25f};
}

} // namespace

WalkResult walkNavPath(const NavPath& path, float progressStep) {
    WalkResult result;
    if (path.centerline.empty()) return result;

    const std::vector<Leg> legs = legsFor(path);
    for (const Leg& leg : legs) result.totalLength += leg.length;
    result.cursorPositions.push_back(path.centerline.front());
    if (result.totalLength == 0.f || !(progressStep > 0.f) || !std::isfinite(progressStep)) return result;

    for (double offset = progressStep; offset < result.totalLength; offset += progressStep)
        result.cursorPositions.push_back(pointAt(legs, static_cast<float>(offset)));
    result.cursorPositions.push_back(pointAt(legs, result.totalLength));
    result.visitedLength = result.totalLength;
    result.visitedFraction = 1.f;
    result.completedLap = path.closed;
    return result;
}

NodeCoverage nodeCoverage(const NavPath& path, const assets::NavGraph& graph,
                          const WalkResult& walk, float tolerance) {
    NodeCoverage result;
    if (!(tolerance >= 0.f) || !std::isfinite(tolerance)) return result;

    std::vector<bool> seen(graph.transitions.size(), false);
    for (int index : path.nodeOrder) {
        if (index < 0 || static_cast<std::size_t>(index) >= graph.transitions.size()) continue;
        const std::size_t nodeIndex = static_cast<std::size_t>(index);
        if (seen[nodeIndex]) continue;
        seen[nodeIndex] = true;
        const assets::NavTransition& node = graph.transitions[nodeIndex];
        if (node.segmentA >= graph.segments.size() || node.segmentB >= graph.segments.size()) continue;
        ++result.totalNodes;
        const Pt target = transitionMidpoint(graph, node);
        for (const Pt cursor : walk.cursorPositions) {
            if (distance(cursor, target) <= tolerance) {
                ++result.visitedNodes;
                break;
            }
        }
    }
    if (result.totalNodes != 0)
        result.fraction = static_cast<float>(result.visitedNodes) / result.totalNodes;
    return result;
}

} // namespace rt::native::gameplay
