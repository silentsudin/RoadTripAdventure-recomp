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

## To record (in play order)

Adventure mode:
- [ ] Peach Town: Q's Factory parts shop (buy a part, money decreases), the town's people and jobs
- [ ] First rank C race from Q's Factory, result screen, winnings
- [ ] Remaining rank C races and the B licence
- [ ] Each other town as it opens: arrival, shops, jobs, its Q's Factory (one section per town)
- [ ] Rank B races and the A licence
- [ ] Rank A races and the Super-A licence
- [ ] World Grand Prix
- [ ] Ending and credits

Other modes:
- [ ] Quick Race: every course (one section each), other cars, laps and results
- [ ] 2 Player (split screen): one race
- [ ] Options: each setting changed and its effect (sound, controls, display)
- [ ] Results screen after races
- [ ] Attract demo (title left alone)

Memory card edge cases:
- [ ] No save: Continue shows "There is no data" (scripted, like the smoke tests)
- [ ] Second slot; overwriting a save; a full or unformatted card; a corrupt save

## Game state to map

`config/game_state.toml` grows as sections reveal fields (diff the progress block at section
markers). Wanted: current town, mileage, licences, races won, parts owned/equipped, in-game time,
race position/lap/time (in RAM during races).
