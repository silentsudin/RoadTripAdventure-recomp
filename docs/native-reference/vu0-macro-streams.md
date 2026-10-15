# VU0 macro streams — negative result, VIF grammar pinned

Updated 2026-10-15. This pass scanned the user's read-only SLUS-20398 disc
extract: all **64** `FLD/*.BIN` files and all **19** `COURSE/*.BIN` files.
There is no `CAR/` directory in that extract.

## PROVEN: the parsed FLD streams are VIF model streams, not VU0 uploads

The 65-entry FLD directory identifies DMA CNT blocks.  A block's VIF FIFO
stream is the DMA tag's upper 64 bits followed by its `QWC * 16` inline bytes;
the block ends at a zero-QWC RET tag.  The scanner accepted **4,295** such
blocks from FLD and decoded **757,487** UNPACKs and **370,032** microprogram
calls without desynchronising.  It found **zero MPG commands**.

`MSCALF` calls dominate, with these exact immediate values:

| VIF command | immediate | FLD count | Reading |
| --- | ---: | ---: | --- |
| `MSCALF` | `0x0008` | 360,194 | VU model program call (the model dumper's entry 4) |
| `MSCALF` | `0x000A` | 4,807 | entry 5 |
| `MSCALF` | `0x0006` | 4,053 | entry 3 |
| `MSCALF` | `0x0004` | 978 | entry 2 |

This agrees with the known VU1 renderer entries in `docs/vu1-microcode.md` and
with `tools/model_dump.cpp`: these are model/terrain draw submissions, not
calls to an asset-supplied VU0 program.  In particular, terrain-packet
material data is sent as ordinary VIF data: `UNPACK V3-32` to VU address 1
(with varying vector count) and `UNPACK V3-32` to address `0x192` (4 or 5
vectors) are the frequent stream forms.  They do not encode texture UV
generation code.

The scanned COURSE CNT blocks have the same result: **539** blocks, **209,364**
UNPACKs, **101,803** calls, and **zero MPG commands**.  Their `MSCALF`
immediates are `8` (98,686), `6` (1,993), `10` (568), and `4` (556).

## PROVEN: VIF encoding needed to find an uploaded microprogram

For a little-endian 32-bit VIF command word:

```text
irq = word bit 31
cmd = (word >> 24) & 0x7f
num = (word >> 16) & 0xff        // zero represents 256 where applicable
imm = word & 0xffff
```

- `cmd == 0x4a` is **MPG**, the VIF microprogram upload command.  `imm` is the
  micro-memory destination slot and the immediately following `num-or-256`
  64-bit instruction pairs are the source byte range to carve and send to
  `vu-dis ... vu0`.
- With IRQ set the byte in the file is `0xca`; it is still `cmd == 0x4a` after
  masking bit 31.  A `0xca` byte sequence alone is not evidence of MPG: it
  must occur at the command position of a structurally valid VIF stream.
- `cmd >= 0x60` is **UNPACK**, which writes VU *data* memory.  Its destination
  is `imm & 0x3ff`; bit 15 selects TOPS addressing and bit 14 is unsigned for
  the packed integer forms.  It cannot upload VU instruction memory.

The command/payload handling above is cross-checked against the local
jak-project `common/dma/dma.h` and the stream walker in
`tools/model_dump.cpp`.  The latter's byte accounting is used here for every
UNPACK format and for `STMASK`, `STROW`, `STCOL`, `DIRECT`, and `MPG`.

## VU0 upload inventory

| corpus | valid VIF chains | MPG uploads | distinct carved bodies | VU0 disassemblies |
| --- | ---: | ---: | ---: | ---: |
| `FLD/*.BIN` | 4,295 | 0 | 0 | 0 |
| `COURSE/*.BIN` | 539 | 0 | 0 | 0 |
| total | 4,834 | **0** | **0** | **0** |

Therefore `/tmp/rt-vu0-macros/` contains no retained `macroNNN.asm`: there is
no structurally valid VIF `MPG` source range to carve.  The raw `0x4a`/`0xca`
matches outside parsed VIF streams were deliberately rejected; candidate
bodies either fail `vu-dis`'s opcode assertions or are non-code data.  No raw
game bytes or disassemblies are stored in this repository.

## UV/material result

**No authentic terrain UV calculation was found in FLD or COURSE data.**  The
terrain-facing FLD streams call the known VU1 model entries and upload vertex
and material/GIF data; they contain no streamed VU0 program.  Consequently
this pass cannot connect PMP packet `materialRef` to a texture or replace the
converter's planar UV stand-in.  It does establish that searching for a VU0
`LQD`/`MADD`/`CLIP`/`FTOI` macro in these two parsed corpora is the wrong next
step.

## OPEN / exact next steps

1. Trace the EE-side `VCALLMS` sites and their preceding VU0 instruction-memory
   loads.  The executable may provide the VU0 microprograms, or it may load a
   different streamed asset class not present in this FLD/COURSE chain grammar.
2. For the FLD texture task, trace the ordinary VU1 material path from the
   `UNPACK V3-32 @ 0x192` A+D/GIF register data and the 256-entry attribute
   table, rather than looking for a VU0 macro upload.
3. If another asset type is found, require a structural VIF parse followed by
   `MPG` carving (slot, instruction count, source range, SHA-256) before
   treating any `0x4a`/`0xca` byte pattern as microcode.
