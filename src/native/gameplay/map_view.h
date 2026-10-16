// Map + second-screen LOGIC layer for the native rebuild (branch
// re/native-complete).  Pure C++: no Vulkan, no SDL, no GS, no disc access.
// The native app owns rendering and never runs the emulated GS, so the map is
// RECONSTRUCTED from recovered identity data instead of diverting emulated
// GS batches (the sibling static-recomp design; see CLAUDE.md "Second
// screen" / "Map = Enhanced", HISTORICAL only).  The GPU/second-window wave
// consumes this module through the draw-list + camera contracts below.
//
// Data grounding (full ledger: docs/native-reference/map-view.md):
//   * world-geography.md  — 1 tile = 0x02000000 fixed units (f32 scale S =
//     20971.51953125 = DAT_00332c0c, PROVEN); stored positions wrap modulo
//     0x10000000 (8 rows x 16 half-columns, PROVEN); odd rows shear one
//     half-column: FUN_0021ba70 adds ((Z & 0x02000000) >> 1) to X before the
//     0x01ffffff mask (PROVEN).
//   * scene-tile-map.md   — town/tile identity: scene table 0x2be438 record
//     {namePtr, u16 tileId, u8 byte6, u8 byte7}; Peach Town = record 1,
//     tile 43 / FLD/223.BIN (PROVEN).
//   * save-layout.md      — progress filters: unvisited house doors 0xBD8
//     (10 x u32, bit n = door n not yet visited, PROVEN), Quick-Pic photos
//     0x508 (16-byte bitmap, photo n = bit n-1, PROVEN).
//   * hud-layout.md 1.3   — the game's own map window placement (CONFIRMED).
//   * config/game_state.toml [map]/[houses]/[doors] — the in-game minimap
//     projection formula and the door/resident table addresses (CONFIRMED).
//
// Where a table's binding is not yet ELF-grounded (door bit <-> door index,
// Quick-Pic photo numbers, the shop letter map), the module takes the
// classification from the CALLER and marks it AUTHORED/UNVALIDATED rather
// than inventing data.  No game bytes ship here.
#pragma once

#include "assets/field_tile_index.h"
#include "assets/nav_routes.h"
#include "gameplay/save.h"
#include "gameplay/vec.h"
#include "gameplay/world_layout.h"

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace rt::native::gameplay {

// ---------------------------------------------------------------- map space
//
// Map space is the stored fixed-point torus with the odd-row shear REMOVED,
// so the 64 streaming tiles form a shear-free 8x8 grid:
//
//     mapX = wrap( X - shear(Z) ),  shear(Z) = (Z & 0x02000000) >> 1
//     mapZ = wrap( Z )              (period 0x10000000 on both axes)
//
// The shear term is the exact amount FUN_0021ba70 ADDS to X for renderer-local
// rebase; map space subtracts it again, so world<->map is lossless.  After
// unshearing, tile id (row, col) — row = ((id>>4)&3)<<1 | (id&1),
// col = (id>>1)&7, from world-geography.md's placement algebra — occupies
// [col<<25, (col+1)<<25) x [row<<25, (row+1)<<25) exactly.  Worked check
// (scene-tile-map.md): Peach Town tile 43 has raw origin (0x0b000000,
// 0x0a000000); odd-row shear 0x01000000 removed -> map (0x0a000000,
// 0x0a000000) = (5<<25, 5<<25), row 5, col 5.

inline constexpr double kMapPeriod = static_cast<double>(kTorasWrapUnits);    // 0x10000000
inline constexpr double kMapTileSpan = static_cast<double>(kTileFixedUnits);  // 0x02000000
inline constexpr int kMapGridTiles = 8;  // 8 x 8 tiles tile the full torus

struct MapPoint {
    double x = 0.0;  // fixed units in map space, [0, kMapPeriod) when canonical
    double z = 0.0;
};

// Wraps a map-space coordinate into [0, kMapPeriod) on one axis (any finite
// double; the stored-grid mask & 0x0fffffff applied in fixed arithmetic first).
double wrapMapAxis(double v);

// Shortest signed displacement from a to b on one torus axis, in
// (-kMapPeriod/2, kMapPeriod/2].  Deterministic, no ties (half-period exactly
// wraps to the negative side).
double mapDelta(double a, double b);

// Raw world fixed position -> canonical map space (wrapToras first, then
// shear-remove; world-geography.md semantics, never the reverse direction).
MapPoint toMapSpace(FixedPos world);

// Map space -> a raw world position that toMapSpace() maps back to the same
// point (re-applies the shear for the wrapped Z row).
FixedPos fromMapSpace(MapPoint p);

// Upper-left (north-west) corner of a tile's map-space cell.
MapPoint mapOriginOfTile(int tileId);

// Streaming tile id whose map-space cell contains p (-1 never happens: every
// canonical point lies in exactly one cell; out-of-canonical inputs wrap
// first).
int tileAtMapPoint(MapPoint p);

// Tile-local game units (0..1600, the units of nav graphs and door quads) ->
// map space, anchored on the tile's cell.
MapPoint tileLocalToMap(int tileId, double localX, double localZ);

// ---------------------------------------------------------------- MapCamera
//
// Normalized viewport point: [-1,1]^2, +u east, +v NORTH.  Pixel point:
// top-left origin, y grows down.
struct NdcPoint {
    double u = 0.0;
    double v = 0.0;
};

struct PixelPoint {
    double x = 0.0;
    double y = 0.0;
};

// Orthographic north-up camera over map space.  The normalized viewport is
// [-1,1]^2 with +u east and +v NORTH (north-up, like the game's own town
// minimap, config [map]); the renderer flips v for y-down pixel spaces.
// zoomTiles = world tiles spanned VERTICALLY by the full normalized height;
// the horizontal span is zoomTiles * aspect (keep aspect = windowWidth /
// windowHeight so world squares stay square).  Every mapping out of the
// camera — normalized, pixel rect, camera <-> world — comes from this one
// object, so a map's box and its content can never disagree (one camera, one
// frame: the structural fix for the sibling app's box-drift bug class).

class MapCamera {
public:
    struct PixelRect {  // top-left origin, pixels, y grows DOWN
        double x = 0.0;
        double y = 0.0;
        double w = 0.0;
        double h = 0.0;
    };

    void setCenterMap(MapPoint p);                     // wraps canonically
    void setCenterWorld(FixedPos world);                // toMapSpace + set
    void setCenterTileLocal(int tileId, double lx, double lz);
    void setZoomTiles(double tiles);                    // vertical tiles; > 0
    void setAspect(double widthOverHeight);             // > 0; 1 = square

    [[nodiscard]] MapPoint centerMap() const { return center_; }
    [[nodiscard]] double zoomTiles() const { return zoomTiles_; }
    [[nodiscard]] double aspect() const { return aspect_; }

    // Visible half extents in map-space fixed units (vertical = zoom/2
    // tiles; horizontal = that times aspect).
    [[nodiscard]] double halfExtentZ() const;
    [[nodiscard]] double halfExtentX() const;

    // Map-space point -> normalized viewport [-1,1]^2 (+v north).  Uses the
    // shortest torus displacement, so points across the seam from the centre
    // project onto the near side: the map is one continuous torus, never torn.
    [[nodiscard]] NdcPoint mapToNormalized(MapPoint p) const;

    // Inverse of mapToNormalized over the visible rect.
    [[nodiscard]] MapPoint normalizedToMap(NdcPoint ndc) const;

    [[nodiscard]] NdcPoint worldToNormalized(FixedPos world) const {
        return mapToNormalized(toMapSpace(world));
    }

    // One connected polyline projected into the viewport as ONE run.
    // `points` must be consecutive vertices of a single connected polyline in
    // map space (separate MapPolyline entries for separate shapes; the draw
    // list builder already builds roads per-segment and routes in adjacency
    // order).  Nothing ever tears across the seam: each vertex is placed at
    // the wrapped representative nearest its predecessor, so a polyline that
    // crosses the seam draws over/through it, and its wrapped-around tail
    // simply leaves the viewport (the torus cut sits at the camera
    // antipode).  Degenerate inputs (< 2 points) produce no runs.
    struct ProjectedRun {
        std::vector<NdcPoint> points;  // normalized, >= 2 points per run
    };
    [[nodiscard]] std::vector<ProjectedRun> projectPolyline(
        const std::vector<MapPoint>& points) const;

    // Normalized -> pixels inside `rect` (py grows down: py = y - v * h/2).
    // The camera aspect must equal rect.w / rect.h for square world squares;
    // the pixel helpers do not re-scale, they only fit [-1,1]^2 into the rect,
    // so the same camera drives box and content (no drift possible).
    [[nodiscard]] static PixelPoint normalizedToPixels(NdcPoint ndc, const PixelRect& rect);
    [[nodiscard]] static NdcPoint pixelsToNormalized(PixelPoint px, const PixelRect& rect);

private:
    MapPoint center_{};
    double zoomTiles_ = 1.0;
    double aspect_ = 1.0;
};

// ---------------------------------------------------------------- markers
//
// Marker kinds ported from the sibling second-screen design ("Q's Factory,
// the shops (P, B, C), Quick-Pic Shops not yet used and houses not yet
// visited").  Classification strings are read from the user's ELF at RUNTIME
// (door table 0x2C0698 -> quads; residents 0x2C22C8 -> 16-byte entries with
// the name pointer at +0xC): Peach Town's door residents are literally
// "Q's Factory Staff", "Parts Shop Staff", "Body Shop Staff", "Paint Shop
// Staff", "Quick-Pic Shop Staff" (x10) and named residents (probe of the
// committed addresses, 2026-10-16; identity strings only, no table data
// ships).  The P/B/C letter map (Parts/Body/Paint) is AUTHORED.
enum class MarkerKind : std::uint8_t {
    Garage,  // "Q's Factory" of the town (every town has one, config [towns])
    ShopP,   // "Parts Shop Staff"
    ShopB,   // "Body Shop Staff"
    ShopC,   // "Paint Shop Staff"
    QuickPic,     // "Quick-Pic Shop Staff", ordinal = photo number n
    HouseUnvisited,  // resident's house, ordinal = door bit in 0xBD8 masks
    TownExit,    // named portal door (e.g. "Entrance to the cave") / keep-out
    RaceGate,    // race destination supplied by the caller
    Player,      // reserved for the renderer's marker palette; never gathered
};

// Total Quick-Pic photos (config [photos]: count = 100; photo n = bit n-1 of
// the 0x508 bitmap, save-layout.md PROVEN).
inline constexpr int kPhotoCount = 100;

// One building door: the raw table shape is four (x,z) f32 corners (32 bytes
// per quad, config [doors]); this module takes the corners already in
// tile-local game units and classifies from the caller (name-based; see the
// kind's doc block).  ordinal = the door's index in the location's list for
// houses (bit n in 0xBD8, "door n clears bit n", config [houses]) and the
// photo number n for Quick-Pic ("Quick-Pic Shop No.n", [photos]) — the
// bit/number binding is documented config language, not yet ELF-verified.
struct TownDoor {
    double x[4] = {0.0, 0.0, 0.0, 0.0};  // quad corners, tile-local game units
    double z[4] = {0.0, 0.0, 0.0, 0.0};
    MarkerKind kind = MarkerKind::HouseUnvisited;
    int ordinal = 0;
};

// Everything gatherMarkers() needs about one town.  Positions are grounded by
// the scene table (scene-tile-map.md): tileId names the tile whose cell
// anchors the town's door quads and street graph; townId is the SAVE location
// axis (0 My Garage .. 9 My City — the ten u32 door bitmasks at 0xBD8); the
// two usually differ from sceneId only for My Garage/My City (scene 0 runs as
// 9, per FUN_0020e708's remap).
struct TownMarkerSource {
    int townId = 0;  // save location index 0..9 (unvisitedDoors axis)
    int sceneId = 0;  // 0x2be438 record index (street graph binding)
    int tileId = 0;   // record's u16 tile id (world anchoring)
    std::vector<TownDoor> doors;
    // Additional town-exit portals without a door quad (config [towns]
    // keep_out portals; Cloud Hill's rainbow gate is the committed one).
    std::vector<std::pair<double, double>> exits;  // tile-local (x, z)
};

// Door-quad centre: the marker placement rule (AUTHORED centroid of the
// CONFIRMED quad corners).
[[nodiscard]] Vec2 doorCentroid(const TownDoor& door);

struct MapMarker {
    MarkerKind kind = MarkerKind::Garage;
    int ordinal = 0;    // door index / photo number, by kind
    int townId = 0;     // save location index of the owning town
    int tileId = 0;     // anchoring tile identity
    MapPoint map;       // pre-projected map-space position (fixed units)
};

// Save-progress helpers (field semantics PROVEN, save-layout.md).
[[nodiscard]] bool photoTaken(const SaveData& save, int photoNumber);
[[nodiscard]] bool houseUnvisited(const SaveData& save, int townId, int doorOrdinal);

// Gathers the town's markers, filtered by save progress (deterministic,
// source order: doors in list order, then exits).  Filters (sibling design,
// AUTHORED-for-native):
//   * Garage / ShopP / ShopB / ShopC: always shown (open businesses; no save
//     field gates them).
//   * QuickPic: only photos not yet taken (photo n taken iff bit n-1 of
//     0x508; "Quick-Pic Shops not yet used").
//   * HouseUnvisited: only doors whose 0xBD8 bit is still set ("houses not
//     yet visited"); town indices outside 0..9 have no door bitmap — their
//     house doors are dropped rather than guessed.
//   * TownExit / RaceGate: passthrough.
//   * Player: never gathered (it comes from the session each frame).
[[nodiscard]] std::vector<MapMarker> gatherMarkers(int townId, const TownMarkerSource& town,
                                                   const SaveData& save);

// ---------------------------------------------------------------- route
//
// Waypoint route over a loaded nav graph (adjacency reuses the existing
// graph module: assets::NavGraph, whose transitions are nodes joining two
// segments — two nodes are adjacent when they share a segment index).
// Node positions follow the established centerline convention
// (gameplay/nav_path.h): the average of the two segments' midpoints.
struct MapRoute {
    std::vector<int> nodes;        // transition indices, from..to inclusive
    std::vector<Vec2> points;      // tile-local game units, one per node
    std::vector<float> cumulative;  // cumulative[i] = length at points[i]; [0] = 0
    bool valid = false;            // valid iff the graph contains both nodes

    [[nodiscard]] float totalLength() const;
    // 0..1 along the route for the nearest point of the polyline (ties ->
    // the earliest segment; clamped).  0 for an invalid/zero-length route.
    [[nodiscard]] float progressAt(Vec2 tileLocalPos) const;
};

// Dijkstra over shared-segment adjacency with deterministic tie-breaks:
// equal-cost pops/settles order by node index, and a predecessor is only
// replaced on strictly shorter cost, so equal-cost paths resolve to the
// lexicographically smaller node sequence.  Weights are Euclidean distances
// between node positions (tile-local units).
[[nodiscard]] MapRoute buildMapRoute(const assets::NavGraph& graph, int fromNode, int toNode);

// Route polyline in map space (anchored on the town's tile, like the roads).
[[nodiscard]] std::vector<MapPoint> routeToMapSpace(const MapRoute& route, int tileId);

// ---------------------------------------------------------------- pages
//
// Second-screen page state machine (sibling design ported AUTHORED-for-
// native; the sibling's GS-signal "map is up" has no native equivalent, so
// the native predicate is defined here: map button held OR navigation
// active).  Deterministic tick machine:
//   * Context A (racing || mapUp): page = Map immediately.
//   * Context B (cruise): page keeps; after `notebookIdleSteps` (default 45,
//     the sibling's 45 vblanks) of untouched cruise it auto-selects
//     Notebook once.  The transition tick into cruise is not itself an idle
//     step: the counter starts on the first quiet tick after it, so the
//     auto-Notebook fires exactly `notebookIdleSteps` vblanks into
//     uninterrupted cruise.
//   * A player tab choice is sticky: it overrides the context until the
//     context KIND changes (A<->B), at which point the override drops and
//     the context rules apply.
//   * Map hand-back: while the second screen exists AND shows Map, the main
//     window suppresses its map; choosing Notebook/Radio (or no second
//     screen) hands the map back to the main window.
enum class SecondScreenPage : std::uint8_t { Map, Notebook, Radio };

struct SecondScreenContext {
    bool racing = false;
    bool mapUp = false;  // native predicate: map button held || navigation active
};

class SecondScreenPages {
public:
    explicit SecondScreenPages(int notebookIdleSteps = 45);

    // One deterministic step.  `playerChoice` is set on the step the player
    // picked a tab (touch/keys), absent otherwise.
    void tick(SecondScreenContext context, std::optional<SecondScreenPage> playerChoice);
    void reset();

    [[nodiscard]] SecondScreenPage page() const { return page_; }
    [[nodiscard]] bool showsMap() const { return secondScreen_ && page_ == SecondScreenPage::Map; }
    // Hand-back rule for the glue: false => the main window may draw a map
    // (its own mode decides whether it does; env RT_APP_MAP_ENHANCED).
    [[nodiscard]] bool mapSuppressedOnMainWindow() const { return showsMap(); }
    [[nodiscard]] int notebookIdleSteps() const { return idleLimit_; }
    void setSecondScreenEnabled(bool enabled) { secondScreen_ = enabled; }
    [[nodiscard]] bool secondScreenEnabled() const { return secondScreen_; }
    [[nodiscard]] int idleSteps() const { return idle_; }

private:
    int idleLimit_;
    SecondScreenPage page_ = SecondScreenPage::Map;
    bool override_ = false;         // a player tab choice is active
    bool pendingNotebook_ = true;  // auto-Notebook armed (cruise start)
    bool mapContext_ = false;      // last tick's context kind (A/B)
    int idle_ = 0;
    bool secondScreen_ = true;
};

// ---------------------------------------------------------------- draw list
//
// The renderer contract: ONE ordered, deterministic content list per map
// frame, in map space.  The GPU wave projects it through a MapCamera at
// render (project() below does tile quads/roads/route/markers/player all from
// the same camera, so nothing can split from anything).  Layer order is
// fixed: ground tiles, roads, route overlay, markers, player.

struct MapTileCell {
    int id = 0;     // streaming tile id 0..63 (field_tile_index resolution)
    int row = 0;    // map-space grid row 0..7 (north..south)
    int col = 0;    // map-space grid column 0..7 (west..east)
    std::string fileName;  // resolved FLD name when a tile index is supplied
};

struct MapPolyline {
    std::vector<MapPoint> points;  // map-space, >= 2 for a drawable line
    bool closed = false;
};

struct MapPlayer {
    MapPoint position;       // map space
    double heading = 0.0;    // radians, 0 = +Z = map north (atan2(dx, dz))
    bool present = false;    // false when the session provides no position
};

struct MapDrawList {
    std::vector<MapTileCell> tiles;      // 64 cells, row-major (row asc, col asc)
    std::vector<MapPolyline> roads;     // nav graph segments, segment-index order
    std::vector<MapPolyline> route;      // 0 or 1 overlay polylines (closed=false)
    std::vector<MapMarker> markers;     // gatherMarkers order
    int routeDestinationNode = -1;      // -1 when no route
    MapPlayer player;
};

// Inputs; null pointers skip the matching layer deterministically.  Roads and
// markers are tile-local data anchored on `town->tileId`; the route overlay
// is `route` (already computed by buildMapRoute over `graph`).  Marker
// filtering needs the save progress (`save`; null => no markers).
struct MapContentSource {
    const TownMarkerSource* town = nullptr;         // markers + tile anchoring
    const assets::NavGraph* graph = nullptr;         // roads (street graph)
    const assets::FieldTileIndex* tileIndex = nullptr;  // ground filenames
    const MapRoute* route = nullptr;                 // route overlay
    const SaveData* save = nullptr;                  // marker progress filters
    int routeDestinationNode = -1;
    FixedPos playerWorld{};       // raw position (any int32; wrapped first)
    double playerHeading = 0.0;   // radians, 0 = +Z = north
    bool hasPlayer = false;
};

[[nodiscard]] MapDrawList buildMapDrawList(const MapContentSource& source);

// One projected frame for the rasterizer: every layer straight from the same
// camera, preserving the draw-list order.  Tile quads are the four cell
// corners (counter-clockwise in +u-east/+v-north space); runs use the same
// pen-up splitting as MapCamera::projectPolyline.  (NdcPoint is declared
// canonically above, next to MapCamera.)
struct MapProjectedFrame {
    struct TileQuad {
        int id = 0;
        NdcPoint corners[4];
    };
    std::vector<TileQuad> tiles;  // row-major, 64 entries
    std::vector<MapCamera::ProjectedRun> roads;
    std::vector<MapCamera::ProjectedRun> route;
    struct MarkerPoint {
        MapMarker marker;
        NdcPoint at;
    };
    std::vector<MarkerPoint> markers;  // draw-list order
    NdcPoint player;
    double playerHeading = 0.0;
    bool playerPresent = false;
};

[[nodiscard]] MapProjectedFrame projectDrawList(const MapDrawList& drawList,
                                                const MapCamera& camera);

// ---------------------------------------------------------------- legacy
//
// The game's own town-minimap placement (CONFIRMED, config [map]; verified
// against the game's Quick-Pic icons to <= 1.5 px):
//
//     fx = 102 + (x - px) * s / 200
//     fy = 183 - (z - pz) * 0.47 * s / 200
//
// north-up, never rotates, centred on the car at (102, 183) in field pixels;
// s = 50 near / 25 far (zoom_table 0x2A42C0; Select toggles).  Kept for
// RT_APP_MAP_ENHANCED marker parity inside the game's bezel window
// (hud-layout.md 1.3: window (45, 312, 113, 102) in 640x448 UI space).
inline constexpr double kLegacyMinimapScaleNear = 50.0;
inline constexpr double kLegacyMinimapScaleFar = 25.0;

struct LegacyMinimapProjection {
    double carX = 0.0;    // player position, tile-local game units
    double carZ = 0.0;
    double scale = kLegacyMinimapScaleNear;

    [[nodiscard]] double fx(double x) const { return 102.0 + (x - carX) * scale / 200.0; }
    // 0.47 = the game's vertical squash for the 102-line window.
    [[nodiscard]] double fy(double z) const { return 183.0 - (z - carZ) * 0.47 * scale / 200.0; }
};

}  // namespace rt::native::gameplay
