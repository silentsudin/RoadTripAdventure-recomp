// Implementation for gameplay/map_view.h.  Pure C++, deterministic: no
// wall-clock time, no randomness, no I/O.  See the header for the grounding
// ledger and docs/native-reference/map-view.md for the design.
#include "gameplay/map_view.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>

namespace rt::native::gameplay {
namespace {

constexpr double kHalfPeriod = kMapPeriod * 0.5;
constexpr float kRouteInf = std::numeric_limits<float>::infinity();

// Streaming id for a map-space grid cell (the inverse of tilePlacement's
// proven algebra, world_layout.cpp: id = (A<<4) | (col<<1) | b with
// A = row>>1 (the (id>>4)&3 pair) and b = row&1 (the id&1 bit)).
int tileIdFromGrid(int row, int col) {
    return ((row >> 1) << 4) | (col << 1) | (row & 1);
}

// Transition-node position with the existing centerline convention
// (gameplay/nav_path.h: the average of the two joined segments' midpoints).
Vec2 nodePos(const assets::NavGraph& graph, int transition) {
    const assets::NavSegment& a = graph.segments[graph.transitions[transition].segmentA];
    const assets::NavSegment& b = graph.segments[graph.transitions[transition].segmentB];
    const float ax = (a.ax + a.bx) * 0.5f;
    const float az = (a.az + a.bz) * 0.5f;
    const float bx = (b.ax + b.bx) * 0.5f;
    const float bz = (b.az + b.bz) * 0.5f;
    return {(ax + bx) * 0.5f, (az + bz) * 0.5f};
}

// Adjacency (the existing graph module's own topology): two transitions are
// neighbours when they share a segment index.
bool transitionsShareSegment(const assets::NavTransition& a, const assets::NavTransition& b) {
    return a.segmentA == b.segmentA || a.segmentA == b.segmentB ||
           a.segmentB == b.segmentA || a.segmentB == b.segmentB;
}

}  // namespace

// ---------------------------------------------------------------- map space

double wrapMapAxis(double v) {
    if (!std::isfinite(v)) return 0.0;  // deterministic degenerate guard
    double r = std::fmod(v, kMapPeriod);
    if (r < 0.0) r += kMapPeriod;
    return r;  // [0, kMapPeriod)
}

double mapDelta(double a, double b) {
    double d = wrapMapAxis(b) - wrapMapAxis(a);
    while (d > kHalfPeriod) d -= kMapPeriod;
    if (d == -kHalfPeriod) d = kHalfPeriod;  // canonical tie -> the + side
    while (d < -kHalfPeriod) d += kMapPeriod;
    return d;  // (-kHalfPeriod, kHalfPeriod]
}

MapPoint toMapSpace(FixedPos world) {
    // Stored-grid wrap first (& 0x0fffffff, FUN_0021ba70 write mask), then
    // the odd-row shear removal (the inverse of the renderer's local rebase
    // "+((fixedZ & 0x02000000) >> 1)", world-geography.md).
    const FixedPos stored = wrapToras(world);
    const std::uint32_t uz = static_cast<std::uint32_t>(stored.z);
    const std::int64_t shear = static_cast<std::int64_t>((uz & 0x02000000u) >> 1);
    const std::int64_t x = static_cast<std::int64_t>(stored.x) - shear;
    const std::int64_t z = static_cast<std::int64_t>(stored.z);
    return {wrapMapAxis(static_cast<double>(x)), wrapMapAxis(static_cast<double>(z))};
}

FixedPos fromMapSpace(MapPoint p) {
    const std::int64_t z = static_cast<std::int64_t>(wrapMapAxis(p.z));
    const std::int64_t shear =
        static_cast<std::int64_t>((static_cast<std::uint32_t>(z) & 0x02000000u) >> 1);
    const std::int64_t x =
        static_cast<std::int64_t>(wrapMapAxis(p.x) + static_cast<double>(shear));
    return {static_cast<std::int32_t>(x), static_cast<std::int32_t>(z)};
}

MapPoint mapOriginOfTile(int tileId) {
    if (tileId < 0 || tileId >= static_cast<int>(assets::kFieldTileCount)) return {};
    const TilePlacement t = tilePlacement(tileId);  // proven placement algebra
    const std::int64_t shear =
        static_cast<std::int64_t>(t.row & 1 ? kHalfTileFixedUnits : 0);
    const std::int64_t x = static_cast<std::int64_t>(t.x0) - shear;
    return {wrapMapAxis(static_cast<double>(x)), wrapMapAxis(static_cast<double>(t.z0))};
}

int tileAtMapPoint(MapPoint p) {
    const int col =
        std::clamp(static_cast<int>(wrapMapAxis(p.x) / kMapTileSpan), 0, kMapGridTiles - 1);
    const int row =
        std::clamp(static_cast<int>(wrapMapAxis(p.z) / kMapTileSpan), 0, kMapGridTiles - 1);
    return tileIdFromGrid(row, col);
}

MapPoint tileLocalToMap(int tileId, double localX, double localZ) {
    const MapPoint origin = mapOriginOfTile(tileId);
    // kFixedUnitsPerGameUnit is the f32-rounded scale S = DAT_00332c0c
    // (20971.51953125 = a dyadic double), so the product is deterministic;
    // 1600 * S stays inside one tile span (span/S = 1600.000036 per
    // world-geography.md) and the wrap keeps results canonical.
    return {wrapMapAxis(origin.x + localX * kFixedUnitsPerGameUnit),
            wrapMapAxis(origin.z + localZ * kFixedUnitsPerGameUnit)};
}

// ---------------------------------------------------------------- MapCamera

void MapCamera::setCenterMap(MapPoint p) { center_ = {wrapMapAxis(p.x), wrapMapAxis(p.z)}; }

void MapCamera::setCenterWorld(FixedPos world) { setCenterMap(toMapSpace(world)); }

void MapCamera::setCenterTileLocal(int tileId, double lx, double lz) {
    setCenterMap(tileLocalToMap(tileId, lx, lz));
}

void MapCamera::setZoomTiles(double tiles) {
    if (std::isfinite(tiles) && tiles > 0.0) zoomTiles_ = tiles;
}

void MapCamera::setAspect(double widthOverHeight) {
    if (std::isfinite(widthOverHeight) && widthOverHeight > 0.0) aspect_ = widthOverHeight;
}

double MapCamera::halfExtentZ() const { return zoomTiles_ * kMapTileSpan * 0.5; }

double MapCamera::halfExtentX() const { return halfExtentZ() * aspect_; }

NdcPoint MapCamera::mapToNormalized(MapPoint p) const {
    return {mapDelta(center_.x, p.x) / halfExtentX(), mapDelta(center_.z, p.z) / halfExtentZ()};
}

MapPoint MapCamera::normalizedToMap(NdcPoint ndc) const {
    return {wrapMapAxis(center_.x + ndc.u * halfExtentX()),
            wrapMapAxis(center_.z + ndc.v * halfExtentZ())};
}

std::vector<MapCamera::ProjectedRun> MapCamera::projectPolyline(
    const std::vector<MapPoint>& points) const {
    std::vector<ProjectedRun> runs;
    if (points.size() < 2) return runs;

    // Continuity through the seam: keep the previous vertex's chosen
    // representative (fixed units, shifted by whole periods on demand) and
    // bring each next vertex to the nearest representative to it.  With
    // all inputs inside one tile (+/-1600 local units) no shift ever fires;
    // a polyline near the seam nonetheless draws through it untorn.
    struct Rep {
        double x = 0.0;
        double z = 0.0;
    };
    Rep prev{wrapMapAxis(points[0].x), wrapMapAxis(points[0].z)};
    const auto project = [this](const Rep& r) {
        // Plain (not shortest-delta) subtraction: the chain is already
        // continuity-chosen, so the run stays one line; the wrapped tail
        // leaving the viewport is honest clipping, not a tear.
        return NdcPoint{(r.x - center_.x) / halfExtentX(), (r.z - center_.z) / halfExtentZ()};
    };
    ProjectedRun run;
    run.points.push_back(project(prev));
    for (std::size_t i = 1; i < points.size(); ++i) {
        Rep next{wrapMapAxis(points[i].x), wrapMapAxis(points[i].z)};
        while (next.x - prev.x > kHalfPeriod) next.x -= kMapPeriod;
        while (next.x - prev.x < -kHalfPeriod) next.x += kMapPeriod;
        while (next.z - prev.z > kHalfPeriod) next.z -= kMapPeriod;
        while (next.z - prev.z < -kHalfPeriod) next.z += kMapPeriod;
        run.points.push_back(project(next));
        prev = next;
    }
    runs.push_back(std::move(run));
    return runs;
}

PixelPoint MapCamera::normalizedToPixels(NdcPoint ndc, const PixelRect& rect) {
    return {rect.x + (ndc.u + 1.0) * 0.5 * rect.w, rect.y + (1.0 - ndc.v) * 0.5 * rect.h};
}

NdcPoint MapCamera::pixelsToNormalized(PixelPoint px, const PixelRect& rect) {
    if (rect.w <= 0.0 || rect.h <= 0.0) return {};
    return {(px.x - rect.x) * 2.0 / rect.w - 1.0, (rect.y + rect.h - px.y) * 2.0 / rect.h - 1.0};
}

// ---------------------------------------------------------------- markers

Vec2 doorCentroid(const TownDoor& door) {
    double x = 0.0;
    double z = 0.0;
    for (std::size_t i = 0; i < 4; ++i) {
        x += door.x[i];
        z += door.z[i];
    }
    return {static_cast<float>(x * 0.25), static_cast<float>(z * 0.25)};
}

bool photoTaken(const SaveData& save, int photoNumber) {
    // 100 photos; photo n taken <=> bit n-1 of the 0x508 bitmap (PROVEN).
    // An unidentifiable photo number (out of range) hides the marker rather
    // than showing a booth that can never be marked used.
    if (photoNumber < 1 || photoNumber > kPhotoCount) return true;
    const std::size_t bit = static_cast<std::size_t>(photoNumber) - 1;
    return (save.photosTaken[bit / 8] & (1u << (bit % 8))) != 0;
}

bool houseUnvisited(const SaveData& save, int townId, int doorOrdinal) {
    // 10 x u32 at 0xBD8, bit n = door n not yet visited (PROVEN).  Locations
    // outside 0..9 have no door bitmap: their house doors are dropped.
    if (townId < 0 || static_cast<std::size_t>(townId) >= save.unvisitedDoors.size())
        return false;
    if (doorOrdinal < 0 || doorOrdinal >= 32) return false;
    return ((save.unvisitedDoors[townId] >> doorOrdinal) & 1u) != 0;
}

std::vector<MapMarker> gatherMarkers(int townId, const TownMarkerSource& town,
                                     const SaveData& save) {
    std::vector<MapMarker> markers;
    markers.reserve(town.doors.size() + town.exits.size());
    for (const TownDoor& door : town.doors) {
        bool keep = false;
        switch (door.kind) {
            case MarkerKind::Garage:
            case MarkerKind::ShopP:
            case MarkerKind::ShopB:
            case MarkerKind::ShopC:
            case MarkerKind::TownExit:
            case MarkerKind::RaceGate:
                keep = true;  // businesses and portals are always shown
                break;
            case MarkerKind::QuickPic:
                keep = !photoTaken(save, door.ordinal);  // not yet used
                break;
            case MarkerKind::HouseUnvisited:
                keep = houseUnvisited(save, townId, door.ordinal);  // not yet visited
                break;
            case MarkerKind::Player:
                keep = false;  // the player marker comes from the session
                break;
        }
        if (!keep) continue;
        const Vec2 centre = doorCentroid(door);
        MapMarker marker;
        marker.kind = door.kind;
        marker.ordinal = door.ordinal;
        marker.townId = townId;
        marker.tileId = town.tileId;
        marker.map = tileLocalToMap(town.tileId, centre.x, centre.z);
        markers.push_back(marker);
    }
    for (std::size_t i = 0; i < town.exits.size(); ++i) {
        MapMarker marker;
        marker.kind = MarkerKind::TownExit;
        marker.ordinal = static_cast<int>(i);
        marker.townId = townId;
        marker.tileId = town.tileId;
        marker.map = tileLocalToMap(town.tileId, town.exits[i].first, town.exits[i].second);
        markers.push_back(marker);
    }
    return markers;
}

// ---------------------------------------------------------------- route

float MapRoute::totalLength() const {
    return cumulative.empty() ? 0.0f : cumulative.back();
}

float MapRoute::progressAt(Vec2 tileLocalPos) const {
    if (!valid || points.size() < 2) return 0.0f;
    float bestDistanceSq = kRouteInf;
    std::size_t bestSegment = 0;
    float bestT = 0.0f;  // param of the projection inside the best segment
    for (std::size_t i = 1; i < points.size(); ++i) {
        const Vec2 a = points[i - 1];
        const Vec2 d = points[i] - a;
        const float lenSq = lengthSq(d);
        float t = 0.0f;
        if (lenSq > 0.0f) t = std::clamp(dot(tileLocalPos - a, d) / lenSq, 0.0f, 1.0f);
        const float distanceSq = lengthSq(tileLocalPos - (a + d * t));
        if (distanceSq < bestDistanceSq) {  // strict: ties keep the earliest segment
            bestDistanceSq = distanceSq;
            bestSegment = i;
            bestT = t;
        }
    }
    const float segmentLength = cumulative[bestSegment] - cumulative[bestSegment - 1];
    const float along = cumulative[bestSegment - 1] + bestT * segmentLength;
    const float total = totalLength();
    return total > 0.0f ? std::clamp(along / total, 0.0f, 1.0f) : 0.0f;
}

MapRoute buildMapRoute(const assets::NavGraph& graph, int fromNode, int toNode) {
    MapRoute route;
    const int count = static_cast<int>(graph.transitions.size());
    if (fromNode < 0 || fromNode >= count || toNode < 0 || toNode >= count) return route;
    if (graph.segments.empty()) return route;

    if (fromNode == toNode) {
        route.nodes.push_back(fromNode);
        route.points.push_back(nodePos(graph, fromNode));
        route.cumulative.push_back(0.0f);
        route.valid = true;
        return route;
    }

    // Shared-segment adjacency with Euclidean weights between node
    // positions; built in ascending pair order so each pair appears once.
    std::vector<std::vector<std::pair<int, float>>> adjacency(count);
    for (int i = 0; i < count; ++i) {
        for (int j = i + 1; j < count; ++j) {
            if (!transitionsShareSegment(graph.transitions[i], graph.transitions[j])) continue;
            const float weight = length(nodePos(graph, i) - nodePos(graph, j));
            adjacency[i].push_back({j, weight});
            adjacency[j].push_back({i, weight});
        }
    }

    // Deterministic Dijkstra: settle the undone node with the smallest
    // (cost, index); replace a predecessor only on strictly smaller cost, or
    // on equal cost when the candidate predecessor index is lower.  Equal
    // total costs therefore pick the lower-index predecessor: the reported
    // tie-break.
    std::vector<float> dist(count, kRouteInf);
    std::vector<int> prev(count, -1);
    std::vector<bool> done(count, false);
    dist[fromNode] = 0.0f;
    for (int step = 0; step < count; ++step) {
        int u = -1;
        for (int i = 0; i < count; ++i) {
            if (done[i] || dist[i] == kRouteInf) continue;
            if (u < 0 || dist[i] < dist[u]) u = i;  // ties: lowest index (scan order)
        }
        if (u < 0) break;
        done[u] = true;
        if (u == toNode) break;
        for (const auto& [v, weight] : adjacency[u]) {
            if (done[v]) continue;
            const float next = dist[u] + weight;
            if (next < dist[v] || (next == dist[v] && (prev[v] < 0 || u < prev[v]))) {
                dist[v] = next;
                prev[v] = u;
            }
        }
    }
    if (dist[toNode] == kRouteInf) return route;

    for (int node = toNode; node >= 0; node = prev[node]) route.nodes.push_back(node);
    std::reverse(route.nodes.begin(), route.nodes.end());
    route.points.reserve(route.nodes.size());
    route.cumulative.reserve(route.nodes.size());
    for (int node : route.nodes) {
        route.points.push_back(nodePos(graph, node));
        if (route.points.size() == 1) {
            route.cumulative.push_back(0.0f);
        } else {
            route.cumulative.push_back(
                route.cumulative.back() +
                length(route.points.back() - route.points[route.points.size() - 2]));
        }
    }
    route.valid = true;
    return route;
}

std::vector<MapPoint> routeToMapSpace(const MapRoute& route, int tileId) {
    std::vector<MapPoint> out;
    if (!route.valid) return out;
    out.reserve(route.points.size());
    for (const Vec2& point : route.points)
        out.push_back(tileLocalToMap(tileId, point.x, point.z));
    return out;
}

// ---------------------------------------------------------------- pages

SecondScreenPages::SecondScreenPages(int notebookIdleSteps)
    : idleLimit_(notebookIdleSteps > 0 ? notebookIdleSteps : 45) {}

void SecondScreenPages::tick(SecondScreenContext context,
                             std::optional<SecondScreenPage> playerChoice) {
    if (playerChoice) {
        // A tap always wins for this step and stays until the context kind
        // changes; it also resets the cruise-idle counter and cancels the
        // auto-Notebook arm.
        page_ = *playerChoice;
        override_ = true;
        idle_ = 0;
        pendingNotebook_ = false;
    }
    const bool mapContextNow = context.racing || context.mapUp;
    if (mapContextNow != mapContext_) {
        // Context kind changed: the player override drops and the context
        // rules take over from here.  The transition tick itself is NOT an
        // idle step: the cruise-idle counter starts on the first quiet tick
        // AFTER the transition (so the auto-Notebook fires exactly
        // `notebookIdleSteps` vblanks into uninterrupted cruise).
        mapContext_ = mapContextNow;
        override_ = false;
        idle_ = 0;
        if (mapContextNow) {
            page_ = SecondScreenPage::Map;  // "map immediately"
            pendingNotebook_ = false;
        } else {
            pendingNotebook_ = true;  // cruise: arm the auto-Notebook
        }
        return;
    }
    if (mapContext_) {
        if (!override_) page_ = SecondScreenPage::Map;
    } else if (!override_ && pendingNotebook_) {
        if (++idle_ >= idleLimit_) {
            page_ = SecondScreenPage::Notebook;  // one-shot, not recurring
            pendingNotebook_ = false;
        }
    }
}

void SecondScreenPages::reset() {
    page_ = SecondScreenPage::Map;
    override_ = false;
    pendingNotebook_ = true;
    mapContext_ = false;
    idle_ = 0;
}

// ---------------------------------------------------------------- draw list

MapDrawList buildMapDrawList(const MapContentSource& source) {
    MapDrawList list;

    // Ground: all 64 tiles, row-major (row asc, then col asc).  The renderer
    // culls by camera; the grid itself never depends on the camera (the
    // sibling app's box-following-the-drawing bug cannot exist here).
    list.tiles.reserve(static_cast<std::size_t>(kMapGridTiles) * kMapGridTiles);
    for (int row = 0; row < kMapGridTiles; ++row) {
        for (int col = 0; col < kMapGridTiles; ++col) {
            MapTileCell cell;
            cell.id = tileIdFromGrid(row, col);
            cell.row = row;
            cell.col = col;
            if (source.tileIndex)
                cell.fileName = (*source.tileIndex)[static_cast<std::size_t>(cell.id)].fileName;
            list.tiles.push_back(std::move(cell));
        }
    }

    if (source.town && source.graph) {
        // Roads: one polyline per graph segment, in segment-index order,
        // anchored on the town's tile (street graphs are tile-local f32).
        list.roads.reserve(source.graph->segments.size());
        for (const assets::NavSegment& segment : source.graph->segments) {
            MapPolyline polyline;
            polyline.points.push_back(tileLocalToMap(source.town->tileId, segment.ax, segment.az));
            polyline.points.push_back(tileLocalToMap(source.town->tileId, segment.bx, segment.bz));
            polyline.closed = false;
            list.roads.push_back(std::move(polyline));
        }
        if (source.route && source.route->valid) {
            MapPolyline polyline;
            polyline.points = routeToMapSpace(*source.route, source.town->tileId);
            polyline.closed = false;
            if (polyline.points.size() >= 2) {
                list.route.push_back(std::move(polyline));
                list.routeDestinationNode = source.routeDestinationNode >= 0
                                               ? source.routeDestinationNode
                                               : static_cast<int>(source.route->nodes.back());
            }
        }
    }

    if (source.town && source.save)
        list.markers = gatherMarkers(source.town->townId, *source.town, *source.save);

    if (source.hasPlayer) {
        list.player.position = toMapSpace(source.playerWorld);
        list.player.heading = source.playerHeading;
        list.player.present = true;
    }
    return list;
}

MapProjectedFrame projectDrawList(const MapDrawList& drawList, const MapCamera& camera) {
    MapProjectedFrame frame;
    frame.tiles.reserve(drawList.tiles.size());
    for (const MapTileCell& cell : drawList.tiles) {
        MapProjectedFrame::TileQuad quad;
        quad.id = cell.id;
        const MapPoint origin = mapOriginOfTile(cell.id);
        // Counter-clockwise in (+u east, +v north): NW, SW, SE, NE.
        const MapPoint corners[4] = {
            {origin.x, origin.z + kMapTileSpan},
            {origin.x, origin.z},
            {origin.x + kMapTileSpan, origin.z},
            {origin.x + kMapTileSpan, origin.z + kMapTileSpan},
        };
        for (int i = 0; i < 4; ++i) {
            const NdcPoint ndc = camera.mapToNormalized(corners[i]);
            quad.corners[i] = ndc;
        }
        frame.tiles.push_back(quad);
    }
    frame.roads.reserve(drawList.roads.size());
    for (const MapPolyline& road : drawList.roads) {
        std::vector<MapCamera::ProjectedRun> runs = camera.projectPolyline(road.points);
        frame.roads.insert(frame.roads.end(), std::make_move_iterator(runs.begin()),
                           std::make_move_iterator(runs.end()));
    }
    for (const MapPolyline& route : drawList.route) {
        std::vector<MapCamera::ProjectedRun> runs = camera.projectPolyline(route.points);
        frame.route.insert(frame.route.end(), std::make_move_iterator(runs.begin()),
                           std::make_move_iterator(runs.end()));
    }
    frame.markers.reserve(drawList.markers.size());
    for (const MapMarker& marker : drawList.markers)
        frame.markers.push_back({marker, camera.mapToNormalized(marker.map)});
    if (drawList.player.present) {
        frame.player = camera.mapToNormalized(drawList.player.position);
        frame.playerHeading = drawList.player.heading;
        frame.playerPresent = true;
    }
    return frame;
}

}  // namespace rt::native::gameplay
