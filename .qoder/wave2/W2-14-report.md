# W2-14 report — Oracle score harness v2

## Summary

Implemented an independent region-aware harness and regression self-test. The four registered v13 histogram continuity values reproduce exactly to three decimals. The coordinator amended acceptance; the named-ROI registry classifies all eight required entries with evidence-grounded ELEMENT transforms or reasoned GAPs. No unsupported FIT claim is made.

### Acceptance amendment record

Original criterion (wave2-plan §W2-14): “a per-scene mapping table pinned with fit residual ≤ 1.0 px” and `ymap_fits=<n>/<n>`; the plan also calls for ≥3 landmark pairs per scene. Coordinator ruling replaces it with a named-ROI y-map registry where each required ROI is FIT (≥3 grounded pairs, residual ≤1 px), ELEMENT (documented per-element constants), or GAP (reasoned absence). Final score format is `ymap_status=<fit>+<element>+<gap>/<total>`. This is the coordinator-authorized threshold amendment; it resolves the contradiction between one scene map and element-specific evidence.

## Files

- `scripts/w2_score.py` — alpha-safe metrics, named canvas/full-resolution ROIs.
- `tests/native_w2/score_selftest.py` — baseline, identity, shifted-image sensitivity, alpha, determinism checks.
- `docs/native-reference/score-harness.md` — usage, metric and evidence notes.
- `.qoder/wave2/W2-14-report.md` — this report.

## Key numbers and reproduction

Command: `python3 tests/native_w2/score_selftest.py`

Observed: `baseline_hist=0.872/0.460/0.532/0.374`, `region_scores=15`, `ymap_status=0+2+6/8`, `self_noise=0.000`, `oracle_vs_oracle=1.000`, `PASS`. These match v13 expected baselines in `.qoder/wave2-plan-sonnet.md:386-389`; exact source images are enumerated by `oracle-golden-frames.md:45-55`. The score and ROI status are revalidated by running the test. Baseline values are measured using the original `scripts/frame_compare.py` continuity metric.

Example diagnostic: `python3 scripts/w2_score.py --scene race --oracle build/scratch/oracle-golden/race_hud_driving.png --native build/scratch/gamelook/race-v13.ppm`.

## Confidence

High for continuity, registry schema/classification, and metric self-checks; medium for initial ROI windows (broad, not hand-segmented masks). Alpha compositing and shifted-image sensitivity are self-tested synthetically. ELEMENTs are directly grounded at `menu-calibration.md:15-24`, `gauge-calibration.md:3-8`, and `.qoder/wave2-plan-sonnet.md:23-27`; all other mappings remain explicitly GAP.

## Open questions

No remaining acceptance blocker. The six GAP classifications may be upgraded only when new cited native/oracle correspondence evidence is supplied; do not silently promote them to FIT or borrow a transform from another ROI.
