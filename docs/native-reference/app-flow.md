# App flow state machine (W2-02)

Pure, table-driven application flow: `src/native/gameplay/app_flow.{h,cpp}`. No SDL, no GPU, no heap. It replaces the ad-hoc `AppState` and `SessionState` plumbing in `app/main.cpp` (declared near line 1301, transitions near 2001-2047) at coordinator integration. It does not edit any existing file.

Names: top-level states use the `RT_APP_STATE` values main.cpp honours (`TITLE`, `MENU`, `PLAYING`), plus `BOOT` and `ATTRACT`. Session values match `gameplay::SessionState` (`session.h`) under `FlowSession`, so the headers do not collide.

## States and words

Each state exports the guest-equivalent words (`FlowWords`). A renderer reads these through `FlowView` (previous + current + alpha). Every field is discrete, so `presented()` snaps at alpha 0.5.

| Word | Address | Source |
|---|---|---|
| `gameMode` | `0x33590A` | `oracle-golden-frames.md`: 0 Adventure, 1 Quick Race, 8 attract. 2 = two-player (`menu.h` `MenuMode`, and the two-player table branch in `StartRaceModeTask`, `build/decomp/fn/0020f950.c:36` `(param_2[10] & 2)`). |
| `scene3dActive` | `0x3356C4` | 2 in race, attract and town (oracle). 0 on title/menu. |
| `scene3dHandler` | `0x3356D8` | `0x21CF80` town, `0x21CE48` race. |
| `driveFlags` | `0x335928` | `0x04` can drive, `0x20` racing, `0x08` finished, `0x10000` HUD hidden (`oracle-golden-frames.md`). `0x100` is set in race and attract but is unnamed. |
| `location` | `0x335923` | 1 = Peach Town. |
| `progressScene` | `progress.scene` | 2 town driving, 6 factory, 10 racing, 3 race over. 2/6/10 are oracle values. 3 is plan-sourced (`wave2-plan-sonnet.md` §W2-02) and is not sampled by any oracle golden. |

Flow states:

| `FlowState` | `FlowMenuPage` / `FlowSession` / `FlowOverlay` | Meaning |
|---|---|---|
| `BOOT` | — | Before the title. |
| `TITLE` | — | "PRESS START BUTTON". `ui_context=0x29AFA0`. |
| `ATTRACT` | — | Attract demo. `game_mode=8`, race handler, flags `0x10104`. |
| `MENU` | `MODE_SELECT` (Adventure / Quick Race / 2 Player / Options / Results), `ADVENTURE_SAVE`, `OPTIONS`, `RESULTS_VIEW` | Main menu and sub-pages. |
| `PLAYING` | `EXPLORE` (overlays `NONE`, `DIALOGUE`, `SHOP`, `FACTORY_MENU`, `FACTORY_EXIT_DIALOGUE`), `RACE`, `RESULTS` | In-game. |

## Transitions

Evidence tags: **[G]** oracle golden (`oracle-golden-frames.md`), **[D]** decomp (`build/decomp/fn/*.c`, line cited), **[M]** main.cpp or session.cpp behaviour, **[X]** design choice with no direct evidence (open).

| From | Event | To | Tag |
|---|---|---|---|
| BOOT | BootComplete | TITLE | [G] title frame reached after boot |
| TITLE | Start | MENU/MODE_SELECT, cursor 0 | [M] `titleAdvance` → `MENU` (main.cpp:2033) |
| TITLE | IdleTimeout (auto, 2700 ticks) | ATTRACT | [G] 45 s no input → `game_mode=8`. The trigger in code is **not traced** (see open questions). |
| ATTRACT | AnyKey | TITLE | [X] |
| MODE_SELECT | NavUp / NavDown | cursor ±1, clamped (rejected at the ends) | [X] wrap vs clamp not observed |
| MODE_SELECT | Confirm, cursor 0 (Adventure) | ADVENTURE_SAVE | [G] Adventure → Continue |
| MODE_SELECT | Confirm, cursor 1 (Quick Race) | PLAYING/RACE, `gameMode=1` | [G] `race_hud`; [D] `StartRaceModeTask` `0x20F950` byte+10 == 1 → quick-race table (`0020f950.c:32`) |
| MODE_SELECT | Confirm, cursor 2 (2 Player) | PLAYING/RACE, `gameMode=2` | [D] `0x20F950` two-player table branch. No oracle frame. |
| MODE_SELECT | Confirm, cursor 3 (Options) | OPTIONS | [G] Down×3 + Cross → Options |
| MODE_SELECT | Confirm, cursor 4 (Results) | RESULTS_VIEW | [X] no behaviour recorded |
| MODE_SELECT | Cancel | TITLE | [X] `main.cpp` shows CANCEL only for the TITLE screen |
| ADVENTURE_SAVE | Confirm | PLAYING/EXPLORE, FACTORY_MENU, `progressScene=6`, `location=1` | [G] `continue_to_factory` |
| ADVENTURE_SAVE / OPTIONS / RESULTS_VIEW | Cancel | MODE_SELECT (cursor kept) | [X] |
| FACTORY_MENU | NavUp / NavDown | cursor ±1, clamped | [G] Down×4 reaches item 4 |
| FACTORY_MENU (cursor 4) | Confirm | FACTORY_EXIT_DIALOGUE | [G] "Drive around town" |
| FACTORY_EXIT_DIALOGUE | DialogueDone | EXPLORE, `progressScene=2`, handler town, `driveFlags=4`, `location=1` | [G] `town_driving` |
| EXPLORE (no overlay) | StartRace | PLAYING/RACE, `gameMode=0`, `progressScene=10` | [D] `0x2105B8` case 4 → `gs_tasks_spawn(0x20F950)` (`002105b8.c:135-136`, sub-state 9) |
| EXPLORE (no overlay) | OpenDialogue | EXPLORE/DIALOGUE | [D] sub-state 8 spawns `dialogueTask 0x23B168` (`002105b8.c:372-373`) |
| EXPLORE (no overlay) | OpenShop | EXPLORE/SHOP | [M] `S` key (main.cpp:2014); [D] shop spawn `0x22AEB0` |
| EXPLORE (DIALOGUE / SHOP) | DialogueDone / CloseShop | EXPLORE (no overlay) | [M] |
| RACE | RaceFinished | RESULTS; `progressScene=3` only for Adventure; `racing` bit cleared, `finished` set | [X] flag effects; scene 3 plan-sourced |
| RESULTS | ResultsConfirm | EXPLORE, `progressScene=2` (town return) | [M] `continueFromResults` (`session.cpp:83-85`); the town return is [X] |
| any PLAYING | Escape | MENU/MODE_SELECT, cursor 0, `gameMode=0` | [M] `returnToMenu` (main.cpp:2009, 2047) |

Overlays are only opened from EXPLORE with no overlay active. StartRace is only accepted with no overlay (main.cpp: `!dialogue.isActive()`).

Anything not in the table is rejected. A rejected event leaves the words unchanged and returns `accepted=false`.

## Direct launches

`AppFlow::directLaunch` matches main.cpp's launch paths:

| `RT_APP_STATE` / `RT_APP_MODE` | `DirectLaunch` | Result |
|---|---|---|
| unset, or unknown value | `TITLE` | TITLE |
| `MENU` | `MENU_MODE_SELECT` | MENU/MODE_SELECT (`main.cpp:1836-1839`) |
| `PLAYING` (default) | `PLAYING_RACE` | Adventure race start (`main.cpp:1841-1843`) |
| `PLAYING`, `RT_APP_MODE=explore` | `PLAYING_EXPLORE` | Town EXPLORE (`main.cpp:1845-1848`) |

## Timing

`step(optional<FlowEvent>)` is one 60 Hz logic tick (`frame-timing.md`: fixed-step logic). It advances the title idle counter, applies at most one event, then auto-fires `IdleTimeout` at `kFlowTitleIdleTicks = 45 * 60 = 2700`. Events reset the idle counter. The title idle counter is the only timed transition.

## ELF cross-check

`w2_app_flow_test --elf` checks the bytes the transitions rely on (the test passes only if all three hold):

1. The 7-word handler selector at `0x21CE08..0x21CE20`: `beq a0,zero,+3; lui 0x22; b +3; addiu 0xCE48 (0x21CE48); lui 0x22; addiu 0xCF80 (0x21CF80); jr ra`. So `a0 != 0` selects the race handler and `a0 == 0` the town handler, which matches `SelectScene3dHandler` (`build/decomp/fn/0021ce08.c`).
2. `StartRaceModeTask` (`0x20F950..0x210154`) calls `SelectScene3dHandler` with `addiu a0,zero,1` in the delay slot (`0x20F9A0`/`0x20F9A4`).
3. `StartTownModeTask` (`0x210430..0x2105B8`) calls `SelectScene3dHandler` (`0x21046C`).

## Test results (2026-10-17, `build/w-W2-02`)

```
W2-02 SCORE golden_words=9/9 edges=62/62 illegal_accepted=0 replay_hash_equal=1 deterministic=1 reachable=26 word_effects=16/16 elf_dispatch=3/3 PASS
```

- **golden_words 9/9**: the nine frames in `oracle-golden-frames.md` replayed from BOOT with the documented scripts. Each check asserts only the words that frame documents (see the caveats in the report).
- **edges 62/62**: for each of 26 reachable contexts and each of 16 events (416 pairs), the machine's acceptance and resulting context match an independent spec table in the test. 62 pairs are legal and all match. 354 are rejected.
- **illegal_accepted 0**: no illegal pair was accepted, in the exhaustive pass or in two 5000-event seeded fuzz runs.
- **replay_hash_equal 1**: two fuzz replays from the same seed give the same FNV-1a word hash.
- **word_effects 16/16**: progress writes, flag bits, the exact 2700-tick title timeout (2699 stays TITLE), direct launches, and the `FlowView` snap rule.

## Open questions

1. **Attract trigger.** The 45 s idle is harness-observed (`test_attract_demo`). The decomp trigger was not traced. The model uses a title-only counter.
2. **Menu idle.** Whether MENU also times out to attract is unknown. Not modelled.
3. **Quick Race / 2P return.** `ResultsConfirm` returns to town, as `session.cpp` does. The oracle does not show what a Quick Race returns to.
4. **Race-over scene 3** is plan-sourced, not oracle-sampled.
5. **Adventure race `driveFlags`.** Modelled as `0x124` (the Quick Race value). Not sampled in Adventure.
6. **Factory words.** `scene3dActive`, `driveFlags` and `scene3dHandler` in the factory are derived, not recorded.
7. **`driveFlags` bit `0x100`** is unnamed.
8. **Options and Results pages** have no internal navigation. The Options page list is a separate menu model (`menu.h`).
9. **Cursor reset on Escape** goes to Adventure. main.cpp keeps `menuFlow`, which may retain the last cursor.
