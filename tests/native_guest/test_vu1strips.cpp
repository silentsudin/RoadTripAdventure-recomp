// W3 gs-sink step 2: replays the oracle's LitStrip captures (vu1_replay's ground
// truth) through the transliterated programs: full VU1 data memory and every XGKICK
// packet must match byte-for-byte, plus the live VF registers the next run reads.
// Reads build/vu1hle-caps/field_lit.vu1cap (game-derived, never committed):
// RT_VU1_CAPTURE overrides; the test SKIPs when absent.
#include "guest/guest_vu1.h"
#include "guest/guest_vif1.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace {

std::vector<std::vector<std::uint8_t>> g_pkts;
std::uint64_t g_pkBytes = 0;

void kickCapture(const std::uint8_t* p, std::uint32_t n) {
    g_pkts.emplace_back(p, p + n);
    g_pkBytes += n;
}

// VU1State as the capture stores it (sizeof 664; field offsets verified against
// the fork's runtime/ps2_vu1.h).
constexpr std::size_t kStateSize = 664;
constexpr std::size_t kOffVi = 512, kOffAcc = 576, kOffQ = 592, kOffClip = 616, kOffTop = 640;

std::uint32_t rd32(const std::uint8_t* p) {
    std::uint32_t v;
    std::memcpy(&v, p, 4);
    return v;
}

}  // namespace

int main() {
    const char* env = std::getenv("RT_VU1_CAPTURE");
    const char* path = env && *env ? env : "build/vu1hle-caps/field_lit.vu1cap";
    std::FILE* f = std::fopen(path, "rb");
    if (!f) {
        std::printf("[vu1strips_test] SKIP: capture not found at %s\n", path);
        return 0;
    }
    char magic[8];
    if (std::fread(magic, 1, 8, f) != 8 || std::memcmp(magic, "VU1CAP01", 8) != 0) {
        std::printf("FAIL: not a VU1 capture\n");
        return 1;
    }
    std::uint32_t codeSize = 0;
    if (std::fread(&codeSize, 4, 1, f) != 1) return 1;
    std::fseek(f, static_cast<long>(codeSize), SEEK_CUR);

    rtguest::vu1SetKick(&kickCapture);
    const int envRecs = std::getenv("RT_VU1_RECORDS") ? std::atoi(std::getenv("RT_VU1_RECORDS")) : 2000;
    const int kMaxRecords = envRecs <= 0 ? 0x7FFFFFFF : envRecs;
    int rec = 0, fails = 0, skipped = 0;
    for (; rec < kMaxRecords; ++rec) {
        char tag[4];
        if (std::fread(tag, 1, 4, f) != 4 || std::memcmp(tag, "REC1", 4) != 0) break;
        std::uint8_t kind;
        std::uint32_t startPC, topH, itopH;
        std::uint8_t dBit, tBit;
        std::uint32_t ssz;
        if (std::fread(&kind, 1, 1, f) != 1 || std::fread(&startPC, 4, 1, f) != 1 ||
            std::fread(&topH, 4, 1, f) != 1 || std::fread(&itopH, 4, 1, f) != 1 ||
            std::fread(&dBit, 1, 1, f) != 1 || std::fread(&tBit, 1, 1, f) != 1 ||
            std::fread(&ssz, 4, 1, f) != 1 || ssz != kStateSize) {
            std::printf("FAIL: record %d header (stateSize %u)\n", rec, ssz);
            return 1;
        }
        std::vector<std::uint8_t> sin(kStateSize);
        std::fread(sin.data(), 1, kStateSize, f);
        std::uint32_t dlen;
        std::fread(&dlen, 4, 1, f);
        std::vector<std::uint8_t> dataIn(dlen);
        std::fread(dataIn.data(), 1, dlen, f);
        std::fseek(f, 1, SEEK_CUR);  // ended
        std::vector<std::uint8_t> sout(kStateSize);
        std::fread(sout.data(), 1, kStateSize, f);
        std::vector<std::uint8_t> dataOut(dlen);
        std::fread(dataOut.data(), 1, dlen, f);
        std::uint32_t npk;
        std::fread(&npk, 4, 1, f);
        std::vector<std::vector<std::uint8_t>> expPk;
        for (std::uint32_t p = 0; p < npk; ++p) {
            std::uint32_t plen;
            std::fread(&plen, 4, 1, f);
            std::vector<std::uint8_t> pk(plen);
            std::fread(pk.data(), 1, plen, f);
            expPk.push_back(std::move(pk));
        }

        // Records for programs not transliterated yet (direct-PC programs): consumed,
        // not compared.
        if (startPC >= 0x80 || (startPC & 0xFu) != 0u) {
            ++skipped;
            continue;
        }

        // Load the run's state, run, diff.
        std::memcpy(rtguest::vu1().vf, sin.data(), 32 * 16);
        std::memcpy(rtguest::vu1().vi, sin.data() + kOffVi, 16 * 4);
        std::memcpy(rtguest::vu1().acc, sin.data() + kOffAcc, 4 * 4);
        std::memcpy(&rtguest::vu1().q, sin.data() + kOffQ, 4);
        std::memcpy(&rtguest::vu1().p, sin.data() + kOffQ + 4, 4);
        std::memcpy(&rtguest::vu1().i, sin.data() + kOffQ + 8, 4);
        std::memcpy(rtguest::vu1Data(), dataIn.data(), dlen);
        g_pkts.clear();
        // The oracle runs with the record header's TOP (execute(..., rec.top, ...)).
        rtguest::vu1OnMscal(startPC, topH, itopH);

        int bad = 0;
        // Compare all data except the interpreter's scratch range (qwords 1006..1023,
        // the microcode's register-save stack the oracle HLE leaves unmodeled too).
        for (std::size_t i = 0; i < dataOut.size(); ++i) {
            const std::size_t qw = i / 16;
            if (qw >= 1006 && qw < 1024) continue;
            if (rtguest::vu1Data()[i] != dataOut[i]) {
                if (fails < 5)
                    std::printf("rec%d data q%zu.%c got %02x exp %02x\n", rec, qw,
                                "xyzw"[(i / 4) % 4], rtguest::vu1Data()[i], dataOut[i]);
                ++bad;
                break;
            }
        }
        if (g_pkts.size() != expPk.size()) {
            if (fails < 5)
                std::printf("rec%d packets got %zu exp %zu\n", rec, g_pkts.size(), expPk.size());
            ++bad;
        } else {
            for (std::size_t p = 0; p < expPk.size(); ++p)
                if (g_pkts[p] != expPk[p]) {
                    std::size_t b = 0;
                    while (b < expPk[p].size() && g_pkts[p][b] == expPk[p][b]) ++b;
                    if (fails < 5)
                        std::printf("rec%d packet %zu differs at byte %zu (len %zu/%zu)\n", rec, p, b,
                                    g_pkts[p].size(), expPk[p].size());
                    ++bad;
                    break;
                }
        }
        // Live VF registers the next runs read (oracle HLE's compared set: 1..18
        // all lanes, 27/28 only W; q from the last DIV; vi14 the save-stack ptr).
        for (int r = 1; r <= 18; ++r)
            for (int l = 0; l < 4; ++l) {
                float a, b2;
                std::memcpy(&a, &rtguest::vu1().vf[r][l], 4);
                std::memcpy(&b2, sout.data() + r * 16 + l * 4, 4);
                if (std::memcmp(&a, &b2, 4) != 0) {
                    if (fails < 5)
                        std::printf("rec%d vf%d.%c got %g exp %g\n", rec, r, "xyzw"[l],
                                    (double)a, (double)b2);
                    ++bad;
                    break;
                }
            }
        for (int r = 27; r <= 28; ++r) {
            std::uint32_t a, b2;
            std::memcpy(&a, &rtguest::vu1().vf[r][3], 4);
            std::memcpy(&b2, sout.data() + r * 16 + 12, 4);
            if (a != b2) {
                if (fails < 5) std::printf("rec%d vf%d.w bits %08x exp %08x\n", rec, r, a, b2);
                ++bad;
            }
        }
        {
            std::int32_t gotVi = rtguest::vu1().vi[14], expVi;
            std::memcpy(&expVi, sout.data() + kOffVi + 14 * 4, 4);
            if (gotVi != expVi) {
                if (fails < 5) std::printf("rec%d vi14 got %d exp %d\n", rec, gotVi, expVi);
                ++bad;
            }
        }
        {
            float a = rtguest::vu1().q, b2;
            std::memcpy(&b2, sout.data() + kOffQ, 4);
            if (std::memcmp(&a, &b2, 4) != 0) {
                if (fails < 5) std::printf("rec%d q got %g exp %g\n", rec, (double)a, (double)b2);
                ++bad;
            }
        }
        if (bad) {
            if (++fails >= 8) {
                std::printf("[vu1strips_test] FAIL: %d failing records (first %d shown)\n", fails, rec + 1);
                return 1;
            }
        }
    }
    std::printf("[vu1strips_test] %s: %d records (%d skipped: unported), %llu packet bytes, %d failing\n",
                fails ? "FAIL" : "PASS", rec, skipped, (unsigned long long)g_pkBytes, fails);
    return fails ? 1 : 0;
}
