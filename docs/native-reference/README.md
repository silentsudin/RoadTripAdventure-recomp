# native-reference — agent index for the PS2 program

Road Trip's PS2 ELF has been statically decompiled and the per-function one-liners live in
[`functions.tsv`](functions.tsv) (731 rows, tab-separated: `address`, `name`, `note`, `callers`,
`callees`). This README is the **index** — the cheapest path from a question like "what is at
`0x0021BA70`?" or "which function handles car collisions?" to an answer.

When the parallel writer is done, a richer narrative per function lives in `functions.md` (same
directory). Treat `functions.md` as the source of truth for behavior; this README just points at it.

---

## 0. Asset/data-format status docs (FLD, COURSE, VU, audio)

| Doc | Subject | State |
|---|---|---|
| [`field-topology-status.md`](field-topology-status.md) | FLD container grammar, 64/64 corpus, tile id/streaming (digit order **proven**), loader chain | current |
| [`world-geography.md`](world-geography.md) | fixed-unit scale **proven** (1 tile = `0x02000000`, divisor f32 @`0x332c0c`), 8×16 half-column torus + row shear, parity-claim correction, Peach Town worked example | current |
| [`asset-map.md`](asset-map.md) | All ROM descriptor tables, 384/384 exact LBA↔file matches, disc layout, orphan/id puzzles | current |
| [`cell-vif-chains.md`](cell-vif-chains.md) | Per-cell VIF chains: 60B vertex records incl. **streamed UVs**; attribute-table rejection | current |
| [`gif-material-streams.md`](gif-material-streams.md) | GIF A+D material header per cell, TEX0→page binding, 3,097 IMAGE transfers | current |
| [`field-textures.md`](field-textures.md) | `fieldTextureCatalog()` upload grammar, GS page bank rule, pixel conventions | current |
| [`field-texture-rendering.md`](field-texture-rendering.md) | decodeFieldTextures() payload-linear decode verdict (oracle-exact), CLUT/alpha pins, UV semantics, textured preview pipeline | current |
| [`vu0-macro-streams.md`](vu0-macro-streams.md) | VIF word grammar; proof there are **no VU0 macro streams** | current |
| [`vu1-programs.md`](vu1-programs.md) | Shared VU1 microprogram images + entry map (WorldStrip = entry 4) | current |
| [`course-data-status.md`](course-data-status.md) | COURSE container regions | current |
| [`course-routes.md`](course-routes.md) | COURSE tail grammar — prop-draw chains (bounded negative result), AI-nav table pointers | current |
| [`course-prop-chains.md`](course-prop-chains.md) | Full prop-draw record decode (N=MSCALF-0xA 18/18, GIFtag 0x264/{ST,RGBAQ,XYZF2}, tile-local floats, header boundary list, C16 zero tail) + parser contract | current |
| [`nav-tables.md`](nav-tables.md) | ROM-resident AI route graphs: static directory `0x2bd578`, 24B f32 segment + 8B transition grammar, runtime extraction spec | current |
| [`nav-route-traversal.md`](nav-route-traversal.md) | FUN_00250068 follower predicate + host deterministic centerline rule (nav_path) | current |
| [`nav-route-consumer.md`](nav-route-consumer.md) | NavPath → authored RaceRoute zone bridge, adaptive width clamps, pinned Peach Town prefix | current |
| [`nav-walkable.md`](nav-walkable.md) | Waypoint arc-length traversal — full-coverage proof (Peach Town 34/34 nodes, lap 1652.558 units) + corpus closure table | current |
| [`nav-drive.md`](nav-drive.md) | RT_APP_ROUTE=1 — deterministic kinematic follower driving the demo car along graph 0 (lap counter, render-local override) | current |
| [`nav-index-binding.md`](nav-index-binding.md) | static nav index = pack-type byte (+0x22); outer-T semantics; graph-pair overlap matrix | current |
| [`action-data-status.md`](action-data-status.md) | ACTION container grammar, 21/21 descriptor join, header variants | current |
| [`action-mscalf6.md`](action-mscalf6.md) | MSCALF imm semantics (6 = ObjectStrip origin+5q records, 8 = WorldStrip), `NUM=1+5n` payload grammar, per-file usage table | current |
| [`sys-gsl-packs.md`](sys-gsl-packs.md) | GSL DMA-chain container grammar (1142 upload units / 49 files verified), A+D `{0x50..0x53}` unit layout, loader path, FONTE glyph-atlas decode | current |
| [`save-layout.md`](save-layout.md) | PS2 Adventure save record `0x3448` field map (money `0x654`, stamps, licence, radio, remaps), card paths, checksum verdict, migration rules | current |
| [`task-system.md`](task-system.md) | async task spine: 48-slot scheduler, tag/category taxonomy, 92 spawn sites / 34 producers, completion + pointer relativization | current |
| [`audio-map.md`](audio-map.md) | SOUND inventory (30 files), music id grammar `0x0A II PP 01` + file table @`0x29ADF0` (29/29 LBN-verified), race-BGM table, radio = VAG streams verdict, SFX path | current |
| [`car-data-status.md`](car-data-status.md) | CAR pack: 154/154 table-E join, 12-word header grammar, self-textured cars (constant 0x4510 chain, per-car liveries), DBP rebase, cat-0xB/0x7 consumer trace | current |
| [`tsq-tvb-format.md`](tsq-tvb-format.md) / [`sndmod-irx.md`](sndmod-irx.md) | Music stack | done |

Code: `src/native/assets/field_converter.{h,cpp}` (packets, textures, terrain),
`field_tile_index.{h,cpp}` (runtime streaming-id → filename resolver), tests under
`tests/native_assets/`.

---

## 1. Lookup protocol (the token-saving path)

Always do these in order. Stop as soon as you have enough.

1. **Search by address or name** in `functions.tsv`. The file is 5 columns:
   `address<TAB>name<TAB>note<TAB>callers<TAB>callees`.
   ```bash
   # by exact address (case-insensitive, with or without the 0x prefix)
   grep -i "^0x0021ba70" docs/native-reference/functions.tsv
   grep -i "21BA70" docs/native-reference/functions.tsv

   # by function name
   grep -F "UpdateVehiclePhysics" docs/native-reference/functions.tsv

   # substring (e.g. "CarAi", "Physics")
   grep -i "CarAi\|Physics" docs/native-reference/functions.tsv

   # follow the call graph either way
   grep -F "0x0021ba70" docs/native-reference/functions.tsv
   ```
   One row gives you: address, name, one-line behavior, callers, callees. For most questions that
   is enough.

2. **Treat the row as unresolved if any of these are true**:
   - `name` starts with `UNKNOWN_` (e.g. `UNKNOWN_0x0023BB88`).
   - `note` ends with `[inferred]` (the row is a best-effort guess; the underlying decompile
     wasn't named).
   - `name` matches `FUN_00212345` / `FUN_002123b8` / `LAB_002xxxxx` — Ghidra's auto labels.
   - The address is missing (no row at all). Then the function is **not yet documented** — see §4.

3. **For full behavior, read `functions.md`** (same directory). It is the parallel writer's
   companion file: longer prose per function, with the same address→name ordering. Use the row
   from `functions.tsv` as the table-of-contents entry.

4. **Ghidra last, not first.** If `functions.tsv` has no row and `functions.md` has no section,
   open the function in Ghidra. **Do not** open Ghidra to re-derive something already documented
   in those two files — that is what this directory exists to prevent.

### One-line cheat sheet

```bash
# "what is at 0x0021BA70?"
grep -i "21BA70" docs/native-reference/functions.tsv

# "who calls SpawnCarEntityTask?"
grep -F "0x00218458" docs/native-reference/functions.tsv   # column 4 = callers
grep -F "SpawnCarEntityTask" docs/native-reference/functions.tsv

# "all car physics in one hit"
grep -iE "Car|Phys|Steer" docs/native-reference/functions.tsv

# list every UNKNOWN row still open
grep -E "UNKNOWN_" docs/native-reference/functions.tsv

# addresses between two values (e.g. all car stuff under 0x0021C000)
awk -F'\t' '$1 < "0x0021c000" && $1 > "0x00218000" {print}' docs/native-reference/functions.tsv
```

---

## 2. Address → name quick index (high-value functions)

These are the functions the native rewrite will actually implement. The columns: address, name,
one-line. The "note" column in `functions.tsv` has the full sentence.

| Address | Name | What it is |
|---|---|---|
| `0x002001B8` | `Ps2InitSystem` | Top-of-ELF boot: IOP reboot, SIF/CD/DMA reset, loads 7 IOP modules, inits MC+PAD, calls into main. |
| `0x00204A00` | `RunRootTask` (task pool runner; see ts) | Task pool tick — dispatcher for all per-frame task callbacks. |
| `0x00204C10` | `ClearTaskPool` | Clears 48 fixed task slots on boot. |
| `0x00204D20` | `InitializeTask` | Initialises one task record. |
| `0x00205018` | `DestroyTaskTree` | Recursively destroys a task and its children. |
| `0x00205088` | `UpdateMenuFlowTask` | Top-level menu flow task. |
| `0x002052D0` | `StartMenuFlow` | Spawns initial menu tasks. |
| `0x0020F950` | `StartRaceModeTask` | Race mode entry: spawns HUD, rivals, environment. |
| `0x00210430` | `StartTownModeTask` | Town mode entry: spawns viewport, block manager, player car. |
| `0x002105B8` | `UpdateTownExplorationTask` | Main per-frame town exploration loop. |
| `0x00211808` | `StartTownExplorationTask` | Town boot: spawns residents, sets town callback. |
| `0x002175D0` | `TestCarCollisionBox` | Narrowphase: 4 corner tests in another car's frame. |
| `0x00217830` | `HandleCarCollisionResponse` | Collision rumble, SFX, recoil. |
| `0x002179D8` | `UpdateCarCarCollisionsTask` | Car↔car broadphase+narrowphase per frame. |
| `0x002180C0` | `LoadCarPerformanceSpecs` | Loads parts (gears, engine, tires, brakes) into car physics. |
| `0x002182B0` | `SpawnCarAtDoor` | Initial spawn pose at a building entrance. |
| `0x00218458` | `SpawnCarEntityTask` | Spawns a car entity with update callback. |
| `0x00218EE0` | `AdvanceVehicleMotion` | Odometer + arc-distance advance by speed. |
| `0x002194B8` | `SteerHeadingVector` | Rotates heading by yaw, shrinks lateral axes (steering model). |
| `0x00219660` | `ApplyCollisionImpulse` | Collision code 0–15 → velocity/yaw impulses. |
| `0x00219ED8` | `QueryCarSurfaces` | Surface query for car in world frame. |
| `0x0021A088` | `UpdateSteeringDamping` | Rate-limits steering cmd, integrates yaw offset. |
| `0x0021A310` | (car motion update — see ts) | Per-frame car motion helper. |
| `0x0021A990` | `AcquireVehicleInput` | Routes player input or AI fnptr; input ring buffer. |
| `0x0021ACD0` | `UpdateSuspensionFlex` | Suspension floats + steering lean per mode. |
| `0x0021AF28` | (suspension solver — see ts) | Per-wheel suspension integration. |
| `0x0021B3D0` | `ResolveGroundContact` | 7-wheel raycast, runs suspension, sets airborne. |
| `0x0021BA70` | `UpdateVehiclePhysics` | **Master vehicle tick**: input, ground, surface, collision substeps, drag, commit. |
| `0x0021CE28` | `SetAICarController` | Installs AI fnptr used by `AcquireVehicleInput`. |
| `0x0021D0B8` | `DefaultAiController` | Default AI car-control function (the one `SetAICarController` installs when given NULL). |
| `0x0021D2D8` | `TransformVecByF15Matrix` | VU0: transform f15 vector by 4×4 f15 matrix rows 0–2. |
| `0x0021D358` | `GetSpeedFromF15Vector` | f15 vec → float length × 216.0 (display speed). |
| `0x0021DC18` | `ComputeChaseCameraBasis` | Smoothed chase-cam offset, outputs 2 basis vectors. |
| `0x0021E070` | `ResolveCameraTerrainCollision` | Camera pitch back-off when clipping terrain. |
| `0x0021E690` | `RunCarCameraTask` | Per-frame car camera task (chase cam, transitions). |
| `0x0021F198` | `ApplyFreeCameraInput` | Free-cam by input bits. |
| `0x0021F698` | `buildCamera` (see ts) | Final camera matrix build. |
| `0x00250068` | `UpdateVehicleWaypointProgress` | AI: tracks current waypoint + lateral error. |
| `0x002508E0` | `CalculateAiSteeringAndThrottle` | AI: steering angle + throttle from waypoint heading. |
| `0x00253230` | `SetAiSteeringTarget` | AI: computes steering input flags from angle delta. |
| `0x00253448` | `SteerAiToDefaultWaypoint` | AI: steer to default path target. |
| `0x00253680` | `UpdateAiCourseSteering` | AI: navigation across course waypoints. |
| `0x0024FCD8` | `CarAiInit` | Per-car AI state init, binds waypoint path tables. |
| `0x00240CD8` | `UpdateTrafficHazard` | Traffic hazard region trigger. |
| `0x002406F8` | `UpdateProximityEvent` | Proximity-triggered multi-state world event. |
| `0x00241BA8` | `UpdateMapCoinCollection` | Map coin collection. |
| `0x00241F68` | `UpdateNearbyCarEffect` | Proximity-driven visual/audio on nearby car. |
| `0x00206F28` | `IntersectPointWithMesh` | Mesh triangle intersect (nearest). |
| `0x00207280` | `FindWorldMeshIntersection` | World mesh intersection with sector adjust. |
| `0x00221300` | `renderInit` (see ts) | One-time render init. |
| `0x00221788` | `drawCar` (see ts) | Per-frame car draw path. |
| `0x002230C8` | `UploadPaintBgColors` | Background palette upload. |
| `0x00223200` | `ResetShowroomLighting` | Showroom 16-light At16 env upload. |
| `0x002236F8` | `DrawCarShowroomRig` | Curated showroom pose. |
| `0x00225790` | `UpdateSceneLightsAndFog` | Fog/sky + light matrix upload from tod table. |
| `0x00225...` | (car lighting — see ts) | `ScaleCarLightMatrix` etc. |
| `0x00224068` | `SpawnCarDustPuffs` | Per-frame dust emitters from wheels. |
| `0x002243E0` | `SpawnCarSmoke` | Smoke emitter (heat-scaled). |
| `0x002246C0` | `SpawnCarSparks` | Spark emitter on proximity. |
| `0x002249B8` | `SpawnCarDebrisCycle` | Rotational debris emitter. |
| `0x00225390` | `AdvanceCarEffectSlot` | Effect slot frame step. |
| `0x0023D8C8` | `GetOrSetPresidentRaceResult` | Per-mode race result store (used by race+minigame tasks). |
| `0x0024E1A0` | `UpdateCarControlState` | Per-car update via `gs_controls_car_object`. |
| `0x002490A0` | `UpdateRaceHud` | HUD render dispatch per player. |
| `0x00249458` | `CalculateSpeedometerValue` | Speedometer value, clamped 186. |
| `0x002495E8` | `GetVehicleRaceTime` | Returns vehicle struct offset 0x14. |
| `0x002495F0` | `FormatRaceTimeBCD` | Race timer → MM:SS packed digits. |
| `0x00256AF8` | `InitSoundRpcDriver` | Boot-time sound RPC init. |
| `0x00257F30` | `MusicRpcPlayBgm` | BGM play (cmd 0xe). |
| `0x00258E50` | (sound-effect play wrapper — see ts) | Shared SE-play entry used by 60+ call sites. |
| `0x00258F30` | `MusicRpcPlayBgm`-family wrappers | BGM slot wrappers (one per slot). |
| `0x00258578` | `SoundInitVolumes` | Boot: stop/reset sound + set channel volumes. |

> When the row's `name` looks like `FUN_002xxxxx` or `LAB_002xxxxx`, the function has no human
> name yet — read the `note` for the inferred role and treat the address as the canonical handle.
> The high-value ones are all named.

---

## 3. Module map (subsystem → address range)

**All ranges marked **[inferred]** are derived from clustering in `functions.tsv` `note` column.
They are starting points for `grep`, not hard boundaries.** Treat them as a "where to look"
hint; a function may sit just outside the range because it shares callers.

| Subsystem | Address range (inferred) | Anchor addresses |
|---|---|---|
| ELF boot / IOP / SIF / CD / DMA | `0x00200000` – `0x002003C0` | `Ps2InitSystem 0x002001B8` |
| Text + 2D draw primitives (font, sprites, dialog) | `0x002003C0` – `0x00204800` | `DrawDialogTextLine 0x00202D10`, `GsDrawDigitsCounter 0x00202020` |
| **Task pool / scheduler** | `0x00204A00` – `0x002052C0` | `ClearTaskPool 0x00204C10`, `DestroyTaskTree 0x00205018` |
| **Menu flow + pause menu + sub-menus** | `0x002052C0` – `0x00210500` | `StartMenuFlow 0x002052D0`, `UpdatePauseMenuTask 0x00214AD0`, `UpdateWorldMapMenuTask 0x00216E00` |
| **World mesh / streaming / collision geometry** | `0x00206F00` – `0x00208600` | `IntersectPointWithMesh 0x00206F28`, `FindWorldMeshIntersection 0x00207280` |
| VU0 vector helpers (reflect, transforms, quat) | `0x00207E00` – `0x00208800`, also `0x00252500` | `TransformRectangle 0x00207EC8`, `BuildTransformMatrix 0x00207F88`, `NormalizeQuatAndRotate 0x00252540` |
| Input / controller mapping | `0x0020A800` – `0x0020E000` | `ReadPadState 0x0020A8A8`, `UpdateControllerInput 0x0020AB10`, `HandleInputEvent 0x0020D008` |
| **Race / town / garage mode dispatch** | `0x0020E700` – `0x00211880` | `UpdateTownBlockTasks 0x0020E708`, `StartRaceModeTask 0x0020F950`, `StartTownModeTask 0x00210430`, `UpdateTownExplorationTask 0x002105B8` |
| **Car physics + AI** | `0x002175D0` – `0x0021D550` | `UpdateCarCarCollisionsTask 0x002179D8`, `LoadCarPerformanceSpecs 0x002180C0`, `SteerHeadingVector 0x002194B8`, `ApplyCollisionImpulse 0x00219660`, `QueryCarSurfaces 0x00219ED8`, `UpdateSteeringDamping 0x0021A088`, `AcquireVehicleInput 0x0021A990`, `UpdateSuspensionFlex 0x0021ACD0`, `ResolveGroundContact 0x0021B3D0`, **`UpdateVehiclePhysics 0x0021BA70`** |
| Fixed-point world / position helpers | `0x0021C600` – `0x0021CE00` | `DecomposeWorldPosition 0x0021C6A0`, `ToRelativeVec4 0x0021C728`, `FixedPosToFloatVec4 0x0021C7F0`, `SaveControlStateForReplay 0x0021CBC8` |
| **Camera** (car + scene + menu) | `0x0021DC00` – `0x0021FB80` | `ComputeChaseCameraBasis 0x0021DC18`, `RunCarCameraTask 0x0021E690`, `SelectAndBuildCamera 0x00238B18` |
| Render setup / car lighting / showroom | `0x00221300` – `0x00226B00` | `renderInit 0x00221300`, `ResetShowroomLighting 0x00223200`, `UpdateSceneLightsAndFog 0x00225790`, `ScaleCarLightMatrix 0x00226058` |
| Car effects (dust, smoke, sparks, debris) | `0x00223D00` – `0x00225B00` | `UpdateCarEffectSlots 0x00223D58`, `SpawnCarDustPuffs 0x00224068`, `SpawnCarSmoke 0x002243E0`, `SpawnCarSparks 0x002246C0` |
| Save / memcard / progress | `0x00227380` – `0x00229FE0` | `MemcardChooserDialogHandler 0x00227380`, `InitSaveSlotDefaults 0x002277B0`, `LoadSaveSlotTask 0x00228B10`, `CommitSaveSlotTask 0x00229758`, `InitProgressRamDefaults 0x00229FD8` |
| Scene / course / event flow | `0x0022A050` – `0x0022D700` | `RunSceneIntroTask 0x0022BFD0`, `RenderShowcaseView 0x0022C280`, `RunCourseSelectHubTask 0x0022CD38`, `RankCarsByProgress 0x0022D7B8` |
| **HUD / score / results** | `0x0022E700` – `0x00238B00` | `DrawScoreAndRemaining 0x0022E7F0`, `DrawRaceResultsScreen 0x00235B58`, `UpdateRaceRecords 0x00237C48` |
| Dialogs / messages / residents | `0x00238950` – `0x0023BC00` | `DrawRaceSummaryRow 0x00238A48`, `DisplayTextDialog 0x00239150`, `ShowSequencedMessages 0x00239748` |
| Progress table / unlock / record helpers | `0x0023BC00` – `0x0023E100` | `CheckUnlockCondition 0x0023CD58`, `GetOrSetPresidentRaceResult 0x0023D8C8`, `MatchSlotPairState 0x0023DB80`, `SpendTimerBudget 0x0023DC58`, `TestProgressFlagForPair 0x0023DE50` |
| World entities / proximity / map coins / props | `0x0023E100` – `0x00242A00` | `UpdateRotatingEffect 0x0023F0A0`, `UpdateProximityEvent 0x002406F8`, `UpdateTrafficHazard 0x00240CD8`, `UpdateMapCoinCollection 0x00241BA8`, `UpdateNearbyCarEffect 0x00241F68` |
| **Carousel / minigame results UI** | `0x00242A00` – `0x00248C00` | `SetupCarouselSlotTransforms 0x00243B38`, `UpdateRaceResultsMenu 0x00248180`, `UpdateCarouselPlayerSelect 0x00248580` |
| **AI / waypoint / pathfinding** | `0x002490A0` – `0x00253700` | `UpdateRaceHud 0x002490A0`, `UpdateVehicleWaypointProgress 0x00250068`, `CalculateAiSteeringAndThrottle 0x002508E0`, `SetAiSteeringTarget 0x00253230`, `UpdateAiCourseSteering 0x00253680` |
| Mini-games (golf, paint, stamp, balloon, slot) | `0x00250F00` – `0x00262A00` | `UpdateGolfMiniGame 0x00251B60`, `TuningGarageSceneTask 0x00254430`, `CarShopVisitSceneTask 0x00254D20`, `PaintEditorEditTask 0x00256640`, name-entry block `0x0025B5A0–0x0025BE30`, stamp/balloon/goal tasks `0x0025DBB8–0x0025FF00`, slot-machine task `0x00260DC0` |
| Boot sequence / progress init / top-level menu → mode dispatch | `0x00264200` – `0x0026E000` | see `functions.tsv` rows ending in `6BE50/6CA48/6D108/6D438/6D550/6D628/6D678` — these are the boot→town dispatch chain |
| Text-API + draw-debug prints | `0x0026D700` – `0x0026E000` | `DrawRaceHudNumbers 0x0026D700`, `PrintDoorParams 0x0026D880`, …, `PrintVec3Norm 0x0026DFB0` |
| **Sound RPC client** | `0x00256AF8` – `0x00258C00` | `InitSoundRpcDriver 0x00256AF8`, `MusicRpcPlayBgm 0x00257F30`, `SoundRpcStopAll 0x002585B0`, BGM-slot wrappers `0x00258E50–0x002590B0` |
| PS2 syscall thunks + low-level libraries | `0x002000B8` (start) and onward, plus helpers at `0x00270000+` referenced by callees (sceCd*, sceMc*, VU helpers at `0x002727c0+`) | anchor: `Ps2ExitThreadId 0x002000B8`; VU0 helper cluster starts near `0x002727C0` |

### Quick range greps

```bash
# all of car physics
awk -F'\t' '$1 > "0x002175D0" && $1 < "0x0021D600"' docs/native-reference/functions.tsv

# all task/scheduler
awk -F'\t' '$1 > "0x00204A00" && $1 < "0x00205300"' docs/native-reference/functions.tsv

# all sound
grep -iE "sound|rpc|bgm|se " docs/native-reference/functions.tsv
```

---

## 4. Do not re-decompile

`functions.tsv` (this directory) + `functions.md` (this directory, when present) are the
**canonical reference** for Road Trip's PS2 ELF program. The two files together cover 731
functions across the address range `0x00200000`–`0x0026E000`.

**Before opening Ghidra, check `functions.tsv` first.** If a function has a row — even one
marked `[inferred]` or with an `UNKNOWN_` name — that row is the answer. Open Ghidra only when:

- the address is **not** in `functions.tsv` at all, and
- it is **not** discussed in `functions.md`.

In other words: Ghidra is for **functions not yet in these files**, not for re-deriving anything
that already has a row. Re-decompiling covered functions is wasted token burn — and risks
drifting from the shared reference that other sessions (and the parallel writer) depend on.

If you discover a row in `functions.tsv` is wrong or stale, **edit `functions.tsv` and
`functions.md` together** so this README and the rest of the codebase keep pointing at the
truth. Do not silently patch around them in local notes.