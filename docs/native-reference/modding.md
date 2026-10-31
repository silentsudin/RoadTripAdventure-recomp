# Modding framework — decision (issue #22)

Status 2026-10-31. Answers "what a mod may replace" for the native rebuild, and
what it may never do. Issue #22 asked for the decision first; this doc is it.

The one mod kind shipping today is the **HD texture pack**
([`hd-texture-packs.md`](hd-texture-packs.md)): a folder of PNG/PAM/PPM images
named by content hash under `<data>/textures/packs/<name>/`, chosen in
Options > Graphics > Texture pack or `RT_TEXTURE_PACK`. That is the shape every
later mod kind follows.

---

## 1. A mod is files, never code

- A mod is a directory `<data>/mods/<id>/` holding `mod.toml` and a `files/`
  tree. No archives (no zip/7z to parse), no DLL/so/dylib, no bytecode patches
  applied to our shipped routines.
- Reason: every shipped routine is a decompiled translation, and nothing in the
  product may load guest code or borrow a recompiler's function table
  (AGENTS.md hard rule). A code-mod ABI would also outlive no invariant we can
  promise, since the decomp is still moving.
- Game scripts (GSL/TSQ bytecode, [`gsl-vm.md`](gsl-vm.md)) are *data* to the
  native VM and may therefore be replaced — as validated containers, section 5.

## 2. Layout

```
<data>/mods/<id>/mod.toml      # manifest
<data>/mods/<id>/files/<disc path>   # e.g. files/CAR0/Q00.BIN, files/SYS/FONTE.GSL
```

```toml
[mod]
name = "HD cars"
version = 1                 # manifest schema
target_elf = "<sha1>"       # optional: refuse to apply to another game build
```

- `<disc path>` is the path inside the disc image, upper case as the game spells
  it (the extraction tree in `first-run-extraction.md` uses the same names).
- Unknown keys are ignored; a malformed manifest rejects the whole mod.
- Enabling is `[mods] enabled = ["<id>"]` in `settings.toml`. Nothing is applied
  implicitly; a mod folder that is not listed has no effect.

## 3. Where an overlay lands

One seam, the extraction boundary: the disc is read once into the data
directory, and the overlay is applied after extraction, per file, before a
loader ever sees it. Textures keep the second seam they already have — the
decode-time pack lookup — because their disc form and their sampled form differ.

Loaders that consume disc files (car models, field tiles, GSL packs, audio)
already validate their own structure; an overlay file gets exactly the same
checks a disc file gets, plus the caps below. Nothing downstream needs a mod
branch.

## 4. Trust boundary

Mod files are untrusted input, like a picked disc image:

- caps: the texture pack's (`kMaxPackPixels = 1 << 24`, `kMaxPackFileBytes =
  72 MiB`, `png_decode.h:51,56`); other file kinds reuse them per decoded image
  or per file.
- paths: relative, canonicalised, no `..` segment, no absolute path, no name
  outside the extraction tree. A path that fails is a mod error, not a crash.
- failures are reported per mod with a reason and the run continues without it;
  a mod never half-applies (validate the whole manifest and file list first).

## 5. What may be replaced, in order

| kind | status | notes |
|---|---|---|
| textures (PNG, any size, per-hash) | **implemented** | packs; no disc-file overlay needed |
| audio: VAG streams, TSQ/TVB banks | next | same overlay at extraction; the decoders exist (`rt_audio_extract`, `tsq-tvb-format.md`, `audio-map.md`) |
| models/animation: `CARn/*.BIN`, `FLD/*` | next | overlay after the existing structural parse; a bad file falls back to the disc's |
| GSL/TSQ script containers | later | overlay only when the VM's own grammar check passes; record the mod's hash in the log |

Tables the code reads as data (tuning, race rules, car stats) are **not** a mod
surface: they live in decompiled code and in `settings.toml`-backed options, and
a table overlay would be a second, unversioned ABI.

## 6. Never

- Ship or commit game data, in any re-encoded form (AGENTS.md).
- Load code, patch instructions, or expose the recompiler's tables.
- Redistribute mods that contain the game's own assets: a mod is the user's
  own files, on the user's own machine.

## 7. Open decisions

- Whether a mod may add *new* content (a texture the game never uploads has no
  hash to match yet; today it is simply never requested).
- Whether mods get an in-game page (Options > Mods) or stay file-driven; the
  pack row is the precedent and listing folders is cheap.
- Conflict order when two mods provide the same path: first in `enabled` wins
  (proposed) — not decided.
