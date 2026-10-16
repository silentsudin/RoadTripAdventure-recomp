// Map/second-screen logic contract tests (gameplay/map_view.{h,cpp};
// design: docs/native-reference/map-view.md).  Synthetic fixtures
// only for the logic checks (dyadic coordinates so expected values are
// exact); the disc section pins the documented graph metadata through the
// user's own ELF and is skipped when no extract exists.
#include "../../src/native/gameplay/map_view.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <vector>

using namespace rt::native::assets;
using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

namespace {

constexpr double kTile = kMapTileSpan;  // 0x02000000
constexpr double kHalf = kMapPeriod * 0.5;

bool near(double a, double b, double eps = 1e-9) { return std::fabs(a - b) <= eps; }

// ---------------------------------------------------------------- map space

void testMapSpaceShearAndWrap() {
    // Peach Town tile 43 (scene-tile-map.md): raw origin (0x0b000000,
    // 0x0a000000), odd row -> shear 0x01000000 removed -> map (0x0a000000,
    // 0x0a000000) = (5<<25, 5<<25).
    const MapPoint peach = toMapSpace({0x0b000000, 0x0a000000});
    CHECK(peach.x == 5.0 * kTile);
    CHECK(peach.z == 5.0 * kTile);

    // Even row: no shear (tile 11, My Garage: row 1?? no - id 11 has b=1...
    // use tile 1: A=0,b=1 -> row 1; tile 2: A=0,B=1,C=2 -> b=0, row 0).
    const TilePlacement evenTile = tilePlacement(2);  // b=0 -> even row 0
    CHECK((evenTile.row & 1) == 0);
    const MapPoint evenPos = toMapSpace({evenTile.x0, evenTile.z0});
    CHECK(evenPos.x == static_cast<double>(evenTile.x0));
    CHECK(evenPos.z == static_cast<double>(evenTile.z0));

    // Odd row interior point: shear removed, round-trips through fromMapSpace.
    const FixedPos odd{0x0b000000 + 0x00800000, 0x0a000000 + 0x00800000};
    const MapPoint oddMap = toMapSpace(odd);
    CHECK(oddMap.x == 0x0a000000 + 0x00800000);
    CHECK(oddMap.z == odd.z);
    const FixedPos back = fromMapSpace(oddMap);
    CHECK(back.x == odd.x && back.z == odd.z);

    // Seam wrap: negative/unwrapped stored inputs canonicalise.  The wrapped
    // Z (0x0FF00000) sits on an ODD row (bit 25 set), so the stored X still
    // carries its row shear — FUN_0021ba70 adds ((Z & 0x02000000) >> 1) at
    // render time (world-geography.md PROVEN; map_view.h "map space" formula
    // mapX = wrap(X - shear(Z))).  Map space removes it before wrapping.
    const MapPoint wrapped = toMapSpace({-0x00100000, -0x00100000});
    CHECK(wrapped.x == kMapPeriod - 0x00100000 - kHalfTileFixedUnits);
    CHECK(wrapped.z == kMapPeriod - 0x00100000);

    // mapDelta: shortest signed displacement, canonical tie to the + side.
    CHECK(mapDelta(0.0, 0x01000000) == 0x01000000);
    CHECK(mapDelta(0x0f000000, 0x01000000) == 0x02000000);  // across the seam east
    CHECK(mapDelta(0x01000000, 0x0f000000) == -0x02000000);
    CHECK(mapDelta(0.0, kHalf) == kHalf);      // exact antipode: + side
    CHECK(mapDelta(kHalf, 0.0) == kHalf);      // same tie from the other end
}

void testTileGridInvariants() {
    // All 64 map cells sit on the clean 8x8 grid after unshearing, and the
    // id<->grid inversion matches world_layout's proven placement.
    for (int id = 0; id < 64; ++id) {
        const TilePlacement t = tilePlacement(id);
        const int row = t.row;
        const int col = (id >> 1) & 7;
        const MapPoint origin = mapOriginOfTile(id);
        CHECK(origin.x == col * kTile);
        CHECK(origin.z == row * kTile);
        CHECK(tileAtMapPoint({origin.x, origin.z}) == id);
        CHECK(tileAtMapPoint({origin.x + kTile * 0.5, origin.z + kTile * 0.5}) == id);
        // Raw fixed origin of an even-row tile is the map origin; an odd-row
        // tile's raw origin carries its +0x01000000 shear.
        CHECK(toMapSpace({t.x0, t.z0}).x == origin.x);
        CHECK(t.x0 - (t.row & 1 ? kHalfTileFixedUnits : 0) == static_cast<std::int32_t>(origin.x));
    }
    // Tile-local anchoring stays inside the cell (1600 * S < one tile span).
    for (int id = 0; id < 64; ++id) {
        const MapPoint p = tileLocalToMap(id, 1600, 1600);
        CHECK(p.x - mapOriginOfTile(id).x < kTile);
    }
}

// ---------------------------------------------------------------- projection

void testProjectionExactness() {
    MapCamera camera;
    camera.setZoomTiles(2.0);  // vertical half extent = exactly 1 tile span
    camera.setAspect(1.0);
    camera.setCenterMap({5.0 * kTile, 5.0 * kTile});  // Peach Town's cell corner

    NdcPoint p = camera.mapToNormalized({5.5 * kTile, 5.0 * kTile});
    CHECK(p.u == 0.5);   // +0.5 tiles east of centre, half extent 1 tile
    CHECK(p.v == 0.0);
    p = camera.mapToNormalized({5.0 * kTile, 4.25 * kTile});
    CHECK(p.u == 0.0);
    CHECK(p.v == -0.75);  // north-up: -0.75 tiles south
    p = camera.mapToNormalized({4.5 * kTile, 5.25 * kTile});
    CHECK(p.u == -0.5);
    CHECK(p.v == 0.25);

    // Tolerance case: game-unit (S-scaled) displacement.  S is the f32
    // rounded 2^25/1600, so 800 * S is 0.4999999... of a tile, not exactly
    // half (world-geography.md's 1600.000036 note).
    const MapPoint fromUnits = tileLocalToMap(43, 800, 0);
    p = camera.mapToNormalized(fromUnits);
    CHECK(near(p.u, 0.5, 1e-6));  // Peach cell origin + 800 units east
    CHECK(std::fabs(p.u - 0.5) > 0.0);  // and genuinely off it (f32 scale)

    // Inverse round-trip over a grid of normalized points.
    camera.setAspect(1.5);
    camera.setZoomTiles(4.0);
    bool inverseOk = true;
    for (int iu = -3; iu <= 3; ++iu) {
        for (int iv = -3; iv <= 3; ++iv) {
            const NdcPoint ndc{iu * 0.3, iv * 0.3};
            const MapPoint m = camera.normalizedToMap(ndc);
            const NdcPoint back = camera.mapToNormalized(m);
            inverseOk = inverseOk && near(back.u, ndc.u, 1e-12) && near(back.v, ndc.v, 1e-12);
        }
    }
    CHECK(inverseOk);
}

void testProjectionTorusWrapAndBounds() {
    // Camera near the seam; a marker across the seam projects on the near
    // side (shortest torus delta), i.e. the wrap is visually continuous.
    MapCamera camera;
    camera.setZoomTiles(2.0);
    camera.setAspect(1.0);
    camera.setCenterMap({0x00800000, 0x00800000});  // near the NW seam
    NdcPoint p = camera.mapToNormalized({0x0F800000, 0x00800000});  // across east seam
    CHECK(p.u == -0.5);
    p = camera.mapToNormalized({0x00800000, 0x0F800000});  // across south seam
    CHECK(p.v == -0.5);

    // Bounds: at zoom >= 8 every point of the torus lies in [-1,1]^2; at any
    // zoom, a point inside the half extents projects inside [-1,1].
    for (double zoom : {0.25, 1.0, 2.0, 4.0, 8.0, 16.0}) {
        MapCamera sweep;
        sweep.setAspect(1.25);
        sweep.setZoomTiles(zoom);
        bool insideOk = true;
        bool wholeTorusOk = true;
        for (double row = 0; row < 8; ++row) {
            for (double col = 0; col < 8; ++col) {
                for (double dz : {0.0, 2.5, 5.0, 7.5}) {
                    for (double dx : {0.0, 2.5, 5.0, 7.5}) {
                        sweep.setCenterMap({col * kTile, row * kTile});
                        const NdcPoint q = sweep.mapToNormalized({dx * kTile, dz * kTile});
                        const bool withinCulling = std::fabs(q.u) <= 1.0 && std::fabs(q.v) <= 1.0;
                        const bool expectInside =
                            std::fabs(mapDelta(sweep.centerMap().x, dx * kTile)) <=
                                sweep.halfExtentX() &&
                            std::fabs(mapDelta(sweep.centerMap().z, dz * kTile)) <=
                                sweep.halfExtentZ();
                        if (expectInside && !withinCulling) insideOk = false;
                        if (zoom >= 8.0 && !withinCulling) wholeTorusOk = false;
                    }
                }
            }
        }
        CHECK(insideOk);
        CHECK(wholeTorusOk);
    }

    // The seam-crossing polyline: two endpoints straddling the seam form ONE
    // run whose projected length is the true small delta (never torn).
    MapCamera seamCamera;
    seamCamera.setZoomTiles(4.0);
    seamCamera.setAspect(1.0);
    seamCamera.setCenterMap({kMapPeriod - 0x1000, 0x1000});  // ON the east seam
    std::vector<MapPoint> crossing{
        {kMapPeriod - 0x1000, 0x1000}, {0x1000, 0x1000}};  // 0x2000 apart across it
    const auto runs = seamCamera.projectPolyline(crossing);
    CHECK(runs.size() == 1);
    CHECK(runs[0].points.size() == 2);
    // The first vertex is the camera centre (u, v) == (0, 0) exactly; the
    // second continues east over the seam by the true 0x2000 fixed units.
    CHECK(runs[0].points[0].u == 0.0);
    CHECK(runs[0].points[0].v == 0.0);
    const double du = (runs[0].points[1].u - runs[0].points[0].u) * seamCamera.halfExtentX();
    CHECK(du == 0x2000);
    CHECK(runs[0].points[1].v == 0.0);
    // degenerate cases
    CHECK(seamCamera.projectPolyline({{0, 0}}).empty());
    CHECK(seamCamera.projectPolyline({}).empty());
}

void testPixelMapping() {
    MapCamera::PixelRect rect{10.0, 20.0, 400.0, 200.0};
    // +v = north paints towards the top of the rect.
    const auto nw = MapCamera::normalizedToPixels({-1.0, 1.0}, rect);
    CHECK(nw.x == 10.0 && nw.y == 20.0);
    const auto se = MapCamera::normalizedToPixels({1.0, -1.0}, rect);
    CHECK(se.x == 410.0 && se.y == 220.0);
    const auto centre = MapCamera::normalizedToPixels({0.0, 0.0}, rect);
    CHECK(centre.x == 210.0 && near(centre.y, 120.0));
    // inverse
    const NdcPoint ndc = MapCamera::pixelsToNormalized({210.0, 120.0}, rect);
    CHECK(near(ndc.u, 0.0) && near(ndc.v, 0.0));
    const NdcPoint bottomRight = MapCamera::pixelsToNormalized({410.0, 220.0}, rect);
    CHECK(near(bottomRight.u, 1.0) && near(bottomRight.v, -1.0));
}

// ---------------------------------------------------------------- markers

TownDoor doorQuad(double cx, double cz, MarkerKind kind, int ordinal) {
    TownDoor d;
    d.kind = kind;
    d.ordinal = ordinal;
    for (int i = 0; i < 4; ++i) {
        d.x[i] = cx + (i & 1 ? 4.0 : -4.0);   // symmetric around (cx, cz)
        d.z[i] = cz + (i & 2 ? 5.0 : -5.0);
    }
    return d;
}

TownMarkerSource peachLikeSource() {
    TownMarkerSource town;
    town.townId = 1;   // save location axis (Peach Town)
    town.sceneId = 1;  // scene-table record (Peach Town)
    town.tileId = 43;  // FLD/223.BIN, scene-tile-map.md
    town.doors = {
        doorQuad(400, 400, MarkerKind::Garage, 0),
        doorQuad(500, 400, MarkerKind::ShopP, 1),
        doorQuad(600, 400, MarkerKind::ShopB, 2),
        doorQuad(700, 400, MarkerKind::ShopC, 3),
        doorQuad(400, 500, MarkerKind::QuickPic, 1),   // photo 1
        doorQuad(400, 600, MarkerKind::QuickPic, 2),  // photo 2
        doorQuad(400, 700, MarkerKind::HouseUnvisited, 7),
        doorQuad(400, 800, MarkerKind::HouseUnvisited, 8),
        doorQuad(500, 800, MarkerKind::Player, 0),    // never gathered
    };
    town.exits = {{800.0, 800.0}, {900.0, 900.0}};
    return town;
}

void testMarkerFiltering() {
    const TownMarkerSource town = peachLikeSource();
    SaveData save;
    save.photosTaken[0] = 0x01;  // photo 1 taken (bit 0)
    save.unvisitedDoors[1] = (1u << 7);  // house door 7 unvisited; door 8 visited

    const std::vector<MapMarker> markers = gatherMarkers(1, town, save);
    CHECK(markers.size() == 8);
    if (markers.size() != 8) return;
    // Deterministic source order: shops first (list order), photo 2 kept /
    // photo 1 dropped, house 7 kept / house 8 dropped, Player never, exits
    // appended last.
    CHECK(markers[0].kind == MarkerKind::Garage);
    CHECK(markers[1].kind == MarkerKind::ShopP);
    CHECK(markers[2].kind == MarkerKind::ShopB);
    CHECK(markers[3].kind == MarkerKind::ShopC);
    CHECK(markers[4].kind == MarkerKind::QuickPic);
    CHECK(markers[4].ordinal == 2);
    CHECK(markers[5].kind == MarkerKind::HouseUnvisited);
    CHECK(markers[5].ordinal == 7);
    CHECK(markers[6].kind == MarkerKind::TownExit);  // exit 0
    CHECK(markers[6].ordinal == 0);
    CHECK(markers[7].kind == MarkerKind::TownExit);  // exit 1
    CHECK(markers[7].ordinal == 1);
    for (const MapMarker& m : markers) {
        CHECK(m.townId == 1);
        CHECK(m.tileId == 43);
        CHECK(m.map.x >= 5.0 * kTile && m.map.x < 6.0 * kTile);
        CHECK(m.map.z >= 5.0 * kTile && m.map.z < 6.0 * kTile);
    }
    // Positions: the garage quad's centroid in Peach's cell (5<<25 + 400*S).
    const double expectX = 5.0 * kTile + 400.0 * kFixedUnitsPerGameUnit;
    CHECK(near(markers[0].map.x, expectX, 1e-6));
    CHECK(near(markers[0].map.z, expectX, 1e-6));  // (400, 400) quad

    // House bits and photo bits are independent: a New-Game-like save (all
    // house bits set at 0xBD8, nothing photographed) shows both photos and
    // both houses.
    SaveData fresh;
    fresh.unvisitedDoors[1] = (1u << 7) | (1u << 8);
    const std::vector<MapMarker> all = gatherMarkers(1, town, fresh);
    CHECK(all.size() == 10);  // garage + 3 shops + 2 photos + 2 houses + 2 exits
    std::size_t houses = 0, photos = 0;
    for (const MapMarker& m : all) {
        if (m.kind == MarkerKind::HouseUnvisited) ++houses;
        if (m.kind == MarkerKind::QuickPic) ++photos;
    }
    CHECK(houses == 2);
    CHECK(photos == 2);

    // A town without a door bitmap (save axis >= 10): shops/photos/exits
    // still show, house doors are dropped, the Player-kind door never shows.
    const std::vector<MapMarker> fieldMarkers = gatherMarkers(10, town, fresh);
    std::size_t fieldHouses = 0, fieldOthers = 0;
    for (const MapMarker& m : fieldMarkers)
        m.kind == MarkerKind::HouseUnvisited ? ++fieldHouses : ++fieldOthers;
    CHECK(fieldHouses == 0);
    CHECK(fieldOthers == 8);  // garage + 3 shops + 2 photos + 2 exits (Player skipped)
}

void testDoorCentroid() {
    const TownDoor d = doorQuad(100, 200, MarkerKind::Garage, 0);
    const Vec2 c = doorCentroid(d);
    CHECK(near(c.x, 100.0, 1e-6));
    CHECK(near(c.z, 200.0, 1e-6));
}

// ---------------------------------------------------------------- route

NavSegment seg(float ax, float az, float bx, float bz) { return {ax, az, bx, bz}; }

NavTransition tr(std::uint8_t a, std::uint8_t b) {
    NavTransition t;
    t.segmentA = a;
    t.segmentB = b;
    t.sideACandidates[0] = 0xFF;  // unreachable candidate bytes
    t.sideACandidates[1] = 0xFF;
    t.sideBCandidates[0] = 0xFF;
    t.sideBCandidates[1] = 0xFF;
    return t;
}

// A diamond: segments 0..3 laid out as the sides of a 10x10 square; the four
// transitions sit on the corners.  Two equal-cost paths 0-1-2 and 0-3-2.
NavGraph diamondGraph() {
    NavGraph g;
    g.segments = {
        seg(0, 0, 10, 0),  // 0: midpoint (5, 0)
        seg(10, 0, 10, 10),  // 1: midpoint (10, 5)
        seg(0, 10, 10, 10),  // 2: midpoint (5, 10)
        seg(0, 0, 0, 10),  // 3: midpoint (0, 5)
    };
    g.transitions = {
        tr(0, 1),  // node 0 at (7.5, 2.5)
        tr(1, 2),  // node 1 at (7.5, 7.5)
        tr(2, 3),  // node 2 at (2.5, 7.5)
        tr(3, 0),  // node 3 at (2.5, 2.5)
    };
    return g;
}

void testRouteOnDiamond() {
    const NavGraph g = diamondGraph();
    const MapRoute route = buildMapRoute(g, 0, 2);
    CHECK(route.valid);
    CHECK((route.nodes == std::vector<int>{0, 1, 2}));  // equal costs -> lower ids
    CHECK(route.points.size() == 3);
    CHECK(near(route.points[0].x, 7.5) && near(route.points[0].z, 2.5));
    CHECK(near(route.points[1].x, 7.5) && near(route.points[1].z, 7.5));
    CHECK(near(route.points[2].x, 2.5) && near(route.points[2].z, 7.5));
    CHECK((route.cumulative == std::vector<float>{0.0f, 5.0f, 10.0f}));
    CHECK(near(route.totalLength(), 10.0f, 1e-6f));

    // Progress: at the source 0, the waypoint 1, the destination 2, and a
    // point off the polyline snap-projecting to the vertical first segment.
    CHECK(near(route.progressAt({7.5f, 2.5f}), 0.0f, 1e-6));
    CHECK(near(route.progressAt({7.5f, 7.5f}), 0.5f, 1e-6));
    CHECK(near(route.progressAt({2.5f, 7.5f}), 1.0f, 1e-6));
    CHECK(near(route.progressAt({0.0f, 0.0f}), 0.0f, 1e-6));  // nearest = source
    CHECK(near(route.progressAt({9.0f, 5.0f}), 0.25f, 1e-6));  // midway by projection

    // Degenerate and invalid cases.
    const MapRoute same = buildMapRoute(g, 2, 2);
    CHECK(same.valid);
    CHECK(same.points.size() == 1);
    CHECK(near(same.totalLength(), 0.0f));
    const float emptyProgress = MapRoute().progressAt({1, 1});  // default route: no crash
    CHECK(emptyProgress == 0.0f);
    CHECK(!buildMapRoute(g, -1, 2).valid);
    CHECK(!buildMapRoute(g, 0, 99).valid);

    // Disconnected graph: no path.
    NavGraph split;
    split.segments = {seg(0, 0, 10, 0), seg(20, 0, 30, 0)};
    split.transitions = {tr(0, 0), tr(1, 1)};
    CHECK(!buildMapRoute(split, 0, 1).valid);

    // Map-space anchoring on Peach's tile.
    const std::vector<MapPoint> mapPoints = routeToMapSpace(route, 43);
    CHECK(mapPoints.size() == 3);
    CHECK(near(mapPoints[0].x, 5.0 * kTile + 7.5 * kFixedUnitsPerGameUnit, 1e-6));
    CHECK(near(mapPoints[0].z, 5.0 * kTile + 2.5 * kFixedUnitsPerGameUnit, 1e-6));
}

// ---------------------------------------------------------------- pages

void tickQuiet(SecondScreenPages& pages, int steps,
               bool racing = false, bool mapUp = false) {
    for (int i = 0; i < steps; ++i)
        pages.tick({racing, mapUp}, std::nullopt);
}

void testPageStateMachine() {
    // Initial page is Map; 45 untouched cruise steps hand the page to the
    // Notebook exactly once (not recurring).
    {
        SecondScreenPages pages;
        CHECK(pages.page() == SecondScreenPage::Map);
        tickQuiet(pages, 44);
        CHECK(pages.page() == SecondScreenPage::Map);
        tickQuiet(pages, 1);
        CHECK(pages.page() == SecondScreenPage::Notebook);
        CHECK(pages.idleSteps() == 45);
        tickQuiet(pages, 100);
        CHECK(pages.page() == SecondScreenPage::Notebook);
    }
    // Custom idle limit.
    {
        SecondScreenPages pages(10);
        tickQuiet(pages, 9);
        CHECK(pages.page() == SecondScreenPage::Map);
        tickQuiet(pages, 1);
        CHECK(pages.page() == SecondScreenPage::Notebook);
    }
    // Player choice is sticky through cruise idling.
    {
        SecondScreenPages pages;
        pages.tick({}, SecondScreenPage::Radio);
        CHECK(pages.page() == SecondScreenPage::Radio);
        tickQuiet(pages, 500);
        CHECK(pages.page() == SecondScreenPage::Radio);
        pages.tick({}, SecondScreenPage::Map);
        CHECK(pages.page() == SecondScreenPage::Map);
        tickQuiet(pages, 500);
        CHECK(pages.page() == SecondScreenPage::Map);  // stays: no auto-Notebook armed
    }
    // Racing drives the map immediately and clears overrides on the kind
    // change; a mid-race Notebook choice is sticky until racing ends.
    {
        SecondScreenPages pages;
        tickQuiet(pages, 60);
        CHECK(pages.page() == SecondScreenPage::Notebook);
        pages.tick({true, false}, std::nullopt);
        CHECK(pages.page() == SecondScreenPage::Map);
        tickQuiet(pages, 300, true);
        CHECK(pages.page() == SecondScreenPage::Map);
        pages.tick({true, false}, SecondScreenPage::Notebook);
        CHECK(pages.page() == SecondScreenPage::Notebook);
        tickQuiet(pages, 100, true);
        CHECK(pages.page() == SecondScreenPage::Notebook);  // sticky mid-race
        tickQuiet(pages, 1, false);  // race ends: context kind change
        CHECK(pages.page() == SecondScreenPage::Notebook);
        CHECK(pages.idleSteps() == 0);
        tickQuiet(pages, 44);
        CHECK(pages.idleSteps() == 44);
        CHECK(pages.page() == SecondScreenPage::Notebook);  // still (re-armed, one-shot)
        pages.tick({true, false}, std::nullopt);            // next race
        CHECK(pages.page() == SecondScreenPage::Map);
    }
    // mapUp (native predicate: map button held / navigation active) behaves
    // like racing while held; releasing re-arms the idle count.
    {
        SecondScreenPages pages;
        tickQuiet(pages, 30);
        CHECK(pages.idleSteps() == 30);
        tickQuiet(pages, 1, false, true);
        CHECK(pages.page() == SecondScreenPage::Map);
        CHECK(pages.idleSteps() == 0);
        tickQuiet(pages, 10, false, true);
        CHECK(pages.page() == SecondScreenPage::Map);
        tickQuiet(pages, 1);
        CHECK(pages.page() == SecondScreenPage::Map);
        CHECK(pages.idleSteps() == 0);
        tickQuiet(pages, 45);
        CHECK(pages.page() == SecondScreenPage::Notebook);
    }
    // Choice and context change in one tick: the context change wins the
    // override (order documented in the header).
    {
        SecondScreenPages pages;
        pages.tick({true, false}, SecondScreenPage::Radio);
        CHECK(pages.page() == SecondScreenPage::Map);  // racing starts this tick
    }
    // Map hand-back: the main window suppresses its map only while the
    // second screen exists AND shows the map.
    {
        SecondScreenPages pages;
        CHECK(pages.secondScreenEnabled());
        pages.tick({true, true}, std::nullopt);
        CHECK(pages.showsMap());
        CHECK(pages.mapSuppressedOnMainWindow());
        // Choosing Notebook/Radio hands the map back to the main window.
        pages.tick({true, true}, SecondScreenPage::Notebook);
        CHECK(pages.page() == SecondScreenPage::Notebook);  // sticky choice
        CHECK(!pages.mapSuppressedOnMainWindow());
        // No second screen at all: nothing is suppressed for the window.
        pages.setSecondScreenEnabled(false);
        CHECK(!pages.showsMap());
        CHECK(!pages.mapSuppressedOnMainWindow());
    }
}

// ---------------------------------------------------------------- draw list

void testDrawListAndProjection() {
    const TownMarkerSource town = peachLikeSource();
    const NavGraph g = diamondGraph();
    const MapRoute route = buildMapRoute(g, 0, 2);
    SaveData save;
    save.photosTaken[0] = 0x01;  // photo 1 taken — mirrors testMarkerFiltering;
                                 // without it both Quick-Pic doors show (9 markers)
    save.unvisitedDoors[1] = (1u << 7);

    MapContentSource source;
    source.town = &town;
    source.graph = &g;
    source.route = &route;
    source.save = &save;
    source.hasPlayer = true;
    source.playerHeading = 0.7853981633974483;  // north-east, CCW-consistent
    const FixedPos playerRaw{0x0b000000 + 0x00400000, 0x0a000000 + 0x00400000};
    source.playerWorld = playerRaw;

    const MapDrawList list = buildMapDrawList(source);
    CHECK(list.tiles.size() == 64);
    bool gridOk = true;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const MapTileCell& cell =
                list.tiles[static_cast<std::size_t>(row) * 8 + col];
            gridOk = gridOk && cell.row == row && cell.col == col &&
                     cell.id == tileAtMapPoint({col * kTile, row * kTile});
            gridOk = gridOk && cell.fileName.empty();  // no tile index supplied
        }
    }
    CHECK(gridOk);
    CHECK(list.roads.size() == g.segments.size());
    CHECK(list.route.size() == 1);
    CHECK(list.route[0].points.size() == route.points.size());
    CHECK(list.routeDestinationNode == 2);  // route's last node
    CHECK(list.markers.size() == 8);
    CHECK(list.player.present);
    // Player raw (Peach tile local-ish, odd row): map position equals tile
    // 43's map cell origin + 0x00400000.
    CHECK(list.player.position.x == 5.0 * kTile + 0x00400000);
    CHECK(list.player.position.z == 5.0 * kTile + 0x00400000);

    // Projection: same camera drives every layer; the orders are the draw
    // list's; determinism holds for repeated projection.
    MapCamera camera;
    camera.setCenterTileLocal(43, 400, 400);
    camera.setZoomTiles(2.0);
    camera.setAspect(1.0);
    const MapProjectedFrame first = projectDrawList(list, camera);
    const MapProjectedFrame second = projectDrawList(list, camera);
    CHECK(first.tiles.size() == 64);
    CHECK(first.roads.size() == list.roads.size());
    CHECK(first.route.size() == 1);
    CHECK(first.markers.size() == list.markers.size());
    CHECK(first.playerPresent);
    bool identical = true;
    for (std::size_t i = 0; i < first.tiles.size(); ++i)
        for (int k = 0; k < 4; ++k)
            identical = identical && first.tiles[i].corners[k].u == second.tiles[i].corners[k].u &&
                       first.tiles[i].corners[k].v == second.tiles[i].corners[k].v;
    CHECK(identical);
    // Camera centred on Peach tile-local (400, 400).  The player stands at
    // raw cell offset +0x00400000 fixed units = 200 game units, NOT 400:
    // 400 game units is ~0x800000 fixed under the proven f32 scale
    // S = 2^25/1600 (world-geography.md; the exact pin above that
    // player.position == cell origin + 0x400000 rules the old expectation
    // out).  So the player projects one eighth of a tile west+south of the
    // camera centre, i.e. -0.125 in a 1-tile half extent (zoom 2, aspect 1).
    CHECK(near(first.player.u, -0.125, 1e-6));
    CHECK(near(first.player.v, -0.125, 1e-6));
    // And the projected frame matches a direct world->normalized projection
    // of the session position: one camera drives every layer, player too.
    const NdcPoint playerNdc = camera.worldToNormalized(playerRaw);
    CHECK(near(first.player.u, playerNdc.u, 1e-9));
    CHECK(near(first.player.v, playerNdc.v, 1e-9));

    // Null-pointer layers are skipped deterministically.
    MapContentSource empty;
    const MapDrawList bare = buildMapDrawList(empty);
    CHECK(bare.tiles.size() == 64);
    CHECK(bare.roads.empty());
    CHECK(bare.route.empty());
    CHECK(bare.markers.empty());
    CHECK(!bare.player.present);
    CHECK(bare.routeDestinationNode == -1);
}

void testLegacyMinimapFormula() {
    // CONFIRMED in-game formula (config [map]): fx/fy hand-checked values.
    LegacyMinimapProjection proj;
    proj.carX = 0.0;
    proj.carZ = 0.0;
    proj.scale = kLegacyMinimapScaleNear;  // s = 50
    CHECK(near(proj.fx(0.0), 102.0));
    CHECK(near(proj.fx(200.0), 152.0));   // +200 units * 50/200 = +50 px
    CHECK(near(proj.fy(0.0), 183.0));
    CHECK(near(proj.fy(100.0), 171.25));  // -100 * 0.47 * 50/200 = -11.75
    proj.scale = kLegacyMinimapScaleFar;  // s = 25
    CHECK(near(proj.fx(200.0), 127.0));
    // Centred on the car: the car itself is (102, 183) wherever it stands.
    proj.carX = 900.0;
    proj.carZ = 700.0;
    CHECK(near(proj.fx(900.0), 102.0));
    CHECK(near(proj.fy(700.0), 183.0));
}

// ---------------------------------------------------------------- disc pins

std::filesystem::path discRoot() {
    if (const char* value = std::getenv("RT_DISC")) return value;
    if (const char* home = std::getenv("HOME"))
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    return {};
}

void testDiscGraphPins() {
    auto tables = NavRouteTables::open(discRoot() / "SLUS_203.98");
    if (!tables) {
        std::printf("map_view: disc ELF unavailable — disc pins skipped\n");
        return;
    }
    // Race route (static directory pair 0 = COURSE/C00 pack, scene-tile-map):
    // 35 segment records; the walked-node coverage 34/34 is nav-walkable.md.
    auto pack = tables->staticGraph(0);
    CHECK(static_cast<bool>(pack));
    if (pack) {
        CHECK(pack->segments.size() == 35);
        CHECK(NavRouteTables::graphWithinTile(*pack));
    }
    // Peach Town street graph (outer dir T=1): 84 segments, tile-local box
    // [418,552]x[443,1084] (scene-tile-map.md tables).
    auto peach = tables->townGraph(1);
    CHECK(static_cast<bool>(peach));
    if (peach) {
        CHECK(peach->segments.size() == 84);
        CHECK(NavRouteTables::graphWithinTile(*peach));
        float minX = 1e9, maxX = -1e9, minZ = 1e9, maxZ = -1e9;
        for (const NavSegment& s : peach->segments) {
            for (float v : {s.ax, s.bx}) { minX = std::fmin(minX, v); maxX = std::fmax(maxX, v); }
            for (float v : {s.az, s.bz}) { minZ = std::fmin(minZ, v); maxZ = std::fmax(maxZ, v); }
        }
        // Doc bounds 418..552 x 443..1084 (scene-tile-map.md T=1 row); the
        // window tolerates the table's print rounding.
        CHECK(minX >= 417.5f && maxX <= 552.5f);
        CHECK(minX <= 418.6f && maxX >= 551.4f);
        CHECK(minZ >= 442.5f && maxZ <= 1084.5f);
        CHECK(minZ <= 443.6f && maxZ >= 1083.4f);
        // Every ordered node pair builds a route that is internally
        // consistent (monotone cumulative, progress clamped, shortest).
        const int n = static_cast<int>(peach->transitions.size());
        int built = 0;
        for (int from = 0; from < n; ++from) {
            for (int to = 0; to < n; ++to) {
                const MapRoute r = buildMapRoute(*peach, from, to);
                if (!r.valid) continue;
                ++built;
                bool monotone = true;
                for (std::size_t i = 1; i < r.cumulative.size(); ++i)
                    monotone = monotone && r.cumulative[i] >= r.cumulative[i - 1];
                CHECK(monotone);
                const float progress = r.progressAt(r.points.empty() ? Vec2{} : r.points.front());
                CHECK(progress >= 0.0f && progress <= 1.0f);
            }
        }
        CHECK(built > 0);
        std::printf("map_view disc: peach graph ordered-pair routes=%d/%d\n", built, n * n);
    }
}

}  // namespace

int main() {
    testMapSpaceShearAndWrap();
    testTileGridInvariants();
    testProjectionExactness();
    testProjectionTorusWrapAndBounds();
    testPixelMapping();
    testMarkerFiltering();
    testDoorCentroid();
    testRouteOnDiamond();
    testPageStateMachine();
    testDrawListAndProjection();
    testLegacyMinimapFormula();
    testDiscGraphPins();
    if (g_fail) {
        std::fprintf(stderr, "%d checks FAILED\n", g_fail);
        return 1;
    }
    std::printf("map_view: all checks passed\n");
    return 0;
}
