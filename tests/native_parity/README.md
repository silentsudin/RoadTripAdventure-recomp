# Native UI-parity checks

Automation for the five structural checks in the
[`docs/native-reference/ui-parity.md`](../../docs/native-reference/ui-parity.md)
verification protocol (the "Structural checks runnable now" list, spec lines
128-134). The suite diff's the **recomp ground truth** (`src/ui/`,
`src/game/GameOptions.cpp`) against the native rebuild contract and the native
sources, and reports every divergence in a machine-readable `findings.json`.

It is deliberately **plain `python3` + stdlib + pytest**: offline, headless,
deterministic, no GPU, no build, no network. The whole suite runs in well under a
second.

## Running

```sh
python3 -m pytest tests/native_parity -q
```

The suite is self-contained: `conftest.py` puts this directory on `sys.path`, so
it can be run from the repo root or directly. To see the per-check output and the
write path of the findings file:

```sh
python3 -m pytest tests/native_parity -q -s
```

`findings.json` is (re)written at session finish, next to this file, regardless of
pass/fail.

### Requirements

- `python3` (3.9+; uses `from __future__ import annotations`).
- `pytest`. If it is missing:

  ```sh
  python3 -m pip install pytest
  ```

No third-party runtime dependencies beyond pytest.

## Exit semantics

The suite **passes (exit 0)** in the current tree. Checks fail **only** on a
`DEFECT` finding. `SPEC_DRIFT`, `SPEC_GAP`, and `KNOWN_GAP` are *reported
findings*, not failures — they are the triage queue for the wave-2c UI drawing
work. `INFO` is unused today.

| Category | Meaning | Fails the run? |
|---|---|---|
| `DEFECT` | Contract violation: unrenderable glyph, wrong Theme colour, missing/out-of-order row, stale pinned gap list, unresolvable mapping. | **yes** |
| `SPEC_DRIFT` | The spec document disagrees with itself or with the source (e.g. a stale numeric factor, a literal anchored where it is absent, a declared string source that yields none). | no |
| `SPEC_GAP` | A user-visible ground-truth literal the spec never names. | no |
| `KNOWN_GAP` | A divergence the spec's acceptable-deviations ledger permits (model-layer gaps, FONTE punctuation substitutions, GPU-wave-pending rows). | no |
| `INFO` | Reserved. | no |

A test may record `SPEC_DRIFT`/`SPEC_GAP`/`KNOWN_GAP`; each such test asserts that
the `DEFECT` count has not risen above its `defect_baseline` fixture.

## The five checks

| # | Module | Protocol item | What it does |
|---|---|---|---|
| 1 | `test_string_manifest.py` | 1 — source-literal → manifest diff | Builds the ground-truth string manifest from the spec's exact source ranges (comments stripped, C escapes decoded, adjacent literals joined), cross-checks the spec's own inline citations, and reports literals the spec never names (`SPEC_GAP`) and citations that don't hold (`SPEC_DRIFT`). |
| 2 | `test_row_order.py` | 2 — adapter row order / visibility | Per-screen ordered fixtures (pause root, display, sound, graphics, controllers, device, buttons, driving) assert the ground-truth rows appear in source order, that shared rows trace to their literal, and that every conditional row has a documented visibility predicate. |
| 3 | `test_options_mapping.py` | 3 — adapter ↔ `OptionsModel` | Maps the recomp settings rows onto the 40-row native `OptionsModel` (`src/native/gameplay/settings.cpp`). Pins the model-gap list and the GPU-wave-pending row set so a change in either is noticed. |
| 4 | `test_fonte_coverage.py` | 4 — FONTE glyph/space per character | Ports `fonteSlotForChar` (`src/native/assets/glyph_atlas.h`) and asserts every character of every visible string is renderable or is one of the four ledgered punctuation substitutions. |
| 5 | `test_geometry_color.py` | 5 — numeric theme/rect constants | Implements `S = max(.5, H/1080 * handheldBoost)` and the 640×480 frame mapping; asserts the design-pixel constants at 1080p/2160p, the viewport cap, and the spec's cited Theme colours/strokes against source-parsed `Theme.h`/`Theme.cpp`. |

### Shared module

`parity_spec.py` is the ground-truth reader and finding registry used by all five
checks:

- **Extraction.** `strip_cpp_comments` blanks `//` and `/* */` comments while
  preserving string/char literals and line structure, so a quoted literal inside a
  comment is never mistaken for UI copy. `decode_c_string` decodes `\uNNNN`,
  `\xNN`, octal and simple escapes. `extract_literals` joins adjacent literals
  (C++ concatenation), decoding **each fragment first** so `"\xC2\xB0" "C"`
  becomes `°C` and never the bogus `\xB0C`.
- **Spec parsing.** `parse_spec_citations` resolves the doc's mixed
  `File.cpp:NN-MM` and bare backticked `NN-MM` citations; a bare range inherits
  the most recently named ground-truth file that actually has literals there.
  `spec_literal_drifts` anchors a backticked literal to its nearest citation
  (within 25 chars), skipping the replacement side of a `->` substitution.
- **Model derivation.** `fonte_slot_for_char` / `fonte_slot_uv`,
  `scale` / `pause_geometry`, `display_frame_factors` /
  `native_uniform_frame_factor`, `parse_theme_colors` / `parse_theme_strokes`,
  and `parse_native_options`.
- **Finding registry.** `record`, `summary`, `findings_payload`,
  `write_findings_json`.

## `findings.json` layout

```jsonc
{
  "spec": "docs/native-reference/ui-parity.md",
  "generated_by": "tests/native_parity",
  "summary": { "SPEC_DRIFT": 0, "SPEC_GAP": 0, "KNOWN_GAP": 0, "DEFECT": 0, "INFO": 0 },
  "spec_gap_breakdown": {
    "by_kind": { "format": 0, "label/value": 0, "hint": 0 },
    "uncited": 0                      // gaps whose source line no spec citation covers
  },
  "string_manifest": {
    "declared_source_ranges": { "src/ui/PauseMenu.cpp": [[293, 303], ...], ... },
    "occurrences_extracted": 0,       // every literal occurrence in the ranges
    "distinct_templates": 0,          // distinct decoded templates
    "visible_templates": 0,           // distinct user-visible templates
    "spec_declared_count": 184        // ui-parity.md:90
  },
  "findings": [
    { "category": "SPEC_DRIFT", "check": "check5-geometry-color", "detail": "...", "evidence": { ... } }
  ]
}
```

`evidence` is finding-specific: citations, screen/row names, byte codes,
`line_cited` flags, ledger references, etc.

## Notes and interpretations

- **Ground truth is the spec's declared ranges**, not the spec's prose. The
  manifest is generated mechanically from the source ranges in the Text-parity
  contract table (ui-parity.md:92-100).
- **The "184 string templates" figure is not reproducible mechanically.** The
  suite records `spec_declared_count: 184` alongside the measured
  `occurrences_extracted` / `distinct_templates` / `visible_templates` counts and
  does **not** fail on the difference; the payload is the evidence for triage.
- **`src/platform/Controllers.h:3-78` is declared as a string source but contains
  no string literals**, so check 1 records a `SPEC_DRIFT` for it (informational).
- **The 1080p frame factor in the spec prose is stale.** ui-parity.md:9 quotes
  `1.6875x`; the native source mapping is a single uniform
  `min(W/640, H/480)` factor (2.25× at 1080p, 4.5× at 4K — matching
  `output-modes.md:156`). Check 5 records this as `SPEC_DRIFT`.
- **Theme colours are compared RGB-only when the spec quotes 6 hex digits and
  full 8-hex RGBA when it quotes alpha** (e.g. `485800F2`, `304090F2`,
  `000A2899`).
- The `KNOWN_GAP` set is intentional: 33 from the model-layer/GPU-wave-pending
  mapping plus the three ledgleted FONTE punctuation substitutions (`…`, `—`,
  `°`).

## Non-goals

- This suite does **not** build, run, or render anything, and it does not edit
  `src/`, the existing tests, or the docs. It is read-only over the tree and only
  writes `findings.json` and pytest's cache in this directory.
- It does **not** perform the dual-run pixel comparison (spec lines 136-142);
  that needs a device and a ROM.
