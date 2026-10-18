// W2-13 — Enhancement retention regression gate (wave-2 batch W2-13).
//
// Self-verifying gate over the enhancement registry (gameplay/enhancements.*):
//   1. re-scans src/native/ at runtime for quoted RT_* env literals and
//      asserts 100% coverage in BOTH directions (code → registry and
//      registry → code). A coverage failure lists the missing ids — this
//      gate is built to have no false passes;
//   2. asserts every registry owner path exists and every entry cites a
//      verifiable doc / code site / plan reference;
//   3. asserts all 17 feature-retention.md ledger rows stay covered;
//   4. asserts the committed data files (tests/native_w2/data/
//      enhancement_registry.{csv,json}) are byte-identical to the compiled
//      registry (registry edit without data regeneration fails);
//   5. runs an in-memory self-test proving the gate detects a deliberately
//      removed flag, an unregistered env var, a broken owner path, and an
//      uncited entry (no file mutations);
//   6. determinism: the source scan runs twice and must produce an
//      identical FNV-1a site-list hash.
//
// No ELF is needed (env/registry contract test): --elf / RT_ELF are
// accepted and ignored, and the SCORE line is always printed (no NO_ELF
// exit). Exit 0 only when every threshold holds; exit 1 otherwise.
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "gameplay/enhancements.h"

namespace {

namespace fs = std::filesystem;
using rt::native::gameplay::CoverageReport;
using rt::native::gameplay::EnvScanResult;
using rt::native::gameplay::EnhancementSpec;

struct Args {
    std::filesystem::path repoRoot;
    std::string elfPath;  // accepted, unused (no ELF needed for this gate)
};

Args parseArgs(int argc, char** argv) {
    Args args;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        const bool hasValue = i + 1 < argc;
        if (arg == "--repo" && hasValue) args.repoRoot = argv[++i];
        else if (arg == "--elf" && hasValue) args.elfPath = argv[++i];
        else if (arg.rfind("--repo=", 0) == 0) args.repoRoot = arg.substr(7);
        else if (arg.rfind("--elf=", 0) == 0) args.elfPath = arg.substr(6);
    }
    if (args.elfPath.empty()) {
        if (const char* env = std::getenv("RT_ELF")) args.elfPath = env;
    }
    if (args.repoRoot.empty()) {
        if (const char* env = std::getenv("RT_REPO")) args.repoRoot = env;
    }
    // Fallbacks: walk up from the executable location, then from the working
    // directory, looking for the repo markers.
    auto looksLikeRepo = [](const fs::path& dir) {
        std::error_code ec;
        return fs::exists(dir / "src" / "native" / "CMakeLists.txt", ec) &&
               fs::exists(dir / "tests" / "native_w2", ec);
    };
    if (args.repoRoot.empty()) {
        std::vector<fs::path> starts;
        if (argv[0]) {
            std::error_code ec;
            auto absolute = fs::absolute(fs::path(argv[0]), ec);
            starts.push_back(absolute.parent_path());
        }
        starts.push_back(fs::current_path());
        for (const auto& start : starts) {
            fs::path dir = start;
            for (int depth = 0; depth < 8 && !dir.empty(); ++depth) {
                if (looksLikeRepo(dir)) { args.repoRoot = dir; break; }
                if (!dir.has_parent_path()) break;
                dir = dir.parent_path();
            }
            if (!args.repoRoot.empty()) break;
        }
    }
    return args;
}

// FNV-1a over a string (data-file / determinism hashing).
std::uint64_t fnv1a(const std::string& data) {
    std::uint64_t hash = 1469598103934665603ull;
    for (unsigned char c : data) { hash ^= c; hash *= 1099511628211ull; }
    return hash;
}

bool readFileIfPresent(const fs::path& path, std::string& out) {
    std::error_code ec;
    if (!fs::exists(path, ec)) return false;
    std::ifstream input(path, std::ios::binary);
    if (!input) return false;
    std::ostringstream buffer;
    buffer << input.rdbuf();
    out = buffer.str();
    return true;
}

std::string firstDiffLine(const std::string& expected, const std::string& actual) {
    std::size_t i = 0;
    while (i < expected.size() && i < actual.size() && expected[i] == actual[i]) ++i;
    std::size_t begin = expected.rfind('\n', i == 0 ? 0 : i - 1);
    begin = (i == 0 || begin == std::string::npos) ? 0 : begin + 1;
    std::size_t end = expected.find('\n', begin);
    if (end == std::string::npos) end = expected.size();
    return "byte " + std::to_string(i) + ", committed line: \"" +
           expected.substr(begin, std::min<std::size_t>(end - begin, 120)) + "\"";
}

struct Check {
    std::string label;
    bool pass = false;
    std::size_t got = 0, total = 0;
    std::vector<std::string> failures;  // ids / reasons, always listed on miss
};

}  // namespace

int main(int argc, char** argv) {
    const Args args = parseArgs(argc, argv);
    std::vector<Check> checks;
    auto addCheck = [&checks](Check check) { checks.push_back(std::move(check)); };

    const fs::path nativeRoot = args.repoRoot / "src" / "native";
    const fs::path dataDir = args.repoRoot / "tests" / "native_w2" / "data";

    if (args.repoRoot.empty() || !fs::exists(nativeRoot)) {
        Check repo{"repo_root", false, 0, 1};
        repo.failures.push_back(
            "could not locate the checkout root (pass --repo <path> or set RT_REPO); "
            "refusing to report a false pass");
        addCheck(std::move(repo));
        // Emit the single SCORE line and fail.
        std::cout << "W2-13 SCORE repo_root=0/1 coverage=0/1 deterministic=0\n";
        return 1;
    }

    // --- 1. Runtime re-scan of src/native (twice, determinism) -------------
    const EnvScanResult scan = rt::native::gameplay::scanRtEnvUsage(nativeRoot);
    const EnvScanResult scan2 = rt::native::gameplay::scanRtEnvUsage(nativeRoot);

    Check deterministic{"deterministic", scan.siteListHash == scan2.siteListHash &&
                                            rt::native::gameplay::registryCsv() ==
                                                rt::native::gameplay::registryCsv(),
                        scan.siteListHash == scan2.siteListHash ? 1u : 0u, 1u};
    addCheck(std::move(deterministic));

    // --- 2. Coverage gate ---------------------------------------------------
    const CoverageReport report = rt::native::gameplay::checkRetentionCoverage(scan, args.repoRoot);

    // Direction 1 (the batch threshold): every env var referenced by code
    // must exist in the registry. 100% or fail, listing the missing ids.
    Check coverage{"coverage", report.codeVarsMissingFromRegistry.empty(),
                   report.coveredCodeVars, report.distinctCodeVars};
    for (const auto& var : report.codeVarsMissingFromRegistry) {
        coverage.failures.push_back("env var used in code but NOT in registry: " + var);
    }
    addCheck(std::move(coverage));

    // Direction 2: every registered flag must still be consumed somewhere in
    // src/native (the silently-dropped-enhancement case).
    const std::size_t registryVarTotal = [&] {
        std::set<std::string> names;
        for (const auto& spec : rt::native::gameplay::enhancementRegistry()) {
            std::string raw(spec.envVars);
            if (raw.empty()) continue;
            std::size_t start = 0;
            while (start < raw.size()) {
                std::size_t stop = raw.find('|', start);
                if (stop == std::string::npos) stop = raw.size();
                if (stop > start) names.insert(raw.substr(start, stop - start));
                start = stop + 1;
            }
        }
        return names.size();
    }();
    Check consumers{"flags_resolve", report.registryVarsAbsentFromCode.empty(),
                    registryVarTotal - report.registryVarsAbsentFromCode.size(),
                    registryVarTotal};
    for (const auto& var : report.registryVarsAbsentFromCode) {
        consumers.failures.push_back("registry env var no longer referenced in code: " + var);
    }
    addCheck(std::move(consumers));

    // --- 3. Owner paths ------------------------------------------------------
    Check owners{"owner_files", report.missingOwners.empty(), report.ownerCount - report.missingOwners.size(),
                 report.ownerCount};
    for (const auto& issue : report.missingOwners) {
        owners.failures.push_back(issue.id + ": " + issue.detail);
    }
    addCheck(std::move(owners));

    // --- 4. Doc / plan citations ---------------------------------------------
    Check docs{"doc_cites", report.docIssues.empty(), report.docCiteCount - report.docIssues.size(),
               report.docCiteCount};
    for (const auto& issue : report.docIssues) {
        docs.failures.push_back(issue.id + ": " + issue.detail);
    }
    addCheck(std::move(docs));

    // --- 5. Ledger coverage (feature-retention.md, 17 rows) ------------------
    Check ledger{"ledger", report.ledgerUncovered.empty(), report.ledgerCovered,
                 rt::native::gameplay::retentionLedgerRows().size()};
    for (const auto& row : report.ledgerUncovered) {
        ledger.failures.push_back("ledger row without any registry entry: " + row);
    }
    addCheck(std::move(ledger));

    // --- 6. Committed data files byte-match the compiled registry ------------
    Check data{"data_files", true, 0, 2};
    const std::string csv = rt::native::gameplay::registryCsv();
    const std::string json = rt::native::gameplay::registryJson();
    std::string committed;
    if (!readFileIfPresent(dataDir / "enhancement_registry.csv", committed)) {
        data.pass = false;
        data.failures.push_back("missing tests/native_w2/data/enhancement_registry.csv (regenerate with rt-w2-enh_registry csv)");
    } else if (committed != csv) {
        data.pass = false;
        data.failures.push_back("enhancement_registry.csv drift vs compiled registry: " + firstDiffLine(csv, committed));
    } else {
        ++data.got;
    }
    if (!readFileIfPresent(dataDir / "enhancement_registry.json", committed)) {
        data.pass = false;
        data.failures.push_back("missing tests/native_w2/data/enhancement_registry.json (regenerate with rt-w2-enh_registry json)");
    } else if (committed != json) {
        data.pass = false;
        data.failures.push_back("enhancement_registry.json drift vs compiled registry: " + firstDiffLine(json, committed));
    } else {
        ++data.got;
    }
    addCheck(std::move(data));

    // --- 7. Self-test: the gate must be able to fail --------------------------
    // 7a. A deliberately removed registry entry (RT_APP_RUMBLE) must be
    // reported as uncovered code.
    std::vector<EnhancementSpec> pruned;
    for (const auto& spec : rt::native::gameplay::enhancementRegistry()) {
        if (std::string(spec.id) != "rumble") pruned.push_back(spec);
    }
    const CoverageReport prunedReport =
        rt::native::gameplay::checkRetentionCoverage(pruned, scan, args.repoRoot);
    const bool removedFlagDetected =
        std::find(prunedReport.codeVarsMissingFromRegistry.begin(),
                  prunedReport.codeVarsMissingFromRegistry.end(),
                  std::string("RT_APP_RUMBLE")) != prunedReport.codeVarsMissingFromRegistry.end();

    // 7b. An unregistered env var appearing in code must be caught.
    EnvScanResult injected = scan;
    rt::native::gameplay::EnvUsageSite fake;
    fake.file = "src/native/app/main.cpp";
    fake.line = 1;
    fake.var = "RT_APP_SELFTEST_UNREGISTERED";
    fake.directGetenv = true;
    injected.sites.push_back(fake);
    injected.byVar[fake.var].push_back(fake);
    const CoverageReport injectedReport =
        rt::native::gameplay::checkRetentionCoverage(injected, args.repoRoot);
    const bool unregisteredDetected =
        !injectedReport.codeVarsMissingFromRegistry.empty() &&
        injectedReport.codeVarsMissingFromRegistry.front() == fake.var;

    // 7c. A broken owner path must be caught.
    std::vector<EnhancementSpec> brokenOwner;
    for (const auto& spec : rt::native::gameplay::enhancementRegistry()) {
        brokenOwner.push_back(spec);
    }
    for (auto& spec : brokenOwner) {
        if (std::string(spec.id) == "rumble") spec.owners = "src/native/gameplay/selftest_does_not_exist.cpp";
    }
    const CoverageReport ownerReport =
        rt::native::gameplay::checkRetentionCoverage(brokenOwner, scan, args.repoRoot);
    bool brokenOwnerDetected = false;
    for (const auto& issue : ownerReport.missingOwners) {
        if (issue.id == "rumble") brokenOwnerDetected = true;
    }

    // 7d. An uncited entry must be caught.
    std::vector<EnhancementSpec> uncited;
    for (const auto& spec : rt::native::gameplay::enhancementRegistry()) {
        uncited.push_back(spec);
    }
    for (auto& spec : uncited) {
        if (std::string(spec.id) == "rumble") spec.docs = "";
    }
    const CoverageReport uncitedReport =
        rt::native::gameplay::checkRetentionCoverage(uncited, scan, args.repoRoot);
    bool uncitedDetected = false;
    for (const auto& issue : uncitedReport.docIssues) {
        if (issue.id == "rumble") uncitedDetected = true;
    }

    const std::size_t selfPassed =
        (removedFlagDetected ? 1 : 0) + (unregisteredDetected ? 1 : 0) +
        (brokenOwnerDetected ? 1 : 0) + (uncitedDetected ? 1 : 0);
    Check selftest{"selftest", selfPassed == 4, selfPassed, 4};
    if (!removedFlagDetected) selftest.failures.push_back("removed flag (RT_APP_RUMBLE) was NOT detected");
    if (!unregisteredDetected) selftest.failures.push_back("unregistered env var was NOT detected");
    if (!brokenOwnerDetected) selftest.failures.push_back("broken owner path was NOT detected");
    if (!uncitedDetected) selftest.failures.push_back("uncited entry was NOT detected");
    addCheck(std::move(selftest));

    // --- Diagnostics (before the single SCORE line) ---------------------------
    std::cout << "w2-13: registry entries=" << report.registryEntries
              << " env-entries=" << report.registryEnvEntries
              << " distinct-code-vars=" << report.distinctCodeVars
              << " sites=" << scan.sites.size()
              << " (direct getenv sites=" << scan.directGetenvSites << ")"
              << " owner-paths=" << report.ownerCount
              << " doc-cites=" << report.docCiteCount << "\n";
    if (!report.docOnlyIds.empty()) {
        std::cout << "w2-13: doc-only statuses (reported gaps):";
        for (const auto& id : report.docOnlyIds) std::cout << " " << id;
        std::cout << "\n";
    }
    for (const auto& check : checks) {
        if (check.pass) continue;
        std::cout << "w2-13 FAIL " << check.label << " " << check.got << "/" << check.total << "\n";
        for (const auto& failure : check.failures) std::cout << "  - " << failure << "\n";
    }

    // --- The single SCORE line -------------------------------------------------
    auto ratio = [](const Check& c) { return c.label + "=" + std::to_string(c.got) + "/" + std::to_string(c.total); };
    const bool allPass = std::all_of(checks.begin(), checks.end(), [](const Check& c) { return c.pass; });
    std::ostringstream score;
    score << "W2-13 SCORE registry=" << report.registryEntries << " "
          << ratio(checks[1]) << " "                      // coverage
          << ratio(checks[2]) << " "                      // flags_resolve
          << "getenv_sites=" << scan.directGetenvSites << "/" << scan.sites.size() << " "
          << ratio(checks[3]) << " "                      // owner_files
          << ratio(checks[4]) << " "                      // doc_cites
          << ratio(checks[5]) << " "                      // ledger
          << ratio(checks[6]) << " "                      // data_files
          << ratio(checks[7]) << " "                      // selftest
          << "deterministic=" << (checks[0].pass ? 1 : 0);
    std::cout << score.str() << "\n";
    return allPass ? 0 : 1;
}
