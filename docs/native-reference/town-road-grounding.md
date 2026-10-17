# Town driving view: pixel grounding

Measurements are from `build/scratch/oracle-golden/town_driving.png` (1280×447 RGB) and `build/scratch/gamelook/town-v12.ppm` (800×600 RGB), read directly with PIL. Oracle-to-UI conversion is `(x/2, y/0.931)` as specified. For visual landmarks, boundaries are approximate (typically ±2–5 px); color means depend on the manually selected region and are not material albedo measurements. Profile samples are unrounded means across the requested x ranges; ours' sample rows are every 25 source pixels and displayed at `y*447/600`.

## Horizontal band profile

Oracle mean RGB across x=60…1220; ours across x=40…760. Delta is ours minus oracle at the nearest mapped sample row (independent scenes/crops make these diagnostic, not pixelwise error).

| Oracle y | Oracle mean RGB | Our source y | Our mean RGB | Δ RGB |
|---:|---:|---:|---:|---:|
|100|(104.4,92.4,83.0)|100|(43.0,154.0,216.0)|(-61.4,+61.6,+133.0)|
|125|(115.1,95.3,71.1)|125|(43.0,154.0,216.0)|(-72.1,+58.7,+144.9)|
|150|(128.4,88.1,68.1)|150|(49.0,150.9,207.8)|(-79.4,+62.8,+139.7)|
|175|(102.9,88.4,73.6)|175|(131.4,116.3,119.5)|(+28.5,+27.9,+45.9)|
|200|(107.5,101.4,65.3)|200|(129.4,109.2,105.0)|(+21.9,+7.8,+39.7)|
|225|(111.3,90.8,79.8)|225|(117.5,103.0,99.0)|(+6.2,+12.2,+19.2)|
|250|(122.0,122.8,125.6)|250|(121.3,105.1,112.9)|(-0.7,-17.7,-12.7)|
|275|(120.4,121.3,122.2)|275|(116.0,98.2,107.1)|(-4.4,-23.1,-15.1)|
|300|(115.9,122.1,122.4)|300|(134.8,124.7,127.5)|(+18.9,+2.6,+5.1)|
|325|(96.0,95.4,100.2)|325|(139.1,126.8,120.7)|(+43.1,+31.4,+20.5)|
|350|(87.7,100.9,120.7)|350|(118.9,115.7,113.4)|(+31.2,+14.8,-7.3)|
|375|(84.4,93.2,85.1)|375|(109.6,115.2,114.7)|(+25.2,+22.0,+29.6)|
|400|(93.2,104.7,100.2)|400|(89.5,100.1,106.0)|(-3.7,-4.6,+5.8)|
|425|(112.1,112.7,112.8)|425|(88.6,105.5,103.8)|(-23.5,-7.2,-9.0)|

Rows in the oracle band represent a mixture of buildings/sky/road; the very different upper profile is partly because our camera is aimed into sky over a distant/low horizon while the oracle horizon is much higher in the frame.

## Oracle landmarks and colors

| Element | Pixel measurement | UI-space equivalent / notes |
|---|---|---|
| Sky/hills horizon | about y=205–214 across the center; center boundary ≈210 | y≈226 UI. Not a perfectly level horizon: architecture and hills interrupt it. |
| Grass roundabout island | visible grass ellipse approximately bbox x=333…804, y=198…231; center (569,215), width≈471, height≈33 | center (284.5,231.0), size≈235.5×35.4 UI. Ellipse is foreshortened; bounds include edge/grass. |
| White dashed road markings | near-white/gray pixels typically RGB≈(205–225,205–225,205–225); individual visible dash lengths vary with perspective, roughly 35–70 px long and 4–8 px thick | ≈17.5–35 UI px long; antialiasing/road shading makes a single exact dash/color ambiguous. |
| Asphalt | broad road sample x=400…470, y=270…285, avoiding markings/car: mean ≈(120,120,121) | Representative local gray, not a full-road average (road is shaded and perspective varying). |
| Orange roof tile | selected roof tile region x=1035…1080, y=65…105: mean ≈(151,103,49) | Texture is highly varied; this is a local tile mean. |
| Tan wall | selected wall x=845…960, y=165…190: mean ≈(164,145,123) | Shading and texture included. |
| Top-center sky | x=590…690, y=15…55: mean ≈(59,145,235) | Bright blue with cloud/gradient variation. |
| Player car | bbox approximately x=484…795, y=250…377 (blue body and dark tires/shadow excluded below) | Width 311 px = 24.3% of 1280 frame; bbox bottom y≈377 (UI y≈405). Approximate visual bbox; shadow can extend lower. |
| Minimap pill | outer bbox x≈59…344, y≈302…423 | UI bbox ≈(29.5,324.4) to (172,454.4). Silver/light-gray border and dark side details. |
| Minimap inner map | interior approximately x=88…319, y=313…414: mean RGB about (102,94,117) | Purple-gray base with roads/markers; average is only a broad interior estimate because map lines occupy substantial area. |
| Minimap marker | red/cyan player arrow near (205,365); red portion ≈(255,35,40), cyan accent ≈(0,190,220) | Dot/arrow is tiny; exact color is affected by antialiasing. |

## Same features in town-v12

| Element | Current frame observation | Difference from oracle |
|---|---|---|
| Horizon | central terrain/sky transition around source y≈230–245 (mapped oracle y≈171–183); white missing-geometry band dominates | About 27–39 oracle pixels higher than the oracle horizon (≈210); horizon placement is substantially off. |
| Roundabout | No corresponding coherent grass island ellipse; sparse green strips/terrain fragments only | Missing. |
| Markings | No visible white dashed plaza markings | Missing. |
| Asphalt | Large flat gray plane; broad sample around (100,400) is ≈(89,100,106), but local profile varies | Gray is in the same broad range as oracle asphalt, though flatter/cooler and the plaza geometry/markings are absent. |
| Roofs/walls | Roof fragments exist; closest textured orange roof region is approximately (176,96,48) in quantized dominant colors; large tan house facade around (200,180,130) | Roof fragment hue is similar but fragmented/too sparse; oracle roofs frame the view. Tan wall may be near the oracle range, but placement/lighting differ. |
| Sky | Flat upper region exact common RGB (43,154,216) | Oracle sampled top-center ≈(59,145,235): ours is 16 darker red, 9 greener, 19 less blue. |
| Car | No player car visible | Missing; target oracle bbox bottom ≈377 px (UI≈405), width≈24.3% of image. |
| Minimap | No minimap visible | Missing. |
| Speedometer/clock | Green dial is partially visible and cut off at lower-right; no matching oracle framing/clock readout apparent | Present but placement/crop and details differ. |

## Recommendations / verdict

- **Sky:** target output-space color near RGB **(59,145,235)** in the sampled top-center oracle patch. For the stated ground-fill authorship convention (GS modulate ×2), author **(30,73,118)** (rounding half-channel values); this is an initial flat-fill target, not a substitute for reproducing the oracle's cloud/gradient texture.
- **Ground/asphalt:** representative oracle road output mean ≈**(120,120,121)**, so a flat author value would be **(60,60,61)** before ×2. The current broad flat gray is darker/cooler at ≈(89,100,106), but color alone is secondary to correcting road shape, plaza placement, island, and markings.
- **Roofs/walls:** sampled target output means are roof **(151,103,49)** and wall **(164,145,123)**. If these are authored in the same ×2 modulated ground-fill path, corresponding authored values are **(76,52,25)** and **(82,72,62)**. These are pixel-derived targets only; verify whether the actual materials pass through that shader before applying them.
- **Camera direction/position:** heading **−1.406 rad at (50.6,44.2) RU is not visually validated as matching the oracle**. In v12, recognizable houses cluster on the left and a large close house fills the right foreground, while the oracle is centered on the roundabout with buildings framing both sides and a blue chase car centered. This indicates scene/camera alignment or spawn placement is off, but images alone cannot establish the required RU correction or heading delta. Do not infer a numeric heading adjustment from this comparison; first align the camera to the island center, then compare landmarks.
- **Car placement:** target car bbox bottom is approximately **y=377 px** (UI y≈405) in the 447px oracle frame, about 84.3% of frame height. Current car is absent, so this is a direct missing-element target.
- **Uncertainty:** horizon, landmark extents, dashes, and minimap feature colors were estimated from visible pixel boundaries/regions and have several-pixel or texture-dependent uncertainty. The two captures differ in resolution/aspect ratio and substantial world geometry; profile deltas must not be interpreted as a color-only calibration.
