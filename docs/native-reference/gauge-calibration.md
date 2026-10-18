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

## Speed display law (mph) — coordinator ruling 2026-10-18

The speed display law is **MPH** (supersedes the former `round(speed / 100)` "kph" provisional mapping and the
divisor candidate table; the oracle frames `race_hud_driving.png` = 63 and `race_hud.png` = 99 show the baked
`mph` label):

* `V = floor(speed_word / 55.07)`, clamped to `0..186`.
* Drawn as **right-aligned 16x23 quads**, ones digit at design position **(573, 388)**, **step -17 px per digit**
  (tens at 556, hundreds at 539).
* Digit texels on **METER0** (256x128 atlas page; native id `hudArt.speedDial`):
  `u = (140 + 16*(d % 7)) / 256`, `v = (23*(d / 7)) / 128` (integer division on `d / 7`).
* Uniform white vertex colour.
* The `mph` text is **baked into the METER0 page**; it is not drawn separately.
* Badge box: x 556..604, y 388..416 (design px).
* Grounding: coordinator ruling 2026-10-18 (earlier-wave verified); oracle PNGs `race_hud_driving.png` /
  `race_hud.png`. The native HUD digit producer is separately pinned: handler 0 calls
  `CalculateSpeedometerDigits` at `0x00249A7C`, which divides the Q15-velocity-derived
  `sqrt(v·v) * 216.0` value by the exact f32 `1.6089999675750732` (`0x00332F48`; `div.s`
  at `0x00249508`). That is **not** a fixed-point encoding of the host `speed_word / 55.07`
  contract: it is the original velocity-vector domain. `hud_live` therefore correctly retains
  `55.07f` for its host speed word instead of substituting the unrelated ROM f32 divisor.
* `race-v13.ppm` contains no gauge (lower-right crop uniform) and is not a validation capture.

**Do not** derive the needle angle from this display law (see below).

## Needle geometry and input law — resolved 2026-10-18 (W4-11)

The 1P METER0 needle is a **tachometer**, not a speedometer.  Its input is the
signed engine-RPM halfword at `car + 0x1D0`; it is independent of the displayed
MPH word and its gear byte remains `car + 0x1FF`.

`0x249DD0` calls `CalculateTachometerValue(car, 0, 1)`; the helper at
`0x249520` loads `lh v0,0x1D0(a0)`, caps values above `10000`, and returns the
result unchanged because its scale parameter is zero.  The handler converts
that value to float, divides by `10000.0f`, adds `-0.7f`, then stores it as the
RotQuads z-rotation (`0x249DE4..0x249E0C`, `0x249EDC`).  RotQuads applies its
half-turn convention, so the exact law on the physical runtime domain
`0 <= rpm <= 10000` is:

```text
theta_rad = pi * (rpm / 10000 - 0.7)
```

Here `theta=0` points up and positive rotation is clockwise on screen.  Thus
idle is `-126°`, and the full scale is `+54°`.  The producer
`AdvanceVehicleMotion` (`0x218EE0`, stores at `car+0x1D0`) clamps the field to
`0..10000`; its gear-ratio use at `car+0x22C + 2*gear` supplies the semantic
cross-check that this is engine RPM, rather than vehicle speed or animation.

The constants are ROM-pinned by `tests/native_w11/gauge_needle_test.cpp`:
`pi` (`0x40490FDB`), divisor `10000.0f`, and bias `-0.7f`; the test also pins
the load/call/divide/add/store instructions above.  The host primitive is
`src/native/gameplay/gauge_needle.h`; integration is deliberately gated by
`kRaceHudNeedleLawResolved` in `race_hud_layout.h`, which W4-11 does not edit.

The authored quad is entry 1, CSA 12, `u=3,v=53,8x59`, pivot at dial-origin
`+(70,68)` = design `(554,362)`, with strip vertices
`(-4,-59),(4,-59),(-4,0),(4,0)`.  It is drawn before the 12x12 hub cap
(`u=16,v=80`, pivot-independent at design `(548,358)`).

Angles below are measured design-space hub-to-tip angles (`0=right`, positive
CCW).  Converting them to the rotation convention (`theta = 90° - tipAngle`)
and inverting the ROM law produces plausible, distinct engine states: near
idle for no throttle and mid-range RPM while driving.  This validates the law
without fitting it to MPH.

| frame | displayed mph | design tip angle | implied RPM from ROM law | note |
|---|---:|---:|---:|---|
| `race_hud.png` | 39 | -146.11° | ~117 | no throttle; near idle |
| `race_hud_driving.png` | 63 | +104.09° | ~6,217 | driving; mid-range |

The former `(1166,353)--(1202,366)` shot-pixel box is the **gear plate**, not
the needle, and must not be used for PCA or a speed-to-angle fit.  The old
two-point MPH affine mapping is withdrawn.

## Gear glyph semantics — ROM/atlas pinned 2026-10-18 (W6-03)

`GetVehicleGear` is the two-instruction helper at `0x002495B8`: its delay slot
`lb v0,0x1ff(a0)` at **`0x002495BC`** returns the signed car byte. Handler 0 calls
it at `0x002499D4`, then emits the glyph with `g % 5` / `g / 5` at
`0x00249A04..0x00249A58`: `u=(g%5)*20+140`, `v=(g/5)*16+80`, `20x14`, destination
`(581,354)` (gauge origin `(484,294)` plus `(97,60)`). There is no +/- adjustment
before indexing.

The contact sheet `build/scratch/w6-03/gear_cells.png`, decoded from canonical
`ITEM/METER0.GSL`, reads row 0 as **R 1 2 3 4** and row 1 as **5 6 blank blank blank**.
Thus `g==0` is **reverse**, not neutral or first gear; `g==1..6` are the visible forward
gears. This METER0 source page is PSMT8 + CT32 (not CT16/CSA12); CSA 12 applies to the
separate PSMT4 needle page. `src/native/gameplay/gear_glyph.h` and
`tests/native_w17/gear_glyph_test.cpp` pin the settled table and index law.

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
