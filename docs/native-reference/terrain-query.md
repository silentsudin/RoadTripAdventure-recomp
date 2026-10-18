# Terrain query — height, surface, tile id at the car

Written 2026-10-17 for W2-04. Constants below were re-read from
`SLUS_203.98` (file offset = vaddr − `0x1FF000`) and from
`build/decomp/fn/*.c`. No disc bytes are embedded.

## What this is

`src/native/gameplay/terrain_query.*` answers three questions at a car
position, with no renderer:

1. **Tile id** from the fixed-point words at `car+0xa0` / `car+0xa8`.
2. **Ground height** in the same game units as `car+0x94`.
3. **Surface word** returned by the mesh probe (the value ground contact
   stores at `car+0x19c`).

## Tile id — `FUN_0021c8d0`

`build/decomp/fn/0021c8d0.c:11-18`. ELF immediates at `0x21c8d4..0x21c914`
(re-read 2026-10-17): `lui/ori` build `0x01ffffff` (Z) and `0x00ffffff` (X);
`sra` by 25 and 24; `andi` masks `1`, `6`, `0x0e`.

```c
if (Z < 0) Z += 0x01ffffff;
if (X < 0) X += 0x00ffffff;
b  = (Z >> 25) & 1;
id = ((Z >> 25) & 6) * 8 + (((X >> 24) - b) & 0x0e) + b;
```

Stored positions are the `0x10000000` torus (`0021ba70.c:398-400`,
`& 0x0fffffff`). One tile is `0x02000000` fixed units. The f32 divisor at
both `0x332bfc` and `0x332c0c` is `20971.51953125` (`0x46A3D70A`), so
`1600 * S == 2^25` at f32 precision (`world-geography.md`).

Inverse placement (id → origin) is the formula in `world-geography.md`, not
a second code site. It agrees with `gameplay/world_layout.cpp::tilePlacement`
on all 64 origins. `assets/field_tile_index.cpp` is a different algebra: it
maps a filename `ABC.BIN` to an id via `(A<<4)|(B<<2)|swap2(C)`. It does not
map a position to an id. The two meet at the scene table
(`scene-tile-map.md`): Peach Town is id 43 / `FLD/223.BIN`, origin
`(X0,Z0) = (0x0b000000, 0x0a000000)`.

Render wrap (`0021ba70.c:423-426`) turns fixed `(X,Z)` into tile-local game
units. Odd-row bit 25 adds `0x01000000` to X before the `0x01ffffff` mask.
The harness car floats at `cars_address` (`0x0177ACE0` = car object `+0x90`)
are already those local units, so a height query does not re-derive the tile.

## Height and surface — PMP strips

The probe is `IntersectPointWithMesh` (`00206f28.c:45-108`), called from
`FindWorldMeshIntersection` (`00207280.c:71,102-116`) on a 16×16 grid of
100-unit cells:

```c
cell = ((int)(x / 100.0) & 0xf) + ((int)(z / 100.0) & 0xf) * 16;
```

Initial floor/ceiling are `DAT_00332b08` / `DAT_00332b0c` = −10000 / +10000
(`0xC61C4000` / `0x461C4000`).

PMP packets in `[offB, offC)` of a FLD or COURSE container
(`field-topology-status.md`):

```text
u32  0x8000|vertexCount, 0x20224000, 0x41, surfaceWord
f32  vertexCount × (x, height, z, q)
f32  (vertexCount−2) × (nx, ny, nz, w)     // slope, not a unit normal
```

Disc index 1 is height and index 2 is the horizontal paired with x. That is
what the collision indexing does (`pfVar3[1]` height, `pfVar3[2]` vs
`point.z`). `field_converter.cpp` comments the same quad as `(x, z, y, q)`
and then stores index 1 as the render-up axis; the names disagree, the
component that is vertical does not. The converter also fans the vertices
for drawing. The collision walk is a **strip**: pairs of triangles,
alternating `>= 0` / `<= 0` edge tests, height

```text
h = v0.height − ((px − v0.x) * nx + (pz − v0.z) * nz)
```

from that triangle's slope quad (`00206f28.c:51` and `:88`).

Surface word is packet word3, returned verbatim (`00206f28.c:66`). A
negative word only moves the floor/ceiling around the probe Y. Bit 28
(`0x10000000`) stores a special height and is not a ground id. Any other
word raises the floor when it is the highest ground still under the ceiling.
Bits 8–10 of that word are the material class in
`config/game_state.toml` (0 asphalt, 1 dirt, 3 grass, 4 snow, 5 hard,
6 water). Peach Raceway asphalt is `0x2000` (class 0). The Peach Town
spawn and street hits are word `0` (also class 0).

A miss does not invent a height. `00207280.c:109-116` writes the probe Y
back (the copy made at entry) and returns −1. That is the fallback.

The host index bins each strip triangle into every 100-unit cell its bbox
overlaps, then runs the same edge test. A point inside a triangle is inside
that bbox, so this matches a full strip scan. The runtime builder that fills
the `param_5+0x78` pointer table was not located; see open questions.

## Oracle poses

Harness floats from `oracle-golden-frames.md`. Mesh is `FLD/223.BIN` (tile
43) for the town poses and `COURSE/C00.BIN` (pack type 0, Peach Raceway;
`course-data-status.md`) for the race poses. Probe Y is the sky bound, so
the sample is the highest ground.

| pose | local (x, z) | oracle car Y | mesh height | surface | abs err |
|---|---|---:|---:|---:|---:|
| `town_driving` | 506.283, 442.230 | 30.000 | 30.000000 | `0x0` | 0 |
| `town_driving_street` | 518.557, 481.538 | 30.098 | 30.200001 | `0x0` | 0.102001 |
| `race_hud` | 616.684, 568.756 | 1.100 | 1.100000 | `0x2000` | ~0 |
| `race_hud_driving` | 747.502, 479.340 | 1.100 | 1.100000 | `0x2000` | ~0 |

Three poses match the car float. The street pose does not, and it is not a
unit-scale error: `0x332bfc` and `0x332c0c` are the same f32, and a ×0.1 or
`<<4/0x19` reinterpretation would move the three exact hits. The containing
street triangle is flat — all three vertices at 30.2, slope `(0, 1, 0)`.
The path from the spawn (height 30.0) steps up by 0.200 at that XZ. The
oracle Y is `car+0x94 = fixedY / S` after 5 s of held gas
(`oracle-golden-frames.md`), i.e. the physics body, not a rest snap onto the
new plateau. Placing both town poses on the Peach origin and calling
`tileIdAt` returns 43 both times.

`C00` and `C01` both return 1.100 at the two race points. They are not the
same mesh (a 200-point sample differed once). Pack type 0 selects `C00`.

## Coverage

10 000 points drawn uniformly from `[0, 1600)²` on the Peach Town mesh, LCG
seed `0x20398 ^ 0x9e3779b9`, all recorded a finite ground hit.
`fallback_hits = 0`. The documented fallback (retain probe Y, `hit = false`)
was not exercised by that sample.

## 6. Screen-space terrain-Y law (W2-04r3)

Written 2026-10-18. Question: at which 448-row UI y does the ground under the car land on the oracle scanout?
Derived from ROM code and game-unit geometry, then checked against (a) the **live engine camera block**
(`cameraBlocks` `0x177E6D0`, oracle app, read between vblanks) and (b) the oracle PNGs. Module:
`scripts/w2_terrain_ymap.py` (`project`, `YMAP_ENTRY`); C++ law and gates: `tests/native_w2/terrain_query_test.cpp`
(`namespace screen`). Every ELF constant is re-read at run time (12 pinned words, `w2_terrain_ymap.py --verify-elf`).

### Camera (ELF)

`RunCarCameraTask` `0x21E690` -> `ComputeChaseCameraBasis` `0x21DC18` -> `buildCamera` `0x21F698`
(matrices at cam `+0/+0x40/+0x80/+0xC0`, eye at `+0x180`, focal at `+0x18C`) -> `BuildCameraViewMatrices` `0x21F450`.

| quantity | value | vaddr |
|---|---|---|
| preset 0 offset (right, up, forward) | `(0, 2, -7)` | `0x29CE00` (`LoadCameraPreset` `0x21FAE8`, stride 32) |
| focal | `500.0` | `0x29CE10` |
| pitch | `0x380` turns of 65536 = 4.921875 deg | `0x29CE14` |
| Y scale `focal * 0.47` | `235.0` | `0x332C88` (`buildCamera`: `[1][1] = focal * DAT_00332c88`) |
| X scale (NTSC) | `500.0` (`fVar6 = 1.0` when `videoMode == 0`; `0x332C84` = 0.8 is the PAL value) | `0x21F698` |
| screen centre | `2048.0` (`DAT_003356E0/E4`) | `lui 0x4500` at `0x21E73C` |
| GS `XYOFFSET` y | `1936.0 = 2048 - 112` | `lui 0x44f2` at `0x2425E4` (`minimap.md` section 1) |
| rear wheel contacts (car-local right, up, forward) | `(+-0.74, 0, -0.66)`; front `(0, 0, 0.68)` | `0x29CA90 / 0x29CAA0 / 0x29CA80` |

`camera-constants.md` labels `0.47` the *horizontal* scale and `0.8` the vertical one. The live matrices say the
opposite: world->clip `[1][1]` = 235 (`0.47 * 500`), `[0][0]` = 500. Y is the squashed axis.

The car matrix rows `car+0x00 / +0x10 / +0x20` are (right, up, forward) in world axes (`+0x90..+0x98` is the
position). The camera is rigid in that frame:

```text
f = cos(phi) * fwd - sin(phi) * up        u = cos(phi) * up + sin(phi) * fwd        phi = 0x380 turns
eye = pos + 2 * u - 7 * f                 (flat road: 2.5932 above, 6.8026 behind the car)
d = g - eye;   Y = d . u;   Z = d . f
sy_gs   = 2048 - 235 * Y / Z              (engine: world->clip row 1 / w, w = Z)
ui448   = 2 * (sy_gs - 1936)              (one field line = two UI lines; minimap.md section 1)
shot_y  = ui448 * 447 / 448               (oracle scanout, minimap.md section 3 "448 mapping")
```

The car origin therefore sits at view `(0, -2, 7)` in every pose and projects to `sy_gs = 2048 + 235*2/7 = 2115.143`,
`ui448 = 358.286`. The ground point under the origin is `project(y_terrain, camera)`; with `dy = y_terrain - car.y`
it moves by about `dy * 67` UI rows per game unit. Eye error of the pose model against the live matrix: `5e-5`
on flat poses. The engine builds the camera from the **previous tick's** car state (eye error 0.031 at lag 1 vs
0.193 at lag 0 over 57 consecutive vblanks), so car floats read in the same vblank are one tick ahead.

### Car height law (explains the 30.098 vs 30.200 street miss)

`ResolveGroundContact` `0x21B3D0` transforms 7 wheel offsets (`0x29CA80..`) to world, probes the terrain at each, and
writes `car.y = (h0*0x21 + ((h1+h2)/2)*0x22) / 0x43` over the front and two rear contacts (before suspension). At
the street pose the contacts read 30.2 (front), 30.0, 30.0, so `car.y = 30.09851`; the harness float is 30.09844.
`car_y_contact_err` over the 5 pinned flat/tilted poses is `7.0e-5`. `heightAt(car.x, car.z)` is the height under
the *origin*, not a car height, which is why the game-unit oracle gate for that pose reads 0.102 (it is not a
unit-scale error and not a load offset).

### Static-pose validation (gate: `|err| <= 0.100` px, 448-space)

Predicted = ELF law + `TerrainMesh::heightAt`. Engine = same ground points through the live world->clip matrix
(`2*(row1/w - 1936)`, pinned derived numbers). Origin and the two rear contacts per pose.

| pose (flat, at rest) | car (x, y, z) | origin law / engine | rear contact law / engine | max abs err |
|---|---|---|---|---:|
| town spawn (`town_driving`) | 506.283, 30.000, 442.230 | 358.2838 / 358.2913 | 376.4023 / 376.4104 | 0.0082 |
| race grid (`race_grid`, countdown) | 457.200, 1.100, 568.800 | 358.2836 / 358.2944 | 376.4020 / 376.4138 | 0.0118 |
| town settled (`town_flat2`, coasted to rest) | 504.297, 30.000, 450.594 | 358.2838 / 358.2852 | 376.4023 / 376.4036 | 0.0014 |

`static_px_err_max = 0.0118`. The brief named poses `(429,48,148)`, `y=56` and `(518,121,369)`; they do not occur
in this doc or anywhere in the tree, so the three poses above were captured from the oracle (same boot/driving
scripts as `oracle-golden-frames.md`; the oracle-golden PNGs were reproduced to mean |diff| 1.2).

### PNG landmark: reported, not gated

The only sharp, known-geometry landmark under the car is the rear-tyre bottom edge. Predicted rear contact is
`shot_y = 375.562` (`376.402` before the 447/448 scale). Observed edge, first bright row below the tyre at
x = 523 and 757: **378.0** in all three poses (`town_flat1` reads 377: field-parity jitter). Residual
`+2.44` px (`+1.60` without the 447/448 factor). That residual is a tyre-geometry offset (the drawn tyre bottom is
below the contact point; it scales as `1/Z`: about `+0.5 px` at depth 22 and `+0.35 px` at depth 32 on the grid
cars) plus a **two-row quantisation**: the edge is a single GS field row, which is 2 shot rows, so no PNG edge can
support a 0.100 px gate. The 0.100 gate is therefore evaluated against the engine camera block; the PNG residual
is printed as `png_tire_edge_res`. Mesh edges (collision mesh projected onto the PNG) did not give a usable
landmark: 7 653 triangles project into the frame but the strong-edge histogram has no peak.

### Driving residual (informational)

Same law, engine camera of tick `t` paired with car state of tick `t-1`, 2 tilted dumps + 14 consecutive-vblank
samples (race 10, town 4, gas held) at 3 ground points each: `driving_px_err_max = 1.905`,
`driving_px_err_mean = 0.272` px. The tilted static poses are within 0.14 px. The remainder is the camera's slewed
tilt (`0x172FF30` x/z slew, `ComputeChaseCameraBasis` rate limits `0.001/frame^2`), not modelled. Pairing the
car floats of the *same* vblank as the camera inflates this to 12 px on race, which is the one-tick lag above and not a
terrain error. The old "4 px at frame 2446" gap from earlier notes is that lag, not a vertex-load offset.

### Trace column and y-map

`tests/native_w2/data/vehicle_trace.csv` gains `terrain_y_px` (448-space ui y of the mesh height under the car with
the level chase camera; range 357.40..362.07). Existing columns are untouched. `W2-04 SCORE ... ymap_agreement_max`
is the C++ recomputation of all 731 rows against the column the Python module wrote (`7e-6` px).
`YMAP_ENTRY` is `status='ELEMENT'`, `scale = 447/448`, `offset = 0` (oracle shot y = ui448 * 447/448); it is not a
`FIT`: there are not three independent landmark pairs at <= 1 px (the PNG edges carry the tyre offset and
quantisation above).

Reproduce: `python3 scripts/w2_terrain_ymap.py --verify-elf && python3 scripts/w2_terrain_ymap.py --selftest`,
and the build/run commands in `.qoder/wave2/W2-04r-report.md`.
