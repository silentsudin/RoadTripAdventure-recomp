#include "gameplay/nav_path.h"

#include <cmath>
#include <limits>
#include <vector>

namespace rt::native::gameplay {
namespace {

Pt midpoint(const assets::NavSegment& segment) {
    return {(segment.ax + segment.bx) * 0.5f, (segment.az + segment.bz) * 0.5f};
}

float dot(Pt a, Pt b) { return a.x * b.x + a.z * b.z; }

Pt heading(const assets::NavGraph& graph, const assets::NavTransition& transition,
           NavPathDirection direction) {
    const Pt a = midpoint(graph.segments[transition.segmentA]);
    const Pt b = midpoint(graph.segments[transition.segmentB]);
    return direction == NavPathDirection::SideA ? Pt{b.x - a.x, b.z - a.z}
                                                : Pt{a.x - b.x, a.z - b.z};
}

void appendSegmentCenter(const assets::NavGraph& graph, std::size_t index,
                         std::vector<bool>& emitted, NavPath& path) {
    if (!emitted[index]) {
        path.centerline.push_back(midpoint(graph.segments[index]));
        emitted[index] = true;
    }
}

} // namespace

float NavPath::totalLength() const {
    float length = 0.f;
    for (std::size_t i = 1; i < centerline.size(); ++i) {
        const float dx = centerline[i].x - centerline[i - 1].x;
        const float dz = centerline[i].z - centerline[i - 1].z;
        length += std::sqrt(dx * dx + dz * dz);
    }
    return length;
}

NavPath buildNavPath(const assets::NavGraph& graph, std::size_t startTransition,
                     NavPathDirection direction) {
    NavPath path;
    const std::size_t count = graph.transitions.size();
    if (startTransition >= count) return path;

    std::vector<bool> visited(count, false);
    std::vector<bool> emitted(graph.segments.size(), false);
    std::size_t current = startTransition;
    while (true) {
        if (current >= count || visited[current]) break;
        const assets::NavTransition& transition = graph.transitions[current];
        if (transition.segmentA >= graph.segments.size() || transition.segmentB >= graph.segments.size()) break;

        visited[current] = true;
        path.nodeOrder.push_back(static_cast<int>(current));
        appendSegmentCenter(graph, transition.segmentA, emitted, path);
        appendSegmentCenter(graph, transition.segmentB, emitted, path);

        const std::uint8_t* candidates = direction == NavPathDirection::SideA
                                             ? transition.sideACandidates
                                             : transition.sideBCandidates;
        // A candidate byte is an unbounded ROM byte, while this host graph
        // has an explicit decoded count.  Do not reinterpret malformed data
        // by silently taking the other branch.
        if (candidates[0] >= count || candidates[1] >= count) break;
        std::size_t best = count;
        float bestDot = -std::numeric_limits<float>::infinity();
        const Pt currentHeading = heading(graph, transition, direction);
        for (std::size_t slot = 0; slot < 2; ++slot) {
            const std::size_t candidate = candidates[slot];
            if (candidate != startTransition && visited[candidate]) continue;
            const assets::NavTransition& next = graph.transitions[candidate];
            if (next.segmentA >= graph.segments.size() || next.segmentB >= graph.segments.size()) continue;
            const float score = dot(currentHeading, heading(graph, next, direction));
            if (score > bestDot) {
                bestDot = score;
                best = candidate;
            }
        }
        if (best == count) break;
        if (best == startTransition) {
            path.nodeOrder.push_back(static_cast<int>(best));
            path.closed = true;
            break;
        }
        current = best;
    }
    return path;
}

} // namespace rt::native::gameplay
