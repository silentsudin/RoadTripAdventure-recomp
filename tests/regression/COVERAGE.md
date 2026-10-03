# Regression coverage

What the suite plays and checks, and what still has to be recorded. Sections are recorded with
`scripts/record_section.py` (see `sections/README.md`); each Adventure section ends with an in-game
save that the next one starts from.

## Covered

| Area | Test | Checks |
|---|---|---|
| Boot, publisher logo, title | `test_smoke::test_boot_to_title` | Title golden; logo jingle (exact sound); silent title |
| Quick Race (first course, default car) | `test_smoke::test_quick_race` | Start and mid-race goldens; 20 s of race sound (exact); the race moves |
| Adventure: New Game, name and currency, intro, first save | `test_smoke::test_adventure_new_game_save` | Save files written; name, currency, money in the save. Produces checkpoint `adventure_first_save` |
| Adventure: Continue from slot 1 | `test_smoke::test_adventure_continue` | Save summary and Q's Factory goldens; progress after loading |
| Peach Town: drive around town | `sections/010_peach_town_drive` | 3 goldens, section sound, scene and money |
| Real-time performance (Quick Race) | `test_perf` (`--perf`) | 60 fps median, VIF1/VU1 thread headroom |
| Every Adventure race (26 events in 9 towns, licence A) | `test_adventure_races` | The driving bot races each one to the finish; start golden and sound; the result is recorded in the progress block with the finishing place. Tin Raceway: "Under construction" golden |
| Every town's open world (9 towns) | `test_towns` | Q's Factory golden per town, "Drive around town": the town loads (scene), goldens before and after driving, town sound |
| Options: Vibration / Speaker / Sound Volume screens | `test_menus::test_options_screens` | Goldens of each screen |
| Options: Vibration TEST | `test_menus::test_vibration_test_button` | Game keeps presenting frames |
| Options: Sound Volume 0 (mute) | `test_menus::test_sound_volume_mute` | A Quick Race is silent |
| Options: Speaker Mono / Stereo | `test_menus::test_speaker_mono`, `test_speaker_stereo_differs` | SPU2 voice registers: mono drives every voice equally left and right; stereo pans some |
| Results screen (with a save and on an empty card) | `test_menus::test_results_*` | Goldens of the slot list, the saved record, "There is no data" |
| Attract demo | `test_menus::test_attract_demo` | Golden at 45 s; demo sound (exact) |
| Every Quick Race course (8, default car) | `test_quick_race` | Course card, start golden and sound, a full race with the bot, the card's records afterwards |
| 2 Player, Race Right-Away: all 10 events (5 courses and Highway, Tunnel, Sliding Door, Obstacle Course, Soccer) | `test_two_player::test_two_player_event` | Card, split-screen start, 20 s of play: golden, sound (exact), the picture moves |
| 2 Player, Random Race and Custom Race with saves on both cards | `test_two_player::test_two_player_saved_cars` | Card prompt, both saves loaded, carousel, card, start and 10 s of the race |
| Q's Factory: Change parts, each category | `test_factory` | Goldens of the fitted part per category |
| Memory card: overwrite a save, load it back | `test_memcard::test_overwrite_save_and_load_it` | The save holds the live state; it loads in a fresh boot |
| Memory card: save to card 2, quit, load from card 2 | `test_memcard::test_save_to_card_2` | "Quit for today" flow, title, Continue from slot 2 restores the progress |
| Memory card: damaged save | `test_memcard::test_damaged_save_loads` | The game keeps no checksum and loads it (golden) |

## To record (in play order)

Adventure mode:
- [ ] Peach Town: Q's Factory parts shop (buy a part, money decreases), the town's people and jobs
- [ ] Result screen and winnings after a race; the licence awards (C → B → A → Super A) in story order
- [ ] Each town's arrival, shops and jobs (the races and the open world are covered above)
- [ ] Tin Raceway once it opens
- [ ] World Grand Prix
- [ ] Ending and credits

Other modes:
- [ ] Quick Race: other cars, and courses that open later
- [ ] 2 Player: finishing an event (needs a second driver), Trade Items, Change Parts, Save
- [ ] Options: Vibration On/Off effect on the pad motors

Memory card edge cases:
- [ ] A full or unformatted card (needs runtime support to simulate)

## Game state to map

`config/game_state.toml` grows as sections reveal fields (diff the progress block at section
markers). Wanted: current town, mileage, licences, races won, parts owned/equipped, in-game time,
race position/lap/time (in RAM during races).
