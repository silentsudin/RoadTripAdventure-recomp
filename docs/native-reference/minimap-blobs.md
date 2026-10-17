# Town minimap blobs — measured constants and texture grounding (B8)

Updated 2026-10-17. Scope: the chrome-pill town minimap in
`build/scratch/oracle-golden/town_driving.png` (player parked behind the car) and
`town_driving_street.png` (same HUD, later frame). Oracle scanout is 1280x447, so
x is 2 px per UI px. Numbers are scan pixels (inclusive) unless marked UI. No
game pixels or decoded images are committed; every number reproduces from the
embedded snippet in section 6.

Two UI conventions appear in the repo and they disagree on y (see section 5):

- **B (448 model)**: `UI_x = sx/2`, `UI_y = sy * 448/447`. This is the
  640x448 frame of `hud-layout.md` §1.3 and the SCISSOR_2 half-lines
  (`SCAY 156..207` -> 312..415). It is the convention the measurements agree with.
- **A (task brief)**: `UI_y = sy/0.93125`, placement `= UI_y - 16`.

## 1. Constants table

All rects are UI (B) unless marked. Scan rects are the source of truth.

| id | what | scan px (incl) | UI rect B: x0..x1 (excl), y0..y1 (excl) | colour | source / confidence |
|---|---|---|---|---|---|
| SCR | map screen fill (untextured) | x 90..319, y 311..414 | x 45..160, y 312..416 (115x104) | modal (110,102,122); vertical drift (108,100,120) at top -> (118,110,129) mid -> (110,102,122) bottom | measured; scissor match exact; fill is a gradient, not flat (GAP 4) |
| BEZ | pill bezel (textured chrome) | not measured | x 30..174, y 304..424 (144x120) | chrome grey | `hud-layout.md` §1.3 (CONFIRMED, from ELF). Centre (102,364) matches SCR centre (102.25,364): supports B |
| LAMP_L_MID | left grip badge, bright lamp | x 70..79, y 353..371 (rim to x 70..85, y 353..374) | x 35..40, y 353.8..372.8 (rim x 35..43, y 353.8..375.8) | fill (0,104,8); rim (8,32,8)/(1,63,1) | measured; palette cluster match (section 3) |
| LAMP_L_TOP | left upper lamp, unlit | x 72..83, y 342..347 | x 36..42, y 342.8..348.8 | (8,32,8) | measured |
| LAMP_L_BOT | left lower lamp, unlit | x 72..83, y 381..385 | x 36..42, y 381.8..386.8 | (8,32,8) | measured |
| LAMP_R_MID | right grip badge, bright lamp | x 326..335, y 353..371 (rim to x 326..341) | x 163..168, y 353.8..372.8 | (0,104,8) | measured; mirror of left |
| LAMP_R_TOP / BOT | right unlit lamps | x 328..339 y 342..347 / x 328..337 y 381..385 | x 164..170 / x 164..169 | (8,32,8) | measured |
| MIRROR | horizontal axis of bezel/lamps | scan x 205.5 | UI x 102.75 | - | (70+341)/2 from rim extents |
| ARROW_R | player dash/heading marker, red | town: x 200..213, y 361..370; street: x 194..209, y 362..370 | town centroid (102.40, 367.18); street (101.42, 367.04) | (255,1,1) | measured; colour set 0x2A4370 = (1,0,0,1) in ELF (CONFIRMED) |
| ARROW_MODEL | arrow geometry | - | isosceles triangle, base 30, length 40 | - | ELF vertex chain at 0x290870 (section 2); oracle shape: base/leg = 0.70 vs model 30/42.7 = 0.70 |
| ARROW_HEAD | heading | town: apex (213,368) over base midpoint (201,365.5), dir +11.8 deg (y-down); street: apex (209,362.5), base mid (199.8,368.5), dir -33 deg | - | - | derived, +-5 deg (3 px tip uncertainty) |
| ARROW_TEAL | second arrow instance / shadow | town x 196..213 y 367..372 (mostly under red); street x 190..207 y 375..382 (clear of red) | - | observed (11,115,128) / (1,116,128); nominal 0x2A4380 = (0,0.7,0.7) -> (0,178,178) | GAP (section 4) |
| RIVAL | rival dot (green, alpha 0.5) | 3x2 to 6x3 scan px on roads (e.g. x164..169 y329..331) | ~3x2 UI px | on-road observed (22..33,127..150,58..70) = 0.5*(0,255,0)+0.5*(45,45,116) | ELF 0x2A4390 = (0,1,0,0.5) (CONFIRMED); colour matches on-road |
| DARK | small dark markers, 5/frame | ~8x3 scan px (e.g. x196..203 y342..344) | ~4x2 UI px | (18,17,20) modal | measured; identity GAP (section 4) |
| BLDG | flat building footprints | 6..210 px clusters, e.g. x152..179 y340..351 | e.g. x76..90 y340.8..352.8 | (78,65,51) | measured; flat fill; most likely house markers (section 4) |
| ROAD | roads | - | - | (45,45,116) | `map-view.md` / NavGraph geometry; not re-derived here |

Town-frame footprint list (scan, from `town_driving.png`, >=6 px):
x116..133 y329..343; x178..191 y334..344; x152..179 y340..351; x206..219 y348..354;
x118..133 y358..361; x180..193 y363..368; x238..245 y364..366; x302..311 y382..392.
Street frame, same texture, shifted by roughly (-8,+9) scan: x144..173 y349..360 etc.
The footprints and the dark/rival markers all move between the two frames while the
chrome does not, so the map content scrolls and the bezel, lamps and arrow are fixed.

## 2. Arrow model and colour sets (ELF, CONFIRMED)

- `0x2A4370` red set = f32 `(1,0,0,1)`; `0x2A4380` teal set = f32 `(0,0.7,0.7,1)`;
  `0x2A4390` rival = f32 `(0,1,0,0.5)`. Read from `SLUS_203.98` at file offset
  `vaddr - 0x1FF000` (the ELF delta used by `sys-gsl-packs.md`).
- `0x290870` arrow chain, vertex records (float view): apex (0, _, 20), base
  corners (-15, _, -20) and (15, _, -20), vertex colour `255,255,255` (0x437F0000)
  modulated by the colour set. That is a 30-wide, 40-long isoceles triangle in
  model units. The oracle red blob fits it: base corners 9..10 scan px apart,
  apex about 12.3 scan px from the base midpoint.
- Map zoom: `0x2A42C8` town table entry 0 = `50.0`, so scale = `50.0 / mapZoomTable[id]`
  per `hud-layout.md` §1.3. The table is `{zoom, 0, x, z}` with 16 B stride:
  `(50,0,202,136)`, `(25,0,442,1910)`, `(105,0,127,1910)`. Which id is active on the
  town screen was not determined (GAP 5).

## 3. Texture / atlas identity

- **Bezel** (0x249208, `hud-layout.md` §1.3): atlas refs left cap 16x120 at (0,192),
  right cap 16x120 at (216,0), 112-wide middle from the 8 px cell atlas at (208,0),
  via `uiArenaSpritesWrap`. The texture table at `0x2A8FE0` does not hold direct
  pointers (its words read as `0xE1411E94`-class values), so the GSL file is NOT
  resolved (GAP 2).
- **Lamps**: the palette search over every SYS and ITEM CLUT unit
  (`palsearch` in section 6, same unit walk as `sys-gsl-packs.md` §1) finds one
  palette holding all three lamp colours within +-2:
  `SYS/POSE.GSL` unit 5, CT16 palette at DBP `0x146`, entries 129..131 and 161..163
  hold (0,106,8), (0,65,0), (8,32,8). No other unit matches all three within +-6.
  This is a candidate, not a proof: the texture index that uses it was not decoded
  (no unswizzle here), and POSE is a notebook-pose pack, so it is PROVISIONAL.
- **Screen, markers, arrow, roads**: no texture. The minimap is PATH1 VU1 geometry
  plus flat primitives (the 0x3380B packet selects GS state only, `gsl-vm.md` §5.3).
  Their colours are literal constants, the arrow colour sets above. Flat fills
  (78,65,51), (18,17,20) and the screen gradient have no texture source to cite.

## 4. GAP (still ungrounded)

1. **House vs. car identity.** The brief's "house markers" is not established. The
   flat (78,65,51) footprints are the best house candidate. The (18,17,20) dark
   markers are small and their shape reads as a flattened car silhouette, which fits
   the symbol-map entry "town cars" (`functions.md`, town/field HUD map). Neither is
   confirmed against ELF data.
2. **Bezel texture file.** `0x2A8FE0` is not a pointer table as assumed; the GSL
   unit that the bezel atlas comes from is unresolved.
3. **Teal second arrow.** Observed `(1..11,115..116,128)` is not the nominal
   `0x2A4380` colour `(0,178,178)` and not a simple alpha blend over the screen or road
   fill. Its position relative to the red arrow changes between frames (under it in
   town, 8 px below in street), so it is not a fixed shadow. Its owner (second player,
   town car, or shadow pass) is unknown.
4. **Screen fill.** The purple fill is a vertical gradient (about 10 levels of drift)
   and may be translucent over the scene. No ELF source for the fill was found.
5. **Mapping convention.** B agrees with the bezel centre and the SCISSOR_2
   half-lines to 1 px. A puts the screen centre about 10 UI px lower than the bezel
   centre. Which convention the native placement space uses is not confirmed.
6. **Zoom id.** The town zoom id that applies to these frames is not pinned, so world
   to pixel scale for the footprints and rival dots is not derived.
7. **Heading accuracy.** The heading figures come from a 3-vertex hull of about 70
   pixels; treat them as +-5 deg.

## 5. Convention check

| anchor | scan | B (x/2, y*448/447) | A (y/0.93125 - 16) | hud-layout / scissor |
|---|---|---|---|---|
| screen top | 311 | 311.7 | 318.0 | 312 (SCAY0 156 x 2) |
| screen bottom (incl) | 414 | 415.9 | 429.6 | 415 (SCAY1 207 x 2) |
| screen centre y | 362.5 | 363.3 | 373.9 | bezel centre 364 |

B matches the scissor and bezel within 1 px; A is 6..14 px off in y. This is the
reason the table gives B first.

## 6. Reproduce

Run from the repo root (`python3 -m pip` not needed, PIL only):

```python
# B8 minimap measurement: python3 minimap_measure.py build/scratch/oracle-golden/town_driving.png
import sys, math
from collections import Counter
from PIL import Image

px = Image.open(sys.argv[1]).convert('RGB').load()
RB = 448 / 447

def pred_screen(c): return 95 <= c[0] <= 130 and 90 <= c[1] <= 125 and c[2] - c[0] >= 8 and c[2] >= 110
def pred_lamp(c):   return c[1] >= 90 and c[0] <= 40 and c[2] <= 40 and c[1] - c[0] >= 60
def pred_dark(c):   return max(c) <= 40 and c[1] - c[0] <= 15
def pred_red(c):    return c[0] > 200 and c[1] < 70 and c[2] < 70
def pred_teal(c):   return c[1] >= 90 and c[2] >= 100 and c[0] <= 40 and c[2] - c[0] >= 60
def pred_bldg(c):   return abs(c[0]-78) <= 6 and abs(c[1]-65) <= 6 and abs(c[2]-51) <= 6
def pred_rival(c):  return c[1] >= 120 and c[0] <= 70 and c[2] <= 100 and c[1] - c[0] >= 70 and c[1] - c[2] >= 50

def comps(region, pred):
    X0, Y0, X1, Y1 = region; seen = set(); out = []
    for y in range(Y0, Y1):
        for x in range(X0, X1):
            if (x, y) in seen or not pred(px[x, y]): continue
            st = [(x, y)]; seen.add((x, y)); pts = []
            while st:
                a, b = st.pop(); pts.append((a, b))
                for n in ((a+1, b), (a-1, b), (a, b+1), (a, b-1)):
                    if X0 <= n[0] < X1 and Y0 <= n[1] < Y1 and n not in seen and pred(px[n]):
                        seen.add(n); st.append(n)
            out.append(pts)
    return out

def show(tag, pts):
    xs = [p[0] for p in pts]; ys = [p[1] for p in pts]
    x0, x1, y0, y1 = min(xs), max(xs), min(ys), max(ys)
    col = Counter(px[p] for p in pts).most_common(1)[0][0]
    print(f"{tag:9s} n={len(pts):4d} scan x{x0}..{x1} y{y0}..{y1} | UI_x {x0/2:.1f}..{(x1+1)/2:.1f} "
          f"UI_y(448) {y0*RB:.1f}..{(y1+1)*RB:.1f} | UI_y(A=/0.93125) {y0/0.93125:.1f}..{(y1+1)/0.93125:.1f} | modal {col}")

scr = [p for c in comps((56, 296, 350, 424), pred_screen) for p in c]
show('SCREEN', scr)
for c in comps((56, 296, 100, 424), pred_lamp) + comps((300, 296, 350, 424), pred_lamp):
    if len(c) >= 6: show('LAMP_BR', c)
for c in comps((56, 296, 100, 424), lambda c: pred_dark(c) and c[1] >= 25) + comps((300, 296, 350, 424), lambda c: pred_dark(c) and c[1] >= 25):
    if len(c) >= 6: show('LAMP_DK', c)
for c in comps((92, 330, 312, 420), pred_dark):
    if len(c) >= 6: show('DARKBLOB', c)
for c in comps((92, 310, 312, 420), pred_bldg):
    if len(c) >= 6: show('BLDG', c)
for c in comps((92, 310, 312, 420), pred_rival):
    if len(c) >= 4: show('RIVAL', c)
red = [(x, y) for y in range(340, 390) for x in range(180, 230) if pred_red(px[x, y])]
teal = [(x, y) for y in range(340, 390) for x in range(180, 230) if pred_teal(px[x, y])]
for tag, pts in (('ARROW_R', red), ('ARROW_T', teal)):
    show(tag, pts)
cx = sum(p[0] for p in red) / len(red); cy = sum(p[1] for p in red) / len(red)
print(f"ARROW_R centroid scan ({cx:.2f},{cy:.2f})  UI_x {cx/2:.2f}  UI_y(448) {cy*RB:.2f}  UI_y(A) {cy/0.93125:.2f}")
def hull(pts):
    pts = sorted(set(pts))
    def cr(o, a, b): return (a[0]-o[0])*(b[1]-o[1]) - (a[1]-o[1])*(b[0]-o[0])
    lo, up = [], []
    for p in pts:
        while len(lo) >= 2 and cr(lo[-2], lo[-1], p) <= 0: lo.pop()
        lo.append(p)
    for p in reversed(pts):
        while len(up) >= 2 and cr(up[-2], up[-1], p) <= 0: up.pop()
        up.append(p)
    return lo[:-1] + up[:-1]
print('ARROW_R hull (scan px):', hull(red))
```

Lamp palette candidate search (the unit walk of `sys-gsl-packs.md` §1; CLUT
units are the 16-wide palette units that follow each texture; `psm` 0 = CT32,
2 = CT16; a unit is a palette candidate when its data is at most 1024 B):

```python
import os, struct
ROOT = '/Users/holzr/Library/Application Support/RoadTripRecomp/disc'
def units(data):
    cur = 0; out = []
    while cur < len(data) and data[cur+3] != 0x70:
        qwc = struct.unpack_from('<H', data, cur)[0]
        regs = {}
        for i in range(4):
            v, r = struct.unpack_from('<QQ', data, cur + 32 + i*16)
            regs[r & 0xff] = v
        bb = regs.get(0x50, 0)
        nloop = (qwc*16 - 96) // 16
        out.append(dict(dbp=(bb >> 32) & 0x3fff, psm=(bb >> 56) & 0x3f,
                        data=data[cur+112: cur+112+nloop*16]))
        cur += (qwc + 1) * 16
    return out
def entries(u):
    d = u['data']
    if u['psm'] == 0: return [tuple(d[i:i+3]) for i in range(0, len(d), 4)]
    res = []
    for i in range(0, len(d), 2):
        v = struct.unpack_from('<H', d, i)[0]
        res.append(((v & 31)*255//31, ((v >> 5) & 31)*255//31, ((v >> 10) & 31)*255//31))
    return res
L = [(0,104,8), (1,63,1), (8,32,8)]
hits = []
for sub in ('SYS', 'ITEM'):
    for fn in sorted(os.listdir(os.path.join(ROOT, sub))):
        if not fn.upper().endswith('.GSL'): continue
        for k, u in enumerate(units(open(os.path.join(ROOT, sub, fn), 'rb').read())):
            if u['psm'] not in (0, 2) or len(u['data']) > 1024: continue
            pe = entries(u)
            per = [min(max(abs(c[j]-t[j]) for j in range(3)) for c in pe) for t in L]
            hits.append((sum(p <= 6 for p in per), max(per), fn, k, hex(u['dbp']), per))
hits.sort(key=lambda h: (-h[0], h[1]))
for h in hits[:5]: print(h)   # top: ('POSE.GSL', 5, '0x146') with per-colour max error [2, 2, 0]
```

ELF reads (colour sets, arrow chain, zoom table) are `struct.unpack_from` at
`vaddr - 0x1FF000` into `SLUS_203.98`:

```python
import struct
f = open('/Users/holzr/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98', 'rb').read()
def rd(va, n): return f[va - 0x1FF000: va - 0x1FF000 + n]
print(struct.unpack('<4f', rd(0x2A4370, 16)))   # red   (1,0,0,1)
print(struct.unpack('<4f', rd(0x2A4380, 16)))   # teal  (0,0.7,0.7,1)
print(struct.unpack('<4f', rd(0x2A4390, 16)))   # rival (0,1,0,0.5)
print(struct.unpack('<12f', rd(0x2A42C8, 48)))  # town zoom table, 16 B stride
print([round(v, 3) for v in struct.unpack('<48f', rd(0x290870, 192))])  # arrow chain
```
