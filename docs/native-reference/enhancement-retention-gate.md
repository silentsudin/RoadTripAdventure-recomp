# Enhancement retention gate (W2-13)

Status: wave-2 batch W2-13 deliverable, 2026-10-18. Coordinator note: merge the
retention-relevant sections into `feature-retention.md`; this file is the gate's
own contract and stays the reference for the registry schema.

Every QoL enhancement, harness contract, and diagnostic flag this native rebuild
ships is a **registered** row in a machine-readable registry, gated by an
automatically re-run regression check so no integration wave can silently drop
it. The registry reconciles the 2026-10-16 `feature-retention.md` ledger (17
rows) with the `RT_*` env contract that actually lives in `src/native/`.

## 1. Registry

* Single source of truth: `src/native/gameplay/enhancements.{h,cpp}`
  (`rt::native::gameplay::enhancementRegistry()`).
* Committed machine-readable copies (byte-verified against the compiled table
  by the gate): `tests/native_w2/data/enhancement_registry.csv` and `.json`.
* Regenerate after a registry edit:
  `build/<dir>/rt-w2-enh_registry csv > tests/native_w2/data/enhancement_registry.csv`
  (and `json` likewise). Forgetting regeneration **fails the gate**.

Columns: `id, ledger, env_vars, persisted_keys, default, gating, owners, docs,
status, notes`.

* `id` — stable snake_case registry id.
* `ledger` — the `feature-retention.md` row it retains, or `-` for
  harness/infra entries. All 17 ledger ids: `widescreen hd_packs second_screen
  frame_generation rumble radio bgm analog_triggers save_states save_migration
  hud traffic_ai chase_camera economy world_streaming task_scheduler minigames`.
* `env_vars` — `|`-separated `RT_*` names (empty for ledger features that have
  no flag yet — they are registered via `persisted_keys` instead).
* `persisted_keys` — `settings.toml` keys (`gameplay/settings.cpp`
  `OptionsModel`) that map onto the env vars (`bootstrapSettings`,
  `main.cpp:190-246`, env-first precedence).
* `gating` — `always | headless/CI | device (gamepad/display) | disc | oracle`.
* `owners` — `|`-separated owning module paths; the gate asserts they exist.
* `docs` — citations, each individually verified by the gate:
  `docs/native-reference/<file>.md`, `code:<repo-relative path>` (file must
  still reference the flag), or `plan` (`.qoder/wave2-plan-sonnet.md` §W2-13).
  At least one citation per entry must still reference the entry's env var.
* `status` — `live` (active in the shipped app loop, default-on or
  matrix-proven), `wired` (implemented + opt-in/settings-glued, contract
  proven), `oracle-only` (harness/tool/diagnostic aid), `doc-only`
  (documented but no runtime consumer — a tracked gap).

Population (2026-10-18): **50 entries** (41 with env flags covering **51
distinct `RT_*` names**, 9 ledger-feature rows), 108 owner paths, 93 doc
citations, 17/17 ledger rows covered.

## 2. Headless capture contract

The deterministic-capture variables every harness/oracle flow relies on:

| Var | Default | Contract |
|---|---|---|
| `RT_APP_HEADLESS` | off | SDL dummy video + null presenter; logic + offscreen render + snapshot without a WindowServer (`main.cpp:1428-1481`) |
| `RT_APP_NOAUDIO` | audio on | skip `initAudio` (`main.cpp:1643`); matrix rows pin it |
| `RT_APP_STATE` | `TITLE` | `TITLE\|MENU\|PLAYING` direct launch (`main.cpp:1340-1345`) |
| `RT_APP_MODE` | unset | `race` forces `SessionState::RACE`, `explore` holds EXPLORE (`main.cpp:1832-1849`) |
| `RT_APP_FRAMES` | 0 (unlimited) | logic-step budget; stays a logic-step count under interp (`frame-interpolation.md:21-23`) |
| `RT_APP_SNAPSHOT` | off | one P6 PPM per presented frame, logic-step-boundary pose under interp (`main.cpp:2294-2301, 2787-2798`) |
| `RT_APP_NORENDER` | render on | skip render+present (`main.cpp:1485`) |
| `RT_TEST_SOCKET` | off | deterministic drive socket (`runtime/testsocket.cpp:57`) |

## 3. The regression gate

Two independent implementations must agree; both re-scan `src/native/` at
runtime (vendored/generated trees excluded: `src/native/android` = Gradle +
SDL3 + `.cxx` build outputs; any dotted directory):

* `tests/native_w2/enhancements_test.cpp` (binary `w2_enhancements_test`) —
  prints exactly one `W2-13 SCORE ...` line, exit 0 only if every check holds.
* `scripts/w2_retention_gate.py` — prints `W2-13 GATE ...`, same contract.

Checks (any miss fails and **lists the offending ids** — the gate is built for
no false passes):

1. `coverage` — every quoted `RT_*` literal in `src/native` code exists in the
   registry (the batch threshold: <100% ⇒ fail). The scan also catches
   helper-mediated flags (`featureDefaultOn`, `setDefaultEnv`, settings
   `boolFlag` call sites), not just direct `getenv` lines.
2. `flags_resolve` — every registered env var is still referenced by code
   (the "silently dropped enhancement" case).
3. `owner_files` — every owner path exists.
4. `doc_cites` — every entry cites ≥ 1 verifiable doc/code/plan citation and
   ≥ 1 citation still references the flag.
5. `ledger` — all 17 `feature-retention.md` rows covered.
6. `data_files` — committed CSV/JSON byte-match the compiled registry (CSV vs
   JSON agreement is also cross-checked by the Python gate).
7. `selftest` — in-memory proof the gate fails on: a removed registry entry
   (`RT_APP_RUMBLE` case), an unregistered env var, a broken owner path, an
   uncited entry.
8. `deterministic` — the source scan runs twice; FNV-1a site-list hash and
   registry emission must be identical.

Run (private build dir; no ELF needed — `--elf`/`RT_ELF` accepted and ignored,
SCORE line always printed):

```
cmake -S src/native -B build/w-W2-13 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-13 --target w2_enhancements_test rt-w2-enh_registry -j
build/w-W2-13/w2_enhancements_test          # W2-13 SCORE ...  (exit 0 = green)
python3 scripts/w2_retention_gate.py        # W2-13 GATE ...   (independent)
```

Current output (2026-10-18):

```
W2-13 SCORE registry=50 coverage=51/51 flags_resolve=51/51 getenv_sites=69/164 owner_files=108/108 doc_cites=93/93 ledger=17/17 data_files=2/2 selftest=4/4 deterministic=1
W2-13 GATE registry=50 env_vars=51/51 flags_resolve=51/51 getenv_sites=69/164 owner_files=108/108 doc_cites=93/93 ledger=17/17 gate=PASS
```

Negative controls (verified 2026-10-18): appending a fake row to the CSV fails
`data_files` 1/2 with exit 1; deleting the `rumble` row from the CSV makes the
Python gate fail with `coverage: ... NOT in registry: RT_APP_RUMBLE` and
`ledger: ... rumble` (exit 1); restore ⇒ exit 0.

## 4. Matrix mapping

`scripts/rt_feature_matrix.sh` currently carries 31 named rows + the
`snapshot-file-nonempty` check (the 2026-10-16 coordinator run that
`feature-retention.md` cites as 23/23 predates the `explore`/`mode_race`/
`glyphs`/`settings`/`interp`/`triggers`/`perflog`/`minimap`/`minimap_off`/
`disc_*` additions). Every `RT_APP_*` flag with a runtime consumer is either a
matrix row, a `featureDefaultOn` default-on path, or a registered diagnostic;
the two doc-only gaps below are tracked, not silently accepted.

## 5. Known gaps (tracked, reported to the coordinator)

* `RT_MENU` (`pause_menu_open`) — documented (`options-menu.md:59,81,194`) and
  spec'd as the `display.menu` env key (`settings.cpp:411-413`), but no
  `src/native` code reads it yet.
* `second_screen` — settings key registered (`settings.cpp:441`), no second
  presenter exists (ledger row MISSING).
* `RT_APP_PERFLOG_EVERY` (`perf-overlay.md:85`) and `RT_APP_FONT_DUMP`
  (`glyph-repaint.md:39`) are documented but not implemented in code.
* `controllers.rumble_strength` (`settings.cpp:484`) is not consumed —
  `rumble::Channel` exposes no global gain (`rumble-haptics.md` §5).
