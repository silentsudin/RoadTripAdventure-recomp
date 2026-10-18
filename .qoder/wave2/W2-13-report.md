# W2-13 report — Enhancement retention registry (feature registry + regression gate)

Date: 2026-10-18. Branch `re/native-complete`. Plan: `.qoder/wave2-plan-sonnet.md` §W2-13 (governing).
Everything here is reproducible with the commands quoted inline; no numbers are transcribed without one.

## Summary

Every `RT_*` enhancement/harness fact in `src/native/` is now a registered,
flag-addressable, automatically-gated row. Two **independent** gate
implementations (C++ test + Python script) re-scan the sources at runtime and
agree exactly:

```
W2-13 SCORE registry=50 coverage=51/51 flags_resolve=51/51 getenv_sites=69/164 owner_files=108/108 doc_cites=93/93 ledger=17/17 data_files=2/2 selftest=4/4 deterministic=1   (exit 0)
W2-13 GATE registry=50 env_vars=51/51 flags_resolve=51/51 getenv_sites=69/164 owner_files=108/108 doc_cites=93/93 ledger=17/17 gate=PASS                                       (exit 0)
```

The gate asserts 100% coverage **both directions** (code→registry and
registry→code), owner existence, verifiable doc citations, 17/17
`feature-retention.md` ledger coverage, byte-exact committed data files, an
in-memory self-test that proves the gate can fail, and a determinism pair.
A coverage failure lists the missing ids; there are no false passes (negative
controls below).

## Files (all batch-owned; nothing else touched)

| File | Role |
|---|---|
| `src/native/gameplay/enhancements.{h,cpp}` | registry (single source of truth) + scan + coverage gate + CSV/JSON emission |
| `src/native/tools/w2_enh_registry_main.cpp` | `rt-w2-enh_registry csv\|json` — deterministic regeneration of the committed data files (Step-0 tool glob) |
| `tests/native_w2/enhancements_test.cpp` | the gate binary `w2_enhancements_test` (auto-registered by the Step-0 test glob) |
| `tests/native_w2/data/enhancement_registry.csv` / `.json` | committed machine-readable registry (50 rows) |
| `scripts/w2_retention_gate.py` | independent Python gate (plan-named; the batch brief's fallback name `scripts/enhancement_matrix.sh` was not needed because the plan names this file) |
| `docs/native-reference/enhancement-retention-gate.md` | gate contract + schema (coordinator merges retention sections into `feature-retention.md`) |
| `.qoder/wave2/W2-13-report.md` | this report |

## How to reproduce

```
cmake -S src/native -B build/w-W2-13 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-13 --target w2_enhancements_test rt-w2-enh_registry -j
build/w-W2-13/w2_enhancements_test          # SCORE line above; exit 0
python3 scripts/w2_retention_gate.py        # GATE line above; exit 0
```

Reproducible inventory counts (macOS grep; `-C` not needed):

| Count | Command | Value |
|---|---|---|
| distinct RT_* names in code | `grep -rhoE '"RT_[A-Z0-9_]+"' src/native --include=*.cpp --include=*.h --include=*.cc \| sort -u \| wc -l` | **51** |
| quoted-literal sites (scan universe) | `grep -rnoE '"RT_[A-Z0-9_]+"' src/native --include=*.cpp --include=*.h --include=*.cc --exclude-dir=android \| wc -l` | **164** |
| direct getenv sites (lines) | `grep -rn 'getenv(' src/native --include=*.cpp --include=*.h --exclude-dir=android \| grep -E '"RT_[A-Z0-9_]+"' \| wc -l` | **65** |
| direct getenv sites (literal occurrences; SCORE `getenv_sites`) | gate-internal: occurrences on `getenv(` lines (multi-literal lines count per literal, e.g. `main.cpp:391` counts BGM + BGM_TSQ) | **69** |
| matrix rows | `grep -oE '^\s*(row\|row_without\|row_required_disc\|row_without_required_disc) [a-z_0-9]+' scripts/rt_feature_matrix.sh` | **31 named rows** (+ snapshot-file-nonempty check) |
| ledger rows | `docs/native-reference/feature-retention.md` table body | **17** |

Scan exclusion (documented, deliberate): `src/native/android` (Gradle/SDL3/
`.cxx` vendored + generated tree; contains zero project `RT_` literals —
verified `grep -rln 'RT_' src/native/android --include='*.cpp' --include='*.h'`
outside `.cxx` returns nothing) and any dotted directory.

## Findings per feature (compact)

Full details (default, gating, owners, docs, notes) live in
`tests/native_w2/data/enhancement_registry.csv`; status taxonomy: live /
wired / oracle-only / doc-only. `L=` ledger row retained.

| Env var(s) | Registry id | Status | Key fact (evidence) |
|---|---|---|---|
| RT_APP_HEADLESS | headless_mode | live | dummy video + null presenter (main.cpp:1428-1481); doc = code + plan |
| RT_APP_NOAUDIO | noaudio | live | skips initAudio (main.cpp:1643); settings sound.audio; matrix rows |
| RT_APP_STATE | start_state | live | TITLE/MENU/PLAYING (main.cpp:1340); app-flow.md words |
| RT_APP_MODE | mode_gate | live | race/explore forced session state (main.cpp:1832-1849); matrix explore/mode_race |
| RT_APP_FRAMES | frame_budget | live | logic-step budget (main.cpp:629); interp keeps it logic-count (frame-interpolation.md:21-23) |
| RT_APP_SNAPSHOT | snapshot_capture | live | P6 PPM per frame; t=0 pose contract under interp (main.cpp:2294-2301, 2787-2798) |
| RT_APP_NORENDER | norender | live | skip render (main.cpp:1485); matrix norender |
| RT_TEST_SOCKET | test_socket | oracle-only | deterministic drive socket (runtime/testsocket.cpp:57) |
| RT_APP_INTERP / _CAP / _RATE | frame_interpolation (L=frame_generation) | wired | 60 Hz logic locked, render poses interpolated (main.cpp:636-654, frame-interpolation.md); matrix interp |
| RT_APP_RUMBLE | rumble (L=rumble) | wired | opt-in exactly "1", gate (player&master)!=0 (main.cpp:1881-1900, rumble-haptics.md §3) |
| RT_APP_RADIO | radio_station_follow (L=radio) | wired | explicit byte wins; explicit BGM beats implicit default (regression 2026-10-17); station owns BGM path (main.cpp:376-411, radio-stations.md §5) |
| RT_APP_BGM / _TSQ | bgm_playback (L=bgm) | wired | TSQ BGM_01 default / VAG path; "0" forces VAG (main.cpp:405-419); matrix bgm/bgm_tsq |
| RT_APP_TRIGGERS | analog_triggers (L=analog_triggers) | wired | exactly "1" (main.cpp:1486-1490); input.{h,cpp} mapping; matrix triggers |
| RT_APP_RESOLUTION / _FULLSCREEN | output_modes (L=widescreen) | wired | preset\|WxH ladder + borderless fs; unset = byte-identical 800x600 (main.cpp:1438-1470, output-modes.md §6) |
| RT_APP_GLYPH_FAMILY | glyph_family | wired | forced ps/xbox/nintendo glyphs (main.cpp:92-100, glyph-repaint.md:114); matrix glyphs |
| RT_APP_MINIMAP | minimap_pill (L=hud) | live | default-on pill+roads; "0" kills (main.cpp:1583, 2570); matrix minimap/minimap_off |
| RT_APP_PERFOVERLAY / _PERFLOG | perf_overlay | wired | exactly "1" each (main.cpp:1491-1497, perf-overlay.md:82-85); matrix perflog |
| RT_APP_TERRAIN | disc_terrain (L=world_streaming) | live | GAME-LOOK v1 default-on with disc (main.cpp:762-769, 938); matrix terrain tiles=64 |
| RT_APP_TEXTURES | field_textures (L=world_streaming) | live | RGBA8 decode + content-hash page dedup (main.cpp:705-752); also HD-pack key source (hd-texture-packs.md:44) |
| RT_APP_TERRAIN_FILE | single_tile_override | wired | one FLD tile (main.cpp:954-967); matrix single_tile |
| RT_APP_TILES_RADIUS | tiles_radius | live | default 1, "all" = grid (main.cpp:940-950) |
| RT_APP_STREAM / _STEP_FRAMES / _ROW / _POS | tile_streaming (L=world_streaming) | live | 4-slot window slot=id&3, sweep, advanceTo demand (main.cpp:804-812, 1524-1545); matrix stream/stream_row/stream_pos |
| RT_APP_NAV / _TOWN | nav_overlay | wired | depth-off overlay strips (preview.cpp:175, 1035); matrix nav/nav_town |
| RT_APP_ROUTE / _TOWN | route_follower | wired | graph-0 / town-graph lap drive (main.cpp:1598-1632); matrix route/route_town |
| RT_APP_HUDTEXT | hud_text_fonte (L=hud) | live | FONTE.GSL decode, default-on with disc (main.cpp:774-802, 1627); matrix hudtext |
| RT_APP_SCOREHUD | score_hud_page (L=hud) | wired | off\|score\|meter (main.cpp:1628-1635); matrix scorehud |
| RT_APP_CARMODEL | car_model | wired | Q{N}.BIN CAR pipeline (main.cpp:1659/1696); matrix carmodel |
| RT_APP_CARMODEL_DUMP | car_livery_dump | oracle-only | PPM livery dump (main.cpp:1747); HD-pack dump-format precedent (hd_pack.h:28-34) |
| RT_DISC | disc_root | live | disc root default ~/Library/.../disc (main.cpp:336, setup_wizard.cpp:70) |
| RT_DATA_DIR | data_root | live | data root (main.cpp:102-104); oracle-golden-frames.md |
| RT_ASSET_DIR | asset_dir | live | packaged-asset override (main.cpp:349; android bootstrap main.cpp:150) |
| RT_MENU | pause_menu_open | **doc-only** | spec'd (settings.cpp:411-413, options-menu.md) but **no code consumer** — gap G-1 |
| RT_APP_DEBUG_OVERLAY | debug_overlay | oracle-only | frustum overlay (preview.cpp:1161); no matrix row by design |
| RT_APP_CARDIAG | car_diagnostics | oracle-only | "=1" pose/band diagnostics (camera.cpp:115, preview.cpp:1089, main.cpp:2435, hud_art.cpp:280) |
| RT_APP_GROUNDFILL | ground_fill_toggle | oracle-only | "0" skips authored fill sheet (main.cpp:1103-1107) |
| RT_APP_SETUP_VERIFY | setup_verify_mode | wired | "full" re-CRC mode (setup_wizard.cpp:122-153) |
| RT_DRIVE_TURN/ARRIVE/VMAX/SLEW/NORENDER | drive_tool_* | oracle-only | rt-drive tool tuning (drive_main.cpp:87-111) |
| (no env) display.second_screen | second_screen (L=second_screen) | **doc-only** | settings key only; one Presenter exists — ledger row still MISSING |
| (no env) save.slot/save.save_state/save.load_state | save_states (L=save_states) | wired | session.cpp:67-73 slot<N>.sav; ledger PARTIAL |
| (no env) P2 import | save_migration (L=save_migration) | wired | save.cpp:456-510; ledger IMPLEMENTED-UNVERIFIED |
| (no env) ai.cpp | traffic_ai (L=traffic_ai) | wired | speed law verified / steering authored; ledger PARTIAL |
| (no env) camera.cpp + main.cpp:2308 view lift | chase_camera (L=chase_camera) | wired | host chase cam + terrain-following view lift (race datum fix 2026-10-17); ledger PARTIAL |
| (no env) session/rules_tables | economy_rules (L=economy) | wired | authored ladder; funnel not routed yet (W2-06 owns it) |
| (no env) task.cpp | task_scheduler (L=task_scheduler) | wired | 48-record scheduler; ledger IMPLEMENTED-UNVERIFIED |
| (no env) minigames.cpp | minigame_rules (L=minigames) | wired | bounded helper scope; ledger PARTIAL |
| (no env) hd_pack.* | hd_texture_packs (L=hd_packs) | wired | StableContentKey + replacement/dump API; app pack-lookup glue pending |

Batch-brief spot-checks: depth-off overlay = nav/route strip pipeline
(preview.cpp:175) → `nav_overlay`; font-atlas upload cache = FONTE decode +
content-key pipeline (`glyph_atlas`, `hd_pack.h:20-40`) → `field_textures`/
`hd_texture_packs`/`glyph_family`; view lift = main.cpp:2308-2321 →
`chase_camera`; 60 Hz interp → `frame_interpolation`; output-modes ladder →
`output_modes`; headless contract (HEADLESS/NOAUDIO/STATE/MODE/FRAMES/
SNAPSHOT) → section-2 rows; rumble opt-in → `rumble`; radio →
`radio_station_follow`. All present.

## Gate verification performed

1. Build: targets `w2_enhancements_test` + `rt-w2-enh_registry` (private dir
   `build/w-W2-13`, `-S src/native` per plan §1; no CMake edits needed —
   Step-0 globs picked both up; `gameplay/enhancements.cpp` was pre-staged at
   `src/native/CMakeLists.txt:111`).
2. Gate green (both implementations, counts above); no ELF needed
   (`--elf`/`RT_ELF` accepted and ignored; SCORE printed regardless — no
   NO_ELF exit).
3. Determinism pair: two binary runs byte-identical output, both exit 0;
   in-test double scan hashes equal (`deterministic=1`).
4. Negative controls (no false passes):
   * append `RT_APP_FAKE_FLAG` row to committed CSV → `FAIL data_files 1/2`,
     exit 1; restore → exit 0.
   * delete the `rumble` row from the CSV → Python gate
     `coverage: env var used in code but NOT in registry: RT_APP_RUMBLE`,
     `ledger: row without any registry entry: rumble`, `gate=FAIL`, exit 1;
     restore → exit 0.
   * in-memory selftest (runs inside every gate invocation): removed
     RT_APP_RUMBLE registry entry, injected `RT_APP_SELFTEST_UNREGISTERED`
     code site, broken owner path, empty docs field — all 4 detected
     (`selftest=4/4`).

## Gaps

Features documented but absent from code (doc-only):
* **G-1 `RT_MENU`** — options-menu.md:59/81/194 + settings.cpp:411-413 spec
  the interim pause-menu open flag; no `src/native` getenv consumer.
* **G-2 `second_screen`** — settings key display.second_screen registered
  (settings.cpp:441) but the app creates one window/Presenter
  (feature-retention.md row still MISSING).
* **G-3 `RT_APP_PERFLOG_EVERY`** (perf-overlay.md:85) and **G-4
  `RT_APP_FONT_DUMP`** (glyph-repaint.md:39) — documented contracts, not
  implemented.
* **G-5 `controllers.rumble_strength`** (settings.cpp:484) — not consumed;
  `rumble::Channel` has no global gain (rumble.h:161-198, rumble-haptics.md §5).

Code-only with no docs/native-reference mention (registered via `code:`/`plan`
citations so the gate still verifies them): `RT_APP_HEADLESS`,
`RT_APP_CARDIAG`, `RT_APP_GROUNDFILL`, `RT_APP_MINIMAP`, `RT_APP_TILES_RADIUS`,
`RT_APP_SETUP_VERIFY`, `RT_ASSET_DIR`, `RT_DRIVE_*` (5). Suggest one line each
in feature-retention.md at merge time.

Ledger reconciliation (code has moved past the 2026-10-16 audit): rumble
(MISSING→wired), frame generation/interp (MISSING→wired), radio
(PARTIAL→wired env contract), HD packs (MISSING→module + keying pipeline
wired, app glue pending), output modes/widescreen (PARTIAL→wired env ladder).
`docs/native-reference/feature-retention.md` is now stale on those rows; the
coordinator merges `enhancement-retention-gate.md` to fix this.

## Confidence

* **High** — inventory + coverage numbers: mechanical greps, two independent
  scanners (C++/Python) produce identical 51-var/164-site/69-direct results,
  data files byte-verified against the compiled table, negative controls fail
  as designed.
* **Medium** — per-entry status/default semantics: read from code + docs by
  hand (50 entries); defaults are quoted from the source lines cited in each
  row, but classification (live vs wired) is judgment.
* **Not verified here** — the feature matrix itself was NOT re-run (needs a
  built `rt-app`, which is not a batch target; the script also carries
  uncommitted edits and is read-only for this batch per the plan).

## Deviations from plan §W2-13 acceptance (for the coordinator)

* Plan threshold line was `registry=17/17 flags_resolve=17/17 matrix=23/23
  determinism_pair=PASS new_regressions=0`. The coordinator's batch brief
  redefined the gate as the runtime getenv re-grep with the threshold
  "coverage <100% ⇒ fail". SCORE now reports `registry=50` (entries),
  `coverage=51/51` (the batch threshold), `flags_resolve=51/51`,
  `ledger=17/17` (the plan's registry=17/17 equivalent), `deterministic=1`.
* `matrix=…` is absent from the SCORE: running 31 matrix rows needs a built
  `rt-app` + up to 300 s/row; that is the coordinator green-bar step
  (plan §4 I-5). Open question below.
* Plan's "determinism pair (two rt-app RT_APP_SNAPSHOT runs, byte-identical
  PPM)" replaced by the double binary run + double scan-hash above (same
  no-flake intent, no rt-app dependency).

## Open questions

1. Should the coordinator wire `rt_feature_matrix.sh` + a two-run
   RT_APP_SNAPSHOT determinism pair into the green bar next to the 73 native
   tests (plan §4 I-5)? The gate's SCORE deliberately does not fake it.
2. G-1/G-3/G-4: implement (`RT_MENU` menu open, `RT_APP_PERFLOG_EVERY`,
   `RT_APP_FONT_DUMP`) or de-document? Registry marks them doc-only so they
   cannot be forgotten.
3. Should `doc-only` statuses hard-fail the gate after a grace wave
   (currently reported, not fatal — a threshold change is coordinator-only)?
4. Matrix grew 23→31 named rows since the ledger's 23/23 note; when the
   coordinator re-runs, feature-retention.md's "23/23" paragraph should be
   updated to the new row count.
