# Sky / ground color calibration (oracle goldens)

Worker B2. Source of truth: `build/scratch/oracle-golden/{title,main_menu,town_driving,race_hud_driving,attract}.png` (1280x447, PGS, SSAA 4). UI mapping for oracle frames: `uiX = px / 2`, `uiY = px / 0.931` (i.e. `447 px = 480 ui`). Native frames are 800x600 with uniform `px/1.25`.

All numbers below are measured by the embedded Python snippets (run from repo root). Scratch PNGs: `build/scratch/hud-art/sky_native_vs_oracle_*.png`.

## 0. Sampling methodology

For each golden we pick a sky column that stays in open sky from y=0 to the horizon, avoiding HUD geometry, cars, buildings, trees. Verification: print the column strip and confirm no sudden object colour before the horizon. Columns are chosen per screen and documented.

```python
from PIL import Image
import numpy as np
im = Image.open('build/scratch/oracle-golden/<name>.png')
px = np.array(im)
col = <chosen>
for y in range(0, 90):
    print(y, tuple(int(v) for v in px[y, col]))
```

## 1. Per-screen measurements

### 1.1 title.png — chosen sky column `x=40` (left margin, avoids the sun glow at x~480-690)

Dominant (modal) row colour is flat across the sky; compressed runs:

| px y range | uiY range | dominant RGB |
|---|---|---|
| 0-1 | 0.0-1.1 | (45,155,215) |
| 2-83 | 2.1-89.2 | (43,154,216) |

Sampled every 5 px (see snippet in §0 with `col=40, y0=0, y1=83`): all rows (43,154,216) except top row.

Horizon: first row whose colour breaks the sky trend by >15 (Euclidean) is transition `83->84` (diff 30.5). **Horizon px y = 83 -> uiY = 89.2** (last sky row y=83 is (45,150,209)).

Bright blobs (thr>=245 connected components, min area 50):
- Dominant sky glow / sun: center px (489,121) -> ui (244.5,130.0), colour (253,236,64), bbox x[102..841] y[32..231], approx radius 370x100 px.
- Sun flare streak: px (644.5,36.5) -> ui (322.2,39.2), colour (255,248,18), bbox x[600..689] y[34..37], approx 90x4 px.

```python
# horizon detection (title)
import numpy as np
from PIL import Image
im=Image.open('build/scratch/oracle-golden/title.png'); px=np.array(im)
prof=px[:,40,:].astype(float)
diff=np.array([np.linalg.norm(prof[y+1]-prof[y]) for y in range(len(prof)-1)])
print('first break:', np.where(diff>15)[0][0], '->', np.where(diff>15)[0][0]+1)
```

### 1.2 main_menu.png — chosen sky column `x=40` (same layout as title)

| px y range | uiY range | dominant RGB |
|---|---|---|
| 0 | 0.0 | (44,155,215) |
| 1-81 | 1.1-87.0 | (43,154,216) |
| 82 | 88.1 | (45,150,208) |
| 83 | 89.2 | (46,148,198) |

Horizon: transition `83->84` (diff 24.4). **Horizon px y = 83 -> uiY = 89.2** (last sky row y=83 (46,148,198)).

Bright blobs: same as title (sun glow center (490,121) px / ui (245,130), colour (253,236,64); flare (651,36) px / ui (327,38), colour (255,248,18)).

### 1.3 town_driving.png — chosen sky column `x=320` (open sky left of centre, avoids buildings/trees; column runs clean to horizon)

Sampled every 5 px (full per-row data in snippet):

| px y | uiY | dominant RGB |
|---|---|---|
| 0 | 0.0 | (12,121,253) |
| 5 | 5.4 | (9,120,253) |
| 10 | 10.7 | (8,119,254) |
| 15 | 16.1 | (11,121,253) |
| 20 | 21.5 | (11,121,253) |
| 25 | 26.9 | (10,121,253) |
| 30 | 32.2 | (11,121,253) |
| 35 | 37.6 | (13,121,253) |
| 40 | 43.0 | (13,121,253) |
| 45 | 48.3 | (14,122,253) |
| 50 | 53.7 | (16,123,253) |
| 55 | 59.1 | (18,123,253) |
| 60 | 64.4 | (19,124,254) |
| 65 | 69.8 | (20,126,253) |
| 70 | 75.2 | (11,122,253) |
| 75 | 80.6 | (3,116,253) |
| 80 | 85.9 | (1,113,248) |

Horizon: transition `80->81` (diff 31.1). **Horizon px y = 81 -> uiY = 87.0** (last sky row y=80 is (1,113,248); y=81 is (12,105,220) ground).

Bright blobs: no large sun; small bright spots only (blue UI / car highlights). Dominant sky is a smooth vertical gradient.

### 1.4 race_hud_driving.png — chosen sky column `x=1240` (right margin, avoids car/track; clean to horizon)

Sampled every 5 px:

| px y | uiY | dominant RGB |
|---|---|---|
| 0 | 0.0 | (34,133,253) |
| 5 | 5.4 | (34,133,253) |
| 10 | 10.7 | (34,133,253) |
| 15 | 16.1 | (34,133,253) |
| 20 | 21.5 | (35,133,253) |
| 25 | 26.9 | (35,134,254) |
| 30 | 32.2 | (37,134,254) |
| 35 | 37.6 | (38,135,253) |
| 40 | 43.0 | (38,136,253) |
| 45 | 48.3 | (40,136,253) |
| 50 | 53.7 | (41,136,253) |
| 55 | 59.1 | (44,138,253) |
| 60 | 64.4 | (53,143,253) |
| 65 | 69.8 | (61,147,253) |
| 70 | 75.2 | (74,155,253) |
| 75 | 80.6 | (92,165,253) |
| 80 | 85.9 | (115,176,249) |

Horizon: transition `80->81` (diff 239.6). **Horizon px y = 81 -> uiY = 87.0** (last sky row y=80 is (115,176,249); y=81 ground (100,80,30)).

Bright blobs: none dominant; track/UI highlights only.

### 1.5 attract.png — no clean sky/ground separation

Column profiles (x=40..1240) are near-uniform grey (92-108) top-to-bottom; no horizon detectable (max adjacent diff < 5 in every column). **Horizon: not identifiable; treat as flat ambient grey (~100,100,100).** Low confidence; do not apply a sky gradient to attract.

## 2. Native (800x600) vs oracle (1280x447) row diff

Native captures resized to 1280x447 with bilinear filtering; per-row MAE over RGB channels; worst 10 rows (sorted by row index) with row-mean colours:

### town-v13 vs town_driving (overall MAE 53.54)
| px y | MAE | oracle row-mean RGB | native row-mean RGB |
|---|---|---|---|
| 168 | 151.06 | (99,86,65) | (215,234,236) |
| 169 | 161.79 | (96,84,63) | (240,246,240) |
| 170 | 162.41 | (97,87,66) | (245,248,241) |
| 171 | 157.88 | (99,90,70) | (242,247,240) |
| 172 | 158.89 | (97,87,68) | (242,247,238) |
| 173 | 158.30 | (100,89,72) | (245,248,242) |
| 174 | 157.02 | (103,89,74) | (244,247,246) |
| 175 | 153.91 | (104,89,74) | (234,243,245) |
| 176 | 155.72 | (104,89,73) | (234,242,245) |
| 177 | 154.98 | (102,87,71) | (232,240,241) |

### race-v13 vs race_hud_driving (overall MAE 49.16)
| px y | MAE | oracle row-mean RGB | native row-mean RGB |
|---|---|---|---|
| 160 | 126.09 | (73,78,61) | (144,202,235) |
| 161 | 128.94 | (76,79,64) | (152,206,236) |
| 162 | 127.34 | (85,84,66) | (156,208,237) |
| 163 | 129.12 | (92,85,65) | (161,210,238) |
| 164 | 124.22 | (102,90,69) | (166,212,239) |
| 168 | 126.19 | (112,109,67) | (192,225,243) |
| 169 | 134.20 | (113,110,67) | (209,233,247) |
| 170 | 134.63 | (114,113,70) | (215,236,247) |
| 171 | 125.21 | (123,121,79) | (216,236,241) |
| 178 | 124.18 | (95,104,80) | (209,228,209) |

### title-v13 vs title (overall MAE 30.85)
| px y | MAE | oracle row-mean RGB | native row-mean RGB |
|---|---|---|---|
| 11 | 67.97 | (44,154,216) | (138,183,136) |
| 12 | 80.31 | (44,154,216) | (151,193,121) |
| 13 | 83.86 | (44,154,216) | (155,197,119) |
| 14 | 83.63 | (44,154,216) | (157,201,125) |
| 15 | 78.12 | (44,154,216) | (158,205,150) |
| 16 | 73.27 | (44,154,216) | (159,206,169) |
| 100 | 70.66 | (117,129,125) | (123,139,130) |
| 101 | 70.67 | (117,129,125) | (126,142,133) |
| 102 | 67.64 | (125,137,132) | (132,147,139) |
| 166 | 71.32 | (158,152,159) | (154,130,115) |

```python
# native vs oracle row MAE
from PIL import Image
import numpy as np
nat = Image.open('build/scratch/gamelook/town-v13.ppm').convert('RGB').resize((1280,447), Image.BILINEAR)
ora = Image.open('build/scratch/oracle-golden/town_driving.png').convert('RGB')
a=np.array(nat).astype(float); b=np.array(ora).astype(float)
mae=np.abs(a-b).mean(axis=(1,2))
order=np.argsort(mae)[::-1][:10]
for y in sorted(order):
    print(y, round(float(mae[y]),2),
          tuple(int(v) for v in b[y].mean(axis=0)),
          tuple(int(v) for v in a[y].mean(axis=0)))
```

## 3. Proposed C++ gradient-stop arrays (sky top -> horizon)

Format: `{uiY0, uiY1, RGBA}` where the fill linearly interpolates between consecutive stops' RGBA (uiY1 of stop i == uiY0 of stop i+1). Residuals are max |measured - filled| over the sky rows (all channels).

### title (sky column x=40, horizon uiY=89.2)
```cpp
// residual 6.86/255
static const SkyStop kTitleSky[] = {
  {  0.0f, 89.2f, { 45,155,215,255 } },
  { 89.2f, 89.2f, { 45,150,209,255 } },
};
```
Actually a single linear segment; equivalent two-stop array: `{0, (45,155,215)}, {89.2, (45,150,209)}`.

### main_menu (horizon uiY=89.2)
```cpp
// residual 1.98/255
static const SkyStop kMainMenuSky[] = {
  {  0.0f, 87.0f, { 44,155,215,255 } },
  { 87.0f, 89.2f, { 43,153,214,255 } },
  { 89.2f, 89.2f, { 46,148,198,255 } },
};
```

### town (horizon uiY=87.0)
```cpp
// residual 5.53/255
static const SkyStop kTownSky[] = {
  {  0.0f, 73.0f, { 12,121,253,255 } },
  { 73.0f, 85.9f, { 16,124,253,255 } },
  { 85.9f, 87.0f, {  1,113,248,255 } },
};
```

### race (horizon uiY=87.0)
```cpp
// residual 3.60/255
static const SkyStop kRaceSky[] = {
  {  0.0f, 56.9f, {  34,133,253,255 } },
  { 56.9f, 74.1f, {  42,137,253,255 } },
  { 74.1f, 85.9f, {  71,153,253,255 } },
  { 85.9f, 87.0f, { 115,176,249,255 } },
};
```

### attract
No gradient; use flat `(100,100,100,255)` or skip.

Gradient-stop generation (re-run to regenerate arrays):
```python
from PIL import Image
import numpy as np
def load(path,col,y0,y1):
    im=Image.open(path); px=np.array(im)
    return px[y0:y1+1,col,:].astype(float)
def seg_cost(prof,i,j):
    n=j-i
    if n==0: return 0.0
    ts=(np.arange(i,j+1)-i)/n
    interp=prof[i][None,:]*(1-ts[:,None])+prof[j][None,:]*ts[:,None]
    return float(np.abs(interp-prof[i:j+1]).max())
def best_partition(prof,M):
    N=len(prof); INF=float('inf')
    dp=[[INF]*N for _ in range(M+1)]; prev=[[-1]*N for _ in range(M+1)]
    for j in range(N): dp[1][j]=seg_cost(prof,0,j)
    for m in range(2,M+1):
        for j in range(m-1,N):
            for i in range(m-2,j):
                c=max(dp[m-1][i],seg_cost(prof,i+1,j))
                if c<dp[m][j]: dp[m][j]=c; prev[m][j]=i
    ends=[N-1]; m=M; j=N-1
    while m>1:
        i=prev[m][j]; ends.append(i); j=i; m-=1
    ends.append(0); ends=sorted(set(ends))
    return dp[M][N-1],ends
# example: town
prof=load('build/scratch/oracle-golden/town_driving.png',320,0,80)
for M in range(1,6):
    cost,ends=best_partition(prof,M)
    print('M',M,'maxerr',round(cost,2))
    if cost<=8:
        for e in ends:
            r,g,b=prof[e]; print(f'  px_y={e} uiY={e/0.931:.1f} RGB=({r:.0f},{g:.0f},{b:.0f})')
        break
```

## 4. Sun badge placement / colour

Present on title and main_menu only (diffuse yellow sky glow + horizontal flare). No sun badge on town/race/attract.

- Diffuse glow centre: px (489,121) -> ui (244.5,130.0); colour (253,236,64); approx radius 370x100 px (ui ~185x107). Low saturation; use as background sky tint, not a badge.
- Sun flare streak (the actual "badge"): px (645,36) -> ui (322,39); colour (255,248,18); approx size 90x4 px (ui ~45x4). Suggested badge: circle radius ~6 ui (12 px) at (322,39), colour (255,248,18,255), or a horizontal flare quad.

```python
from PIL import Image
import numpy as np
im=Image.open('build/scratch/oracle-golden/title.png'); px=np.array(im)
mask=(px[:,:,0]>=240)&(px[:,:,1]>=230)&(px[:,:,2]<=30)
ys,xs=np.where(mask)
print('flare centroid px', (round(float(xs.mean()),1), round(float(ys.mean()),1)))
print('ui', (round(float(xs.mean())/2,1), round(float(ys.mean())/0.931,1)))
print('bbox px', int(xs.min()),int(xs.max()),int(ys.min()),int(ys.max()))
```

## 5. Open questions / confidence

- title/main_menu sky is essentially flat; the gradient array is nearly constant. If the native renderer already fills a flat colour, the residual is dominated by the sun glow, not the gradient.
- town/race gradients are smooth but measured on a single column; horizontal variation (sun glow) not captured. A 2D gradient would be more accurate but out of scope.
- attract has no identifiable sky/ground; low confidence. Escalate if attract needs sky.
- Native row diffs show large errors on ground rows (town/race y~160-178), not sky rows; sky calibration alone will not fix those (ground/geometry issue).
- Column choices assume left/right margins are HUD-free; verified by strip inspection but not by full HUD mask. If a HUD element overlaps, re-pick column using the §0 snippet.
