# B7 — Verification/capture script hardening

Worker: B7 · Date: 2026-10-17 · Repo: RoadTripAdventure-decomp

Files written: `scripts/post_recovery_verify.sh`, `scripts/rt_feature_matrix.sh`, this report, and the required capture artifact `build/scratch/gamelook/town-B7.ppm`. No git state changes; `src/native/**`, `CMakeLists.txt`, `third_party/`, `build/native/` untouched.

## 1. The town correctness bug, verified against main.cpp

`src/native/app/main.cpp` (read-only) direct-launch semantics:

- `initialAppState()` maps `RT_APP_STATE=PLAYING` → `AppState::PLAYING` (main.cpp:1340).
- `bool directStartRace = state == AppState::PLAYING;` (main.cpp:1844) — the first PLAYING logic step calls `session.startRace()` (main.cpp:2239-2242). The comment says it plainly: "expose its initial EXPLORE state for one frame, then start the same first AI race".
- `RT_APP_MODE=explore` sets `directStartRace = false` and "holds SessionState::EXPLORE indefinitely" (main.cpp:1845-1849); `RT_APP_MODE=race` forces `SessionState::RACE` at startup (main.cpp:1832-1839).

So `run town-v9 RT_APP_STATE=PLAYING RT_APP_FRAMES=120` captured the race HUD, not the town view, and `frame_compare` scored the wrong screen against `oracle-golden/town_driving.png`. Reproduced live (10-frame run, no explore):

```
rt-app session state=EXPLORE lap=0 finished=0 money=1000
rt-app session state=RACE   lap=0 finished=0 money=1000     <-- auto-race after 1 frame
```

### Fix in post_recovery_verify.sh

- Town capture line now: `capture town-v9 town_driving.png RT_APP_STATE=PLAYING RT_APP_MODE=explore RT_APP_FRAMES=120` (frame count kept at 120; explore holds the view for the whole run so the original count stays right).
- The "intra-run determinism pair (town)" legs (old lines 43-44) got the same `RT_APP_MODE=explore` — they are town captures too (60 frames kept).
- Race line keeps `RT_APP_MODE=race` (that one was already correct).

### Other scripts checked for the same bug

`grep -rn "RT_APP_STATE=PLAYING" scripts/` found only `android_build.sh:46`, `thor_setup_test.sh:26`, `thor_full_run.sh:40` — device smoke runs (600–6000 frames) that *want* the direct-launch auto-race; no town claim, not fixed (also outside this task's write scope). `oracle_golden_capture.sh` drives the PS2 oracle through rtharness pad menus, not `RT_APP_STATE`, so it is unaffected.

## 2. Timeouts + no-silent-skip + per-capture summary (post_recovery_verify.sh)

- `run_timed SECS CMD...` helper: uses `timeout` (Linux/coreutils), falls back to `gtimeout`, then to a `perl alarm` wrapper so it works on a bare macOS host. rc 124/142 = killed by timeout.
- Every rt-app invocation is bounded: env-recovery probe (`RT_VERIFY_PROBE_TIMEOUT`, default 60s), each capture, both determinism legs (`RT_VERIFY_TIMEOUT`, default 300s), and the whole matrix run (`RT_MATRIX_TIMEOUT`, default 2400s — the full matrix legitimately needs ~15-20 min, see §4).
- Capture + score are now one `capture()` step: rc≠0 or empty snapshot ⇒ `CAP-SUMMARY <name> FAIL (capture rc=…, snapshot missing/empty; no score)` and the failure is counted — scoring can no longer be silently skipped by a dead capture. `frame_compare` failure is likewise a counted FAIL with the snapshot kept for inspection.
- One-line summary per capture with the score when available: `CAP-SUMMARY <name> PASS hist=<x> <BAND>`. Bands (FAR/NEAR/CLOSE) are reported but **not** gated — `docs/native-reference/frame-compare.md` explicitly says they are calibration trend info, not pass/fail. Only tool failures (capture, scoring, matrix, determinism, push) count toward `CHAIN-SUMMARY failures=N` and the chain exit code (`CHAIN-PASS`/`CHAIN-FAIL`, non-zero exit on any failure).

## 3. Matrix coverage audit (rt_feature_matrix.sh)

Gate coverage before the edit: score HUD ✔ (`scorehud` row), terrain/stream/nav/hudtext/render-modes/host-wiring ✔, disc rows ✔. Missing: **explore mode**, **RT_APP_MINIMAP**, and the `RT_APP_MODE=race` forced-mode log. The "banner" (SCORE.GSL `raceBanner`/METER0.GSL dial art, main.cpp:1693-1695, 2362-2437) has **no RT_APP env gate and no success log line** (only `rt-app banner FAIL:` on upload failure), so it is not row-able with the presence-grep pattern — see §5.

Rows added:

| Row | Env | Asserts |
|---|---|---|
| `explore` | `RT_APP_STATE=PLAYING RT_APP_MODE=explore` | `rt-app session state=EXPLORE` — the exact contract the town captures depend on (would have caught this bug) |
| `mode_race` | `RT_APP_STATE=PLAYING RT_APP_MODE=race` | `rt-app mode: RACE (forced by RT_APP_MODE=race)` |
| `minimap` (disc) | `… RT_APP_MODE=explore RT_DISC=…` | `rt-app minimap: town street segments=` (main.cpp:1589) |
| `minimap_off` (disc) | `… RT_APP_MINIMAP=0 …` | that line **absent** |

For the absence check I added two helpers in the same style as the existing ones: `row_without` (mirror of `row()`, inverted grep) and `row_without_required_disc` (SKIP without disc). No other mechanism changed; `row()` now runs through `run_timed "$ROW_TIMEOUT"` (default `RT_MATRIX_ROW_TIMEOUT=300`) and labels timeouts distinctly: `FAIL <name> (timeout after 300s)`.

## 4. Test output

Full matrix run (default 300s row bound, no rebuild; `build/native/rt-app` as-is):

```
PASS title … PASS playing
PASS explore          <- new
PASS mode_race        <- new
PASS terrain          <- needed >120s (first TERRAIN=1 TILES_RADIUS=all run pays scene/pipeline warm-up); passes at the 300s default
PASS … (textured, stream, stream_row, stream_pos, nav, nav_town, hudtext, scorehud,
       carmodel, route, route_town, norender, snapshot, snapshot-file-nonempty,
       interp, triggers, perflog, glyphs, settings)
PASS minimap          <- new (disc)
PASS minimap_off      <- new (disc)
PASS single_tile … PASS disc_nav

rt-app feature matrix: 32 passed, 0 failed, 0 skipped
MATRIX_EXIT=0
```

Manual town capture with the fixed env (rc=0 proven):

```
$ timeout 300 env RT_APP_MODE=explore RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 \
    RT_APP_FRAMES=60 RT_APP_SNAPSHOT=build/scratch/gamelook/town-B7.ppm \
    ./build/native/rt-app
TOWN_RC=0
-rw-r--r@ 1440015 build/scratch/gamelook/town-B7.ppm   (800x600 + P6 header)
rt-app session state=EXPLORE lap=0 finished=0 money=1000   <-- held, no RACE takeover
rt-app snapshot: build/scratch/gamelook/town-B7.ppm (800x600)  (x60)

$ python3 scripts/frame_compare.py --oracle build/scratch/oracle-golden/town_driving.png \
    --native build/scratch/gamelook/town-B7.ppm --label town-B7 --json /tmp/town-B7.json
histogram_intersection = 0.5317   verdict = NEAR   non_black(native) = 0.75
```

NEAR is the expected calibration band for the current native scene (see frame-compare.md); the point proven here is capture correctness (EXPLORE held, real scene pixels, rc=0), not parity.

## 5. Recommended rows NOT added (and why)

- **Banner/HUD-art success row** — the shipped-art path (`raceBanner` + `speedDial` from SCORE.GSL/METER0.GSL) is auto-loaded from the disc with no RT_APP env and logs nothing on success (`rt-app banner FAIL:` only on failure). Needs a one-shot success log in `main.cpp` (e.g. `rt-app hudart: banner+dial loaded`) before a matrix row can assert it. main.cpp is off-limits this wave.
- **`RT_APP_PERFOVERLAY=1`** — overlay toggle; no stable assert line identified in this pass.
- **`RT_APP_GROUNDFILL=0`** — marked "TEMP diagnostic switch" in main.cpp:1103; gating a matrix row on a temp switch would fossilize it.
- **`RT_APP_RADIO` / station-follow row** — audio rows exist for BGM; a radio-follow row (`rt-app radio: station-follow owns the BGM path`, main.cpp:411) would fit the existing `row_required_disc` pattern; left out to keep this wave's diff to the audited gates.
- **`RT_APP_RUMBLE`** — needs a physical pad channel; host matrix runs headless/CI without one.
- **`RT_APP_RESOLUTION`/`RT_APP_FULLSCREEN`** — window chrome envs; byte-determinism of snapshots is the matrix's contract, so resolution rows belong in a separate visual check, not here.

## 6. Behavior notes for the next runner

- `post_recovery_verify.sh` now exits non-zero if any step failed (capture, score, matrix, determinism, push) and prints `CHAIN-SUMMARY failures=N` + `CHAIN-PASS/FAIL`. `CHAIN-DONE` is still printed for log-compat. It still ends with `git push origin re/native-complete` (pre-existing behavior; not executed during this task).
- Verdict bands are displayed, not gated (per frame-compare.md "not release pass/fail results").
- Tunables: `RT_VERIFY_TIMEOUT` (300), `RT_VERIFY_PROBE_TIMEOUT` (60), `RT_MATRIX_TIMEOUT` (2400), `RT_MATRIX_ROW_TIMEOUT` (300).
- First `TERRAIN=1 TILES_RADIUS=all` row on this host exceeded 120s; 300s covers it. If a slower CI host appears, raise `RT_MATRIX_ROW_TIMEOUT` rather than removing the bound.
