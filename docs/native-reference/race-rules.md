# Race rules (W2-05)

Pure host-side scoring rules grounded in the SLUS-203.98 ELF.  The module is
`src/native/gameplay/race_rules.{h,cpp}`; it does not execute guest code or
embed game bytes.

## Pins

* **Payout ladder**: `u32[5][6]` at ELF virtual address `0x29FCB8`.  Consumers
  are `SumPlayerPoints` (`0x236238`) and `UpdateRankingsAndAdvance`
  (`0x236858`). Place is zero-based; place >= 6 earns zero.
* **Race records**: 26 records with stride `0x10` at `0x2BDDD0`; course byte
  is +0, tier byte is +3. Records 0--23 are licence races (tiers 0--2), while
  records 24/25 are WGP (`course=17/15`, `tier=4`).
* **Result remap**: `UpdateRaceRecords` (`0x237C48`, lines 35--46) converts
  race IDs 26/27 to record slots 24/25. Best lap/total arrays are three bands
  of 26 (`SaveData` offsets `0x1040`/`0x1178`, 78 rows total).
* **Licence**: `EnterLicenseScreen` (`0x237508`, lines 42--72) scans all 24
  licence records whose tier equals the current licence and requires stored
  place-1 <= 5. `0xFF` (not raced) therefore fails. It awards exactly +1,
  capped by the no-Super-A guard (`licence == 3`).
* **WGP**: `0x236858`, lines 37--78 sums each driver's three-byte score per
  completed round, then uses a strict-greater selection sort. Thus equal totals
  retain driver input order. At round 7 only the top six get the tier ladder
  payout.
* **Lap core**: `RecordLapSplitTimes` (`0x22D390`, lines 20--67) compares lap
  timer to category best, stores up to three split cells then shifts the window,
  clears timer and increments lap. Task flags/SFX are intentionally outside the
  pure model.

## Reproduction

```sh
python3 - <<'PY'
import os,struct
p=os.path.expanduser('~/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98')
b=open(p,'rb').read();o=0x29FCB8-0x1FF000
for t in range(5): print(struct.unpack_from('<6I',b,o+t*24))
o=0x2BDDD0-0x1FF000
print([(i,)+struct.unpack_from('<4B',b,o+i*16) for i in range(26)])
PY
```

## GAP

The 0x22D390 category index derives from live vehicle flags and the precise
record-table RAM layout is not a persistent `SaveData` schema. The module keeps
the proven timer/split behavior only; it does not invent a mapping for those
transient categories.
