# W2-02 report: application flow state machine

Status: **PASS** on the pre-registered acceptance (`golden_words=9/9`, `edges=62/62`, `illegal_accepted=0`, `replay_hash_equal=1`), with two caveats on golden strength (below).

## Summary

Built a pure, table-driven app flow (`BOOT → TITLE → ATTRACT / MENU → PLAYING{EXPLORE, RACE, RESULTS}`, with EXPLORE overlays for dialogue, shop and the factory list). It has no SDL, GPU or heap. It is a drop-in for the `AppState`/`SessionState` plumbing in `main.cpp`, and the names match what `main.cpp` honours (`TITLE`, `MENU`, `PLAYING`; `EXPLORE`, `RACE`, `RESULTS`). Integration into `main.cpp` is a coordinator step (I-1); I did not touch it.

The machine is 60 Hz fixed-step. `step()` is one logic tick, with an interpolation-facing `FlowView` (previous/current/alpha). The title idle timeout is the only timed transition: 2700 ticks = 45 s.

## Files (all new; no existing file edited)

| Path | Role |
|---|---|
| `src/native/gameplay/app_flow.h` | Enums, `FlowWords` (guest-equivalent words), `AppFlow`, `FlowView`, `hashFlowWords`. |
| `src/native/gameplay/app_flow.cpp` | Transition logic and word effects. |
| `tests/native_w2/app_flow_test.cpp` | Acceptance test, `w2_app_flow_test`. |
| `docs/native-reference/app-flow.md` | Transition table with evidence tags, direct launches, ELF cross-check, open questions. |
| `.qoder/wave2/W2-02-report.md` | This file. |

Not touched: `CMakeLists.txt`, `main.cpp`, `third_party/`, `build/native/`, any sibling batch's file. Build dir: `build/w-W2-02`.

## Result

```
W2-02 SCORE golden_words=9/9 edges=62/62 illegal_accepted=0 replay_hash_equal=1 deterministic=1 reachable=26 word_effects=16/16 elf_dispatch=3/3 PASS
```

| Metric | Value | What it checks |
|---|---|---|
| `golden_words` | 9/9 | Nine oracle frames replayed from BOOT with the documented input scripts. Checks are the words each frame documents. |
| `edges` | 62/62 | 26 reachable contexts × 16 events = 416 pairs. The machine must match an independent spec table in the test: 62 legal pairs accepted with the right successor. |
| `illegal_accepted` | 0 | Exhaustive pass (354 illegal pairs) + two seeded 5000-event fuzz runs. |
| `replay_hash_equal` | 1 | Two fuzz replays from seed `0x5EED0202` give the same FNV-1a hash. |
| `deterministic` | 1 | Whole suite run twice, combined digest equal. |
| `word_effects` | 16/16 | Progress writes (10/3/2/0 by path), flag bits, exact 2700-tick title timeout, direct launches, `FlowView` snap rule. |
| `elf_dispatch` | 3/3 | Handler selector bytes at `0x21CE08..0x21CE20`; `StartRaceModeTask` calls it with `a0=1` (delay slot at `0x20F9A4`); `StartTownModeTask` calls it. |

The acceptance line from the plan has a fixed shape. This line adds `reachable`, `word_effects` and `elf_dispatch` fields and a `PASS`/`FAIL` token. The four pre-registered fields are unchanged.

### Golden mapping (`oracle-golden-frames.md`)

| Golden | Script | Asserted words |
|---|---|---|
| title | BOOT, 720 idle ticks | state TITLE, game_mode 0 |
| main_menu | + Start, 180 ticks | state MENU/MODE_SELECT, game_mode 0 |
| options | + Down×3, Confirm | state MENU/OPTIONS, game_mode 0 |
| attract | BOOT, 2700 idle ticks | ATTRACT, game_mode 8, active 2, handler race, flags `0x10104`, scene 0 |
| race_hud | + Quick Race (Down, Confirm), 2447 ticks | RACE, game_mode 1, handler race, active 2, flags `0x124`, scene 0 |
| race_hud_driving | + 731 ticks | RACE, game_mode 1, handler race, scene 0 |
| factory_menu | + Continue (Confirm, Confirm) | PLAYING/EXPLORE/FACTORY_MENU, game_mode 0, scene 6, location 1 |
| town_driving | factory → Down×4, Confirm, DialogueDone, 720 ticks | EXPLORE, handler town, flags `4`, scene 2, location 1, game_mode 0 |
| town_driving_street | town_driving + 300 ticks | same tuple as town_driving |

## Caveats on the 9/9

1. **Three goldens (title, main_menu, options) assert state and game_mode only.** The oracle records only the UI context for those frames. The handler/flags/scene words are not documented there, so they are not asserted.
2. **`race_hud_driving` asserts handler/game_mode/scene but not flags.** Its drive flags are not recorded.
3. **`town_driving_street` re-asserts the town tuple.** The +12.274 car motion is not modelled; the flow has no position.
4. **The Cross×8 presses in the quick race and the Cross×2 "Come again!" presses** are modelled as the single flow events (Confirm / DialogueDone). In-race and dialogue button presses are not flow events. The golden scripts were translated accordingly.

## Mutation check (reproducible)

I injected deliberate bugs into `src/native/gameplay/app_flow.cpp`, rebuilt, ran, and restored the file (`cmp` identical afterwards).

| Mutant | Before word-effects tests | After |
|---|---|---|
| Quick Race writes `progressScene=10` | FAIL (golden 7/9) | FAIL |
| Adventure race-over does not write scene 3 | **PASS (gap)** | FAIL (word_effects 15/16) |
| Escape also accepted from EXPLORE/SHOP (illegal) | FAIL (edges 59/60, illegal 73, replay 0) | FAIL |

The second mutant survived the first version of the test. The word-effect section was added to close that gap. Restored file: PASS.

Missing ELF: `w2_app_flow_test --elf /nonexistent/SLUS` exits 2 and prints `W2-02 NO_ELF`.

## Reproduce

```sh
cmake -S src/native -B build/w-W2-02 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-02 --target w2_app_flow_test -j
build/w-W2-02/w2_app_flow_test --elf "$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98"; echo $?
# expect the SCORE line above, exit 0
```

Runtime about 0.01 s user time per run; well inside the 60 s budget.

## Confidence

- **High:** the mechanics of the machine as built (BFS and fuzz against an independent spec table; ELF dispatch bytes; the 9 documented golden tuples; the 2700-tick title timeout). The ELF checks were run against the retail binary, not a copy.
- **Medium:** the Adventure-race-over `progressScene=3` (plan-sourced, not sampled), Adventure race flags (assumed equal to Quick Race), factory-overlay words (derived).
- **Low / design:** results-return to town, Cancel-to-title from MODE_SELECT, clamped (not wrapped) navigation, Escape cursor reset, Results and Options sub-pages. These are marked `[X]` in `app-flow.md`.

## Open questions

1. **Attract trigger.** The 45 s idle is harness-observed. I did not trace what in the decomp starts the attract demo. The model uses a title-only idle counter. Needs a `game_mode=8` setter trace (a W2-02 follow-up or a later wave).
2. **Menu idle.** Does MENU time out to attract? Not known. Not modelled.
3. **Quick Race / 2P return path.** The oracle never shows what follows a Quick Race result. `ResultsConfirm` returns to town, mirroring `session.cpp:83-85`. Likely wrong for non-Adventure modes.
4. **Race-over scene 3** is plan-sourced; no oracle frame samples it.
5. **`driveFlags` bit `0x100`** is set in race and attract and is still unnamed (`oracle-golden-frames.md` UNDOCUMENTED #3).
6. **Two-player race** uses the two-player table branch (`0020f950.c:36`). No oracle frame. Its HUD and results are out of scope here.
7. **Factory is modelled as a town-handler EXPLORE overlay.** The oracle does not record the handler for that frame.
8. **Integration.** The coordinator step (I-1) must map `AppFlow` to `main.cpp`'s `AppState`/`SessionState`, including the `RT_APP_STATE` / `RT_APP_MODE` launch mapping. The `main.cpp` reads of `menuFlow` (cursor persistence) need checking at that point.

## Notes

- The working tree has many pre-existing uncommitted changes from other batches and from the user. None are mine. I did not run `git add`, `commit` or `push`.
- All evidence cited in the doc is from `build/decomp/fn/*.c` (line numbers checked), `docs/native-reference/oracle-golden-frames.md`, `frame-timing.md`, `main.cpp`, `session.cpp`, and the retail ELF bytes. No ROM bytes were committed.
