# Accuracy vs the emulator oracle (golden policy)

**Directive (user, standing):** the emulator (PS2Recomp running the real
guest, paraLLEl-GS) is the golden reference. Native output must reach 100%
accuracy against it while costing far less to produce. Native-vs-native
determinism pairs are regression pins only — they prove nothing about
correctness and are never sufficient evidence on their own.

## Protocol

Harness: `scripts/rt_oracle_diff.py` (drives both sides, writes
`build/scratch/oracle-diff/`: `oracle_<scene>.png`, `native_<scene>.ppm`,
`diff_<scene>.png` side-by-sides, `accuracy.json`). Re-run cost after first
capture: `--skip-oracle` reuses cached oracle PNGs (the oracle boot is the
slow half; native reruns are seconds).

- **Oracle side:** rtharness `Game(..., env={"RT_PROGRESSIVE_FIELDS": "1",
  "RT_GS_SSAA": "2"})` → scanout 640x448 progressive (SSAA=1 alone stays
  640x224 single-field). Scene anchors:
  - *title*: `boot_to_main_menu()` logo/attract frame;
  - *menu*: main menu up (New Game row focused);
  - *race*: `quick_race.open_course + start_race`, countdown/first-drive
    checkpoint;
  - *town*: checkpoint `adventure_first_save` loaded, town drive frame.
- **Native side:** `build/native/rt-app` with `RT_TIME=virtual`,
  `RT_APP_STATE`/`RT_APP_MODE` per scene, snapshot dumped at the matched
  moment (native extent is **800x600**; the report states the mismatch).
- **Comparison:** native resized onto oracle size, then the regression
  tolerance rule (`tests/regression/rtharness/goldens.py`: each pixel against
  the other's 3x3 neighbourhood both ways, off when a channel differs by more
  than 48): **fail above 3% of all pixels or 30% of any 32px block**.
  Reported per scene: `mad`, `pct16` (>16 levels), `pct48`, `tolerant_pct`.
- Outputs under `build/scratch/` are derived from the game: never committed.

## Baseline (2026-10-18, wave-8 HEAD)

| scene | mad | pct16 | pct48 | tolerant | verdict |
|-------|----:|------:|------:|---------:|---------|
| title | 31.6 | 40.5% | 27.0% | 23.4% | FAIL |
| menu  | 55.1 | 75.5% | 58.8% | 57.2% | FAIL |
| race  | 54.3 | 79.4% | 54.6% | 54.0% | FAIL |
| town  | 74.9 | 96.4% | 75.4% | 74.8% | FAIL |

(All also exceed the 30%-per-block bound; `worst_block_pct` ≈ 100.)

This is the honest quantification of the user's "graphics are unbelievably
broken" report. The gaps are **scene content**, not pixel tuning:

- **title** — native draws a synthesized panel; oracle shows the real
  publisher/title art sequence (full-screen textured pictures from disc).
  Closest scene already (flat background keeps mad down).
- **menu** — native reproduces row labels via our FONTE path but the oracle
  menu carries the game's background art, logo and selection styling.
- **race** — native draws sky + flat road ribbon + HUD box; missing: own car,
  opponent grid cars, terrain textures, roadside props, course-map HUD art.
- **town** — worst scene: the oracle frame is the full 3D town (buildings,
  streets, cars, character); native has no town scene content yet.

## Tractability tiers

1. **Static scenes** (title art, main menu, card screens, carousel,
   countdown): exact-match achievable — geometry and art are deterministic.
2. **Early dynamic frames** (race start, first town frames): achievable
   modulo small camera/physics offsets.
3. **Late dynamic frames**: blocked by physics drift until per-subsystem
   behavioral proofs land (native car physics approximates the guest; pixel
   parity late requires matching trajectory, not just rendering).

Wave 9+ targets scene-content packages (own car, grid cars, textured
terrain, real title/menu art); each delivery is validated by tolerant-diff
improvement on its scene, recorded in the table above (update this doc per
wave with a dated row).

## Performance side of the directive

Native must also *beat* the oracle's cost. Lockstep headless wall time is a
weak proxy (it pins both to virtual frames); the honest comparison is per
scene CPU/wall at equal frame counts, measured on host and device (the
oracle via `scripts/android/measure.py` style sampling, native via its
`[perf]` windows). Recorded per wave in the wave-board until a dedicated
budget doc has real numbers in it.
