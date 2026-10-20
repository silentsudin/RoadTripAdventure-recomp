// Disc-image intake through the first-run wizard: a tiny synthetic ISO9660
// image (PVD at sector 16, root dir, one boot-ELF-named file) and its raw
// 2352-byte-sector .bin twin must extract to identical bytes, and the
// boot-ELF CRC gate must reject a payload that is not the real build.
// All data is synthetic and lives in a temp dir; nothing touches the repo.

#include "../../src/native/assets/iso_extract.h"
#include "../../src/native/assets/setup_wizard.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using namespace rt::native::assets;
namespace fs = std::filesystem;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

namespace {
void le32(std::vector<std::uint8_t>& v, std::size_t at, std::uint32_t x) { for (int i = 0; i < 4; ++i) v[at + i] = (x >> (8 * i)) & 0xFF; }
void be32(std::vector<std::uint8_t>& v, std::size_t at, std::uint32_t x) { for (int i = 0; i < 4; ++i) v[at + i] = (x >> (8 * (3 - i))) & 0xFF; }

void record(std::vector<std::uint8_t>& dir, std::size_t at, const std::string& name, std::uint32_t lba, std::uint32_t size, bool isDir) {
    std::size_t len = 33 + name.size(); if (len % 2) ++len;
    dir[at] = static_cast<std::uint8_t>(len);
    le32(dir, at + 2, lba); be32(dir, at + 6, lba); le32(dir, at + 10, size); be32(dir, at + 14, size);
    dir[at + 25] = isDir ? 2 : 0; dir[at + 28] = 1; dir[at + 31] = 1;
    dir[at + 32] = static_cast<std::uint8_t>(name.size());
    std::memcpy(dir.data() + at + 33, name.data(), name.size());
}

// 20 cooked sectors: PVD@16, root dir@17, SLUS_203.98;1 @18 (the payload).
std::vector<std::uint8_t> makeIso(const std::string& payload) {
    std::vector<std::uint8_t> iso(20 * 2048, 0);
    auto* pvd = iso.data() + 16 * 2048;
    pvd[0] = 1; std::memcpy(pvd + 1, "CD001", 5); pvd[6] = 1;
    std::memset(pvd + 40, ' ', 32); std::memcpy(pvd + 40, "INTAKE", 6);
    le32(iso, 16 * 2048 + 80, 20); be32(iso, 16 * 2048 + 84, 20);
    std::vector<std::uint8_t> r(34, 0);  // PVD root record
    record(r, 0, std::string(1, '\0'), 17, 2048, true); r[0] = 34;
    std::memcpy(pvd + 156, r.data(), 34);
    std::vector<std::uint8_t> dir(2048, 0);
    record(dir, 0, std::string(1, '\0'), 17, 2048, true);
    record(dir, 34, std::string(1, '\1'), 17, 2048, true);
    record(dir, 68, "SLUS_203.98;1", 18, static_cast<std::uint32_t>(payload.size()), false);
    std::memcpy(iso.data() + 17 * 2048, dir.data(), 2048);
    std::memcpy(iso.data() + 18 * 2048, payload.data(), payload.size());
    return iso;
}

// Raw Mode 2 twin: 12 sync + 4 header + 8 subheader, user data at +24.
std::vector<std::uint8_t> toRaw(const std::vector<std::uint8_t>& iso) {
    std::vector<std::uint8_t> raw;
    for (std::size_t s = 0; s < iso.size() / 2048; ++s) {
        std::vector<std::uint8_t> sec(2352, 0);
        std::memset(sec.data() + 1, 0xFF, 10); sec[15] = 2;
        std::memcpy(sec.data() + 24, iso.data() + s * 2048, 2048);
        raw.insert(raw.end(), sec.begin(), sec.end());
    }
    return raw;
}

void write(const fs::path& p, const std::vector<std::uint8_t>& b) {
    std::ofstream(p, std::ios::binary).write(reinterpret_cast<const char*>(b.data()), static_cast<std::streamsize>(b.size()));
}

std::string readBoot(const fs::path& image, std::uint32_t expectedCrc, bool* gateOk) {
    IsoImage img;
    std::string out;
    if (!img.open(image)) return out;
    GateConfig cfg; cfg.expectedBootCrc = expectedCrc;
    auto gate = runGateChain(img, cfg);
    *gateOk = gate.has_value();
    auto entry = img.locate(kBootElfName);
    if (entry) img.readFile(*entry, [&](const std::uint8_t* d, std::size_t n) { out.append(reinterpret_cast<const char*>(d), n); return true; });
    return out;
}
}  // namespace

int main() {
    const fs::path tmp = fs::temp_directory_path() / ("rt-disc-intake-" + std::to_string(std::rand()));
    fs::create_directories(tmp);
    const std::string payload = "NOT-THE-REAL-BOOT-ELF";
    const auto iso = makeIso(payload);
    write(tmp / "t.iso", iso);
    write(tmp / "t.bin", toRaw(iso));

    Crc32 crc; crc.update(reinterpret_cast<const std::uint8_t*>(payload.data()), payload.size());
    bool okIso = false, okBin = false, badIso = true, badBin = true;
    CHECK(readBoot(tmp / "t.iso", crc.value(), &okIso) == payload);
    CHECK(readBoot(tmp / "t.bin", crc.value(), &okBin) == payload);  // iso and bin twin yield identical bytes
    CHECK(okIso && okBin);
    readBoot(tmp / "t.iso", kBootElfCrc32, &badIso);  // production CRC rejects the synthetic payload
    readBoot(tmp / "t.bin", kBootElfCrc32, &badBin);
    CHECK(!badIso && !badBin);

    // Wizard: a file in RT_DISC is taken as an image; the hard CRC gate refuses it and nothing is extracted.
    setenv("RT_DATA_DIR", (tmp / "data").c_str(), 1);
    setenv("RT_ASSET_DIR", (tmp / "assets").c_str(), 1);
    for (const char* name : {"t.iso", "t.bin"}) {
        setenv("RT_DISC", (tmp / name).c_str(), 1);
        CHECK(!RunFirstRunSetupIfRequired());
        CHECK(!fs::exists(tmp / "data/disc/SLUS_203.98"));
    }

    // CHD: hard, actionable failure (no silent stub).
    write(tmp / "t.chd", [] { std::vector<std::uint8_t> v(40 * 2048, 0); std::memcpy(v.data(), "MComprHD", 8); return v; }());
    FirstRunExtraction flow;
    CHECK(!flow.run(tmp / "t.chd", tmp / "chd-out"));
    CHECK(flow.snapshot().message.find("chdman extractcd") != std::string::npos);

    fs::remove_all(tmp);
    std::printf(g_fail ? "native_assets_disc_intake_test: FAILED (%d)\n" : "native_assets_disc_intake_test: ok\n", g_fail);
    return g_fail ? 1 : 0;
}
