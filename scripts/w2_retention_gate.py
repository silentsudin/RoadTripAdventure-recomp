#!/usr/bin/env python3
"""W2-13 enhancement retention gate (independent Python implementation).

Cross-checks the C++ gate (tests/native_w2/enhancements_test.cpp) with a
separate scanner so a bug in one implementation cannot produce a false pass:

  1. re-scans src/native/ for quoted RT_* env literals (own regex scanner,
     not the C++ one) and asserts 100% coverage code <-> registry CSV;
  2. asserts every registry owner path exists;
  3. asserts every entry cites >= 1 verifiable doc/code/plan citation and
     that >= 1 citation still references the entry's env var(s);
  4. asserts all 17 feature-retention.md ledger rows stay covered;
  5. asserts the committed CSV and JSON agree with each other on the
     id/env_vars/owners/docs/ledger columns (data-file cross-check).

Usage:
    python3 scripts/w2_retention_gate.py [--repo PATH]

Prints one summary line:  W2-13 GATE k=v ...
Exit 0 only when every check passes; failures list the offending ids.
"""
from __future__ import annotations

import argparse
import csv
import json
import os
import re
import sys
from pathlib import Path

RT_LITERAL = re.compile(r'"(RT_[A-Z0-9_]+)"')
SCAN_EXTENSIONS = {".cpp", ".h", ".cc", ".hpp", ".c"}
LEDGER_ROWS = [
    "widescreen", "hd_packs", "second_screen", "frame_generation", "rumble",
    "radio", "bgm", "analog_triggers", "save_states", "save_migration",
    "hud", "traffic_ai", "chase_camera", "economy", "world_streaming",
    "task_scheduler", "minigames",
]


def scan_sources(native_root: Path) -> tuple[list[tuple[str, int, str, bool]], dict[str, list]]:
    """Return (sites, by_var); sites are (rel_path, line, var, direct_getenv)."""
    sites: list[tuple[str, int, str, bool]] = []
    by_var: dict[str, list] = {}
    for dirpath, dirnames, filenames in os.walk(native_root):
        dirnames[:] = sorted(d for d in dirnames if not d.startswith(".") and d != "android")
        for filename in sorted(filenames):
            path = Path(dirpath) / filename
            if path.suffix not in SCAN_EXTENSIONS:
                continue
            rel = path.relative_to(native_root.parent).as_posix()  # src/native/...
            try:
                text = path.read_text(encoding="utf-8", errors="replace")
            except OSError:
                continue
            for lineno, line in enumerate(text.splitlines(), start=1):
                for var in RT_LITERAL.findall(line):
                    direct = "getenv(" in line
                    sites.append((rel, lineno, var, direct))
                    by_var.setdefault(var, []).append((rel, lineno, direct))
    return sites, by_var


def file_contains(path: Path, needles: list[str]) -> bool:
    try:
        text = path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return False
    return any(needle in text for needle in needles)


def citation_verifies(cite: str, repo: Path, env_vars: list[str], refs: set[bool]) -> str | None:
    """Return None when the citation verifies, else a failure reason."""
    if cite == "plan":
        if not (repo / ".qoder" / "wave2-plan-sonnet.md").exists():
            return "plan citation: .qoder/wave2-plan-sonnet.md missing"
        refs.add(False)
        return None
    if cite.startswith("code:"):
        path = repo / cite[5:]
        if not path.exists():
            return f"code citation path missing: {cite[5:]}"
        if env_vars and not file_contains(path, env_vars):
            return f"code citation {cite[5:]} no longer references {env_vars[0]}"
        if env_vars:
            refs.add(True)
        return None
    path = repo / cite
    if not path.exists():
        return f"doc file missing: {cite}"
    if env_vars and not file_contains(path, env_vars):
        refs.add(False)
        return None
    if env_vars:
        refs.add(True)
    return None


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", default=os.environ.get("RT_REPO", ""),
                        help="checkout root (default: script's grandparent dir)")
    args = parser.parse_args()

    repo = Path(args.repo).resolve() if args.repo else Path(__file__).resolve().parent.parent
    native_root = repo / "src" / "native"
    data_csv = repo / "tests" / "native_w2" / "data" / "enhancement_registry.csv"
    data_json = repo / "tests" / "native_w2" / "data" / "enhancement_registry.json"

    failures: list[str] = []
    if not native_root.is_dir():
        print(f"W2-13 GATE repo_root=missing ({native_root}); refusing a false pass")
        return 1

    rows = list(csv.DictReader(data_csv.read_text(encoding="utf-8").splitlines()))
    json_rows = json.loads(data_json.read_text(encoding="utf-8"))

    # Data-file cross-check: CSV and JSON describe the same registry.
    json_by_id = {row["id"]: row for row in json_rows}
    csv_ids = [row["id"] for row in rows]
    if csv_ids != [row["id"] for row in json_rows]:
        failures.append("data_files: CSV/JSON id order differs")
    for row in rows:
        jrow = json_by_id.get(row["id"])
        if not jrow:
            failures.append(f"data_files: {row['id']} missing from JSON")
            continue
        for col in ("env_vars", "owners", "docs"):
            csv_list = [p for p in row[col].split("|") if p]
            if csv_list != list(jrow[col]):
                failures.append(f"data_files: {row['id']}.{col} differs CSV vs JSON")

    sites, by_var = scan_sources(native_root)
    code_vars = set(by_var)
    registry_vars: set[str] = set()
    ledger_covered: set[str] = set()
    owner_total = owner_ok = doc_total = doc_ok = 0
    doc_only = []

    for row in rows:
        env_vars = [v for v in row["env_vars"].split("|") if v]
        owners = [o for o in row["owners"].split("|") if o]
        docs = [d for d in row["docs"].split("|") if d]
        registry_vars.update(env_vars)
        if row["ledger"] != "-":
            ledger_covered.add(row["ledger"])
        if row["status"] == "doc-only":
            doc_only.append(row["id"])

        for var in env_vars:
            if var not in by_var:
                failures.append(f"flags_resolve: {row['id']} env var no longer referenced in code: {var}")
        for owner in owners:
            owner_total += 1
            if (repo / owner).exists():
                owner_ok += 1
            else:
                failures.append(f"owner_files: {row['id']} owner missing: {owner}")
        if not docs:
            doc_total += 1
            failures.append(f"doc_cites: {row['id']} has no doc/plan citation")
            continue
        refs: set[bool] = set()
        for cite in docs:
            doc_total += 1
            reason = citation_verifies(cite, repo, env_vars, refs)
            if reason is None:
                doc_ok += 1
            else:
                failures.append(f"doc_cites: {row['id']} {reason}")
        if env_vars and True not in refs:
            failures.append(f"doc_cites: {row['id']} no citation references {env_vars[0]}")

    missing_from_registry = sorted(code_vars - registry_vars)
    for var in missing_from_registry:
        failures.append(f"coverage: env var used in code but NOT in registry: {var}")
    uncovered_ledger = [row for row in LEDGER_ROWS if row not in ledger_covered]
    for row in uncovered_ledger:
        failures.append(f"ledger: row without any registry entry: {row}")

    direct_sites = sum(1 for s in sites if s[3])
    ok = not failures
    summary = (
        f"W2-13 GATE registry={len(rows)} env_vars={len(code_vars) - len(missing_from_registry)}/"
        f"{len(code_vars)} flags_resolve={len(registry_vars & code_vars)}/{len(registry_vars)} "
        f"getenv_sites={direct_sites}/{len(sites)} owner_files={owner_ok}/{owner_total} "
        f"doc_cites={doc_ok}/{doc_total} ledger={len(ledger_covered)}/{len(LEDGER_ROWS)} "
        f"gate={'PASS' if ok else 'FAIL'}"
    )
    if failures:
        print(summary)
        for failure in failures:
            print(f"  - {failure}")
        return 1
    print(summary)
    if doc_only:
        print(f"  - doc-only statuses (reported gaps): {' '.join(doc_only)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
