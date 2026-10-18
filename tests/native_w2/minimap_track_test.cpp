#include "assets/nav_routes.h"
#include "gameplay/minimap_track.h"
#include <bit>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>

namespace mm = rt::native::gameplay::minimap;
using rt::native::assets::NavGraph;

static std::uint64_t hash(const mm::TrackPolyline& lines) {
    std::uint64_t h = 14695981039346656037ull;
    for (const auto& line : lines.segments)
        for (float value : {line.a.x, line.a.y, line.b.x, line.b.y}) {
            h ^= std::bit_cast<std::uint32_t>(value); h *= 1099511628211ull;
        }
    return h;
}

int main() {
    bool poly = true, det = true, zoom = true, arrowOk = true, pins = true;
    std::size_t nodes = 0;
    auto check = [&](const NavGraph& graph) {
        nodes += graph.segments.size();
        if (graph.segments.empty()) { poly = false; return; }
        const auto origin = mm::trackNode(graph.segments.front());
        // Consecutive nodes means transition-connected midpoint pairs, NOT
        // adjacent array entries. ROM maximum for ids 0..11 is 376 units.
        for (const auto& t : graph.transitions) {
            if (t.segmentA >= graph.segments.size() || t.segmentB >= graph.segments.size()) {
                poly = false; continue;
            }
            const auto a = mm::trackNode(graph.segments[t.segmentA]);
            const auto b = mm::trackNode(graph.segments[t.segmentB]);
            const float distance = std::hypot(a.x - b.x, a.y - b.y);
            poly &= std::isfinite(distance) && distance <= 400.f;
        }
        for (int level = 0; level < 5; ++level) {
            const auto first = mm::buildTrackPolyline(graph, origin.x, origin.y, level);
            det &= hash(first) == hash(mm::buildTrackPolyline(graph, origin.x, origin.y, level));
            for (const auto& s : first.segments)
                det &= mm::scissorContains(mm::kScissorTownNtsc, s.a) &&
                       mm::scissorContains(mm::kScissorTownNtsc, s.b);
            const auto arrow = mm::buildTrackArrow(origin.x, origin.y, .7f, level);
            arrowOk &= arrow.position.x == 102.f && arrow.position.y == 183.f && arrow.scale == .62f;
        }
        // Unclipped displacement must grow; clipped length need not grow when
        // an edge leaves the window. Check every node at all five zoom levels.
        for (const auto& s : graph.segments) {
            const auto node = mm::trackNode(s);
            float previous = -1.f;
            for (int level = 0; level < 5; ++level) {
                const auto p = mm::trackProject(origin.x, origin.y, node.x, node.y, level);
                const float distance = std::hypot(p.x - 102.f, p.y - 183.f);
                zoom &= distance + .001f >= previous;
                previous = distance;
            }
        }
    };
    NavGraph synthetic;
    synthetic.segments = {{-100.f, 0.f, 100.f, 0.f}, {-100.f, 100.f, 100.f, 100.f},
                          {0.f, 200.f, 200.f, 200.f}, {0.f, 0.f, 200.f, 0.f}};
    synthetic.transitions = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};
    check(synthetic);
    const auto visible = mm::buildTrackPolyline(synthetic, 0.f, 0.f, 0);
    poly &= visible.segments.size() == 4;
    for (std::size_t i = 1; i < visible.segments.size(); ++i)
        poly &= visible.segments[i - 1].b.x == visible.segments[i].a.x &&
                visible.segments[i - 1].b.y == visible.segments[i].a.y;
    mm::TrackSegment crossing{{-100.f, 180.f}, {300.f, 180.f}};
    det &= mm::clipTrackSegment(crossing) && std::fabs(crossing.a.x - 45.f) < .0001f &&
           std::fabs(crossing.b.x - 158.f) < .0001f;
    mm::TrackSegment outside{{0.f, 0.f}, {1.f, 0.f}};
    det &= !mm::clipTrackSegment(outside);
    mm::TrackSegment invalid{{std::numeric_limits<float>::quiet_NaN(), 180.f}, {100.f, 180.f}};
    det &= !mm::clipTrackSegment(invalid);
    const auto north = mm::buildTrackArrow(0.f, 0.f, 0.f, 0);
    const auto east = mm::buildTrackArrow(0.f, 0.f, 1.57079632679f, 0);
    arrowOk &= north.vertices[0].y < north.position.y && east.vertices[0].x > east.position.x;

    std::filesystem::path path;
    if (const char* env = std::getenv("RT_ELF")) path = env;
    else if (const char* home = std::getenv("HOME"))
        path = std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
    if (!std::filesystem::exists(path)) std::cout << "NO_ELF\n";
    else {
        const auto tables = rt::native::assets::NavRouteTables::open(path);
        if (!tables) pins = false;
        else for (std::size_t id = 0; id < 12; ++id) {
            const auto graph = tables->staticGraph(id);
            if (!graph) { pins = false; continue; }
            std::cout << "GRAPH id=" << id << " nodes=" << graph->segments.size() << '\n';
            check(*graph);
            if (id == 0) {
                // Directory[0] at 0x2BD578 -> first cross-section 0x2AC620:
                // (590,569)->(590,571). Independent raw-byte pin, not roundtrip.
                std::ifstream rom(path, std::ios::binary);
                rom.seekg(0x2ac620 - 0x1ff000);
                std::array<unsigned char, 16> bytes{};
                rom.read(reinterpret_cast<char*>(bytes.data()), 16);
                pins &= bool(rom) && graph->segmentVaddr == 0x2ac620;
                const auto& s = graph->segments.front();
                const std::array<float, 4> expected{590.f, 569.f, 590.f, 571.f};
                const std::array<float, 4> actual{s.ax, s.az, s.bx, s.bz};
                for (std::size_t i = 0; i < 4; ++i) {
                    const std::size_t o = i * 4;
                    const std::uint32_t raw = std::uint32_t(bytes[o]) | (std::uint32_t(bytes[o+1]) << 8) |
                        (std::uint32_t(bytes[o+2]) << 16) | (std::uint32_t(bytes[o+3]) << 24);
                    pins &= raw == std::bit_cast<std::uint32_t>(expected[i]) && actual[i] == expected[i];
                }
            }
        }
    }
    poly &= arrowOk && pins;
    std::cout << "SCORE nodes=" << nodes << " polyline=" << (poly ? "ok" : "fail")
              << " clipdet=" << det << " zoommono=" << zoom << '\n';
    return poly && det && zoom ? 0 : 1;
}
