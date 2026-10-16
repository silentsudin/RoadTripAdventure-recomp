"""Check 1 - string-manifest diff (ui-parity.md verification protocol item 1).

Builds the ground-truth string manifest from the spec's declared source ranges,
cross-checks the spec's own inline citations, and reports:

* SPEC_DRIFT - the spec anchors a literal to a source location where that
  literal is not present, or declares a string-source range that yields none.
* SPEC_GAP   - a user-visible ground-truth literal the spec never names.

Neither category is a build failure at this stage (they are triaged findings);
only DEFECT would fail the check, and check 1 does not emit DEFECT.
"""

from __future__ import annotations

import parity_spec as ps

CHECK = "check1-string-manifest"


# --- declaration / extraction ------------------------------------------------

def test_declared_source_ranges_exist_and_are_ordered():
    for path, ranges in ps.STRING_SOURCES.items():
        full = ps.REPO_ROOT / path
        assert full.is_file(), f"declared string source missing: {path}"
        line_count = len(full.read_text(encoding="utf-8", errors="replace").split("\n"))
        for a, b in ranges:
            assert 1 <= a <= b <= line_count, f"{path}: bad declared range {a}-{b}"


def test_manifest_is_deterministic():
    first = ps.manifest()
    second = ps.manifest()
    assert [(s.file, s.line, s.value) for s in first] == \
           [(s.file, s.line, s.value) for s in second]


def test_manifest_has_user_visible_templates(manifest):
    visible = [s for s in manifest if ps.is_user_visible(s.value)]
    # The spec claims a 184-template inventory (ui-parity.md:90). We do not fail
    # when a mechanical extraction differs, but the manifest must be non-empty
    # and cover every declared source.
    assert visible, "no user-visible strings extracted"
    covered = {s.file for s in visible}
    assert set(ps.STRING_SOURCES) - covered <= {"src/platform/Controllers.h"}


def test_declared_string_source_yields_literals():
    """A declared string-source range that has zero literals is spec drift."""
    for path, ranges in ps.STRING_SOURCES.items():
        literals = ps.extract_literals(path, ranges)
        if not literals:
            ps.record("SPEC_DRIFT", CHECK,
                      f"declared string source {path} {ranges} contains no string literals",
                      file=path, ranges=[list(r) for r in ranges])
    # informational only - never a hard failure (a drift is a report finding).
    assert True


# --- inline-citation cross-check ---------------------------------------------

def test_spec_citations_resolve():
    cites = ps.parse_spec_citations()
    assert cites, "no citations parsed from ui-parity.md"
    infile = [c for c in cites if c.resolved]
    assert infile, "no ground-truth citations resolved"


def test_no_spec_literal_drift(defect_baseline):
    drifts = ps.spec_literal_drifts()
    for d in drifts:
        ps.record("SPEC_DRIFT", CHECK,
                  f"spec anchors {d['literal']!r} to {d['citation']} where it is absent",
                  **d)
    assert ps.defect_count() == defect_baseline


# --- spec gaps ---------------------------------------------------------------

def _documented_values() -> set[str]:
    """Every normalized spec backtick token plus normalized doc substrings."""
    normalized = {ps.normalize_for_spec(t) for _, t in ps.spec_backtick_tokens()}
    return normalized


def test_ground_truth_literals_missing_from_spec(defect_baseline):
    doc_norm = ps.normalize_for_spec(ps.load_spec_text())
    tokens = _documented_values()
    # Source lines the spec explicitly cites (per ground-truth file): a literal on
    # one of these lines is "covered by range citation" even when the doc does not
    # quote it verbatim (e.g. "exact strings at lines 563-597").
    cited_lines: dict[str, set[int]] = {}
    for c in ps.parse_spec_citations():
        if c.resolved in ps.STRING_SOURCES:
            span = cited_lines.setdefault(c.resolved, set())
            span.update(range(c.start, c.end + 1))

    gaps = []
    for s in ps.manifest(only_visible=True):
        if not ps.is_user_visible(s.value):
            continue
        # Concatenation fragments (leading/trailing space) are assembly pieces,
        # not standalone templates.
        if s.value != s.value.strip():
            continue
        nv = ps.normalize_for_spec(s.value)
        if nv in tokens:
            continue
        if len(nv) >= 16 and nv in doc_norm:
            continue        # long copy quoted verbatim in the doc
        if len(nv) <= 3:
            continue
        gaps.append(s)

    distinct: dict[str, ps.ManifestString] = {}
    for s in gaps:
        distinct.setdefault(s.value, s)
    for value, s in sorted(distinct.items()):
        ps.record("SPEC_GAP", CHECK,
                  f"ground-truth literal not named in ui-parity.md: {value!r}",
                  citation=s.citation, length=len(value),
                  kind=ps.classify_literal(value),
                  line_cited=s.line in cited_lines.get(s.file, set()))
    # report-only: a gap list is expected until triaged.
    assert ps.defect_count() == defect_baseline
    assert len(distinct) >= 0
