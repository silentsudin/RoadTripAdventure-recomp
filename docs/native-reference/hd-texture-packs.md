# HD texture packs — native design ledger

Updated 2026-10-16. Authoritative spec for the HD-texture-pack **logic module**
(`src/native/assets/hd_pack.{h,cpp}`, tests `tests/native_assets/test_hd_pack.cpp`).
The module is pure C++: content keys, pack-folder indexing, replacement requests,
a dump writer and a CPU mip-chain builder. It contains no GPU code and no
game/disc bytes; the **GPU wave** (a later increment, outside this module)
consumes the `ReplacementRequest` contract defined here.

Companion docs: the disc-side format evidence lives in
[`car-data-status.md`](car-data-status.md) (liveries), [`sys-gsl-packs.md`](sys-gsl-packs.md)
(UI/HUD/source packs), [`field-textures.md`](field-textures.md) (terrain pages).
The **sibling recomp app's** HD-pack system (CLAUDE.md, "Texture dumps and HD
packs") is reference precedent: its *ideas* are ported; each port is labelled
**CONFIRMED** (proven in the sibling's production) or **AUTHORED** (a native-first
decision documented here). Native pack keys are derived from **OUR decode path**
(disc packs decoded to RGBA8 at load), never from GS-memory uploads — the sibling
hashes GS readbacks; we hash the equivalent decoded texels a scene draws.

---

## 1. Native decode-path integration points (READ-ONLY refs)

Textures reach the renderer through exactly four decode sites today; every key
is computed over the `ImageRecord`/rgba each one already emits:

| decode site | output | ref |
|---|---|---|
| CAR livery | one 128×128 RGBA8 `ImageRecord` per `CARn/Qnn.BIN` | `src/native/assets/car_models.cpp:183` (`extractCarLivery` → `convertImage`) |
| FLD terrain pages | `FieldTexturePage::rgba` (w·h·4, R at byte 0) | `src/native/assets/field_converter.cpp:1003` (`decodeFieldTextures`), struct at `field_converter.h:264` |
| FONTE glyph atlas | 512×384 RGBA8 atlas ImageRecord | `src/native/assets/glyph_atlas.h:90` (`decodeGlyphAtlas`) |
| generic upload chains | `convertImage`/`convertImageBank` (CSM1 palette order applied, byte0=R, TEXA/AEM alpha) | `src/native/assets/image_converter.h:37` / `image_converter.cpp:714` |

App-side seams where decoded images become sampled textures (all READ-ONLY
today; the glue changes are named in §7):

- `src/native/app/main.cpp:431-440` — `textureContentHash`: in-repo FNV-1a
  content-dedup precedent for terrain pages (`pagesByHash`).
- `src/native/app/main.cpp:1008-1009` — livery `ImageRecord` → `PreviewTexture`.
- `src/native/app/main.cpp:915-950` — FONTE atlas decode → `PreviewTexture` →
  `previewUploadFontAtlas`.
- `src/native/preview/preview.cpp:680-694` — `hashTexture`: FNV-1a over
  dims+RGBA; `preview.cpp:698` `ensurePageTexture`: the per-content GPU upload
  cache — **the point a pack lookup replaces an upload**.
- Dump-writer pattern reuse: `src/native/app/main.cpp:1021-1028`
  (`RT_APP_CARMODEL_DUMP`, P6 PPM) and `main.cpp:1393-1404`
  (`RT_APP_SNAPSHOT`, P6 PPM).

## 2. StableContentKey — semantics and feed (FROZEN)

**Content, not location** [CONFIRMED idea, ported): the key is a 64-bit FNV-1a
(offset basis `0xcbf29ce484222325`, prime `0x100000001b3` — the exact constants
of `hashTexture`/`textureContentHash`, so the families cross-check) over:

```
byte 0      kStableKeyLayoutVersion = 1   (a deliberate feed revision exists so
byte 1      format tag (PixelFormat byte)  any future change re-keys, never
bytes 2-5   width  LE32                    silently)
bytes 6-9   height LE32
bytes 10..  decoded RGBA8 texels, row-major, exactly W*H*4 bytes
```

Rules (each pinned by a test):

- **Same decoded texels ⇒ same key across runs**, regardless of which disc file,
  folder shuffle, palette bank or session produced them. Provenance, offsets and
  variant ids are NOT in the feed.
- **Palette-swapped content = different key** [CONFIRMED]: the hash is applied
  *after* palette application — over the texels `image_converter` already
  emitted (fades, flashes and alternate CSA banks are separate textures and
  need separate pack images). This matches the native decode exactly; the
  sibling's GS-side equivalent is "a palette swap is a different texture".
- **Dims are in the feed**: the same bytes reshaped are different content.
- A key mismatch is only a miss; a pack may never change a drawn pixel
  [CONFIRMED "dumping changes no pixel" idea].

**AUTHORED details:** format-tag + layout-version bytes in the feed (the
sibling hashes pixels only); 16-hex lowercase filename spelling with
parse/print inverse functions; null-key semantics (rejected decodes never key,
and the all-zero hex name is unparseable, so a null key is never nameable).

Not cryptographic: 64-bit FNV-1a birthday bound is ~2^32 images; the corpus is
~10^3–10^4 textures and every hit is addressable by the live decoded content,
so this is adequate; the ledger records the choice explicitly.

## 3. Pack folder spec (user-facing)

A **pack** is one flat folder (no recursion) of image files [CONFIRMED]. File
names follow the grammar:

```
<key16hex>_<W>x<H>[_src<Wn>x<Hn>][_token...] . png|pam|ppm
```

- `key16hex` — 16 hex digits (case-insensitive) of the texture's
  StableContentKey; always first.
- `W`×`H` — **the pack image's own dimensions**. Any size, any scale factor,
  non-power-of-two, any aspect: 512×512 for a 4× pack of a 128×128 dump, or
  100×75, both replace 128×128 [CONFIRMED "at any size"].
- `_src<Wn>x<Hn>` — optional [AUTHORED]: the native dims the key was dumped
  from, for the stale-mismatch guard (§5). Bare `<key16hex>.png` (no dims) is
  the documented **fallback naming**: accepted, but then the pack image's dims
  are unknown to the index and the request defers them to load time.
- `_token...` — tolerated free tokens: the sibling's `_psm13` format tags,
  upscaler `_4x` markers, anything alphanumeric.
- Extensions accepted by the index: `png` | `pam` | `ppm` (grammar is
  extension-agnostic; acceptance is scan policy). PNG is the canonical pack
  format (what GPU waves decode; carries its own alpha); PAM/PPM are accepted
  so a **raw dump folder is already a loadable pack**.

**Collision rule** [CONFIRMED idea, AUTHORED determinism]: when several files
carry one key, the **last wins** and one warning is surfaced per collision
(`PackWarnings::collisions`; the winning `PackEntry::hadCollision` is set, and
the request note repeats it). The sibling uses raw walk order ("the last file
the walk meets"); the native scan sorts names byte-wise ascending first, so
"last" is lexicographic — the same folder indexes identically on every
platform. Sibling war story this rule exists for: two PNGs of one hash shared
a cache file and one was always "missing", re-converting at every start.

Scan counters: `collisions`, `skippedUnparseable` (16-hex-prefixed but
malformed), `skippedOtherExtension`, `ignoredForeign` (readme.txt,
.DS_Store — benign). Unparseable names and foreign files never warn an
end user; collision counts do (pack makers should rename).

## 4. Dump mode

`RT_APP_TEXTDUMP=<dir>` turns on dump writes at the decode sites (§1): every
decoded native texture is written as `<key16hex>_<W>x<H>.pam|ppm`, where W×H
are **the decoded native dims** — a dump is a trivial 1× pack image, so one
grammar covers both sides.

- **PAM** (`TUPLTYPE RGB_ALPHA`, DEPTH 4) is the default: keeps alpha
  (HUD sprites and the font atlas need it). **PPM** (P6, RGB only) reuses the
  committed snapshot-writer pattern at `main.cpp:1021`/`main.cpp:1395` for
  eyeballing. Both are plain ASCII headers + raw bytes — **no third-party
  image dependency was added** (an in-repo PNG encoder exists only inside the
  Vulkan-linked preview tool, `preview.cpp:64` `encodePng`, which this pure
  library must not depend on).
- Re-dumping the same texture overwrites the same file (keys are content, not
  location), so dump folders converge instead of accumulating.
- **Conversion note for pack makers:** upscale each dump to PNG (alpha
  upscaled separately — AI upscalers give edge halo when alpha is inferred),
  keep the 16-hex key, and name the result with the **OUTPUT image's dims**:
  `4bda..._128x128.pam` → `4bda..._512x512.png`. Optionally append
  `_src128x128` so the stale guard stays armed. Tools that keep the dump's
  dims on an upscaled file (carrying P6 parity) will fail the index grammar
  check on sizes only if the dims are absent — names with wrong-but-present
  dims are the one case the `_src` guard catches (§5).
- A dump folder is **directly pack-usable**: name grammar, extensions and key
  spelling are identical, pinned by `test_dump_folder_usability` (scan of a
  written dump folder serves a replacement request).

## 5. ReplacementRequest — precedence and stale-mismatch

`requestReplacement(index, key, nativeW, nativeH)` classifies:

1. **Miss** — no entry with the key (also: null key, failed index scan). Draw
   native, no note. Known-miss bookkeeping (memo "this key has no pack image")
   is GPU-wave territory here [AUTHORED sketch, sibling's `m_knownMiss` idea].

2. **Replace** — entry found; `path`, pack dims, `mipLevelCount`, `policy`,
   native-dim echo and optional note (collision note; bare-fallback
   "dims unknown at load"). Any W×H accepted; alpha is the pack file's own
   (never re-expanded through the PS2 0x80 convention)
   [CONFIRMED].

3. **StaleMismatch** — entry names `_src` dims that disagree with the live
   texture's dims [AUTHORED, guard]: do not replace, draw native, surface the
   reason. This catches pack files kept under a name from a different
   materialisation of the same content key (mis-renamed during conversion)
   which the key hash alone cannot disprove, since the key already contains
   the source content's dims.

GPU-wave contract struct (frozen in `hd_pack.h`):

```
verdict, key, path, width, height (pack image dims; 0 = read at load),
mipLevelCount = floor(log2(max(W,H))) + 1  (0 when dims unknown),
policy { maxAnisotropy = 16, trilinearMin, clampToEdgeOnClampedAxes, keepPackAlpha },
nativeWidth/nativeHeight echo, note
```

## 6. Mip / anisotropy / clamp contract

- **Full mip chain** [CONFIRMED]: replacements sample trilinear/aniso, so the
  request sizes the chain and `buildMipChain` provides every level (base level
  included) as RGBA8 for upload. Chain rule [AUTHORED, matches Vulkan's
  `max(1, extent >> level)` exactly]: level dims `max(1, prev>>1)` per axis;
  **no power-of-two padding** (padding would invent border texels and break
  the name-dims; Vulkan/Metal accept non-PoT chains); ends at 1×1;
  `mipLevelCount` = `floor(log2(max(W,H)))+1`. Box filter with taps clamped
  to the prior level's edge, integer average `(sum+2)>>2` (half rounds up),
  straight RGBA (a premultiply→filter→unpremultiply A/B variant is a GPU-wave
  note for AI-upscaled packs — recorded, not decided here).
- **Anisotropy default 16** [CONFIRMED sibling value; "costs next to nothing"].
- **Clamp policy** [CONFIRMED idea, per-draw resolution]: axes the native draw
  clamps (PS2 `CLAMP`/`REGION_CLAMP`) sample **clamp-to-edge** so a pack image
  never bleeds its far edge at any mip level; wrapping axes keep repeat
  addressing. The request carries the policy booleans; per-axis choice is the
  GPU wave's job because only it sees each draw's clamp state.
- Pack images are **addressed by the original texture's size** [CONFIRMED]:
  native-dim echo on the request; the pack image supplies pixels in the same
  UV space.

## 7. Env contract and glue (pseudocode)

```
RT_APP_TEXPACK=<dir>     actives one pack folder (one at a time, sibling model);
                         "" / unset disables. Switching drops and rescans.
RT_APP_TEXTDUMP=<dir>    dumps every decoded texture into <dir> (re-dumps
                         overwrite the same content-keyed file).
```

Glue at a decode site (e.g. the car-livery pipeline in main.cpp — read-only
today; the GPU wave wires this):

```cpp
ImageRecord tex = extractCarLivery(...).image;         // native decode (palette applied)
StableContentKey key = stableContentKey(tex);
if (dumpEnabled)                                       // RT_APP_TEXTDUMP
    writeDumpFile(dumpDir, key, tex.width, tex.height, tex.pixels, DumpFormat::PamRgba);

if (packIndex.valid()) {                               // rescan on pack switch
    ReplacementRequest req = requestReplacement(packIndex, key, tex.width, tex.height);
    if (req.verdict == ReplacementVerdict::Replace) {
        // GPU wave: decode req.path (image bytes are NOT read by the logic
        // layer), verify its dims (req.width/height or the header for bare
        // fallbacks), buildMipChain() and upload with the sample policy.
        uploadReplacement(req, /*pack pixels*/);
        return;                                       // sampled instead of native
    }
    // Miss / StaleMismatch: fall through to the native texels.
}
uploadNative(tex);
```

Preview seam (identical shape): `preview.cpp:698` `ensurePageTexture` hashes
the page and caches the VkImage; the glue mirrors that key (`hashTexture`
mixes dims+RGBA like the stable feed) through `requestReplacement` and, on
Replace, uploads the pack image + chain instead of `tex.rgba` — same sampling
cache, one extra index lookup per content.

## 8. Budget / eviction sketch for the GPU wave (AUTHORED, sibling-precedent)

Numbers below are the sibling app's proven production values, proposed for
the native wave [AUTHORED — no native measurement exists yet]:

- Create replacement images at most **24 MB per frame** (pending ones wait);
- Keep replacements within a **2 GB budget** (env-tunable in the sibling,
  `RT_TEXTURE_PACK_BUDGET_MB`; native name when the wave lands:
  `RT_APP_TEXPACK_BUDGET_MB`);
- Evict **oldest-first** any replacement **unused for 2 s**, down to
  **80 %** of budget, then invalidate the native texture cache once so
  predictions rebind — native equivalent of the sibling's per-page upload
  cache rebuild;
- A "pack usage" counter (how many of the pack's images have been used) is
  already derivable: entries requested at least once / `index.size()`.

The native decode-at-load model is simpler than the sibling's GS-readback
races: nothing dumps mid-frame, no palette-instance churn can re-key a bound
texture, and the scroll-cycle case (sibling `m_knownMiss`) does not exist —
native textures are keyed once per decode.

## 9. How pack keys survive future texture re-decodes

The key is a function of **content only**. Consequences:

- Bit-identical re-decode (new loader location, folder shuffle, streaming
  order): same key, existing packs and dumps keep working untouched. This is
  what makes the dump-then-upscale workflow durable across app changes.
- Deliberate decode fix (palette convention, alpha expansion, REGION
  handling): the decoded texels change ⇒ the key changes ⇒ old pack entries
  simply stop matching (inert; no mis-binding possible, since a wrong-content
  replacement never wins a byte-exact key match). Pack folders gain dead
  files; packs/dumps must be re-dumped once per decode fix. `kStableKeyLayoutVersion`
  makes feed changes the same, plus explicit.
- Because `kStableKeyLayoutVersion` is byte 0 of the feed, "which key space am
  I in" is self-documenting; do NOT bump it casually — it re-keys everything.

## Ledger

**CONFIRMED** (sibling precedent, ported):
- Content-hash naming over decoded texels: same texture ⇒ same name across
  runs/disc locations; palette swap ⇒ different texture; hashes AFTER palette.
- Pack = flat folder of images keyed by 16-hex prefix, any size, own alpha.
- Full mip chain + trilinear/aniso (default 16) for replacements; clamped axes
  sample clamp-to-edge (GPU wave resolves per axis); replacement addressed by
  the original texture's size; pack/dump must never change a pixel; one pack
  active at a time; collision = one image per key, last wins, surfaced.
- Budget/eviction values (24 MB/frame create cap, 2 GB budget, evict after
  2 s unused to 80 %) proposed unchanged.

**AUTHORED** (native-first, in this module):
- Keys from the disc-decode pipeline (`ImageRecord` after palette), never GS
  uploads; feed = layout version + format tag + dims + texels; 16-hex
  lowercase filename spelling; strict parse grammar incl. dims caps.
- One dims pair in pack names = the pack image's own dims (sibling names
  keep the dumped dims); optional `_src` token + StaleMismatch guard;
  bare-name fallback with load-time dims.
- Deterministic lexicographic last-wins (sibling uses OS walk order).
- Mip chain: no PoT padding, floor-halving, clamped box taps, `(sum+2)>>2`.
- Dump formats: PAM (RGBA default) + PPM (snapshot-writer parity), zero new
  third-party deps; extension acceptance `png/pam/ppm`.

**OPEN** (GPU wave, out of scope here):
- PNG/PAM/PPM decoding of pack files (the logic layer never reads pixels);
  an ASTC/BC7 transcode cache (sibling proves the win on Android/GL-class
  GPUs) — sizeable follow-up, needs its own evidence pass.
- Per-draw clamp-state plumbing (REGION_CLAMP semantics of the native draw
  path) and the HUD-atlas region semantics under replacement.
- Actual native budget measurements; `RT_APP_TEXPACK_BUDGET_MB` naming.
- Options-menu surface ("Texture pack" row), launch-time pack warm-up UX.

## As implemented in the native app (2026-10-28)

`assets/texture_pack.{h,cpp}` (+ `assets/png_decode.{h,cpp}`) is the app glue,
`preview/texture_pack_bridge.h` the renderers' seam. hd_pack keeps owning the
naming/index/verdict contract; nothing here invents a second one.

- **Selection**: `RT_TEXTURE_PACK=<folder|name>` — an existing directory is used
  as-is, otherwise the name is a folder under `<data>/textures/packs`.
  `RT_TEXTURE_DUMP=<dir>` writes every texture the renderer uploads as
  `<key>_<W>x<H>.pam` (PAM keeps alpha), on the same naming a pack folder
  accepts. Both are loaded once at startup; there is no live switch and no
  Options row yet (the row needs a dynamic folder list in the catalogue).
- **Where the lookup happens**: on a texture-cache MISS only, in both backends
  (`preview_vk.cpp` `ensurePageTexture`, `preview_gl.cpp` `textureFor`), so a
  frame pays nothing for an inactive pack. The cache key mixes the pack's
  generation, so a switch can never serve the previous pack's image.
- **Formats**: PNG (8-bit, colour types 0/2/3/4/6, PLTE + tRNS honoured,
  non-interlaced) and PAM P7 (depth 1..4) / PPM P6, all decoded to RGBA8 with
  the file's own alpha. 16-bit and interlaced PNGs are refused with a logged
  reason rather than guessed at (`assets/png_decode.cpp`, its own inflate — no
  zlib/libpng dependency ships). A pack file is untrusted input: images are
  capped at 16.7M pixels (4096x4096 RGBA8 = 64 MB), the IHDR is checked the
  moment it is read, image data before the IHDR is refused, an IDAT above
  2*raster + 64 KiB is refused before it is kept, the inflate never expands
  past that raster, and a file above 72 MB is never read, so a corrupt or
  hostile file cannot allocate past its own stated size. These are resource
  caps, not a proof about every legal stream: a valid image padded with
  empty deflate blocks is refused too, and a pad adds no picture.
- **Fallback**: a miss, a stale `_src` name, an unreadable file and a refused
  image all draw the native texture and log once. Verified end to end on
  `race@110` (headless, Vulkan **and** GL): dumping 146 textures leaves the
  snapshot byte-identical to the golden; a pack made from those dumps with
  every texel forced magenta changes 421,929 of 1,440,015 bytes (29 %) with
  the same count on both backends; an empty pack folder is byte-identical
  again. Unit contract test: `native_assets_texture_pack_test` (76 checks).
- **Not implemented yet** (the OPEN list above, unchanged): mip chains and
  anisotropic sampling of replacements, per-axis clamp policy, the ASTC/BC7
  transcode cache, the budget/eviction window, the Options "Texture pack" row
  and launch-time warm-up.
