// Enhancement retention registry (wave-2 batch W2-13).
//
// Single source of truth for every QoL enhancement / harness contract this
// native rebuild ships: id, RT_* env flag(s), persisted settings key(s),
// default, mode gating (headless / device / disc), owning module path(s),
// doc citation(s), and retention status. The committed machine-readable
// copies live in tests/native_w2/data/enhancement_registry.{csv,json} and
// are byte-verified against this table by w2_enhancements_test, so a
// registry edit that forgets the data files (or vice versa) fails the gate.
//
// The regression gate (checkRetentionCoverage) re-scans src/native at
// runtime and asserts BOTH directions:
//   1. every RT_* env literal referenced by code appears in the registry;
//   2. every registry env flag still appears in code (a flag whose consumer
//      was deleted fails the gate — the "silently dropped enhancement" case);
//   3. every owner path exists; every entry cites at least one verifiable
//      doc (docs/native-reference file), code site, or the wave-2 plan, and
//      at least one citation still references the entry's env var(s);
//   4. all 17 feature-retention.md ledger rows are covered by >= 1 entry.
// A coverage failure lists the missing ids; there are no false passes.
#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace rt::native::gameplay {

// Retention status taxonomy (superset of the live/wired/oracle-only triad;
// "doc-only" marks a documented flag with no runtime consumer — a gap the
// report tracks, e.g. RT_MENU):
//   live        — active in the shipped app loop (default-on or matrix-proven)
//   wired       — implemented + opt-in / settings-glued, contract proven
//   oracle-only — harness, tool, or diagnostic aid (never user-facing)
//   doc-only    — documented/spec'd but no runtime getenv consumer (gap)
enum class RetentionStatus { Live, Wired, OracleOnly, DocOnly };

const char* retentionStatusName(RetentionStatus status);
RetentionStatus retentionStatusFromName(const std::string& name);

struct EnhancementSpec {
    const char* id;             // stable registry id (snake_case)
    const char* ledger;         // feature-retention.md ledger row id, or "-"
    const char* envVars;        // '|'-separated RT_* env names ("" if none)
    const char* persistedKeys;  // '|'-separated settings.toml keys ("" if none)
    const char* defaultValue;   // compiled-in default behaviour
    const char* gating;         // mode gating: headless/device/disc/always
    const char* owners;         // '|'-separated owning module paths (repo-relative)
    const char* docs;           // '|'-separated citations:
                                //   docs/native-reference/<file>.md
                                //   code:<repo-relative source path>
                                //   plan  (.qoder/wave2-plan-sonnet.md §W2-13)
    RetentionStatus status;
    const char* notes;
};

// The registry, in stable emission order (CSV/JSON row order).
const std::vector<EnhancementSpec>& enhancementRegistry();

// Canonical feature-retention.md ledger row ids (17 rows, 2026-10-16 audit),
// in ledger order. The gate asserts every one is covered by >= 1 entry.
const std::vector<std::string>& retentionLedgerRows();

// One quoted RT_* literal occurrence found in a scanned source line.
struct EnvUsageSite {
    std::string file;   // repo-relative path (src/native/...)
    int line = 0;       // 1-based
    std::string var;    // e.g. RT_APP_RUMBLE
    bool directGetenv = false;  // the literal sits on a getenv() call line
};

struct EnvScanResult {
    std::vector<EnvUsageSite> sites;               // every quoted RT_* literal
    std::map<std::string, std::vector<EnvUsageSite>> byVar;
    int directGetenvSites = 0;                     // subset of sites on getenv lines
    std::uint64_t siteListHash = 0;                // FNV-1a over the site list
};

// Walks <nativeRoot> (…/src/native) and extracts every quoted RT_[A-Z0-9_]+
// literal from .c/.cc/.cpp/.hpp/.h sources. Vendored/generated trees are
// excluded (the android/ Gradle+SDL3 tree and any dotted build dir such as
// .cxx); project code lives in app/assets/audio/gameplay/preview/runtime/
// tools plus top-level headers, all of which are scanned. Deterministic:
// directory order is sorted, files are sorted within each directory.
EnvScanResult scanRtEnvUsage(const std::filesystem::path& nativeRoot);

struct CoverageIssue {
    std::string id;     // offending registry id, env var, or ledger row
    std::string detail; // human-readable failure reason (no false passes)
};

struct CoverageReport {
    // Direction 1: code → registry.
    std::vector<std::string> codeVarsMissingFromRegistry;
    // Direction 2: registry → code.
    std::vector<std::string> registryVarsAbsentFromCode;
    // Owner paths that do not exist on disk.
    std::vector<CoverageIssue> missingOwners;
    // Entries with an empty docs field, or a citation that does not verify.
    std::vector<CoverageIssue> docIssues;
    // Ledger rows with no covering registry entry.
    std::vector<std::string> ledgerUncovered;
    // Registry ids whose status is doc-only (reported, not fatal).
    std::vector<std::string> docOnlyIds;

    std::size_t registryEntries = 0;
    std::size_t registryEnvEntries = 0;   // entries with >= 1 env var
    std::size_t distinctCodeVars = 0;
    std::size_t coveredCodeVars = 0;      // distinctCodeVars found in registry
    std::size_t ownerCount = 0;           // owner paths checked
    std::size_t docCiteCount = 0;         // citations checked
    std::size_t ledgerCovered = 0;

    bool ok() const;
};

// Full gate. repoRoot is the checkout root (owner paths and doc citations are
// resolved against it). Pass a custom registry span for the self-test.
CoverageReport checkRetentionCoverage(const EnvScanResult& scan,
                                      const std::filesystem::path& repoRoot);
CoverageReport checkRetentionCoverage(const std::vector<EnhancementSpec>& registry,
                                      const EnvScanResult& scan,
                                      const std::filesystem::path& repoRoot);

// Deterministic CSV emission (header + one row per registry entry, fixed
// column order, no timestamps). Must byte-match the committed
// tests/native_w2/data/enhancement_registry.csv.
std::string registryCsv();
// Deterministic JSON emission (array of objects, fixed key order).
std::string registryJson();

}  // namespace rt::native::gameplay
