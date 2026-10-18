# W2-05 — race rules report

## Outcome

Added a pure `race_rules` module covering payout lookup, lap split bookkeeping,
licence qualification/award, persistent record-slot remapping and WGP standings.
No rendering, GL, ROM bytes, or integration edits are included.

## Files

* `src/native/gameplay/race_rules.h`
* `src/native/gameplay/race_rules.cpp`
* `tests/native_w2/race_rules_test.cpp`
* `docs/native-reference/race-rules.md`

## Reproducible pins

* `0x29FCB8`: 30 `u32` payout cells. Reproduce:
  `python3 -c "import struct,os;d=open(os.path.expanduser('~/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98'),'rb').read();print(struct.unpack_from('<30I',d,0x29FCB8-0x1FF000))"`.
  Result: `(800,500,400,300,200,100,...,10000,7500,5000,3000,2000,1000)`.
* `0x2BDDD0`, stride `0x10`: 26 records; first 24 have tiers 0--2, slots 24/25
  are `(course,tier)=(17,4)/(15,4)`. Reproduce with the snippet in
  `docs/native-reference/race-rules.md`.
* `0x237C48`: race IDs 26/27 are remapped to slots 24/25; three 26-row best-time
  bands produce 78 storage cells.
* `0x237508`: each tier has three tested vectors (all <=5, one 6, one 0xFF):
  awards only the first, `9/9`.
* `0x236858`: seven-round, eight-driver standings sum bytes and stable-sort
  descending by score; 1,000 deterministic fuzz cases match an independent
  test-side insertion-sort reference.

## Validation

```sh
cmake -S src/native -B build/w-W2-05 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-05 --target w2_race_rules_test -j
RT_ELF="$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98" build/w-W2-05/w2_race_rules_test
```

Expected score: `W2-05 SCORE payout=30/30 licence_vectors=9/9 records=24+2 wgp_fuzz=1000/1000 rank_fuzz=1000/1000 bands=78/78 deterministic=1`.

Without an ELF, the test prints `NO_ELF` and exits zero as required by the
batch dispatch hard rule.

## Confidence / GAP

High for payout, record geometry/remap, licence gating and WGP sorting; each is
directly grounded in the cited functions. Medium for the isolated lap model:
`0x22D390` proves split shifting and best comparison, but its transient
vehicle-flag-to-category mapping is not a persistent `SaveData` rule. It is
left explicitly unmodeled rather than fabricated.
