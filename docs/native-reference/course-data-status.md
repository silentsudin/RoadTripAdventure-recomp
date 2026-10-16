# COURSE/*.BIN data status (Road Trip, SLUS-20398)

2026-10-15 local-analysis pass (no OpenAI quota left this window; run in-repo).

- 19 course files (C00–C20, gaps at C15/C17). Header `u32[5+]`:
  magic `0x20` ([magic, offA, offB, offC, size]) or `0x30` (extra region
  boundaries at words 5+, size at word 4 differs from last boundary).
- Region roles mirror FLD tiles: `[0x20,offA)` model descriptors,
  `[offA,offB)` main terrain/mesh stream (1.7–3.5 MB), `[offB,offC)`
  mid stream, `[offC,size)` trailing per-course table.
- Trailing table prologue (identical across courses): i16 `[48, 0, 80, 0, 0,
  0, N, 0, ...]` where `N` varies per course (12..22 — likely an object/prop
  count), then repeating 32-byte records whose words are
  `0x10000000, 0, 0x01000201, 0, 0x60000000, 0, 0, 0` constant-template
  entries — **VU0 macro / DMA control words**, same family as the FLD
  `0x1012xxxx` tags (see `field-topology-status.md`).
- No raw u32 copies of the race ladder values 58500/144000/162000 exist in
  any course file (they are computed in ELF code, not stored per-course).
- **ROM LBA linkage (2026-10-16, PROVEN):** the misc-type descriptor table at
  ELF vaddr `0x29be50` — 12 records `{tag 0x00333388, sectorOffset,
  sectorCount}` — matches `COURSE/C00..C11.BIN` **12/12 by ceil(size/2048)
  sectors, in table order** (race index → filename index). These sectors
  (`0x8AE6`–`0xC43D`) sit entirely before the FLD tile ranges (min `0xE5C8`),
  so courses and tiles never overlap. `C12..C20` are claimed by other
  descriptor tables in the wider `0x29aexx`–`0x29bfxx` region (see
  `asset-map.md` for the full sweep).
- Conclusion (updated 2026-10-16, refined by
  [`course-routes.md`](course-routes.md)): the earlier "blocked on VU0 macro RE"
  verdict is **void** — the game contains no VU0 macro streams at all
  (`vu0-macro-streams.md`). The bounded pass found COURSE tails are **prop
  render chains** (`N` == MSCALF `0xA` prop-draw count per course), not
  route/checkpoint records; AI navigation instead consumes ROM-resident
  24-byte segment + 8-byte transition tables (`FUN_00250068` /
  `FUN_0024fcd8`) whose COURSE linkage remains open. The native app's
  authored monotonic-zone routes (`gameplay/route.*`) remain the shipped
  stand-in; a runtime ROM-table route extractor is the next candidate;
  per-course scalar anchors (prop count `N`, region sizes) are available via
  `assets/course_data.*`.
