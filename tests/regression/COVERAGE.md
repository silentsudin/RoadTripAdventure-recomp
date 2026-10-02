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

## To record (in play order)

Adventure mode:
- [ ] Peach Town: Q's Factory parts shop (buy a part, money decreases), the town's people and jobs
- [ ] Result screen and winnings after a race; the licence awards (C → B → A → Super A) in story order
- [ ] Each town's arrival, shops and jobs (the races and the open world are covered above)
- [ ] Tin Raceway once it opens
- [ ] World Grand Prix
- [ ] Ending and credits

Other modes:
- [ ] Quick Race: every course (one section each), other cars, laps and results
- [ ] 2 Player (split screen): one race
- [ ] Options: Vibration On/Off effect on the pad motors

Memory card edge cases:
- [ ] No save: Continue shows "There is no data" (scripted, like the smoke tests)
- [ ] Second slot; overwriting a save; a full or unformatted card; a corrupt save

## Game state to map

`config/game_state.toml` grows as sections reveal fields (diff the progress block at section
markers). Wanted: current town, mileage, licences, races won, parts owned/equipped, in-game time,
race position/lap/time (in RAM during races).
