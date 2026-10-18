# W2-01 report — town/shop event-script VM

Status: **PASS** (`exit 0`), wall time ≈ 0.2 s.

```
W2-01 SCORE tables=21/330/307 entry_clean=307/307 unknown_ops=0 branch_oob=0 op_vectors=52/52 deterministic=1 branch_bleed=1 extra_vectors=17/17 PASS
```

## Summary

* Bit-faithful host port of `InterpretScriptCommands` (0x23AD00..0x23B167): all 26 live ops, the 3 aliased no-op/unknown stubs (9, 10, 12) and the unknown path for op 0/0x1E/0x1F, with the original's report-and-skip rule and prefix end (byte ≥ 0x20). Widths and operand roles were re-derived from a Capstone disassembly of the whole function (not only the lossy decomp) and corrected against `gsl-vm.md` §5.1 (8 corrections, listed in `docs/native-reference/script-vm.md` §3).
* ELF-resident script image reader (3-level walk from the town directory 0x2A7ED0) with a documented, insensitive level-2 length rule. Census = **21 towns / 330 level-1 slots / 307 level-2 tables** (equals the plan's un-pinned snapshot); 2757 distinct level-3 pointers (plan's number, reproduced exactly).
* **307/307** entry scripts decode clean; ops {0,9,10,12,0x1E,0x1F} occur **0** times in any entry prefix; every branch operand is in bounds except one raw-index quirk (see below).
* The VM exposes `IScriptHost` (one method per named callee) + `IScriptMemory`, so later waves bind it to SaveData without touching the VM.

## Coordinator ground truth — re-verified against the ELF (all matched)

| claim | result |
|---|---|
| jump table @0x2EC750, 29 entries, targets in 0x23AD50–0x23B0E0 | ✔ (test checks all 29) |
| ops 9, 10, 12 → same stub 0x23B0E0 | ✔; 27 distinct targets, 26 live |
| "word 30 = 0; word 31 = 'next'" | ✔ with 1-based indexing (0-based 29 = 0, 30 = `next`); `next` is the start of the adjacent string `"next msg #%d"`, not a sentinel |
| script directory @0x2A7ED0; spawners 0x2105B8 / 0x22AEB0 | ✔ (`002105b8.c:373`, `0022aeb0.c:130,188,221`); also read by 0x26BE50 (menu) |
| timer op "value = frames − 1" | **Not applicable**: this VM has no timer/frame op; the rule is the race-HUD timer (W2-10). Not re-verified here. |

No mismatch → no STOP condition.

## Files (all new)

* `src/native/assets/script_image.{h,cpp}` — ELF reader, census, level-2 length
* `src/native/gameplay/script_vm.{h,cpp}` — op table, `runScriptPrefix`, `decodeScriptPrefix`, `IScriptHost`, `IScriptMemory`
* `tests/native_w2/script_vm_test.cpp` — `w2_script_vm_test`
* `docs/native-reference/script-vm.md`, this report

(no CMake/main.cpp/existing-file edits; private build dir `build/w-W2-01`)

## Key numbers (pinned in the test as counts + FNV-1a; no ROM bytes)

| item | value |
|---|---|
| census | 21 / 330 / 307; table words 3174; ends next/zero/foreign/last = 96/205/5/1; FNV `0x2f5ffedee5bc99af` |
| script references / distinct starts | 2867 / 2478 (clean 2864, non-clean 3) |
| ops decoded / branch operands | 1431 / 1096; hist FNV `0xe30b5918821f704c`; target FNV `0xecbb81a298a79c34` |
| strict OOB / bleed | 0 / 1 |
| reachable from entries via prefix branches | 1026 (0 unclean) |
| replay (330 slots × 3 host modes) | 1604 ops, 524 branches, 0 StepLimit, 0 unknown, 0 BadRead; FNV `0x5ac718a076d1b868` |
| synthetic vectors | 52/52 core (26 ops × {taken, not-taken} or {bare, chained} for effect ops), 17/17 extras (op 08 third arm, op 18 boundaries 53999/54000/198000/198001/−1, op 1B remaining paths, op 1C signed-vs-unsigned, op 0E index 0/5, chain through a taken branch, prefix end bytes, StepLimit, BadRead), 18/18 unknown-op vectors (ops 0/9/10/12/0x1E/0x1F × {skip, no-skip} + static classification) |

An independent Python implementation (appendix) reproduces the census, all three hashes except the replay hash, the op histogram and the mode-0/1 replay totals (ops 549/537, branches 311/39, done 1/0). Mutation check: breaking op 03's skip width makes the test fail (51/52, replay pin fails); restored.

## Reproduce

```
cmake -S src/native -B build/w-W2-01 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-01 --target w2_script_vm_test -j
build/w-W2-01/w2_script_vm_test --elf "$RT_ELF" [--verbose]; echo $?     # 0 PASS, 1 FAIL, 2 NO_ELF
python3 ref.py     # appendix; compare with --verbose output
```

Note: after adding a new `tests/native_w2/*.cpp` the build dir must be re-configured once (the glob is evaluated at configure time with the Makefile generator).

## Deviations / judgement calls (please review)

1. **`branch_oob=0` is defined as strict OOB (idx ≥ length) excluding one "bleed"**: table 0x2A7138 (town 7, slot 28) op 08's second branch idx == its length (10), and the next table follows immediately, so the original reads that table's name word. This is source data, faithfully reproduced; reported as `branch_bleed=1` rather than hidden. If the coordinator wants it counted as OOB, the score would be `branch_oob=1` (threshold miss) — the decision is the coordinator's, the data is pinned either way.
2. SCORE line carries two extra trailing tokens (`branch_bleed`, `extra_vectors`) before `PASS|FAIL`; the pre-registered tokens are unchanged and in order.
3. Plan Owns lists `script_image.{h,cpp}`; created (brief said "expect" only script_vm). Uses only the ELF; no CMake change needed (EXISTS-guarded sources).
4. VM adds `maxOps` (default 65536) — the original has no bound.
5. "Unknown ops = 0" is measured over **entry** prefixes (spec (b)); over all 2867 references, 3 start with op 0 / zero bytes (non-entry, unreachable via prefix branches).

## Confidence

* Op widths/branch semantics/unknown rule: **high** (disassembly + decomp + clean decode of 2867 real references on exact boundaries + independent Python VM).
* Table length rule: **high** for this ELF (3 independent end conditions, insensitive bound); the length is a derived notion (not stored in-game).
* Host callee semantics (what the flag bitmasks mean for gameplay): **medium**, taken from helper decomps/symbols; VM does not depend on them.

## Open questions

1. Meaning of shop `+0x14` (`word14`, op 0x18 gate 54000..198000) — likely a price/prize value; the 0x22AEB0 spawner writes 54000.
2. What `+0x22` (label) is vs `+0x23` (town) — the unknown-op report prints `+0x22`.
3. The 3 non-clean references (two point at zero bytes, one is op 0E then 0x00): dead data or reached only via text escape codes (escape arguments index the same tables — W2-07 scope).
4. Escape-code references (`RenderEscapeText` / dialogueTask switch) also index the level-2 tables; a full reachability closure needs W2-07's text decoder.
5. `GetFlaggedSlotCount` arg meanings (slot `a`, bit `b`, third param 0 = save slot) and `ReleaseStampFlag` side effects are only described by symbols; binding to SaveData is later-wave work.

## Appendix — independent Python reference (ELF read-only; no bytes embedded)

```python
import struct,collections,sys
import os
p=os.environ.get('RT_ELF',os.path.expanduser('~/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98'))
d=open(p,'rb').read()
off=lambda v:v-0x1FF000
def u32(v): return struct.unpack('<I',d[off(v):off(v)+4])[0]
def u8(v): return d[off(v)]
M=(1<<64)-1
def fnv():
    class F:
        h=0xcbf29ce484222325
        def add(s,v,n=8):
            v&=M
            for i in range(n):
                s.h=((s.h^((v>>(8*i))&0xff))*0x100000001b3)&M
    return F()
DEND=0x29a880+0x9adfb
top=[u32(0x2A7ED0+4*i) for i in range(1,22)]
slots=[]; l2=set()
for a in top:
    n=0
    while u32(a+4*n)!=0: l2.add(u32(a+4*n)); n+=1
    slots.append(n)
starts=sorted(l2)
info=[]
for i,a in enumerate(starts):
    nxt=starts[i+1] if i+1<len(starts) else None
    lim=nxt if nxt else DEND
    n=0
    while a+4*n+4<=lim and 0x2F0000<=u32(a+4*n)<DEND: n+=1
    e=a+4*n
    kind=3 if nxt is None else (0 if e==nxt else (1 if u32(e)==0 else 2))
    info.append((a,n,kind))
ch=fnv()
for v in top: ch.add(v,4)
for v in slots: ch.add(v,4)
for a,n,k in info: ch.add(a,4); ch.add(n,4); ch.add(k,1)
print('towns',len(top),'slots',sum(slots),'l2',len(starts),'words',sum(n for a,n,k in info),collections.Counter(k for a,n,k in info),hex(ch.h))
W={1:(3,[2]),2:(3,[2]),3:(4,[3]),4:(2,[1]),5:(3,[2]),6:(3,[2]),7:(2,[1]),8:(3,[1,2]),0xb:(3,[2]),0xd:(3,[2]),0xe:(7,[1,2,3,4,5,6]),0xf:(2,[]),0x10:(2,[]),0x11:(3,[]),0x12:(1,[]),0x13:(2,[]),0x14:(3,[]),0x15:(2,[]),0x16:(2,[1]),0x17:(2,[]),0x18:(2,[1]),0x19:(3,[2]),0x1a:(3,[2]),0x1b:(2,[1]),0x1c:(3,[2]),0x1d:(4,[3])}
def dec(ptr,L,adj):
    p_=ptr; ops=[]; bo=0; bl=0; oob=0
    while True:
        b=u8(p_)
        if b>=0x20: return 0,ops,b,p_,bo,bl,oob
        if b not in W: return 1,ops,b,p_,bo,bl,oob
        w,br=W[b]; ops.append(b)
        for o in br:
            bo+=1; i=u8(p_+o)
            if i>=L:
                if i==L and adj: bl+=1
                else: oob+=1
        p_+=w
th=fnv(); hist=collections.Counter(); refs=0; clean=0; unk=0; opsd=0; bot=0; blt=0; oobt=0; dist=set(); ec=0
for a,n,k in info:
    adj=(k==0)
    st,ops,b,e,_,_,_=dec(u32(a+4),n,adj)
    if st==0: ec+=1
    for i in range(1,n):
        ptr=u32(a+4*i); refs+=1; dist.add(ptr)
        st,ops,b,e,bo,bl,oob=dec(ptr,n,adj)
        if st==0: clean+=1
        else: unk+=1
        th.add(a,4); th.add(i,4); th.add(st,1); th.add(len(ops),4); th.add(b,1); th.add(e-ptr,4)
        opsd+=len(ops); bot+=bo; blt+=bl; oobt+=oob
        for o in ops: hist[o]+=1
oh=fnv()
for op in range(32): oh.add(op,1); oh.add(hist[op],4)
print('entryClean',ec,'refs',refs,'distinct',len(dist),'clean',clean,'unk',unk,'ops',opsd,'bo',bot,'bleed',blt,'oob',oobt,hex(th.h),hex(oh.h))
print(sorted((hex(k),v) for k,v in hist.items()))
# python VM for modes 0 and 1
def run(table,mode,town,shop,word14):
    f98=[(M if mode==0 else 0)]*4
    cur=u32(table+4); ops=0; br=0; done=0; res=0
    T=town; S=shop
    tr=1 if mode==0 else 0
    def jump(i):
        nonlocal cur,br
        cur=u32(table+4*i); br+=1
    while True:
        op=u8(cur)
        if op>=0x20: break
        ops+=1
        if ops>4096: raise Exception('limit')
        a1=u8(cur+1); a2=u8(cur+2); a3=u8(cur+3)
        def cond(taken,idx,w):
            nonlocal cur
            if taken: jump(idx)
            else: cur+=w
        tf=lambda i:(f98[i>>6]>>(i&63))&1
        if op==1: cond(tf(a1)!=0,a2,3)
        elif op==2: cond(tf(a1)==0,a2,3)
        elif op==3: cond(tr,a3,4)
        elif op==4: cond(tr,a1,2)
        elif op in(5,6): cond(tr,a2,3)
        elif op==7: cond(tr,a1,2)
        elif op==8:
            if tr: jump(a1)
            elif tr: cur+=3
            else: jump(a2)
        elif op==0xb: cond((2 if mode==0 else 0)==a1,a2,3)
        elif op==0xd: cond(tr,a2,3)
        elif op==0xe:
            if not tf(0x50): cur+=7
            else:
                idx=5 if mode==0 else 0
                jump(u8(cur+1+idx))
        elif op==0xf: f98[a1>>6]|=1<<(a1&63); cur+=2
        elif op==0x10: f98[a1>>6]&=~(1<<(a1&63)); cur+=2
        elif op==0x11: cur+=3
        elif op==0x12: done=1; res=10; cur+=1
        elif op==0x13: cur+=2
        elif op==0x14: done=1; res=12; T=a1; S=a2; cur+=3
        elif op==0x15: cur+=2
        elif op==0x16: cond(tr,a1,2)
        elif op==0x17: cur+=2
        elif op==0x18:
            inr=((word14-54000)&0xffffffff)<0x23281
            cond(not inr,a1,2)
        elif op==0x19: cond(tr,a2,3)
        elif op==0x1a:
            cnt=0 if mode==0 else 100
            cond(a1<=100-cnt,a2,3)
        elif op==0x1b:
            lic=3 if mode==0 else 0
            if lic!=3: cur+=2
            elif tr: jump(a1)
            elif tf(0x57): cur+=2
            else: jump(a1)
        elif op==0x1c:
            t8=T-256 if T>=128 else T
            cond(t8==a1,a2,3)
        elif op==0x1d: cond(tr,a3,4)
        else: raise Exception('unknown op %x'%op)
    return ops,br,done
for mode in (0,1):
    to=tb=td=0
    for ti,a in enumerate(top):
        for s in range(slots[ti]):
            tbl=u32(a+4*s)
            o,b,dn=run(tbl,mode,ti+1,s,54000 if mode==0 else 0)
            to+=o;tb+=b;td+=dn
    print('mode',mode,to,tb,td)
```
