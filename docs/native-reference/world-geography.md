# World geography — streaming grid and tile-local coordinates

Updated 2026-10-16. This document records only addresses, constants, formulas,
and computed examples; it does not embed disc data.

## Scale verdict: **PROVEN — one tile is exactly `1 << 25` fixed units**

The position state used by field/car code is fixed point. The conversion
constant is the little-endian float at `0x00332c0c`:

```
bytes     0a d7 a3 46
f32       20971.51953125 = 33554432 / 1600 = (1 << 25) / 1600
S         = 20971.51953125 fixed units per terrain/nav unit
T         = 0x02000000 = 33554432 fixed units per tile
```

This is the f32-rounded quotient used by the game (`2^25/1600 =
20971.52` is not exactly representable; the stored value is the nearest f32,
so one tile spans `1600.000036` render units at f32 precision), not an
inferred "approximately 2^25" scale.

### Evidence chain

* `FUN_0021c8d0` (`0x0021c8d0`) receives two integer coordinates and shifts
  them by immediate amounts `0x18` and `0x19`; its only callers are six sites
  in `FUN_002201c0` plus debug display `FUN_0026d700` (7 call sites).
* `drawSceneField` calls `FUN_002201c0` with offsets `+0/+8` of the selected
  `0x9c`-byte car record. Thus streaming follows the selected car's horizontal
  fixed positions, not camera floats. `FUN_002201c0` compares those positions
  with stream origin `param_4+0x58/+0x5c` and requests neighbours at literal
  deltas `±0x01000000` and `±0x02000000`.
* `FUN_0021ba70` accumulates horizontal fixed coordinates at `car+0xa0` and
  `car+0xa8`, masks both with literal `0x0fffffff`, then forms the render/game
  vector:

  ```c
  renderY = (float)fixedY / DAT_00332c0c;
  renderZ = (float)(fixedZ & 0x01ffffff) / DAT_00332c0c;
  renderX = (float)((fixedX + ((fixedZ & 0x02000000) >> 1)) & 0x01ffffff)
            / DAT_00332c0c;
  ```

  The load at `0x0021c494` is the sole read of `DAT_00332c0c`; raw bytes above
  establish the divisor. The reset path also multiplies render X/Z by nearby
  `DAT_00332c08` before adding them to fixed fields.

Therefore local terrain/nav displacement `u` is `u*S` fixed units, and
`1600*S == 0x02000000`.

## Full mapping

Use `X` for the first `FUN_0021c8d0` input (car `+0xa0`) and `Z` for the
second (car `+0xa8`). Terrain packets use `(localX, localZ, height)`: their
horizontal axes map to `(X,Z)`; height is the car/vector Y axis and has no id.

### Fixed coordinate to tile id

The function, retaining actual shift/mask constants:

```c
if (Z < 0) Z += 0x01ffffff;
if (X < 0) X += 0x00ffffff;
b  = (Z >> 25) & 1;
id = ((Z >> 25) & 6) * 8 + (((X >> 24) - b) & 0x0e) + b;
```

Filename digits use the independently-proven descriptor mapping:

```
id = (A << 4) | (B << 2) | swap2(C)
swap2(0)=0, swap2(1)=2, swap2(2)=1, swap2(3)=3
```

Solving those equations gives placement for every `ABC.BIN`:

```
b     = C >> 1
row   = (A << 1) | b
col   = (B << 1) | (C & 1)

X0    = ((col << 1) | b) << 24
Z0    = row << 25

fixed(X,Z; localX,localZ) = (X0 + S*localX, Z0 + S*localZ)
extent = [X0,X0+0x02000000) × [Z0,Z0+0x02000000)
```

`localX` and `localZ` increase in the same directions as fixed `X` and fixed
`Z`; no axis reversal occurs. The renderer's local X conversion adds
`b*0x01000000` before masking `0x01ffffff`: this is the odd-row half-slot
shear. After rebase a tile's horizontal range is local `[0,1600]`; `renderZ`
likewise uses `Z mod T`.

There are two distinct wraps:

* `T=0x02000000` is the per-tile/local-render wrap (`&0x01ffffff`) and the
  neighbour-comparison span in `FUN_002201c0`.
* Car fixed positions are retained modulo `0x10000000` by the literal
  `&0x0fffffff` writes in `FUN_0021ba70`: the stored grid is an 8×8 torus.

### Correction to the earlier parity shorthand

The previously stated `id&1 == (id>>4)&1` constraint is **not** in the code.
It assigned `A` to the wrong Z bits. Actual immediate operations establish:

```
A = (Z >> 26) & 3       // id bits 5:4, original Z bits 27:26
b = (Z >> 25) & 1       // id bit 0, also C's high bit
```

All 64 ids are reachable: four `A` values × two `b` values × eight columns.
The scripted scene table is a genuine second load path, but not evidence for a
missing half of this grid. Independently verified 2026-10-16: instruction-level
match over `0x21c8d0`–`0x21c920`; all 64 derived placements round-trip through
the formula back to their ids; the disc `FLD/` filename set maps exactly onto
the 64-id space.

## Worked example — Peach Town, id 43 / `FLD/223.BIN`

(Corrected 2026-10-16 per scene-tile-map.md: the earlier draft worked id 11 /
`FLD/023.BIN` and called it Peach Town — id 11 is **My Garage**. Peach Town is
tile 43, same half-column, four rows south.)

`43 = 0b101011`; therefore `A=2`, `B=2`, `C=3`:

```
b=1, row=5, col=5
X0=0x0b000000, Z0=0x0a000000

fixed extent: X=[0x0b000000,0x0d000000), Z=[0x0a000000,0x0c000000)
unwrapped terrain units: X=[8800,10400), Z=[8000,9600)
```

For gameplay rendering the odd-row rebase maps it to `renderX=localX` and
`renderZ=localZ`, both `[0,1600]`; the absolute origin remains raw fixed
`(0x0b000000,0x0a000000)`, not renderer-local `(0,0)`.

## Navigation cross-check

I independently decoded static graph 0 from the user ELF:

* Its PT_LOADs both give `p_vaddr-p_offset=0x001ff000` (data: vaddr
  `0x0029a880`, offset `0x0009b880`).
* At `0x002bd578` (file `0x000be578`), pair is
  `{0x002ac620,0x002ac968}`; difference/24 is 35 segments.
* Reading each segment's first four fields as little-endian f32 gives
  `x=[358,822]`, `z=[328,572]`; first four include
  `(590,569)->(590,571)` and `(600,569)->(600,571)`.

These endpoints already fit `[0,1600]^2`, proving tile-local storage, not
fixed point. (Corrected 2026-10-16 per scene-tile-map.md: the static
directory `0x2bd578` is pack-type-indexed, so graph 0 is the `COURSE/C00.BIN`
race route, not a town street graph — the earlier "candidate Peach" framing
was wrong.) The centering exercise below uses the tile-11 (`FLD/023.BIN`,
My Garage/My City) origin and is retained as scale/axis validation only:

```
fixed graph point = (0x0b000000 + S*x, 0x02000000 + S*z)
unwrapped units   = (8800 + x, 1600 + z)
```

Its bounds become `[9158,9622] × [1928,2172]`, inside the tile-11
(My Garage/My City) extent. This validates scale/axes, not the
graph-to-town identity.

## OPEN evidence

* **Static graph 0 = Peach: RESOLVED (2026-10-16) — false.** The static
  directory `0x2bd578` is pack-type-indexed, so graph 0 is the
  `COURSE/C00.BIN` race route, not a town street graph. Peach Town's street
  graph lives in the outer directory `0x2d98a8` at scene index `T=1`
  (pair `{0x2c5ff0,0x2c67d0}`, 84 segments). Full proof chain:
  [scene-tile-map.md](scene-tile-map.md).
* **Scripted category-9 load role:** `FUN_00210158` loads the scene-table id,
  but a transition trace plus following car fixed `(X,Z)` is needed to show
  preload, teleport, or resident replacement semantics.
* **Artist-facing global offset:** fixed-grid origin and car conversion are
  proven. Follow a minimap/UI/camera consumer of car fixed position to establish
  whether it adds an independent presentation offset.
