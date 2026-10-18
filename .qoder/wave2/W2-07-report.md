# W2-07 — Dialogue & text engine: report

Branch `re/native-complete`, working tree only (no git state changed). Author: batch W2-07 sub-agent (claude-sonnet-5-5).
All numbers below were produced in this session; every one has a repro command or snippet in the appendix. Text found in the ROM was treated as data only.

## 1. Summary

* `text_script` (`src/native/gameplay/text_script.{h,cpp}`) ports the original text path from the ELF: glyph advance/sprite geometry (`uiDrawGlyph`, `uiTextWidth`, `uiDrawTextShade`, `uiDrawNumberAuto`), `RenderEscapeText` (3-line pages, `$` codes, typewriter limit, scroll window), the whole message-window task `dialogueTask` (states 0..6, 24 command bytes), the window frame/arrow geometry, and the three string helpers.
* **Validation method**: a ~200-line MIPS interpreter executes the *real ELF instructions* of those functions on synthetic inputs (seeded) and writes numbers to `tests/native_w2/data/text_script_vectors.txt` (13 306 vectors incl. 48 full message-window scenarios of 9 376 ticks). The C++ port reproduces **all** of them bit-exactly. The fixture regenerates byte-identically (checked with `cmp`).
* **Headline finding**: the original has **no automatic word wrap**. Lines are hand-broken (`\n`) in the data; a page is exactly 3 lines of 24 px; advance is the proportional table `0x29A880` at 1:1. A greedy wrap at any single width is consistent with ≤ 133/1 613 original multi-line messages.
* Acceptance line (this machine, ELF present):

```
W2-07 SCORE advance_table=96/96 wrap_oracle_within2px=1.00(16/16) escapes=63/63 bounded_copy_fuzz=10000/10000 deterministic=1 oracle_vectors=13306/13306 elf_msgs=2476/2476 positions=2/2 fullwidth=96/96 font_glyphs=94/94 font_lower=26/26 font_slots=91/91
```

## 2. Files

| file | status |
|---|---|
| `src/native/gameplay/text_script.h`, `.cpp` | new (staged in `src/native/CMakeLists.txt` Step-0 list) |
| `tests/native_w2/text_script_test.cpp` | new → target `w2_text_script_test` |
| `tests/native_w2/data/text_script_vectors.txt` | new, 0.97 MB of numbers (no ROM text; synthetic inputs) |
| `tests/native_w2/data/text_extents.csv` | new, 16 rows of measured pixel extents |
| `docs/native-reference/text-script.md` | new |
| `.qoder/wave2/W2-07-report.md` | this file |

Nothing else was written (no CMake, `main.cpp`, third_party, build/native, journal/board edits). Private build dir `build/w-W2-07`.

## 3. Key numbers and how to reproduce

```
cmake -S src/native -B build/w-W2-07 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-07 --target w2_text_script_test -j
RT_ELF="$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98" build/w-W2-07/w2_text_script_test --verbose; echo $?
```
(`RT_ELF` overrides `--elf`. Without an ELF: prints `NO_ELF`, runs the ROM-independent checks, exit 0 if they pass. Mutation check: flipping one digit of one `COPY` vector → exit 1; missing data dir → exit 1.)

| acceptance key | value | threshold | what it is |
|---|---|---|---|
| `advance_table` | **96/96** | n/n | `uiGlyphWidths` `0x29A880..0x29A8DF` vs `kAdvanceTable` |
| `wrap_oracle_within2px` | **1.00 (16/16)** | ≥ 0.90 | predicted ink width vs measured extent, strings of `main_menu`/`options`/`factory_menu` (±2 UI px). mean +1.25, max 2.0 |
| `escapes` | **63/63** | n/n | 36 semantic vectors (every `$` code class, counts, limit, page end) + 22 jump-table slots `0x2EC6C0` (21 codes A..U: exactly A G N R S T U live, 7 distinct bodies) + 5 census checks (every `$X` in the ELF text ∈ {A G N R S T U}) |
| `bounded_copy_fuzz` | **10000/10000** | 10000/10000 | never exceeds capacity, never splits a pair, prefix of the unbounded reference, stops only by cap |
| `deterministic` | 1 | 1 | whole suite twice, digest equal |
| `oracle_vectors` | 13306/13306 | all | emulated-ELF fixtures: glyph 478, width 300, number-cells 22, render 1 270, runaway 360, extract 500, copy 700, subst 300, window ticks 9 376 (+ 37 frame/icon vectors) |
| `elf_msgs` | 2476/2476 | all | every ELF message with a clean prefix pages to a command byte in the table |
| `positions` | 2/2 | all | dialogue pen x 70 and plate right-alignment x 574 vs oracle (≤ 2 UI px) |
| `font_glyphs/lower/slots` | 94/94, 26/26, 91/91 | all | FONTE atlas has all 94 glyphs (lower case included) and the sprite (u,v) of the drawer equals the slot formula |

Census of the ELF script text (`census_wrap.py`, independent of the C++): 307 level-2 tables, 2 478 distinct message starts, 2 476 clean prefixes (the 2 unclean distinct starts belong to the 3 non-clean references of `script-vm.md` §6), 9 366 text lines (80 contain `$`), 5 779 pages; 9 260/9 286 `$`-free lines (99.7 %) fit the 498-px window interior; `$` codes: G 51, U 16, T 12, S 2, R 1 (A, N never); command bytes: 24 distinct, all in `kCommands` (00:698, 0C:726, 01:312, 10:226, 09:156, 11:100, 07:90, 0D:90, …).

## 4. Ruling (a): op-handler width discrepancy `script-vm.md` §5.1 vs `gsl-vm.md` §5.1

**Ruling: `script-vm.md` is correct; `gsl-vm.md` §5.1 is wrong for ops 0x03 and 0x08 and agrees on every other width.**

Method (ROM bytes, no inference): `arbitrate.py` (appendix) executes the **real `InterpretScriptCommands` 0x23AD00..0x23B167** from the ELF in the MIPS interpreter. For each opcode the operand bytes are `0x21..0x28` (≥ 0x20, so a skipped operand can never decode as an op), the 23 `jal` targets inside the function (22 callees + `printf`) are stubbed (all-0, all-1, and op-specific settings to reach each arm), the branch table maps index *i* to a unique marker address. "Skip width" = returned cursor − start when the branch is **not** taken; "taken" = the table index the returned cursor equals.

| op | real skip width | taken branch = operand # | script-vm.md | gsl-vm.md | verdict |
|---|---:|---|---|---|---|
| 01 02 05 06 0B 0D 19 1A 1C | 3 | 2 | 3 | 3 | agree |
| **03** | **4** | **3** | 4 (`slot,bit,idx`) | **3 (`a,b`)** | gsl-vm wrong: op 03 has three operands; the decomp reads `pbVar8[3]` as the index and skips `pbVar8+4` |
| 04 07 16 18 1B | 2 | 1 (18: skip when 54000 ≤ ctx[+0x14] < 198001) | 2 | 2 | agree |
| **08** | **3** | **1 or 2** | 3 (two branch idx, 3-way) | **2 ("idx + optional skip")** | gsl-vm wrong: `Match(town,shop)≠0 → table[op1]`, else `Fcb8≠0 → skip 3`, else `table[op2]` (run: all-0 → idx op2, all-1 → idx op1, Match=0/Fcb8=1 → skip 3) |
| 0E | 7 | 2 (= `1 + FindCurrentTaskIndex()`) | 7 | "6-byte span" (= width 7) | agree (wording only) |
| 0F 10 13 15 17 | 2 | – | 2 | 2 | agree |
| 11 14 | 3 | – | 3 | 3 | agree |
| 12 | 1 | – | 1 | none | agree |
| 1D | 4 | 3 | 4 | 4 | agree |

(`gsl-vm.md` widths were derived as 1 + number of listed operands.) The role descriptions of `gsl-vm.md` for 0x18/0x1B ("STRONG") are consistent with `script-vm.md`'s decoded ones. The same emulation also reproduces the unknown-op rule exactly (ops 0, 9, 0x0A, 0x0C, 0x1E, 0x1F).

## 5. Ruling (b): what ops 9, 0x0A, 0x0C (JT `0x2EC750`, shared target `0x23B0E0`) do in text/dialogue terms

**They do nothing for the text.** They are unassigned slots of the *pre-control prefix* namespace; the handler (`0x23B0E0`) is the generic unknown-op stub. Real-code results (`arbitrate.py`, `unknown_cases`):

| script bytes | returned cursor offset | printf calls |
|---|---:|---|
| `09 5a` | +1 | `"Unknown pre-control code %d in town T%02d shop %d\n"` (9, shop+0x22, shop+0x21), then `"%s\n"` of the new cursor |
| `0a 0a 0a 5a` | +3 | same, code 10 |
| `0c 01 21 22 5a` | +2 | same, code 12 — the following control byte `01` is **swallowed**, text starts at `21` |
| `09 05 0a 0f 5a` | +4 | all following bytes < 0x20 swallowed, text starts at the first printable |

Meaning for dialogue: the opcode byte is consumed; if the next byte is < 0x20 the cursor skips it and *every following control byte*, else it stays right after the opcode; no state changes, no text escape, no command. Practical corollary: **a message whose text begins with a blank line (`\n`, = op 0x0A) loses its leading newlines and logs the diagnostic**. None of the 2 478 ELF messages triggers it (ops {0, 9, 10, 12, 0x1E, 0x1F} never occur in a prefix, matching W2-01). They are **not** related to the end-of-text *command* bytes: commands are a separate namespace dispatched by `dialogueTask` (cmd 0x09 = tab-list menu, 0x0C = goto, 0x0A = typewriter default) that happens to reuse the numbers 0..0x18. The same note closes the W2-01 open question: the 3 non-clean script references cannot be reached through text escapes (escape codes only draw; none changes the cursor).

## 6. (c) FONTE facts

* Pitch 16×24: **confirmed** (the drawer's `u = (g%31)·16`, `v = (g/31)·24`; `font_slots=91/91`).
* "Uppercase-only, lowercase maps to slot < 0 and is silently skipped": **not supported by the ELF**. `uiDrawGlyph` has no lower-case special case; `fonteSlotForChar` already returns a valid slot for `0x61..0x7A`; the atlas cells are non-empty (26/26) with proper x-height shapes (e.g. `u` ink rows 6..19 vs `W` 0..19) and the oracle prints "Quick Race", "What do you want to do?". The only skipped input in the original is **space** (advance 8, no quad) and bytes outside `0x21..0x7E` (handled by the Shift-JIS / control paths). If a native renderer drops lower case, that is a native bug, not original behaviour.

## 7. Other corrections / discoveries (all evidenced)

1. **Message-window arrow tab is at (304, 416)**, not (304, 392) (`hud-layout.md` §1.4): disassembly `addiu $v0,$s2,0x48` with `s2 = 0x158`, and the emulated frame event `S 304 416 88 256 32 24`. The blinking arrow icon is `(310, 420)` atlas(260,180) 20×20.
2. `dialogue.h::boundedText` ("passes bytes through unchanged", "keeps 2-byte escapes") misdescribes `CopyDialogueTextBounded`: it is an ASCII → full-width Shift-JIS widener with a character limit; 0 static callers.
3. Menu text is proportional at 1:1 (advance table), not mono at `scaleX 0.77`; `menu-calibration.md`'s "12.25 UI per cell" is the mean proportional advance ("Quick Race": 128 px of advance, 123 px ink). The claim "selected label is drawn 6 UI further left" is an artefact of the selection bar's bright end-cap (scan 912..922); measured with `x ≥ 920, B ≥ 170` the selected "Adventure" starts at scan 924 like the others (UI 462) — **likely** (visual crop checked), not re-verified against `buildMainMenu`.
4. Original-code hazards found by running it (the native port bounds all of them, reports instead of hanging): (i) the line-skip loop of `RenderEscapeText` never terminates when `scroll/12` exceeds the number of lines or the text ends right after its last newline (360 vectors); (ii) `SubstituteNameTokens` copies the tail through a 48-byte stack buffer; (iii) `CopyDialogueTextBounded` with `limit` > length reads past the NUL; (iv) an accent prefix (`^ ` ~ /`) in dialogue text is drawn as the composite *and* the plain follower (RenderEscapeText steps 1 byte, uiDrawGlyph looks ahead 1); (v) `$` at end of text swallows the NUL.
5. The emulator initially mis-implemented R5900 `mult rd,rs,rt` (3-operand form writes `rd`), which silently broke `GetSlotDataPtr` and the plate-width animation; this was caught because the C++ port disagreed with the first fixture (SUBST 171/300, window 8758/9518), the interpreter was fixed (`mult` now writes `rd`), the fixture regenerated, and every class now agrees. (Evidence that the oracle is not just a copy of the port.)

## 8. Confidence

| item | confidence |
|---|---|
| glyph geometry/advance, `$` codes, page/typewriter/scroll rules, command table, frame geometry | **high** — bit-exact against emulated ELF code on 13 306 vectors; census over all ELF messages |
| arbitration (a), (b) | **high** — real code executed, per-op results tabulated |
| pixel-width oracle | high for the 16 strings (mean +1.25 UI, max 2.0; predicted ≥ measured in 16/16 and **5 of 16 sit exactly at the +2.0 limit** — a systematic anti-aliasing-fringe offset, not a layout error) |
| window-frame arrow position correction | high (disassembly + emulation), not seen in an oracle dialogue frame at the arrow (the golden shows the plate and frame, tab visible at scan x ≈ 608..672) |
| sound ids, palette banks | recorded constants only |

## 9. Open questions / GAPs

* `uiKanaGlyphUV` / `uiKanjiGlyphUV` (Japanese sprites) not decoded; only advance 20 is pinned.
* Semantics of the `$G` (progress+0x646) and `$U` (progress+0x63F) strings and of the `$A`/`$T` id words.
* Palette banks (frame 7, text 10, plate/arrow 0) and sound ids (0x25 open, 0x23 blip, 0x1A confirm, 0x26 close, 0x306 / 0x303) are constants without semantics; `MessageWindow` emits them as `Effect`s for the audio batch.
* Child task functions (0x239260 menu message, 0x23BC90 yes/no, 0x239900, 0x23A320, 0x239A80, 0x23BCD0, 0x23BE18, 0x23BE88, 0x239748, 0x239F68, 0x23A140, 0x23A460, 0x229538, 0x22BFD0, 0x269E88) are named only by address; their ports belong to W2-12/W2-13 style batches.
* Integration (coordinator): bind `runPrefix` to `gameplay::runScriptPrefix` (W2-01) and `IEscapeHost` to `SaveData`; `ITextMemory` to `assets::ScriptImage`.
* Native `wrapGreedy` is deliberately unused by the original-text path; use it only for native-authored strings.

---

## Appendix A — oracle tooling (all files are used verbatim; save into one directory, run from the repo root with `capstone`, `numpy`, `Pillow`)

Layout: `emu.py oracle.py gen_fixtures.py mipsdis_helper.py arbitrate.py atlas.py meas.py extents.py scripts.py census_wrap.py mipsdis.py`. Do **not** name any of them `dis.py` (shadows the stdlib).

```
PYTHONPATH=<dir> python3 <dir>/gen_fixtures.py /tmp/vec.txt && cmp /tmp/vec.txt tests/native_w2/data/text_script_vectors.txt   # ~2 min, byte-identical
PYTHONPATH=<dir> python3 <dir>/extents.py /tmp/ext.csv && cmp /tmp/ext.csv tests/native_w2/data/text_extents.csv                 # needs build/scratch/oracle-golden/*.png
PYTHONPATH=<dir> python3 <dir>/arbitrate.py                                                                                        # §4/§5 tables
PYTHONPATH=<dir> python3 <dir>/census_wrap.py                                                                                      # §3 census
python3 <dir>/mipsdis.py 23a8e8 23ad00                                                                                            # disassembly of RenderEscapeText
```
Fixture line formats: `GLYPH bytes x y2 shade nextX [S,x,y,u,v,w,h]`, `WIDTH bytes w`, `RENDER scroll limit course text cursor|- newLimit nEvents fnv`, `RUNAWAY scroll limit course text` (original does not terminate), `SCN/MSG/TICK` (window scenarios; ticks list pad, child-done, state, scroll, limit, cursor, pending, event count/hash, ordered effects, memory effects) — see `gen_fixtures.py`.

### emu.py — MIPS (R5900 subset) interpreter

```python
#!/usr/bin/env python3
"""Tiny MIPS (R5900 subset) interpreter used as the ELF oracle for W2-07.
Executes the *real* instructions of text-engine functions from SLUS_203.98
with libc/GS-writer stubs.  Reads the ELF at vaddr-0x1FF000; RAM overlay is a dict."""
import struct, sys, os
from capstone import *
from capstone.mips import *
ELF=os.environ.get("RT_ELF") or os.path.expanduser("~/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98")
BASE=0x1FF000
M64=(1<<64)-1
def s32(v): v&=0xFFFFFFFF; return v-(1<<32) if v&0x80000000 else v
def s64(v): v&=M64; return v-(1<<64) if v>>63 else v
class Emu:
    _img=None; _cache={}; _md=None
    def __init__(s):
        if Emu._img is None: Emu._img=open(ELF,'rb').read()
        s.img=Emu._img
        s.ram={}            # byte overlay
        s.r=[0]*32; s.hi=0; s.lo=0
        if Emu._md is None:
            Emu._md=Cs(CS_ARCH_MIPS, CS_MODE_MIPS64+CS_MODE_LITTLE_ENDIAN); Emu._md.detail=True
        s.md=Emu._md
        s.cache=Emu._cache
        s.sprites=[]; s.numbers=[]; s.stubs={}; s.steps=0; s.watch=set(); s.stores=[]
        s.r[28]=0x00340000   # gp
        s.w32(s.r[28]-0x5470,0x03000000)  # uiBatchPtr
        s.w32(s.r[28]-0x546c,0)           # uiBatchCount
    # memory
    def rb(s,a):
        a&=0xFFFFFFFF
        if a in s.ram: return s.ram[a]
        o=a-BASE
        if 0<=o<len(s.img) and (0x200000<=a<0x29A848 or 0x29A880<=a<0x33567B): return s.img[o]
        return 0
    def wb(s,a,v): s.ram[a&0xFFFFFFFF]=v&0xFF
    def r16(s,a): return s.rb(a)|s.rb(a+1)<<8
    def r32(s,a): return s.r16(a)|s.r16(a+2)<<16
    def w16(s,a,v): s.wb(a,v); s.wb(a+1,v>>8)
    def w32(s,a,v): s.w16(a,v); s.w16(a+2,v>>16)
    def cstr(s,a):
        o=bytearray()
        while True:
            b=s.rb(a)
            if b==0: return bytes(o)
            o.append(b); a+=1
    def put(s,a,data):
        for i,b in enumerate(data): s.wb(a+i,b)
    def dec(s,pc):
        i=s.cache.get(pc)
        if i is None:
            raw=bytes(s.rb(pc+k) for k in range(4))
            i=next(s.md.disasm(raw,pc)); s.cache[pc]=i
        return i
    def note(s,m,a,v):
        if a in s.watch: s.stores.append((m,a,v))
    def reg(s,x): return 0 if x==0 else s.r[x]
    def setr(s,x,v):
        if x: s.r[x]=v&M64
    def call(s,addr,args=(),maxsteps=2_000_000):
        regs=[4,5,6,7,8,9,10,11]
        for i,a in enumerate(args): s.r[regs[i]]=a&M64
        s.r[29]=0x01F00000; s.r[31]=0xFFFFFFF0
        pc=addr; s.steps=0
        while pc!=0xFFFFFFF0:
            pc=s.run_one(pc)
            s.steps+=1
            if s.steps>maxsteps: raise RuntimeError("step limit")
        return s.r[2]
    def run_one(s,pc):
        if pc in s.stubs:
            ret=s.stubs[pc](s)
            if ret is not None: s.r[2]=ret&M64
            return s.r[31]&0xFFFFFFFF
        i=s.dec(pc); m=i.mnemonic; ops=i.operands
        def R(k): return s.reg(ops[k].reg and REGMAP[ops[k].reg])
        nxt=pc+4
        # branch handling
        br=m in ('beq','bne','beqz','bnez','beql','bnel','blez','bgtz','bltz','bgez','b','j','jal','jr','jalr','beqzl','bnezl','bgezl','bltzl','blezl','bgtzl')
        if br:
            tgt=None; taken=False; likely=m.endswith('l') and m not in ('jal',)
            if m in('beq','beql','bne','bnel'):
                a=s.reg(REGMAP[ops[0].reg]); b=s.reg(REGMAP[ops[1].reg]); tgt=ops[2].imm
                taken=(a==b) if m.startswith('beq') else (a!=b)
            elif m in('beqz','beqzl','bnez','bnezl'):
                a=s.reg(REGMAP[ops[0].reg]); tgt=ops[1].imm; taken=(a==0) if m.startswith('beqz') else (a!=0)
            elif m in('blez','bgtz','bltz','bgez','blezl','bgtzl','bltzl','bgezl'):
                a=s64(s.reg(REGMAP[ops[0].reg])); tgt=ops[1].imm
                mm=m.rstrip('l') if m not in ('bltz','bgez') else m
                taken={'blez':a<=0,'bgtz':a>0,'bltz':a<0,'bgez':a>=0}[m[:4] if m[:4] in('blez','bgtz','bltz','bgez') else m]
            elif m=='b': tgt=ops[0].imm; taken=True
            elif m=='j': tgt=ops[0].imm; taken=True
            elif m=='jal': tgt=ops[0].imm; taken=True
            elif m=='jr': tgt=s.reg(REGMAP[ops[0].reg])&0xFFFFFFFF; taken=True
            elif m=='jalr': tgt=s.reg(REGMAP[ops[0].reg])&0xFFFFFFFF; taken=True
            if m=='jal': s.setr(31,pc+8)
            if m=='jalr': s.setr(31,pc+8)
            if m in('jal','jalr') and tgt in s.stubs:
                # run delay slot first then stub
                s.exec(pc+4)
                ret=s.stubs[tgt](s)
                if ret is not None: s.r[2]=ret&M64
                return pc+8
            if likely and not taken: return pc+8
            s.exec(pc+4)
            return tgt if taken else pc+8
        s.exec(pc); return pc+4
    def exec(s,pc):
        i=s.dec(pc); m=i.mnemonic; ops=i.operands
        g=lambda k: s.reg(REGMAP[ops[k].reg])
        rd=lambda k: REGMAP[ops[k].reg]
        if m=='nop': return
        if m=='break': raise RuntimeError("break at %x"%pc)
        if m in('addiu','addi'): s.setr(rd(0),s32(g(1)+ops[2].imm)&M64 if False else s64(s32(g(1)+ops[2].imm)))
        elif m=='move': s.setr(rd(0),g(1))
        elif m=='daddiu': s.setr(rd(0),g(1)+ops[2].imm)
        elif m in('addu','add'): s.setr(rd(0),s64(s32(g(1)+g(2))))
        elif m=='subu': s.setr(rd(0),s64(s32(g(1)-g(2))))
        elif m=='daddu': s.setr(rd(0),g(1)+g(2))
        elif m=='dsubu': s.setr(rd(0),g(1)-g(2))
        elif m=='or': s.setr(rd(0),g(1)|g(2))
        elif m=='and': s.setr(rd(0),g(1)&g(2))
        elif m=='xor': s.setr(rd(0),g(1)^g(2))
        elif m=='nor': s.setr(rd(0),~(g(1)|g(2)))
        elif m=='ori': s.setr(rd(0),g(1)|(ops[2].imm&0xFFFF))
        elif m=='andi': s.setr(rd(0),g(1)&(ops[2].imm&0xFFFF))
        elif m=='xori': s.setr(rd(0),g(1)^(ops[2].imm&0xFFFF))
        elif m=='lui': s.setr(rd(0),s64(s32((ops[1].imm&0xFFFF)<<16)))
        elif m=='sll': s.setr(rd(0),s64(s32(g(1)<<ops[2].imm)))
        elif m=='srl': s.setr(rd(0),s64(s32((g(1)&0xFFFFFFFF)>>ops[2].imm)))
        elif m=='sra': s.setr(rd(0),s64(s32(g(1)))>>ops[2].imm)
        elif m=='sllv': s.setr(rd(0),s64(s32(g(1)<<(g(2)&31))))
        elif m=='srav': s.setr(rd(0),s64(s32(g(1)))>>(g(2)&31))
        elif m=='srlv': s.setr(rd(0),s64(s32((g(1)&0xFFFFFFFF)>>(g(2)&31))))
        elif m=='dsll': s.setr(rd(0),g(1)<<ops[2].imm)
        elif m=='dsll32': s.setr(rd(0),g(1)<<(ops[2].imm+32))
        elif m=='dsrl': s.setr(rd(0),(g(1)&M64)>>ops[2].imm)
        elif m=='dsrl32': s.setr(rd(0),(g(1)&M64)>>(ops[2].imm+32))
        elif m=='dsra': s.setr(rd(0),s64(g(1))>>ops[2].imm)
        elif m=='dsra32': s.setr(rd(0),s64(g(1))>>(ops[2].imm+32))
        elif m=='dsllv': s.setr(rd(0),g(1)<<(g(2)&63))
        elif m=='dsrlv': s.setr(rd(0),(g(1)&M64)>>(g(2)&63))
        elif m=='dsrav': s.setr(rd(0),s64(g(1))>>(g(2)&63))
        elif m=='negu': s.setr(rd(0),s64(s32(-g(1))))
        elif m=='not': s.setr(rd(0),~g(1))
        elif m=='slt': s.setr(rd(0),1 if s64(g(1))<s64(g(2)) else 0)
        elif m=='sltu': s.setr(rd(0),1 if g(1)<g(2) else 0)
        elif m=='slti': s.setr(rd(0),1 if s64(g(1))<ops[2].imm else 0)
        elif m=='sltiu': s.setr(rd(0),1 if g(1)<(ops[2].imm&M64) else 0)
        elif m=='movn':
            if g(2)!=0: s.setr(rd(0),g(1))
        elif m=='movz':
            if g(2)==0: s.setr(rd(0),g(1))
        elif m=='mflo': s.setr(rd(0),s.lo)
        elif m=='mfhi': s.setr(rd(0),s.hi)
        elif m in('div','divu'):
            a=s32(g(0)) if m=='div' else g(0)&0xFFFFFFFF; b=s32(g(1)) if m=='div' else g(1)&0xFFFFFFFF
            if b==0: s.lo=0;s.hi=0
            else:
                q=abs(a)//abs(b)*(1 if (a<0)==(b<0) else -1); r=a-q*b
                s.lo=s64(s32(q)); s.hi=s64(s32(r))
        elif m in('mult','multu'):
            w=s.r32(pc); rs_=(w>>21)&31; rt_=(w>>16)&31; rd_=(w>>11)&31   # R5900: mult rd,rs,rt also writes rd=lo
            a=s.reg(rs_)&0xFFFFFFFF; b=s.reg(rt_)&0xFFFFFFFF
            if m=='mult': a=s32(a); b=s32(b)
            p=a*b; s.lo=s64(s32(p)); s.hi=s64(s32(p>>32))
            if rd_: s.setr(rd_,s.lo)
        elif m in('mul',):
            s.setr(rd(0),s64(s32(s32(g(1))*s32(g(2)))))
        if m in('lb','lbu','lh','lhu','lw','lwu','ld','sb','sh','sw','sd'):
            base=REGMAP[ops[1].mem.base]; a=(s.reg(base)+ops[1].mem.disp)&0xFFFFFFFF
            if m=='lb': v=s.rb(a); s.setr(rd(0),s64(v-256 if v&0x80 else v))
            elif m=='lbu': s.setr(rd(0),s.rb(a))
            elif m=='lh': v=s.r16(a); s.setr(rd(0),s64(v-65536 if v&0x8000 else v))
            elif m=='lhu': s.setr(rd(0),s.r16(a))
            elif m=='lw': s.setr(rd(0),s64(s32(s.r32(a))))
            elif m=='lwu': s.setr(rd(0),s.r32(a))
            elif m=='ld': s.setr(rd(0),s.r32(a)|s.r32(a+4)<<32)
            elif m=='sb': s.wb(a,g(0)); s.note(m,a,g(0)&0xFF)
            elif m=='sh': s.w16(a,g(0)); s.note(m,a,g(0)&0xFFFF)
            elif m=='sw': s.w32(a,g(0)); s.note(m,a,g(0)&0xFFFFFFFF)
            elif m=='sd': v=g(0); s.w32(a,v); s.w32(a+4,v>>32); s.note(m,a,v&0xFFFFFFFF)
            return
        known=('dsll32','dsrl','dsrl32','dsra','dsra32','dsllv','dsrlv','dsrav','negu','not','move','addiu','addi','daddiu','addu','add','subu','daddu','dsubu','or','and','xor','nor','ori','andi','xori','lui','sll','srl','sra','sllv','srav','srlv','dsll','slt','sltu','slti','sltiu','movn','movz','mflo','mfhi','div','divu','mult','multu','mul','lb')
        if m not in known: raise RuntimeError("unimpl %s %s at %x"%(m,i.op_str,pc))
REGMAP={}
names=['zero','at','v0','v1','a0','a1','a2','a3','t0','t1','t2','t3','t4','t5','t6','t7','s0','s1','s2','s3','s4','s5','s6','s7','t8','t9','k0','k1','gp','sp','fp','ra']
for n,idx in zip(names,range(32)):
    REGMAP[getattr(__import__('capstone.mips_const',fromlist=['x']),'MIPS_REG_'+n.upper(),None)]=idx
REGMAP[getattr(__import__('capstone.mips_const',fromlist=['x']),'MIPS_REG_ZERO')]=0
for n in ('MIPS_REG_0','MIPS_REG_8','MIPS_REG_FP','MIPS_REG_S8'):
    pass
```

### oracle.py — stubs, host data and harnesses

```python
import emu, struct
from emu import s32
TASK=0x01000000; TXT=0x02000000; CTX=0x01100000; CRS=0x01200000
HOST=dict(name=b"Ann", U=b"Gold", G=b"Gem", S=1234567,
          A={0:5000,1:123456,2:-4500,3:999999999}, T={0:9000+150*7, 1:0, 2:123456, 7:54321},
          T7=54321, course=[b"Alpha",b"Bravo",b"Charlie",b"Delta"])
def mk():
    e=emu.Emu()
    ev=[]
    e.events=ev
    def spr(s):
        def sh(v):
            v&=0xFFFF; return v-65536 if v&0x8000 else v
        ev.append(('S',)+tuple(sh(s.r[k]) for k in (5,6,7,8,9,10)))
    e.stubs[0x2063C8]=spr
    e.stubs[0x2039E8]=lambda s: None
    def num(s): ev.append(('N',s32(s.r[4]),s32(s.r[5]),s32(s.r[6]),s32(s.r[7])))
    e.stubs[0x202150]=num
    def fmt(s):
        dest=s.r[4]&0xFFFFFFFF; f=s.cstr(s.r[5]).decode(); args=[s32(s.r[k]) for k in (6,7,8,9,10)]
        out=(f%tuple(args[:f.count('%')])).encode()+b"\0"
        s.put(dest,out)
    e.stubs[0x27EEE0]=fmt
    def strcpy(s):
        d=s.r[4]&0xFFFFFFFF; src=s.r[5]&0xFFFFFFFF; s.put(d,s.cstr(src)+b"\0"); return d
    e.stubs[0x27F1E0]=strcpy
    def strcat(s):
        d=s.r[4]&0xFFFFFFFF; src=s.r[5]&0xFFFFFFFF; s.put(d+len(s.cstr(d)),s.cstr(src)+b"\0"); return d
    e.stubs[0x27EF70]=strcat
    def strncpy(s):
        d=s.r[4]&0xFFFFFFFF; src=s.r[5]&0xFFFFFFFF; n=s32(s.r[6]); c=s.cstr(src)
        for i in range(n): s.wb(d+i, c[i] if i<len(c) else 0)
        return d
    e.stubs[0x27F430]=strncpy
    # host data
    e.put(0x177FD8C,HOST['name']+b"\0"); e.put(0x177FD9F,HOST['U']+b"\0"); e.put(0x177FDA6,HOST['G']+b"\0")
    e.w32(0x1782A6C,HOST['S']); e.w32(0x1780A10,HOST['T7'])
    for i,v in HOST['A'].items(): e.w32(0x177FCFC+4*i,v&0xFFFFFFFF)
    for i,v in HOST['T'].items(): e.w32(0x1787008+4*i,v&0xFFFFFFFF)
    for i,c in enumerate(HOST['course']):
        e.put(CRS+0x40*i,c+b"\0"); e.w32(0x2BE398+4*i,CRS+0x40*i)
    e.put(CTX+0x100,b"\0"); e.w32(CTX+0x10,CTX+0x100)
    return e
def render(text,scroll,limit,course=0):
    e=mk(); e.put(TXT,text+b"\0\0\0\0\0\0\0\0")
    e.wb(CTX+0x100,course)
    e.w16(TASK+2,limit&0xFFFF); e.wb(TASK+0x26,scroll)
    r=e.call(0x23A8E8,(TASK,TXT,CTX),maxsteps=60000)&0xFFFFFFFF
    nl=e.r16(TASK+2); nl=nl-65536 if nl&0x8000 else nl
    return dict(ret=r,cursor=(r-TXT) if r else None,newlimit=nl,events=e.events)
def glyph(b):
    e=mk(); e.put(TXT,b+b"\0\0\0\0"); r=e.call(0x2017B0,(100,200,0,TXT)); return s32(r),e.events
def textwidth(b):
    e=mk(); e.put(TXT,b+b"\0"); return s32(e.call(0x203F40,(TXT,)))

# ---------------------------------------------------------------- dialogueTask
P_=0x01300000; TBL=0x01400000; NAMEA=0x01500000; MSG0=0x02000000; MSGSTRIDE=0x800
ADDR_TOWNVAR=0x1780A3C
def fnv(vals):
    h=0xcbf29ce484222325
    for v in vals:
        for b in range(8):
            h=((h^((v>>(8*b))&0xFF))*0x100000001b3)&0xFFFFFFFFFFFFFFFF
    return h
def ev_hash(events):
    vals=[]
    for ev in events:
        k=ev[0]; nums=list(ev[1:])+[0]*6
        kind={'S':0,'N':1,'T':2}[k]
        vals.append(kind)
        vals += [x & 0xFFFFFFFFFFFFFFFF for x in nums[:6]]
    return fnv(vals)
def mk_dialogue(name, msgs, table_words):
    e=mk()
    ev=e.events; fx=[]
    e.fx=fx
    e.put(NAMEA,name+b"\0")
    for k,m in enumerate(msgs): e.put(MSG0+MSGSTRIDE*k, m+b"\0\0\0\0\0\0\0\0")
    for i,w in enumerate(table_words): e.w32(TBL+4*i,w)
    e.w32(TASK+0x1c,TBL); e.w32(TASK+0x18,P_)
    e.w32(CTX+0x10,CTX+0x100)
    def snd(s): fx.append("snd:%x"%(s.r[4]&0xFFFFFFFF))
    e.stubs[0x258E50]=snd
    def spawn(s): fx.append("spawn:%x:%x"%(s.r[5]&0xFFFFFFFF, s.r[6]&0xFFFFFFFF)); return 0x01700000
    e.stubs[0x204C48]=spawn
    e.stubs[0x204DE8]=lambda s: fx.append("kill")
    e.stubs[0x27EA30]=lambda s: fx.append("next:%x"%s.r32(TASK+0x28))
    e.stubs[0x23C7B0]=lambda s: fx.append("rel")
    e.stubs[0x23C418]=lambda s: fx.append("attr:%d:%d"%(s.r[5]&0xFF,s.r[6]&0xFF))
    e.stubs[0x23C5C0]=lambda s: fx.append("photo:%d"%(s.r[4]&0xFF))
    e.stubs[0x23C8A8]=lambda s: fx.append("stamp:%d"%(s.r[4]&0xFF))
    e.stubs[0x23AD00]=lambda s: s.r[6]
    def texa(s): ev.append(('T',s32(s.r[4])))
    e.stubs[0x203990]=texa
    e.watch={P_+4,CTX+0x28,CTX+0x18,CTX+0x20,CTX+0x21,CTX+0x23,ADDR_TOWNVAR}
    return e
def flush_stores(e):
    out=[]; st=e.stores; i=0
    while i<len(st):
        m,a,v=st[i]
        if a==P_+4: out.append("pdone:%d"%s32(v))
        elif a==CTX+0x28: out.append("f400:%d"%((v>>10)&1))
        elif a==CTX+0x18: out.append("talk:%d"%v)
        elif a==CTX+0x20: out.append("c20:%d"%v)
        elif a==CTX+0x23:
            # warp = +0x23 store then +0x21 store
            if i+1<len(st) and st[i+1][1]==CTX+0x21:
                out.append("warp:%d:%d"%(v,st[i+1][2])); i+=1
            else: out.append("c23:%d"%v)
        elif a==CTX+0x21: out.append("c21:%d"%v)
        elif a==ADDR_TOWNVAR: out.append("town:%d"%v)
        i+=1
    e.stores=[]
    return out
def dialogue_tick(e,tick,pad,done):
    e.events.clear(); e.fx.clear(); e.stores=[]
    e.w32(e.r[28]-0x546c,0)   # uiBatchCount is reset by the frame code each frame
    e.w16(CTX+0x30,0x50 if pad else 0)
    if done is not None:
        e.wb(TASK+0x24,1); e.w32(TASK+4,done&0xFFFFFFFF)
    e.call(0x23B168,(TASK,CTX))
    e.wb(TASK+0x24,0)   # task walker clears +0x24 after the callback
    e.w32(TASK+8,e.r32(TASK+8)+1)   # ... and increments the tick count
    # merge ordered stub fx with store fx: stores interleave by program order is lost;
    # we record stores separately and append (the model emits memory effects in the same
    # order relative to each other, and stub effects in order; see merge note in report)
    st=flush_stores(e)
    sc=e.rb(TASK+0x26); lim=e.r16(TASK+2); lim=lim-65536 if lim&0x8000 else lim
    obs=dict(state=e.rb(TASK+0x25),scroll=sc,limit=lim,cursor=e.r32(TASK+0x28),pend=e.rb(TASK+0x27),
             nev=len(e.events),evhash=ev_hash(e.events),fx=list(e.fx),st=st)
    return obs
```

### gen_fixtures.py — fixture generator

```python
#!/usr/bin/env python3
"""W2-07: generate tests/native_w2/data/text_script_vectors.txt by EXECUTING the ELF's text-engine
functions in a MIPS interpreter (emu.py + oracle.py).  Inputs are synthetic (authored here, seed fixed);
outputs are numbers.  No ROM text/bytes are written."""
import random, sys, os
import oracle
from oracle import *
R=random.Random(0x5707)
out=[]
def H(b): return b.hex() if b else "-"
def evtok(ev):
    k=ev[0]
    return k+","+",".join(str(x) for x in ev[1:])
out.append("# W2-07 text-engine oracle vectors; produced by executing SLUS_203.98 code (see .qoder/wave2/W2-07-report.md)")
out.append("# HOST name U G S T7 A(id:val;..) T(id:val;..) course(4 names, ';' separated)  [hex for strings]")
out.append("HOST %s %s %s %d %d %s %s %s"%(H(HOST['name']),H(HOST['U']),H(HOST['G']),HOST['S'],HOST['T7'],
    ";".join("%d:%d"%(k,v) for k,v in HOST['A'].items()),";".join("%d:%d"%(k,v) for k,v in HOST['T'].items() if k!=7),
    ";".join(H(c) for c in HOST['course'])))
# ---- glyph vectors
cnt=0
def glyph_vec(b,x,y,sh):
    global cnt
    e=mk(); e.put(TXT,b+b"\0\0\0\0")
    r=e.call(0x2017B0,(x,y,sh,TXT)); nx=s32(r)
    sp=[evtok(v) for v in e.events]
    out.append("GLYPH %s %d %d %d %d %s"%(H(b),x,y,sh,nx," ".join(sp) if sp else "-"))
for c in range(0x20,0x7f): glyph_vec(bytes([c,0x41]),R.randint(0,600),R.randint(0,900),R.choice([0,0,0,2,4,8,14]))
for pre in b"/`^~":
    for n in range(0x20,0x7f): glyph_vec(bytes([pre,n]),R.randint(0,600),R.randint(0,900),R.choice([0,0,6,14]))
glyph_vec(b"\x81\x40",10,20,0); glyph_vec(b"\x81\x8b",10,20,0); glyph_vec(b"\x81\x8b",10,20,6)
# ---- width vectors
def rnd_str(n,hi=True,pre=True):
    s=bytearray()
    while len(s)<n:
        t=R.random()
        if t<0.05 and pre: s+=bytes([R.choice(b"/`^~"),R.randint(0x20,0x7e)])
        elif t<0.08 and hi: s+=bytes([0x81,R.choice([0x40,0x8b,0x41,0x75])])
        else: s.append(R.choice([c for c in range(0x20,0x7f) if c not in b"/`^~"]))
    return bytes(s)
for i in range(300):
    b=rnd_str(R.randint(0,40)); e=mk(); e.put(TXT,b+b"\0"); out.append("WIDTH %s %d"%(H(b),s32(e.call(0x203F40,(TXT,)))))
# ---- number cells
for v in [-5,0,1,9,10,99,100,999,1000,9999,10000,99999,100000,999999,1000000,9999999,10000000,99999999,100000000,999999999,1000000000,2000000000]:
    e=mk(); r=s32(e.call(0x204460,(0,0,v,10))); out.append("NUMCELLS %d %d"%(v,r//10))
# ---- frame vectors
for y in (0x158,100,0,-8):
    e=mk(); e.call(0x2028B0,(y,)); out.append("FRAMEA %d %s"%(y," ".join(evtok(v) for v in e.events)))
for y in (0x158,100,0):
    for w in (0,1,17,64,133,255,300):
        e=mk(); e.call(0x202B70,(y,w)); out.append("FRAMEB %d %d %s"%(y,w," ".join(evtok(v) for v in e.events)))
for idx in range(0,12):
    e=mk(); e.call(0x203E70,(0x136,0x1a4,idx)); out.append("ICON 310 420 %d %s"%(idx," ".join(evtok(v) for v in e.events)))
# ---- render vectors
words=[b"the",b"Quick",b"brown",b"fox",b"jumps",b"over",b"lazy",b"dog",b"Hi!",b"OK",b"1,000",b"you",b"get",b"a",b"prize",b"Zzz",b"mi",b"ll",b"w"]
def rnd_text():
    t=bytearray(); nl=R.randint(1,8)
    for li in range(nl):
        for wi in range(R.randint(0,7)):
            k=R.random()
            if k<0.10:
                c=R.choice(b"ABCDEFGHIJKLMNOPQRSTUVWXYZ")
                t+=b"$"+bytes([c])
                if c in b"AT": t+=bytes([R.choice([0,1,2,3,0x30,0x31,0x37,0x32,0x90,5])])
            elif k<0.13: t+=b"$"+bytes([R.choice(b"a0\x81$ ")]) 
            elif k<0.17: t+=bytes([R.choice(b"/`^~"),R.choice(b"aeiou AeiouCcs")])
            elif k<0.20: t+=b"\x81\x40" if R.random()<.5 else b"\x81\x8b"
            else: t+=R.choice(words)
            if R.random()<0.8: t+=b" "
        if li<nl-1 or R.random()<0.75: t+=b"\n"
    k=R.random()
    if k<0.5: t+=bytes([R.choice(list(range(0,0x19))+[0x1a,0x1f]) if True else 0])
    elif k<0.6: t+=b"x"
    return bytes(t)
for i in range(1600):
    t=rnd_text()
    sc=R.choice([0,0,0,R.randint(1,100),R.randint(1,12),12,24,36,R.randint(0,255)])
    lim=R.choice([0x7fff,0x7fff,R.randint(0,80),R.randint(0,10),-1])
    crs=R.randint(0,3)
    try:
        r=render(t,sc,lim,crs)
    except RuntimeError:
        out.append("RUNAWAY %d %d %d %s"%(sc,lim,crs,H(t))); continue
    cur="-" if r['ret']==0 else str(r['cursor'])
    if i<40: out.append("RENDERX %d %d %d %s %s %d %s"%(sc,lim,crs,H(t),cur,r['newlimit']," ".join(evtok(v) for v in r['events']) or "-"))
    out.append("RENDER %d %d %d %s %s %d %d %x"%(sc,lim,crs,H(t),cur,r['newlimit'],len(r['events']),ev_hash(r['events'])))
# ---- extract / copy / subst
def run_extract(src,dl,k):
    e=mk(); e.put(TXT,src+b"\0"); e.put(TXT+0x400,dl+b"\0")
    r=e.call(0x24EA00,(TXT+0x800,TXT,TXT+0x400,k))&0xFFFFFFFF
    return None if r==0 else e.cstr(TXT+0x800)
for i in range(500):
    dl=bytes(R.choice(b"#\n,;| ") for _ in range(R.randint(0,3)))
    src=bytes(R.choice(b"ab#\n,;| cd\x81\x90") for _ in range(R.randint(0,14)))
    k=R.randint(-2,4)
    r=run_extract(src,dl,k)
    out.append("EXTRACT %s %s %d %s"%(H(src),H(dl),k,"NULL" if r is None else (H(r) if r else "EMPTY")))
def run_copy(src,limit):
    e=mk(); e.put(TXT,src+b"\0"*64); r=e.call(0x24E7D8,(TXT+0x800,TXT,limit)); return e.cstr(TXT+0x800)
for i in range(700):
    n=R.randint(0,12); s=bytearray()
    for _ in range(n):
        t=R.random()
        if t<0.12: s+=bytes([R.randint(0x81,0xEF),R.randint(0x40,0xFC)])
        elif t<0.2: s.append(R.randint(1,0x1f))
        else: s.append(R.randint(0x20,0x7e))
    limit=R.choice([0,0,R.randint(1,16),R.randint(1,6)])
    out.append("COPY %s %d %s"%(H(bytes(s)),limit,H(run_copy(bytes(s),limit))))
def run_subst(t):
    e=mk(); e.put(TXT,t+b"\0"); r=e.call(0x264CB0,(TXT,))&0xFFFFFFFF
    return None if r==0 else e.cstr(TXT)
for i in range(300):
    t=bytes(R.choice(b"ab $N$x$NN") for _ in range(R.randint(0,14)))
    r=run_subst(t); out.append("SUBST %s %s"%(H(t),"NONE" if r is None else (H(r) if r else "EMPTY")))
# ---- dialogue scenarios
def build_scenario(sid):
    n=R.randint(2,5)
    msgs=[]
    def tidx(): return R.randint(2,n+1)
    def mkmsg():
        lines=R.randint(1,7); t=bytearray()
        for li in range(lines):
            for wi in range(R.randint(1,4)):
                t+=R.choice(words)+b" "
                if R.random()<0.08: t+=b"$"+R.choice([b"G",b"N",b"S",b"U",b"T7",b"A\x01",b"R"])+b" "
            t+=b"\n" if (li<lines-1 or R.random()<0.7) else b""
        cmdp=R.choice([0,0,0xC,0x1,0x2,0x3,0x4,0x6,0x7,0xD,0xE,0xF,0x10,0x11,0x13,0x14,0x15,0x16,0x17,0x18,0x5,0x8,0x9,0xB,0x12])
        t+=bytes([cmdp])
        if cmdp in (0xC,0xE,0x13): t+=bytes([tidx()])
        elif cmdp==3: t+=bytes([R.choice([0,2])])
        elif cmdp in (4,6): t+=bytes([R.choice([1,2,3])])
        elif cmdp==0xD: t+=bytes([R.randint(1,9),R.randint(1,9),0])
        elif cmdp==0x11: t+=bytes([R.randint(1,9),tidx()])
        elif cmdp==0x14: t+=bytes([R.randint(1,9),R.randint(0,9)])
        elif cmdp==0x15: t+=bytes([R.randint(0,9),R.randint(0,9),tidx()])
        else: t+=bytes(R.randint(1,9) for _ in range(2))
        return bytes(t)
    for k in range(n): msgs.append(mkmsg())
    words_t=[NAMEA]+[MSG0+MSGSTRIDE*k for k in range(n)]+[MSG0+MSGSTRIDE*(R.randrange(n)) for _ in range(2)]
    # table: [0]=name, [1]=entry (msg0), [2..]=targets (msg1.. + extras)
    tbl=[NAMEA, MSG0]+[MSG0+MSGSTRIDE*k for k in range(1,n)]+[MSG0+MSGSTRIDE*R.randrange(n) for _ in range(3)]
    # make indexes 2..n+1 valid: table length >= n+2
    while len(tbl)<n+2: tbl.append(MSG0)
    name=R.choice([b"Shop",b"Q's Factory",b"A",b"Long Name Plate"])
    e=mk_dialogue(name,msgs,tbl)
    out.append("SCN %d %s %d %s"%(sid,H(name),len(msgs)," ".join("%x"%w for w in tbl)))
    for k,m in enumerate(msgs): out.append("MSG %d %d %s"%(sid,k,H(m)))
    st_count=collections_default(); 
    pad_p=R.choice([0.03,0.08,0.2]); wait=0
    last_state=0
    for t in range(0,300):
        # pad and child-done injection decided from the emulator's own state
        state=e.rb(TASK+0x25)
        pad=R.random()<pad_p
        done=None
        if state in (4,5,6):
            st_count[state]+=1
            if st_count[state]>=R.randint(2,5):
                st_count[state]=0
                if state==5: done=R.choice([-1,-1,R.randint(2,n+1),R.randint(2,n+1)])
                else: done=R.randint(2,n+1)
        o=dialogue_tick(e,t,pad,done)
        out.append("TICK %d %d %d %s %d %d %d %x %d %d %x %s %s"%(sid,t,1 if pad else 0,("-" if done is None else str(done)),
            o['state'],o['scroll'],o['limit'],o['cursor'],o['pend'],o['nev'],o['evhash'],",".join(o['fx']) or "-",",".join(o['st']) or "-"))
        if 'kill' in o['fx']: break
def collections_default():
    import collections; return collections.defaultdict(int)
for sid in range(48): build_scenario(sid)
open(sys.argv[1],'w').write("\n".join(out)+"\n")
print(len(out),"lines")
```

### mipsdis_helper.py — disassembly helper used by arbitrate.py

```python

import struct
from capstone import *
ELF="/Users/holzr/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98"
d=open(ELF,'rb').read()
def lines(lo,hi):
    md=Cs(CS_ARCH_MIPS, CS_MODE_MIPS64+CS_MODE_LITTLE_ENDIAN)
    for i in md.disasm(d[lo-0x1FF000:hi-0x1FF000],lo):
        yield "%08x: %s %s"%(i.address,i.mnemonic,i.op_str)
```

### arbitrate.py — arbitration (a)/(b) driver

```python
#!/usr/bin/env python3
"""W2-07 arbitration (a): run the REAL InterpretScriptCommands 0x23AD00 from the ELF in the MIPS
interpreter and measure, per opcode, how many bytes the cursor advances when the branch is NOT taken
(skip width) and where it lands when taken.  Callee predicates are stubbed with configurable results."""
import re, sys, subprocess, collections
import emu, oracle
from emu import s32
Tk=0x01000000; P=0x01300000; C=0x01100000; TBLB=0x01400000; SCR=0x02000000; MARK=0x03000000
# jal targets inside the function
import mipsdis_helper as md
def targets():
    out=set()
    for line in md.lines(0x23AD00,0x23B168):
        m=re.search(r"jal 0x([0-9a-f]+)",line)
        if m: out.add(int(m.group(1),16))
    return sorted(out)
TG=targets()
def run(op, operands, ret, ram=None, ctxw14=0, town=5):
    e=emu.Emu(); e.events=[]
    calls=[]
    for t in TG:
        def mk(t=t):
            def f(s):
                calls.append(t)
                if t in ret: return ret[t]
                return ret.get('default',0)
            return f
        e.stubs[t]=mk()
    e.w32(Tk+0x20,TBLB); e.w32(Tk+0x18,P)
    e.wb(C+0x21,3); e.wb(C+0x22,4); e.wb(C+0x23,town); e.w32(C+0x14,ctxw14)
    for i in range(256):
        e.w32(TBLB+4*i,MARK+0x100*i); e.wb(MARK+0x100*i,0x5a)
    for a,v in (ram or {}).items(): e.wb(a,v)
    script=bytes([op])+bytes(operands)+bytes([0x5a])*10
    e.put(SCR,script)
    r=e.call(0x23AD00,(Tk,C,SCR),maxsteps=20000)&0xFFFFFFFF
    return r,calls
def classify(r):
    if SCR<=r<SCR+16: return ('skip',r-SCR)
    if MARK<=r<MARK+0x100*256: return ('branch',(r-MARK)//0x100)
    return ('other',hex(r))
if __name__=="__main__":
    print("op | not-taken skip width (cfg) | taken branch idx")
    OPR=[0x21+i for i in range(8)]   # operand bytes (all >= 0x20 so they never decode as ops): idx values 0x21..
    cfgs={'all0':{'default':0},'all1':{'default':1}}
    res={}
    for op in list(range(0,0x20)):
        row=[]
        for name,cfg in cfgs.items():
            # extra arbitration configs
            for extra in ([{}] if op not in (8,0x18,0x1b,0x1c,0x0e) else [{}]):
                r,calls=run(op,OPR,cfg)
                row.append((name,classify(r),[hex(c) for c in calls]))
        res[op]=row
        print("%02x"%op, " | ".join("%s:%s%s"%(n,c[0],c[1]) for n,c,_ in row))

def arbitrate_matrix():
    """Per-op: skip width (branch NOT taken) and the operand index used as branch index (taken)."""
    # (op, ret config, ram, ctxw14, town) giving the SKIP path, then the TAKEN path
    S={ 8:dict(ret={0x23DB80:0,0x23DCC0:1}),
        0x0b:dict(ret={'default':0}), 0x18:dict(ret={'default':0},ctxw14=100000),
        0x1a:dict(ret={0x23D968:90,'default':0}), 0x1c:dict(ret={'default':0}),
        0x1b:dict(ret={'default':0}),
        }
    T_={ 8:dict(ret={0x23DB80:1}), 0x0b:dict(ret={0x23D8C8:0x21}), 0x18:dict(ret={'default':0}),
         0x1a:dict(ret={0x23D968:0,'default':0}), 0x1c:dict(ret={'default':0},town=0x21),
         0x0e:dict(ret={'default':1,0x23DDB8:1}), 0x1b:dict(ret={'default':0,0x23D628:1},ram={0x177FDB1:3}) }
    OPR=[0x21+i for i in range(8)]
    rows=[]
    for op in range(1,0x1E):
        if op in (9,10,12): continue
        skip=None; br=None
        for cfg in ([S[op]] if op in S else [dict(ret={'default':0}),dict(ret={'default':1})]):
            r,calls=run(op,OPR,cfg['ret'],cfg.get('ram'),cfg.get('ctxw14',0),cfg.get('town',5)); c=classify(r)
            if c[0]=='skip': skip=c[1]
        for cfg in ([T_[op]] if op in T_ else [dict(ret={'default':0}),dict(ret={'default':1})]):
            r,calls=run(op,OPR,cfg['ret'],cfg.get('ram'),cfg.get('ctxw14',0),cfg.get('town',5)); c=classify(r)
            if c[0]=='branch': br=c[1]-0x21+1   # operand position (1-based after opcode)
        rows.append((op,skip,br))
    return rows
def unknown_cases():
    out=[]
    cases={'op09_then_text':[9,0x5a],'op0a_blank_lines':[0x0a,0x0a,0x0a,0x5a],'op0c_swallows_op01':[0x0c,0x01,0x21,0x22,0x5a],
           'op09_ctrl_run':[9,5,0x0a,0x0f,0x5a],'op00':[0,0x5a],'op1e':[0x1e,0x0a,0x5a],'op1f_then_op':[0x1f,0x01,0x03,0x21,0x5a]}
    for name,bs in cases.items():
        e=emu.Emu()
        prints=[]
        for t in TG:
            if t==0x27EA30:
                def pf(s,prints=prints): prints.append((s.cstr(s.r[4]),s32(s.r[5]),s32(s.r[6]),s32(s.r[7])))
                e.stubs[t]=pf
            else: e.stubs[t]=lambda s: 0
        e.w32(Tk+0x20,TBLB); e.w32(Tk+0x18,P); e.wb(C+0x21,3); e.wb(C+0x22,4); e.wb(C+0x23,5)
        e.put(SCR,bytes(bs)+bytes([0x5a])*8)
        r=e.call(0x23AD00,(Tk,C,SCR),maxsteps=20000)&0xFFFFFFFF
        out.append((name,bs,r-SCR,prints))
    return out
if __name__=="__main__":
    print("\nop skipwidth branchoperand(1-based)")
    for op,sk,br in arbitrate_matrix(): print("%02x %s %s"%(op,sk,br))
    print("\nunknown-op cases (script bytes -> returned cursor offset; printf calls)")
    for n,bs,off,pr in unknown_cases(): print(n,bytes(bs).hex(),'->',off,[(f[0][:48],)+f[1:] for f in pr])
```

### atlas.py — FONTE atlas decode (python prototype of the ink measurement)

```python
import numpy as np
D="/Users/holzr/Library/Application Support/RoadTripRecomp/disc/"
elf=open(D+"SLUS_203.98",'rb').read()
g=open(D+"SYS/FONTE.GSL",'rb').read()
raw=np.frombuffer(g[0x70:0x70+512*384//2],dtype=np.uint8)
idx=np.empty(512*384,dtype=np.uint8); idx[0::2]=raw&15; idx[1::2]=raw>>4
idx=idx.reshape(384,512)
o=0x29A860-0x1FF000
W=[x if x<128 else x-256 for x in elf[o:o+256]]
def slot(c):
    gi=c-0x21; return (gi//31)*32+(gi%31)
def cellbox(c):
    s=slot(c); col=s%32; row=s//32
    return idx[row*24:(row+1)*24, col*16:(col+1)*16]
```

### meas.py — ink-extent prediction/measurement helpers

```python
from atlas import *
from PIL import Image
import numpy as np
G="build/scratch/oracle-golden/"
BRIGHT=np.ones(16,bool); BRIGHT[[0,1,3,7]]=False
def pred(s, start=0):
    pen=start; L=None; R=None
    for ch in s:
        c=ord(ch)
        if c==0x20: pen+=8; continue
        w=W[c]; uoff=(16-w)//2; qw=w//2+8
        cb=cellbox(c); m=BRIGHT[cb]
        xs=np.where(m.any(0))[0]
        # visible columns: [uoff, uoff+qw) of the cell -> dest pen + (x-uoff)
        xs=[x for x in xs if uoff<=x<uoff+qw]
        if xs:
            l=pen+min(xs)-uoff; r=pen+max(xs)-uoff
            L=l if L is None else min(L,l); R=r if R is None else max(R,r)
        pen+=w
    return pen,L,R
def meas(img,box):
    a=np.array(Image.open(G+img).convert('RGB')).astype(int)
    x0,y0,x1,y1=box
    sub=a[y0:y1,x0:x1]
    m=(sub[...,0]>=150)&(sub[...,1]>=150)&(sub[...,2]>=100)
    xs=np.where(m.any(0))[0]; ys=np.where(m.any(1))[0]
    return x0+xs.min(), x0+xs.max(), y0+ys.min(), y0+ys.max()
```

### extents.py — oracle pixel extents -> text_extents.csv

```python
from meas import *
# image; string; search box (scan px) x0,y0,x1,y1; bmin; pen_ui (-1 unknown; -2 = plate right-aligned at 574)
ROWS=[('main_menu.png','Adventure',(920,277,1176,300),170,-1),
('main_menu.png','Quick Race',(918,300,1182,325),170,-1),
('main_menu.png','2 Player',(918,325,1182,349),170,-1),
('main_menu.png','Options',(918,349,1182,373),170,-1),
('main_menu.png','Results',(918,373,1182,397),170,-1),
('options.png','Vibration',(512,176,828,200),170,-1),
('options.png','Speaker',(505,200,828,224),170,-1),
('options.png','Sound Volume',(505,224,828,248),170,-1),
('options.png','Exit',(505,248,828,272),170,-1),
('factory_menu.png','Change parts',(146,48,556,72),170,-1),
('factory_menu.png','Race',(140,73,560,97),100,-1),
('factory_menu.png','Save data',(140,97,560,121),100,-1),
('factory_menu.png','Quit game',(140,121,560,145),100,-1),
('factory_menu.png','Drive around town',(140,145,562,169),100,-1),
('factory_menu.png','What do you want to do?',(130,335,800,372),100,70),
('factory_menu.png',"Q's Factory",(878,318,1146,346),170,-2)]
def measure(img,box,bmin):
    a=np.array(Image.open(G+img).convert('RGB')).astype(int)
    x0,y0,x1,y1=box; sub=a[y0:y1,x0:x1]
    m=(sub[...,0]>=150)&(sub[...,1]>=150)&(sub[...,2]>=bmin)
    xs=np.where(m.any(0))[0]; ys=np.where(m.any(1))[0]
    return int(x0+xs.min()),int(x0+xs.max()),int(y0+ys.min()),int(y0+ys.max())
if __name__=="__main__":
    import sys
    lines=["# image;string;box_x0;box_y0;box_x1;box_y1;bmin;pen_ui;meas_x0;meas_x1;meas_y0;meas_y1  (scan px, 1280x447; pen_ui -1=unknown -2=right-aligned plate at 574)"]
    ok=0
    for img,s,box,bmin,pen in ROWS:
        m=measure(img,box,bmin); p=pred(s)
        wm=(m[1]-m[0]+1)/2; wp=p[2]-p[1]+1
        ok+= abs(wm-wp)<=2
        print(f"{s:25s} measW={wm:6.1f} predW={wp:4d} diff={wp-wm:5.1f}")
        lines.append(";".join(map(str,[img,s,*box,bmin,pen,*m])))
    print(ok,len(ROWS))
    if len(sys.argv)>1: open(sys.argv[1],'w').write("\n".join(lines)+"\n")
```

### scripts.py — ELF script 3-level walk (census helper)

```python
import struct, collections
ELF="/Users/holzr/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98"
d=open(ELF,'rb').read()
def u8(v): return d[v-0x1FF000]
def u32(v): return struct.unpack_from('<I',d,v-0x1FF000)[0]
DATAEND=0x33567B
def census():
    towns=[u32(0x2A7ED0+4*i) for i in range(1,22)]
    l1={}
    for t in towns:
        a=t; slots=[]
        while u32(a)!=0: slots.append(u32(a)); a+=4
        l1[t]=slots
    l2=sorted({s for v in l1.values() for s in v})
    tables={}
    for i,t in enumerate(l2):
        nxt=l2[i+1] if i+1<len(l2) else 0x2A8000
        n=0; a=t
        while a<nxt and u32(a)>=0x2F0000 and u32(a)<DATAEND:
            n+=1; a+=4
        tables[t]=[u32(t+4*k) for k in range(n)]
    return towns,l1,tables
OPW={1:3,2:3,3:4,4:2,5:3,6:3,7:2,8:3,0xB:3,0xD:3,0xE:7,0xF:2,0x10:2,0x11:3,0x12:1,0x13:2,0x14:3,0x15:2,0x16:2,0x17:2,0x18:2,0x19:3,0x1A:3,0x1B:2,0x1C:3,0x1D:4}
def prefix_end(a):
    while True:
        op=u8(a)
        if op>=0x20: return a
        if op in OPW: a+=OPW[op]
        else: return None
```

### census_wrap.py — message/command/escape/wrap census

```python
#!/usr/bin/env python3
"""W2-07 census: message starts, command bytes, `$` codes, line-width statistics, greedy-wrap consistency."""
import collections
from scripts import *          # ELF 3-level walk (script_vm.md); no text is printed
from atlas import W            # uiGlyphWidths
towns,l1,tables=census()
starts={w for ws in tables.values() for w in ws[1:]}
print('tables',len(tables),'distinct message starts',len(starts))
cmd=collections.Counter(); esc=collections.Counter(); msgs=[]; clean=0
for s in sorted(starts):
    p=prefix_end(s)
    if p is None: continue
    clean+=1; q=p; lines=[]; cur=[]
    while u8(q)>=0x20 or u8(q)==10:
        b=u8(q)
        if b==10: lines.append(bytes(cur)); cur=[]
        else: cur.append(b)
        q+=1
    if cur or not lines: lines.append(bytes(cur))
    cmd[u8(q)]+=1
    for l in lines:
        for i in range(len(l)-1):
            if l[i]==0x24: esc[chr(l[i+1])]+=1
    msgs.append(lines)
print('clean prefix',clean,'commands',sorted((hex(k),v) for k,v in cmd.items()),'esc',dict(esc))
def width(s): return sum(8 if c==0x20 else W[c] for c in s)
m2=[l for l in msgs if not any(b'$' in x for x in l) and len(l)>=2 and all(x for x in l)]
ws=sorted(width(x) for l in m2 for x in l[:-1])
print('multi-line $-free messages',len(m2),'non-final line px p50',ws[len(ws)//2],'p90',ws[int(len(ws)*.9)],'max',ws[-1])
ints=[]
for l in m2:
    lo=max(width(x) for x in l); hi=10**9
    for a,b in zip(l,l[1:]): hi=min(hi,width(a)+8+width(b.split(b' ')[0]))
    ints.append((lo,hi))
best=max((sum(1 for lo,hi in ints if lo<=w<hi),w) for w in range(150,700))
print('greedy-consistent messages: best',best,'at 498:',sum(1 for lo,hi in ints if lo<=498<hi))
```

### mipsdis.py — disassembler CLI

```python
import sys, struct
from capstone import *
ELF="/Users/holzr/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98"
d=open(ELF,'rb').read()
def dis(lo,hi):
    md=Cs(CS_ARCH_MIPS, CS_MODE_MIPS64+CS_MODE_LITTLE_ENDIAN)
    code=d[lo-0x1FF000:hi-0x1FF000]
    for i in md.disasm(code,lo):
        print("%08x: %08x  %s %s"%(i.address,struct.unpack('<I',i.bytes)[0],i.mnemonic,i.op_str))
if __name__=="__main__":
    dis(int(sys.argv[1],16),int(sys.argv[2],16))
```
