# W2-04 report — terrain height, surface, tile algebra

## Summary

New module `terrain_query` answers tile id, ground height, and surface word
at a car `(x, z)`. Tile id is `FUN_0021c8d0` verbatim. Height/surface follow
`IntersectPointWithMesh` over PMP triangle strips (not the render fan in
`field_converter.cpp`).

The acceptance Y threshold **fails**. Three of the four oracle poses match
the mesh to float noise. The Peach Town street pose is a flat 30.200 triangle;
the harness car Y is 30.098 because the body is still climbing a 0.200 curb
after 5 s of gas. That is not a unit/offset bug (`0x332bfc` == `0x332c0c`,
and rescaling would break the three exact hits). Exit code is 1, as required
when `oracle_y_err_max > 0.05`.

```
W2-04 SCORE oracle_y_err_max=0.102001 tile_id_ok=1000/1000 coverage=1.000000 fallback_hits=0 deterministic=1 surface_pins=4/4 elf_pins=13/13 round_trip=64/64 town_tile=2/2 FAIL
```

## Files

- `src/native/gameplay/terrain_query.h`
- `src/native/gameplay/terrain_query.cpp`
- `tests/native_w2/terrain_query_test.cpp`
- `docs/native-reference/terrain-query.md`
- this report

No existing file was edited. No ROM bytes committed.

## Numbers

| check | result | threshold |
|---|---|---|
| `oracle_y_err_max` | **0.102001** | ≤ 0.05 — **miss** |
| `tile_id_ok` | 1000/1000 | exact |
| `coverage` | 1.000000 (10000/10000) | ≥ 0.99 |
| `fallback_hits` | 0 (pinned) | documented miss → retain probe Y |
| `deterministic` | 1 | two full runs, FNV-1a equal |
| surface words | 4/4 (`0` town, `0x2000` race) | pin |
| ELF immediates | 13/13 | pin |
| id round-trip | 64/64 | pin |
| town poses → id 43 | 2/2 | pin |

Per-pose mesh height vs harness car Y (`oracle-golden-frames.md`):

| pose | mesh | height | car Y | abs err | surface |
|---|---|---:|---:|---:|---|
| town 506.283, 442.230 | `FLD/223.BIN` | 30.000000 | 30.000 | 0 | `0x0` |
| town 518.557, 481.538 | `FLD/223.BIN` | 30.200001 | 30.098 | 0.102001 | `0x0` |
| race 616.684, 568.756 | `COURSE/C00.BIN` | 1.100000 | 1.100 | ~0 | `0x2000` |
| race 747.502, 479.340 | `COURSE/C00.BIN` | 1.100000 | 1.100 | ~0 | `0x2000` |

Street triangle is flat (vertices all 30.2, slope ny = 1). The spawn plateau
along the driven chord stays at 30.0 until that step. `car+0x94` is
`fixedY / S` (`0021ba70.c:423`), updated by the suspension filter, not a
rest snap. A contact-point blend along the travel heading (body points at
`0x29CA80`, weights `0x21/0x43` and `0x22/0x43` from `0021b3d0.c:97`) gives
30.149, still 0.051 over 30.098, and the golden does not record heading.

### Tile algebra vs existing modules

`tileIdAt` is the decomp formula. `tilePlacementOf` is the
`world-geography.md` inverse. Origins match `gameplay/world_layout.cpp` on
64/64 ids (re-checked this pass; zero origin mismatches, both round-trip
through the shift/mask tree). `assets/field_tile_index.cpp` is the filename
digit map `id = (A<<4)|(B<<2)|swap2(C)`, not a position formula. They agree
where the scene table joins them: Peach Town = id 43 = `FLD/223.BIN`.

Collision strips vs `field_converter` fans: the converter's vertical
component is disc index 1, same as the collision walk. Its fan triangulation
is a render choice. This batch follows the strip in `00206f28.c`.

### Reproduce

ELF pins re-read before citing: `0x332bfc` and `0x332c0c` = `0x46A3D70A`;
`0x332b08` = `0xC61C4000`; `0x332b0c` = `0x461C4000`; `0x21c8d4` = `lui 0x1ff`,
`0x21c8d8` = `ori 0xffff`, `0x21c8fc` = `sra 25`, `0x21c900` = `andi 1`,
`0x21c904` = `andi 6`, `0x21c908` = `sra 24`, `0x21c914` = `andi 0x0e`.

```sh
cmake -S src/native -B build/w-W2-04 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-04 --target w2_terrain_query_test -j
build/w-W2-04/w2_terrain_query_test --elf "$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98"
echo $?   # 1 while oracle_y_err_max=0.102001 > 0.05; 2 if the ELF is missing
```

A later sibling drop of `gameplay/hud_live.cpp` does not compile
(`Page::MeterDial` / `Page::MeterDigits` missing). That file is not owned
here and was not edited. The score above was produced by compiling
`terrain_query.cpp` into `librt_native_assets.a` on the configure that
preceded that break, then relinking only `w2_terrain_query_test` against
that archive. A clean `--build` will succeed again once `hud_live.cpp`
compiles; this batch's sources already did (`terrain_query.cpp.o`, no
warnings).

## Confidence

- Tile algebra: **high**. Instruction immediates, 1000/1000 closed-form,
  64/64 round-trip, both town poses bind to id 43.
- Mesh query on the three flat poses and the surface words: **high**.
  Independent Python strip walk and the C++ query agree, including race
  surface `0x2000`.
- Street residual explanation: **high** that the mesh height is 30.200 and
  that this is not a scale error. **Medium** on the exact suspension lag
  (heading was not in the golden, so the 7-point blend cannot be closed).

## Open questions

1. Re-register `oracle_y_err_max` (suggested ≤ 0.11, or score the three rest
   / flat poses only)? The street body Y will not equal mesh height without
   a heading and the suspension state from that frame. Do not apply a global
   Y offset — it would break the exact poses.
2. The runtime cell-list builder behind `param_5+0x78` was not found. The
   host bins by bbox, which is a superset of "triangles that contain the
   point". Oracle poses have a single containing triangle, so they do not
   depend on that. Overlapping ground could differ if the builder drops
   triangles.
3. `C01.BIN` also returns 1.100 at both race poses. Pack type 0 is `C00`;
   a 200-point sample differed once, so the files are not duplicates.
