# Gauge calibration

Measured 2026-10-17 from the two 1280x447 race oracles.  Screenshot X is
twice the game coordinate; the supplied 480-line conversion is
`uiY = screenshotY / 0.93125`.  The dial itself is a 448-line HUD element,
so its sampled raster locations below are quoted in screenshot pixels.  This
avoids silently mixing the 448-line sprite coordinate with the 480-line
presentation conversion.

## Assets and digit identity

`build/scratch/hud-art/` was absent at start.  I decoded the first 256x128
unit of `build/scratch/present/data/disc/ITEM/METER10.GSL` to
`build/scratch/hud-art/METER10.png`, and wrote the ten `meter10-digit-N.png`
crops.  This is a **PSMT8 + CT32** page in this disc, not PSMT4.  Its dynamic
row is exactly the documented `u = digit * 20, v = 102, 20x26` row.  METER0
was likewise decoded to `METER0.png`; it is the baked dial, not the digit-row
source.

The cell locations in the oracle are X=1064/1102/1140 (UI X=532/551/570),
with the 100s cell absent when zero.  Reading the blue/white glyph masks
against the decoded cell shapes gives:

| oracle | visible cells | reading | unit printed in dial art |
|---|---:|---:|---|
| `race_hud_driving.png` | 6, 3 | **63** | mph |
| `race_hud.png` | 9, 9 | **99** | mph |

The surrounding `mph` sprite and the lack of a leading-zero cell are useful
independent checks.  Do **not** relabel these as kph merely because a host
variable is named `kph`.

### Correlation limitation

The raw GSL cell masks are available, but a naïve whole-20x26 SSD against the
oracle is not a reliable classifier: each cell carries semi-transparent black
shadow texels, and the final GS composite includes the green METER0 dial and
the overlapping `mph` label.  In particular, 6/8 and 3/8 have high mask
correlation if the label is included.  The identities above use the opaque
blue/white glyph strokes only (and their full-cell source addresses), rather
than reporting a misleading whole-rect score as a measurement.  The embedded
script writes all inputs needed to rerun a compositor-aware score.

If METER10 had not been accessible, the missing item would be the **METER10
texture unit plus its CLUT**, not an invented PNG.  Extract its 256x128 PSMT4
or PSMT8 image and following 16x16 CLUT unit, then decode each pixel with GS
CSM1 index swizzle and the quad's CLUTSA/CSA bank.  `src/native/assets/hud_art.cpp`
lines 102--129 implement the PSMT4+CT16 path (including the nontrivial CSA
base), and lines 70--99/206--224 show the PSMT8 CT32/CT16 paths.  For this
actual file the PSMT8+CT32 path is sufficient.

## Native speed divisor

The current renderer in `src/native/app/main.cpp:2478-2479` displays
`round(renderSnapshot.speed / 100.0)` and calls that value `kph`; it therefore
uses **100 native speed units per displayed unit**.  `race-v13.ppm` contains
no gauge (the lower-right crop is uniform), so it cannot tie a native numeric
sample to either 63 or 99.  It must not be cited as a validation capture.

Candidates, ordered by what is actually established:

| native units / displayed unit | status | reason |
|---:|---|---|
| 100 | current/provisional | exactly the live main.cpp mapping; no paired capture validates it |
| 107.5269 | plausible cap-constrained alternative | `car_physics.cpp` caps the authored host state at 20000; this maps it to the shipped 186-mph HUD cap (`20000 / 186`) |
| 66.85 | only if the consumer deliberately wants kph text/value | maps 20000 to the original 299.27-kph equivalent, but conflicts with the dial's `mph` label |

The shipped speed algebra is more specific but is in a different raw-speed
domain: `rules_tables.{h,cpp}` says `mph = raw / 1.6089999676`, integer
truncated and capped at 186 (`kSpeedoMphDivisor`).  Until a capture records
both `renderSnapshot.speed` and the displayed digits, the conversion between
the host car's 0..20000 units and that raw domain is OPEN.  Recommendation:
keep /100 only as a labelled provisional choice; prefer 107.5269 when the
host cap is intended to coincide with the shipped 186-mph full scale.

## Needle geometry

Angles use the conventional screen-space gauge convention: hub-to-tip, 0° =
right, +90° = up.  A difference mask between the two static dial rasters
isolates the two moving needle components.  PCA of each component gives:

| frame | displayed mph | hub (px) | component bbox (px, exclusive) | angle |
|---|---:|---|---|---:|
| `race_hud_driving` | 63 | (1103, 360) | (1066,303)--(1116,360) | **+118.80°** |
| `race_hud` | 99 | (1103, 360) | (1166,353)--(1202,366) | **+0.36°** |

A two-point affine fit in displayed-mph units is

`angle_deg = 326.07 - 3.2900 * mph`  (equivalently rest angle `-33.93°`).

Its residuals are 0.00° for each supplied point because two points determine
two coefficients; that is **not** independent confirmation.  It is only a
candidate mapping, and `main.cpp` currently has **no needle render or needle
mapping** (only the METER0 dial blit and METER10 digit quads at 2440--2497).
The physically surprising extrapolated rest angle is a warning not to commit
this fit without at least one low-speed frame.  If the needle is driven from
native units directly, combine it with divisor `D` as
`angle = -33.93 - 3.2900 * (speed / D)`.

## Embedded reproduction Python

```python
from pathlib import Path
from PIL import Image
import numpy as np
from scipy.ndimage import label

def u64(b, off): return int.from_bytes(b[off:off+8], "little")
def bits(v, first, count): return (v >> first) & ((1 << count) - 1)
def csm1(i): return (i & ~0x18) | ((i & 8) << 1) | ((i & 0x10) >> 1)

def units(blob):
    at, out = 0, []
    while at + 112 <= len(blob) and blob[at + 3] != 0x70:
        assert blob[at + 3] == 0x10
        qwc, p = int.from_bytes(blob[at:at+2], "little"), at + 16
        tag, blit, reg = u64(blob, p+80), u64(blob, p+16), u64(blob, p+48)
        n = tag & 0x7fff; assert bits(tag, 58, 2) == 2 and qwc == 6+n
        out.append((bits(blit,56,6), bits(reg,0,12), bits(reg,32,12), p+96, n*16))
        at += (qwc + 1) * 16
    return out

def decode_meter(path):
    b = Path(path).read_bytes(); us = units(b)
    for i, (psm,w,h,pix,n) in enumerate(us):
        if (psm,w,h) != (0x13,256,128): continue       # this disc's METER page
        psm2,pw,ph,pp,_ = us[i+1]; assert (psm2,pw,ph) == (0,16,16)
        rgba=[]
        for q in range(w*h):
            c = pp + csm1(b[pix+q])*4; r,g,bl,a = b[c:c+4]
            rgba += [r,g,bl, 255 if a > 128 else a*255//128]
        return Image.frombytes("RGBA", (w,h), bytes(rgba))

m = decode_meter("build/scratch/present/data/disc/ITEM/METER10.GSL")
for d in range(10): m.crop((20*d,102,20*d+20,128)).save(f"meter10-digit-{d}.png")

# Needle components from frame difference.  Their PCA major axes are
# (drive: -0.48171186,-0.87632966) and (race: .99998031,-.00627523), yielding
# 118.7973 and .3595 degrees with atan2(-dy, dx), respectively.
a=np.asarray(Image.open("build/scratch/oracle-golden/race_hud_driving.png").convert("RGB")).astype(int)
b=np.asarray(Image.open("build/scratch/oracle-golden/race_hud.png").convert("RGB")).astype(int)
changed=np.abs(a-b).max(2)>30
```
