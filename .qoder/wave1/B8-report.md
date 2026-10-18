# B8 — Minimap blob grounding (house markers, grip badge, dash/heading marker)

Worker: B8 · Date: 2026-10-17 · Repo: RoadTripAdventure-decomp

Files written: `docs/native-reference/minimap-blobs.md` (constants table, GAP list,
embedded reproduce snippets), this report. No git state changes. `src/`, `third_party/`,
`build/native/`, `.qoder/plan-journal.md`, `.qoder/wave-board.md` untouched. No decoded
or extracted images committed; all numbers come from embedded PIL/struct snippets (the
three snippets in the doc were executed and reproduce the numbers below).

## Result

| item | measured (scan px, 1280x447 oracle) | UI (640x448 model, B) | ELF / texture source | status |
|---|---|---|---|---|
| Screen fill | x 90..319, y 311..414 | x 45..160, y 312..416 | untextured; gradient (110,102,122) modal | measured; scissor SCAX/SCAY match exactly |
| Grip badge (left, bright lamp) | x 70..79, y 353..371 (rim to 85, 374) | x 35..40, y 353.8..372.8 | fill (0,104,8); palette candidate `SYS/POSE.GSL` unit 5 (CT16 @DBP 0x146) | measured; source PROVISIONAL |
| Grip badge (right) | x 326..335 (mirror, axis scan x 205.5) | x 163..168 | same | measured |
| Unlit lamps (top/bottom each side) | x 72..83, y 342..347 / 381..385 | x 36..42 | (8,32,8) | measured |
| Dash/heading marker (red) | town x 200..213 y 361..370; street x 194..209 y 362..370 | centroid (102.4,367.2) town, (101.4,367.0) street | colour set 0x2A4370 = (1,0,0,1) CONFIRMED; geometry 0x290870 = 30x40 isoceles triangle (apex +20, base ±15 at -20) | geometry + colour grounded; heading ±5° |
| Heading vs roads | apex 12.3 px from base midpoint; town +11.8° (y-down), street −33° | - | triangle is screen-fixed; roads and markers scroll ~(−8,+9) between frames | measured |
| Teal second arrow | town partly under red; street 8 px below red | - | colour set 0x2A4380 = (0,0.7,0.7,1), observed (11,115,128) ≠ nominal | GAP |
| Rival dot (green, α0.5) | 3x2..6x3 px on roads, on-road (22..33,127..150,58..70) | ~3x2 | ELF 0x2A4390 = (0,1,0,0.5) CONFIRMED; matches 0.5 blend over road (45,45,116) | grounded |
| "House" candidates A: flat footprints | 6–210 px clusters, colour (78,65,51), 9–10/frame | e.g. x 76..90 y 340.8..352.8 | flat fill, no texture | best house candidate; not confirmed |
| "House" candidates B: dark markers | 5/frame, ~8x3 px, (18,17,20) | ~4x2 | flat fill; shape reads as a car silhouette; symbol-map "town cars" | identity GAP |
| Pill bezel | not measured | x 30..174, y 304..424 | `hud-layout.md` §1.3, 0x249208 atlases (0,192)/(216,0)/(208,0) | CONFIRMED in docs; GSL file unresolved |

## Convention conflict (important for the native placement)

The brief's mapping (`UI_y = y/0.93125`, placement `− 16`) does not agree with the repo's
own geometry. The scissor half-lines (SCAY 156..207 → 312..415) and the bezel centre
(304+120/2 = 364) both match the 448 model `UI_y = y·448/447` to within 1 px. Under the
brief's mapping the screen centre is at placement ~374 vs bezel centre 364. The doc
gives both columns. I recommend the native uses the 448 model unless the user can point
to a source for `/0.93125` and `−16`.

## GAP

1. House vs. car identity for the dark (18,17,20) markers and the flat (78,65,51)
   footprints; neither tied to ELF data.
2. Bezel GSL file: `0x2A8FE0` is not the pointer table it was assumed to be.
3. Teal second arrow: colour and owner unexplained; it moves relative to the red arrow.
4. Screen fill gradient and possible translucency over the scene; no ELF source.
5. Active town zoom id (`mapZoomTable` entry) not pinned, so the world→pixel scale is not derived.
6. Lamp texture: POSE palette match is a candidate only; no unswizzle done, so the texture
   index is not decoded and the file is a notebook pack, which is suspicious.

## Method notes

- Connected components per colour class in the minimap region, bboxes in scan px,
  mapped to UI with both conventions. Roads/footprints shift between frames; chrome,
  lamps and arrow do not.
- Palette search: walked every SYS and ITEM GSL with the `sys-gsl-packs.md` unit grammar,
  matched the three lamp colours within ±6 per palette.
- ELF reads at `vaddr − 0x1FF000` (ELF delta from `sys-gsl-packs.md`).
- Task-brief note: the two oracle frames are a Peach Town parked view and a Paint Shop
  street view; the arrow and map content differ between them, the chrome does not.
