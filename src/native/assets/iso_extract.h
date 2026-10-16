// First-run asset extraction for the native rebuild: a pure ISO9660
// (ECMA-119) disc-image reader, verification gate and weighted extractor
// over the USER'S OWN disc image.  The app ships no game data (CLAUDE.md);
// at first run the user points it at their own dump (.cue/.bin or .iso),
// the gate proves it is this game, and the native asset set is extracted
// into the app data dir (the same "<data>/disc" tree the sibling installer
// produces and the native modules read through disc-root conventions).
// Flow spec: docs/native-reference/first-run-extraction.md.
//
// Provenance of every committed constant here is a metadata-only fact
// (names, sizes, CRCs) per CLAUDE.md "Boot ELF facts" / RomInstaller.h;
// no bytes of any disc are embedded.  Dev-time evidence statements marked
// [disc-verified] were observed read-only against the user's own dump.

#pragma once

#include "field_converter.h"  // expected<Value, Error>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

namespace rt::native::assets {

// ---------------------------------------------------------------- constants

inline constexpr std::uint32_t kIsoSectorBytes = 2048;   // ECMA-119 logical sector
inline constexpr std::uint32_t kIsoPvdLba = 16;          // PVD lives at sector 16
inline constexpr std::uint32_t kIsoRawSectorBytes = 2352;  // raw CD sector (.bin)
inline constexpr std::uint32_t kRawSectorDataOffsetMode2 = 24;  // Mode 2/XA: 12 sync + 4 header + 8 subheader
inline constexpr std::uint32_t kRawSectorDataOffsetMode1 = 16;  // Mode 1: 12 sync + 4 header
inline constexpr std::uint32_t kIsoExtractChunkSectors = 256;  // 512 KiB streaming chunks

// The gate file: name from the sibling's committed constants
// (src/rom/RomInstaller.h kBootElfName); production CRC32 from CLAUDE.md
// "Boot ELF facts" (verified read-only: zlib.crc32 over the good dump's
// SLUS_203.98 == 0x5A49851D, size 1,280,576).
inline constexpr const char* kBootElfName = "SLUS_203.98";
inline constexpr std::uint32_t kBootElfCrc32 = 0x5A49851D;

// ---------------------------------------------------------------- errors

enum class IsoError : std::uint8_t {
    NotReadable = 0,  // the image cannot be opened / is empty / is a container we do not support (CHD)
    NotISO,           // opened, but no valid ISO9660 PVD at sector 16
    NotThisGame,      // a valid ISO9660 image, but not this game: gate file absent at its expected path, or required manifest assets absent
    CorruptFile,      // the gate file's CRC32 is not the expected build's; or extracted assets failed post-extract validation
    Io,               // the image or destination became unreadable/unwritable mid-run (truncated image, full disk)
};

const char* describeIsoError(IsoError error) noexcept;

// ---------------------------------------------------------------- CRC32

// Standard CRC-32 (IEEE 802.3 reflected poly 0xEDB88320, init 0xFFFFFFFF,
// final xor 0xFFFFFFFF: zlib-crc32 compatible).  Streamed so the boot-ELF
// gate hashes a megabyte file in bounded memory.
class Crc32 {
public:
    void update(const std::uint8_t* data, std::size_t size) noexcept;
    std::uint32_t value() const noexcept { return m_crc ^ 0xFFFFFFFFu; }

private:
    std::uint32_t m_crc = 0xFFFFFFFFu;
};

// ---------------------------------------------------------------- entries

// One walked directory-record.  `name` is the '/'-joined image path with the
// ISO9660 ";<version>" suffix stripped and a lone trailing '.' removed
// ("SLUS_203.98;1" -> "SLUS_203.98"); the exact case as mastered is kept.
struct IsoEntry {
    std::string name;
    std::uint32_t lba = 0;
    std::uint64_t bytes = 0;
    bool isDir = false;
};

// Raw-sector stride of an opened image.
enum class IsoStride : std::uint8_t {
    Cooked2048,  // .iso: user data begins at each 2048-byte boundary
    RawMode2,    // raw .bin Mode 2/CD-XA: 12 sync + 4 header + 8 subheader, user data at +24 (the PS2 CD form; the good dump is this)
    RawMode1,    // raw .bin Mode 1: 12 sync + 4 header, user data at +16
};

struct IsoOpenOptions {
    // Loose mode: accept raw 2352-byte-sector .bin images, with the stride
    // auto-detected from the sector-16 area (see IsoImage::open).  When
    // false only cooked 2048-stride images are accepted.
    bool allowRawBin = true;
    // Follow a .cue sheet's FILE line to its backing file.  Only the single
    // data-track .cue/.bin pair players keep is handled; TRACK/INDEX lines
    // are ignored.
    bool allowCue = true;
};

// Parsed facts of one opened volume (also returned by IsoImage::open).
struct IsoVolume {
    std::string volumeId;              // PVD identifier, space-trimmed (informational; never gated on)
    IsoStride stride = IsoStride::Cooked2048;
    std::uint32_t volumeSpaceSectors = 0;  // PVD volume space size
    IsoEntry root;                     // the PVD's own root directory record
};

// ---------------------------------------------------------------- image

// Pathless ISO9660 reader: everything is resolved by walking directory
// records from the PVD's root record (LBA/size at PVD offsets 156..), never
// from host paths or filenames on the image file.  Records are parsed by
// their name-length field only, so any trailing SUSP/Rock Ridge bytes are
// ignored harmlessly (PS2 masters carry no RR 'NM' names: their identifiers
// are the direct ISO ones; the good dump's are all-uppercase with ';1'
// versions on files [disc-verified], and lowercase variants occur on other
// PS2 dumps, so every lookup below folds case).
class IsoImage {
public:
    IsoImage() = default;
    ~IsoImage();
    IsoImage(IsoImage&&) noexcept;
    IsoImage& operator=(IsoImage&&) noexcept;
    IsoImage(const IsoImage&) = delete;
    IsoImage& operator=(const IsoImage&) = delete;

    // Opens `image` and validates the PVD at sector 16.
    //   NotReadable: cannot open / empty / .bin raw not allowed by options /
    //                a CHD container (not supported here; see
    //                first-run-extraction.md for the deferral).
    //   NotISO: no valid PVD (type byte 1, 'CD001' at +1, version 1, sane
    //           root record) at any probed stride.
    //   Io: the file shrank under us / a probe read failed.
    // 'CD001' probe offsets, in order: 16*2048 (cooked), 16*2352+24
    // (Mode 2), 16*2352+16 (Mode 1).  The order matters only for adversarial
    // images: a true cooked image has its PVD exactly at the first probe,
    // a true raw image at one of the other two, and the remaining probes
    // land in zeroed system-area bytes [disc-verified on the good dump:
    // its cooked probe reads all zero], so the detection is unambiguous.
    expected<IsoVolume, IsoError> open(const std::filesystem::path& image,
                                       IsoOpenOptions options = {});
    void close() noexcept;
    bool isOpen() const noexcept { return m_file != nullptr; }

    IsoStride stride() const noexcept { return m_stride; }
    const std::string& volumeId() const noexcept { return m_volumeId; }
    std::uint32_t volumeSpaceSectors() const noexcept { return m_volumeSectors; }
    const IsoEntry& root() const noexcept { return m_root; }

    // Depth-first walk from the PVD root over directory extents: a
    // directory's children are reported in the order its records appear on
    // the disc, subdirectories depth-first (LIFO), which reproduces the
    // sibling installer's walk order.  Includes directory entries
    // (isDir == true); '.'/'..' records are skipped; hidden flags are kept.
    // The 'records never cross a 2048-byte boundary' rule is honored via the
    // zero-length-at-sector-pad skip.
    expected<std::vector<IsoEntry>, IsoError> walk() const;

    // Whole-path lookup, case-insensitive (dumps master either case), with
    // an exact-case preference: an exact match anywhere in the walk wins
    // over any case-folded one; among folded matches the first in walk
    // order is returned.  NotThisGame: image does not contain `name`.
    expected<IsoEntry, IsoError> locate(std::string_view name) const;

    // Streams `entry`'s bytes to `sink(bytes, count)` in chunkSectors-sector
    // chunks (bounded memory for arbitrary-size files); `progress(done,
    // total)` is invoked per chunk.  Returns false on a read error (a
    // truncated image -> Io at the call site) or when `sink` refuses more
    // data (used by the extractor for user cancellation).
    bool readFile(const IsoEntry& entry,
                  const std::function<bool(const std::uint8_t*, std::size_t)>& sink,
                  const std::function<void(std::uint64_t, std::uint64_t)>& progress = {},
                  std::uint32_t chunkSectors = kIsoExtractChunkSectors) const;

private:
    bool readSectors(std::uint32_t lba, std::uint32_t count, std::uint8_t* out) const;
    expected<std::vector<IsoEntry>, IsoError> listDir(const IsoEntry& dir) const;

    std::FILE* m_file = nullptr;
    IsoStride m_stride = IsoStride::Cooked2048;
    std::uint32_t m_dataOffset = 0;  // user bytes within a raw sector (0 / 16 / 24)
    std::string m_volumeId;
    std::uint32_t m_volumeSectors = 0;
    IsoEntry m_root;
};

// ---------------------------------------------------------------- gate

// The ordered verification gate chain (first failure stops the run; the
// typed errors are exactly the UX table's rows in first-run-extraction.md):
//   1. image readable              -> NotReadable / Io (open)
//   2. PVD valid at sector 16      -> NotISO (open)
//   3. gate file at expected path, case-insensitive lookup
//                                   -> NotThisGame (locate)
//   4. stream gate file, CRC32 the bytes
//                                   -> CorruptFile (mismatch) / Io (read error)
struct GateConfig {
    std::string_view bootElfPath = kBootElfName;   // canonical expectation
    // Injectable so tests pin pass/fail with synthetic fixtures; the
    // production constant is kBootElfCrc32 (CLAUDE.md "Boot ELF facts").
    std::uint32_t expectedBootCrc = kBootElfCrc32;
};

enum class GateStage : std::uint8_t { Located, Hashing };

struct GateResult {
    IsoEntry bootElf;
    std::uint32_t actualCrc = 0;
    IsoVolume volume;
};

// Runs stages 3..4 over an open image (call open() first for 1..2).
// `progress(stage, done, total)`: Located fires once with (0, size);
// Hashing fires per chunk with (bytesDone, bytesTotal).  When non-null,
// `actualCrcOut` receives the streamed CRC even when the check fails (for
// the failure screen's "expected X, found Y" detail).
expected<GateResult, IsoError> runGateChain(const IsoImage& image, const GateConfig& config,
                                            const std::function<void(GateStage, std::uint64_t, std::uint64_t)>& progress = {},
                                            std::uint32_t* actualCrcOut = nullptr);

// -------------------------------------------------------------- manifest

// Data-driven extraction manifest: which parts of the image the native app
// needs and why.  Every rule's pattern was verified against the reference
// inventory of the good dump (the sibling extract's .lbn_map.tsv sidecar,
// name/lba/BYTES, 376 entries: folders ACTION, CAR0..CAR4, CARS, COURSE,
// FLD, ITEM, SHOP, SOUND, SYS at the root [disc-verified]; CAR5 is ABSENT
// on the good dump and tolerated here).  Provenance strings cite module
// consumers and docs/native-reference pages, not guesses.
enum class AssetRule : std::uint8_t {
    Required,  // plan fails (NotThisGame) when the pattern matches nothing on a gate-passed image
    Optional,  // extracted when present; absence tolerated (CAR5 on the good dump)
};

struct ManifestEntrySpec {
    const char* pattern;  // "<folder>" = the whole folder; "<folder>/*" = folder
                          // contents file-by-file; "<folder>/*.EXT" = an
                          // extension subset; a bare "<name>" = one root
                          // file.  Matched case-insensitively.
    AssetRule rule;
    const char* provenance;  // citation for why this is in the plan
};

const std::vector<ManifestEntrySpec>& nativeAssetManifest();

// ---------------------------------------------------------------- plan

struct ExtractionPlanItem {
    std::string path;          // canonical target path ("SYS/EFFECT.GSL"): manifest spelling for the folder, uppercased leaf
    std::string imagePath;     // exact on-disc spelling ("sys/effect.gsl" on a lowercase dump)
    std::uint32_t lba = 0;
    std::uint64_t bytes = 0;
    std::size_t walkIndex = 0;  // source entry's position in the walk (sidecar row order)
    const ManifestEntrySpec* rule = nullptr;
};

struct ExtractionPlan {
    std::vector<ExtractionPlanItem> items;  // deterministic order: heaviest first (weight = bytes, sibling precedent), ties by path
    std::uint64_t totalBytes = 0;
};

// Builds the concrete plan for THIS image: every manifest rule applied
// case-insensitively over the walked entries, canonicalized and de-duplicated
// (two on-disc entries folding to one canonical path keep the first in walk
// order; never occurs on real discs).  Required rules that match nothing ->
// NotThisGame; when `missingOut` is set it receives the unmatched patterns
// (for the failure screen's "missing: SYS, FLD" detail).
expected<ExtractionPlan, IsoError> buildExtractionPlan(const std::vector<IsoEntry>& entries,
                                                       std::vector<std::string>* missingOut = nullptr);

// ---------------------------------------------------------------- eta

// Weighted-progress ETA from measured throughput, TaskProgress-style
// (AUTHORED semantics ported from sibling src/platform/ProgressEta.h, in
// seconds the caller supplies): a sliding sample window of at most 40 s and
// at least 1 s span; unknown (-1) for the first 3 s of the phase or until
// 5 % of the work is done, so early lumpy rates never misestimate.
class ThroughputEta {
public:
    void begin(std::uint64_t totalBytes, double nowSeconds);
    void sample(std::uint64_t doneBytes, double nowSeconds);
    std::uint64_t total() const noexcept { return m_total; }
    // Seconds left at this sample, or < 0 while unknown.  Not a phase start.
    double secondsLeft(std::uint64_t doneBytes, double nowSeconds) const;
    static std::string describe(double seconds);  // "About 3 minutes left" / "" while unknown

private:
    double m_phaseStart = 0.0;
    std::uint64_t m_total = 0;
    std::deque<std::pair<double, std::uint64_t>> m_samples;  // (seconds, doneBytes)
};

// ------------------------------------------------------------- state flow

// The setup-screen state machine (pure logic here; the GPU wave's screen
// reads ExtractionSnapshot each frame and draws step/bar/percent/eta/item).
// Phases in order: Verifying (gate chain + plan) -> Extracting (weighted,
// heaviest-first progress with ETA) -> Validating (counts and byte sizes
// against the plan) -> Ready.  Failed/Cancelled carry the typed error and a
// user-action for the failure UI (retry / choose another image / quit).
enum class ExtractionPhase : std::uint8_t {
    Verifying = 0, Extracting, Validating, Ready, Failed, Cancelled,
};

struct ExtractionSnapshot {
    ExtractionPhase phase = ExtractionPhase::Verifying;
    int stepIndex = 0;       // Verifying = 0, Extracting = 1, Validating = 2 (of stepCount)
    int stepCount = 3;
    double fraction = 0.0;   // current step's share, 0..1 (monotonic within a step)
    std::uint64_t bytesDone = 0;  // Extracting: plan bytes written so far
    std::uint64_t bytesTotal = 0; // Extracting: total plan bytes
    double etaSeconds = -1.0;     // Extracting only; < 0 = not estimable yet, else seconds left
    std::string currentItem;      // Verifying: gate stage; Extracting: the file being written; Validating: "123/376"
    IsoError error = IsoError::NotReadable;  // valid when phase == Failed
    std::string message;           // human-readable failure detail for the screen
};

// Existing-install report for the fast path (the sibling installer's full
// extract satisfies the manifest too; both flows share the disc dir).
struct InstallStatus {
    bool installed = false;
    std::uint64_t files = 0;   // entries in the sidecar
    std::uint64_t bytes = 0;
    std::vector<std::string> missingRules;  // Required patterns with no on-disk match
};

// Reads "<discDir>/.lbn_map.tsv" (the sibling sidecar format:
// "<path>\t<lba>\t<bytes>"), folds it case-insensitively against the
// manifest, and stats every matched Required file for existence and size.
// Read failures simply mean "not installed" (re-extract); never throws.
InstallStatus validateInstalledAssets(const std::filesystem::path& discDir);

// One first-run attempt.  Blocking: run() performs the whole flow and the
// render thread polls snapshot() per frame (mutex-guarded copy);
// requestStop() cancels cleanly between chunks (phase -> Cancelled).
// Extracts into `discDir` (create it first if absent): plan items at their
// canonical paths, plus ".lbn_map.tsv" (sidecar for native consumers, the
// sibling's format) and ".native_assets.json" (this flow's own install
// marker; the sibling's ".installed.json" is never touched).
class FirstRunExtraction {
public:
    explicit FirstRunExtraction(GateConfig config = {});

    // True when the flow reached Ready.  Before anything else it tries the
    // fast path: validateInstalledAssets(discDir) — an already-complete
    // install (e.g. the sibling's full-disc extract) short-circuits to
    // Ready and the image is never even opened.
    bool run(const std::filesystem::path& imagePath, const std::filesystem::path& discDir);
    ExtractionSnapshot snapshot() const;
    void requestStop() { m_stop.store(true, std::memory_order_relaxed); }

    // Extracted-entry sidecar rows in walk order ("<path>\t<lba>\t<bytes>\n"
    // per file, files only): same format as the sibling installer's
    // .lbn_map.tsv (the reader side lives in field_tile_index.cpp).
    static std::string buildLbnMapText(const std::vector<ExtractionPlanItem>& itemsInWalkOrder);

private:
    void fail(IsoError error, std::string message);
    void setPhase(ExtractionPhase phase, std::string detail);
    void setDetail(std::string detail);
    void setFraction(double fraction);
    void pushProgress(std::uint64_t done, std::uint64_t total);  // Extracting callback target

    GateConfig m_config;
    mutable std::mutex m_mutex;
    ExtractionSnapshot m_snapshot;
    ThroughputEta m_eta;
    std::atomic<bool> m_stop{false};
};

}  // namespace rt::native::assets
