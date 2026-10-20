# First-run disc intake and extraction (native rebuild)

The native app ships **no game data**. At first run it is pointed at the user's own
dump; intake verifies it is Road Trip (USA, SLUS-203.98) and extracts the native
asset set into the app data dir. Implementation: `src/native/assets/iso_extract.{h,cpp}`
(reader/gate/extractor) and `src/native/assets/setup_wizard.cpp` (source selection).

## Accepted sources

- `RT_DISC=<dir>` — a pre-extracted disc directory; extraction is skipped.
- `RT_DISC=<file>` — an `.iso`, or a `.bin` with its sibling `.cue`, or the `.cue`
  itself. The needed tree is extracted into `<data>/disc` (the boot-ELF gate runs
  inside the extractor), then `RT_DISC` is re-pointed at that directory so every
  later reader sees the same shape (setup_wizard.cpp ~line 157).
- Auto-scanned **directory** candidates: `<data>/rom`, `<data>/disc`,
  `/storage/emulated/0/Download/RoadTripDisc`. A bare `.iso` found in Downloads is
  only logged ("skipped-not-yet"): auto-selecting image *files* is deferred until
  the setup-UI file picker lands with W14 parity. `shortcut:` until then, set
  `RT_DISC` to the image path explicitly.

## Gate

The hard gate is the boot ELF: entry `0x200008`, CRC32 `0x5A49851D` (metadata-only
constants, same facts as the sibling `RomInstaller.h`; nothing from the disc is
embedded or committed). The extractor reads the ELF out of the image and refuses a
non-matching disc before extracting anything.

## Reader

Authored ISO9660 (ECMA-119) reader — 2048-byte logical sectors, PVD at sector 16;
`.bin` carries raw 2352-byte sectors with Mode 1 (data at +16) or Mode 2/XA
(+24) data offsets; extraction streams in 256-sector (512 KiB) chunks. The reader
semantics are an authored port of the sibling product's `src/rom/IsoReader` where
noted in comments — **without libchdr** (coordinator decision).

Statements marked `[disc-verified]` in the sources were observed read-only against
the user's own dump at development time.

## CHD (deferred)

`.chd` is refused up-front by magic sniff with a typed `NotReadable`, surfaced to
the user as: convert with `chdman extractcd -i disc.chd -o disc.cue` and point
`RT_DISC` at the `.cue`/`.bin`. libchdr is a permitted optional dependency but is
not needed for the accepted formats; the sibling (oracle) product's CHD support
stays on that side.

## Tests

`tests/native_assets/test_disc_intake.cpp` builds synthetic ISO fixtures in code —
no real disc bytes in the repo, ever.
