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
| World Grand Prix: all 7 stages | `test_world_grand_prix` | From saves edited to each stage (Super A licence, two teammates, earlier stages done and won): factory, briefing, start golden and sound, the bot races the stage, the game marks it done (stages 1-6); the factory after each stage |
| The race against President Forest and the ending | `test_president` | From a WGP-won save in Cloud Hill: the secretary and Forest's challenge, the 1-on-1 race on Endurance Run (bot), Forest's concession, the mansion, the credits, "Thank You for Playing", the "Became the President!" stamp (stamp 100 earned) and the president's body; back in town |
| Notebook stamps: "Visited all the houses in ..." | `test_stamps::test_visited_all_houses` | Every door the town counts, one by one: the visit is recorded (the town's unvisited-doors bit clears), then the town's stamp. **Shortcut:** buildings are entered by warping their door onto the car (traffic and other doors parked); the visits run for real. A resident who is out turns you away (nothing is recorded). Passing: Peach Town, Fuji City, Sandpolis, Chestnut Canyon, Mushroom Road, Cloud Hill. Every door but the stamp expected to fail: Papaya Island (door 10, Shirley, drives around White Mountain at this checkpoint), White Mountain (door 17, Bigfoot Joe, never in; reason unknown, to check against PCSX2). Expected failure: My City (its houses come with the story) |
| Notebook stamps given by talking to a resident | `test_stamps::test_talk_stamp` | Visit, accept the first offer, stamp earned: 2 (Kinsera), 11 (Princess Nanaha), 17 (Otomi), 18 (Iwasuke), 59 (Gene's greeting, a text entry), 73 (Luke), 83 (Casa). Found by surveying every door of every town with accept-first answers; the other stamps need tasks, races or items first |
| Notebook stamps from mini-games | `test_stamps::test_stamp_29_played_roulette`, `test_stamps::test_minigame_stamp` | Played through for real: Roulette (29: bet, drive the car-ball into a pocket, leave the table), Curling (69: three slides). Not yet: Soccer (needs a full team, story), and the course mini-games (Rock Climbing, Volcano, Figure 8, Obstacle Course, Ski Jumping, the King's Sliding Door Race, Travis's races, Barrel Dodging), which need a bot that follows the course |
| Notebook stamp 86 (Dust, Cloud Hill) | `test_stamps::test_stamp_86_angels_wings` | Chase Dust's car (doors parked so the chase stays out of the shops), talk, stamp earned |
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
- [ ] Ending, without shortcuts. The test reaches it with two edits: an NPC's script entry
  points at the Secretary (instead of driving to the mansion gate), and the race result is set to
  "won" (the bot laps in about 1:30 and Forest is faster). Also: what (beyond stage results)
  makes a played-through WGP end in the win.
- [ ] Quick Race: other cars, and courses that open later
- [ ] 2 Player: finishing an event (needs a second driver), Trade Items, Change Parts, Save
- [ ] Options: Vibration On/Off effect on the pad motors

Memory card edge cases:
- [ ] A full or unformatted card (needs runtime support to simulate)

## Shortcuts in the town tests

These keep the town tests deterministic; each skips something a player would do:

- **Door warping** (`TownDriver.warp_into`): a building's door is moved onto the car, so the drive
  there is skipped; the building itself (people, scripts, items, stamps) runs for real. Driving to
  doors is covered separately by `enter_door` tests on doors the bot reaches reliably.
- **Quiet streets** (`park_traffic`, `park_doors`): street NPC cars and the other doors are moved
  off the map so they cannot interrupt a test.
- **Carried progress**: towns whose buildings send you elsewhere (Fuji City's maze guard puts you in
  the Treasure Hunting Maze) are visited in several games; the visits made in earlier games are
  carried by editing the town's unvisited-doors mask, as a save would. Every visit is real, but a
  town split this way is not one continuous session, so a visit that broke a later one only in the
  same session would go unnoticed. Towns where no building strands the car still run in one game
  (a new game starts only after a stranding or a failed visit).
- **Residents' hours**: Santa Claus (White Mountain door 10) is in at some hours only; his door,
  and only his, is retried each game hour for up to a day. Doors whose resident is never in at
  this checkpoint (Shirley, Bigfoot Joe) are left out, and their towns' stamps are expected
  failures; every other door of those towns is still required.
- **Stamps a warp can award without the task**: entering some buildings by warp awards a stamp for
  *reaching* them (Grandpa Tal's house gives "Cleared Barrel Dodging!", the maze guard gives
  "Found the location of Treasure Hunting Maze!", Mason at the top gives "Completed Rock
  Climbing!"). Those stamps are only counted as covered by a test
  that does the real task; the house-visit tests do not assert them.

## Notebook stamps (in progress)

The town bot (`rtharness/town.py`) drives any town from its map on the disc: ground heights
from the map's collision triangles, A* paths around cliffs, water and traffic, walls learned
when it gets stuck, chases of NPC cars (by predicting where they will be), and the dialogue
read from RAM (speaker, text). Stamps are bits in the progress block (`[stamps]` in
`config/game_state.toml`).

What awards each stamp (from the game's code and scripts):
- 92 stamps are awarded by NPC scripts (script opcode 0x0D lists the stamps) or by the race,
  mini-game and story code (direct calls to the award function, or the Figure 8 table).
- The "Visited all the houses in ..." stamps: leaving a building clears its bit in the town's
  unvisited-doors mask (0x23C7B0) and, once the mask is empty, awards the town's stamp (table at
  0x2A2B20). "My City is now complete!" still to trace.
- Who is behind each door: per location, 16-byte entries at the pointers in 0x2C22C8 (the fourth
  word names the resident); after the doors come the town's drivers about town. Characters can
  live in one town and drive in another (Shirley: Papaya Island house, White Mountain driver).

Next: per town, the residents, deliveries and mini-games behind its stamps.

## Game state to map

`config/game_state.toml` grows as sections reveal fields (diff the progress block at section
markers). Wanted: current town, mileage, licences, races won, parts owned/equipped, in-game time,
race position/lap/time (in RAM during races).
