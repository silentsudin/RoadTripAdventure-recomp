// W2-01 acceptance: town/shop event-script VM (InterpretScriptCommands).
//   w2_script_vm_test [--elf PATH] [--allow-skip] [--verbose]
// ELF: --elf, else $RT_ELF, else ~/Library/Application Support/RoadTripRecomp/
// disc/SLUS_203.98.  Missing ELF => exit 2 (NO_ELF) unless --allow-skip (exit 0,
// prints SKIP).  Prints exactly one `W2-01 SCORE ...` line; exit 0 only on PASS.
// No ROM bytes are embedded: only counts and FNV-1a hashes of derived numbers.

#include "assets/script_image.h"
#include "gameplay/script_vm.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace rt::native;
using gameplay::IScriptHost;
using gameplay::IScriptMemory;
using gameplay::ScriptShop;
using gameplay::ScriptTask;

namespace {

bool gVerbose = false;

// ---------------------------------------------------------------------------
// Pinned numbers (derived from the ELF; reproduce with the commands in
// .qoder/wave2/W2-01-report.md and docs/native-reference/script-vm.md).
// ---------------------------------------------------------------------------
struct Pins {
    // Census (town directory 0x2A7ED0).
    std::size_t towns = 0;
    std::size_t level1Slots = 0;
    std::size_t level2Tables = 0;
    std::uint64_t sumTableWords = 0;  // sum of level-2 table lengths
    std::uint32_t endNextTable = 0, endZeroPad = 0, endForeign = 0, endLast = 0;
    std::uint64_t censusHash = 0;
    // Entry/target decode.
    std::size_t entryClean = 0;
    std::size_t targetsDistinct = 0;               // distinct [1..len) script starts
    std::size_t targetRefs = 0;                    // sum(len-1)
    std::size_t targetsClean = 0;
    std::size_t targetsUnknown = 0;
    std::uint64_t targetHash = 0;
    std::uint64_t opHistHash = 0;
    std::uint32_t opsDecoded = 0;
    std::uint32_t branchOperands = 0;
    std::uint32_t bleed = 0;
    // Replay.
    std::uint64_t replayHash = 0;
};

constexpr std::uint64_t kFnvOffset = 0xcbf29ce484222325ULL;
constexpr std::uint64_t kFnvPrime = 0x100000001b3ULL;

struct Fnv {
    std::uint64_t h = kFnvOffset;
    void add(std::uint64_t v, int bytes = 8) {
        for (int i = 0; i < bytes; ++i) h = (h ^ ((v >> (8 * i)) & 0xff)) * kFnvPrime;
    }
};

// ---------------------------------------------------------------------------
// Memory over an arbitrary byte buffer (synthetic vectors).
// ---------------------------------------------------------------------------
class FlatMemory final : public IScriptMemory {
public:
    FlatMemory(std::uint32_t base, std::size_t size) : base_(base), data_(size, 0) {}
    void put8(std::uint32_t a, std::uint8_t v) { data_.at(a - base_) = v; }
    void put32(std::uint32_t a, std::uint32_t v) {
        for (int i = 0; i < 4; ++i) put8(a + i, static_cast<std::uint8_t>(v >> (8 * i)));
    }
    void putBytes(std::uint32_t a, const std::vector<std::uint8_t>& b) {
        for (std::size_t i = 0; i < b.size(); ++i) put8(a + static_cast<std::uint32_t>(i), b[i]);
    }
    bool readU8(std::uint32_t a, std::uint8_t& o) const override {
        if (a < base_ || a - base_ >= data_.size()) return false;
        o = data_[a - base_];
        return true;
    }
    bool readU32(std::uint32_t a, std::uint32_t& o) const override {
        if (a < base_ || a - base_ + 4 > data_.size()) return false;
        o = 0;
        for (int i = 0; i < 4; ++i) o |= static_cast<std::uint32_t>(data_[a - base_ + i]) << (8 * i);
        return true;
    }

private:
    std::uint32_t base_;
    std::vector<std::uint8_t> data_;
};

// ---------------------------------------------------------------------------
// Scripted host for synthetic vectors: every predicate returns a settable
// value; every call is logged with its arguments.
// ---------------------------------------------------------------------------
class ScriptedHost final : public IScriptHost {
public:
    std::map<std::string, std::int64_t> ret;  // predicate name -> value (default 0)
    std::int8_t licence = 0;
    std::int32_t taskIdx = 0;
    std::int32_t countBits = 0;
    std::ostringstream log;

    std::int64_t r(const char* n) const {
        const auto it = ret.find(n);
        return it == ret.end() ? 0 : it->second;
    }
    std::string str() const { return log.str(); }

    std::int64_t testFlagBitFc98(std::uint8_t id) override { log << "tf98(" << int(id) << ");"; return r("tf98"); }
    std::int64_t getFlaggedSlotCount(std::uint8_t s, std::uint8_t b, std::int32_t z) override {
        log << "gfsc(" << int(s) << ',' << int(b) << ',' << z << ");";
        return r("gfsc");
    }
    std::int64_t testStampFlag(std::int8_t t, std::uint8_t s) override {
        log << "tsf(" << int(t) << ',' << int(s) << ");";
        return r("tsf");
    }
    std::int64_t testFlagFc68(std::uint8_t id) override { log << "tfc68(" << int(id) << ");"; return r("tfc68"); }
    std::int64_t testStateBitFc78(std::uint8_t id) override { log << "tsb78(" << int(id) << ");"; return r("tsb78"); }
    std::int64_t checkSlotPresence() override { log << "csp();"; return r("csp"); }
    std::int64_t matchSlotPairState(std::int8_t t, std::uint8_t s) override {
        log << "msps(" << int(t) << ',' << int(s) << ");";
        return r("msps");
    }
    std::int64_t testFlagFcb8(std::int8_t t, std::uint8_t s) override {
        log << "tfcb8(" << int(t) << ',' << int(s) << ");";
        return r("tfcb8");
    }
    std::int64_t testProgressFlagForPair(std::int8_t t, std::uint8_t s) override {
        log << "tpfp(" << int(t) << ',' << int(s) << ");";
        return r("tpfp");
    }
    std::int64_t getOrSetPresidentRaceResult(std::int64_t a) override {
        log << "gprr(" << a << ");";
        return r("gprr");
    }
    std::int64_t testFlagFca8Clear(std::uint8_t id) override { log << "tfca8c(" << int(id) << ");"; return r("tfca8c"); }
    std::int64_t testProgressFlag(std::uint8_t b) override { log << "tpf(" << int(b) << ");"; return r("tpf"); }
    std::int32_t countBitsFca8Fcb0(std::int32_t s) override { log << "cb(" << s << ");"; return countBits; }
    std::int64_t testObjectAttribute(std::uint32_t o, std::uint8_t a, std::uint8_t b) override {
        log << "toa(" << std::hex << o << std::dec << ',' << int(a) << ',' << int(b) << ");";
        return r("toa");
    }
    std::int32_t findCurrentTaskIndex() override { log << "fcti();"; return taskIdx; }
    std::int8_t licenceByte() override { log << "lic();"; return licence; }

    void setFlagBitFc98(std::uint8_t id) override { log << "sf98(" << int(id) << ");"; }
    void clearFlagBitFc98(std::uint8_t id) override { log << "cf98(" << int(id) << ");"; }
    void clearFlaggedSlotBit(std::uint8_t s, std::uint8_t b, std::int32_t z) override {
        log << "cfsb(" << int(s) << ',' << int(b) << ',' << z << ");";
    }
    void recordTaskResult(std::uint8_t c) override { log << "rtr(" << int(c) << ");"; }
    void releaseStampFlag(const ScriptShop& s) override {
        log << "rsf(" << int(s.town) << ',' << int(s.shop) << ");";
    }
    void setStateBitFc88(std::uint8_t id) override { log << "ssb88(" << int(id) << ");"; }
    void setProgressFlagAndQueue(std::uint8_t id) override { log << "spfq(" << int(id) << ");"; }
    void onUnknownPreControl(std::uint8_t c, std::int8_t tl, std::uint8_t s) override {
        log << "unk(" << int(c) << ',' << int(tl) << ',' << int(s) << ");";
    }
    void onUnknownSkipped(std::uint32_t cur) override { log << "unkskip(" << std::hex << cur << std::dec << ");"; }
};

// ---------------------------------------------------------------------------
// Synthetic vectors.
// ---------------------------------------------------------------------------
constexpr std::uint32_t kBase = 0x10000;
constexpr std::uint32_t kScript = 0x10100;   // script under test
constexpr std::uint32_t kTable = 0x10200;    // level-2 table: idx i -> kTarget + 0x10*i
constexpr std::uint32_t kTarget = 0x10300;   // target i at kTarget + 0x10*i ('X' text)
constexpr std::uint32_t kTableWords = 16;
constexpr std::uint8_t kText = 0x54;         // 'T' terminates the prefix (>= 0x20)

std::uint32_t target(unsigned idx) { return kTarget + 0x10 * idx; }

struct Rig {
    FlatMemory mem{kBase, 0x1000};
    Rig(const std::vector<std::uint8_t>& script) {
        mem.putBytes(kScript, script);
        mem.put8(kScript + static_cast<std::uint32_t>(script.size()), kText);
        for (unsigned i = 0; i < kTableWords; ++i) {
            mem.put32(kTable + 4 * i, target(i));
            mem.put8(target(i), 'X');
        }
    }
};

struct VectorResult {
    bool ok = true;
    std::string what;
};

struct VecCtx {
    int total = 0;
    int pass = 0;
    int extraTotal = 0;
    int extraPass = 0;
    Fnv hash;
};

struct Expect {
    std::uint32_t cursor;
    std::string log;
    std::uint32_t branches;
};

// Run one vector; `core` selects the 52-count vs the extras.
void runVector(VecCtx& c, bool core, const char* name, const std::vector<std::uint8_t>& script,
               const std::function<void(ScriptedHost&, ScriptShop&)>& setup, const Expect& e,
               const std::function<bool(const ScriptTask&, const ScriptShop&)>& post = nullptr) {
    Rig rig(script);
    ScriptedHost host;
    ScriptShop shop;
    shop.town = 5;
    shop.shop = 9;
    shop.townLabel = 7;
    shop.word14 = 100000;
    ScriptTask task;
    task.tableBase = kTable;
    setup(host, shop);
    const gameplay::ScriptRun run = gameplay::runScriptPrefix(rig.mem, host, task, shop, kScript);
    bool ok = run.status == gameplay::ScriptStatus::Ok && run.cursor == e.cursor &&
              host.str() == e.log && run.branchesTaken == e.branches;
    if (ok && post) ok = post(task, shop);
    if (core) {
        ++c.total;
        c.pass += ok ? 1 : 0;
    } else {
        ++c.extraTotal;
        c.extraPass += ok ? 1 : 0;
    }
    c.hash.add(run.cursor, 4);
    c.hash.add(run.opsExecuted, 4);
    for (char ch : host.str()) c.hash.add(static_cast<std::uint8_t>(ch), 1);
    if (!ok) {
        std::cerr << "VECTOR FAIL " << name << ": status=" << int(run.status) << " cursor=0x" << std::hex
                  << run.cursor << " want 0x" << e.cursor << std::dec << " branches=" << run.branchesTaken
                  << " want " << e.branches << "\n  log=" << host.str() << "\n  want=" << e.log << "\n";
    }
}

using Setup = std::function<void(ScriptedHost&, ScriptShop&)>;

// Conditional op: {taken, not-taken} pair against the same bytes.
void branchPair(VecCtx& c, const char* name, std::vector<std::uint8_t> script, std::uint32_t width,
                unsigned takenIdx, const char* key, const std::string& callLog, bool takenWhenTrue = true,
                const Setup& extra = nullptr) {
    // "true" result of the predicate:
    runVector(c, true, name, script,
              [&](ScriptedHost& h, ScriptShop& s) {
                  h.ret[key] = 1;
                  if (extra) extra(h, s);
              },
              takenWhenTrue ? Expect{target(takenIdx), callLog, 1}
                            : Expect{kScript + width, callLog, 0});
    runVector(c, true, name, script,
              [&](ScriptedHost& h, ScriptShop& s) {
                  h.ret[key] = 0;
                  if (extra) extra(h, s);
              },
              takenWhenTrue ? Expect{kScript + width, callLog, 0}
                            : Expect{target(takenIdx), callLog, 1});
}

void coreVectors(VecCtx& c) {
    constexpr unsigned I = 7;  // branch index used by most vectors
    branchPair(c, "op01", {0x01, 0x21, I}, 3, I, "tf98", "tf98(33);");
    branchPair(c, "op02", {0x02, 0x21, I}, 3, I, "tf98", "tf98(33);", /*takenWhenTrue=*/false);
    branchPair(c, "op03", {0x03, 5, 6, I}, 4, I, "gfsc", "gfsc(5,6,0);");
    // op 04/08/16 read (town=+0x23, shop=+0x21) = (5, 9) from the rig.
    branchPair(c, "op04", {0x04, I}, 2, I, "tsf", "tsf(5,9);");
    branchPair(c, "op05", {0x05, 10, I}, 3, I, "tfc68", "tfc68(10);");
    branchPair(c, "op06", {0x06, 10, I}, 3, I, "tsb78", "tsb78(10);");
    branchPair(c, "op07", {0x07, I}, 2, I, "csp", "csp();");
    // op 08: {match -> table[A]} vs {no match, Fcb8 set -> skip 3}.
    runVector(c, true, "op08/match", {0x08, 6, 9},
              [](ScriptedHost& h, ScriptShop&) { h.ret["msps"] = 2; }, {target(6), "msps(5,9);", 1});
    runVector(c, true, "op08/skip", {0x08, 6, 9},
              [](ScriptedHost& h, ScriptShop&) { h.ret["msps"] = 0; h.ret["tfcb8"] = 1; },
              {kScript + 3, "msps(5,9);tfcb8(5,9);", 0});
    // op 0B: Get(-1) == result.
    runVector(c, true, "op0B/eq", {0x0B, 2, I}, [](ScriptedHost& h, ScriptShop&) { h.ret["gprr"] = 2; },
              {target(I), "gprr(-1);", 1});
    runVector(c, true, "op0B/ne", {0x0B, 2, I}, [](ScriptedHost& h, ScriptShop&) { h.ret["gprr"] = 1; },
              {kScript + 3, "gprr(-1);", 0});
    branchPair(c, "op0D", {0x0D, 3, I}, 3, I, "tfca8c", "tfca8c(3);");
    // op 0E: flag 0x50 set -> table[byte[1 + FindCurrentTaskIndex()]]; else skip 7.
    runVector(c, true, "op0E/reload", {0x0E, 2, 3, 4, 5, 6, 8},
              [](ScriptedHost& h, ScriptShop&) { h.ret["tf98"] = 1; h.taskIdx = 4; },
              {target(6), "tf98(80);fcti();", 1});
    runVector(c, true, "op0E/skip", {0x0E, 2, 3, 4, 5, 6, 8},
              [](ScriptedHost& h, ScriptShop&) { h.ret["tf98"] = 0; h.taskIdx = 4; },
              {kScript + 7, "tf98(80);", 0});
    // Effect ops: {op then text} and {op chained with a following op}.
    runVector(c, true, "op0F/a", {0x0F, 9}, [](ScriptedHost&, ScriptShop&) {}, {kScript + 2, "sf98(9);", 0});
    runVector(c, true, "op0F/b", {0x0F, 9, 0x0F, 10}, [](ScriptedHost&, ScriptShop&) {},
              {kScript + 4, "sf98(9);sf98(10);", 0});
    runVector(c, true, "op10/a", {0x10, 9}, [](ScriptedHost&, ScriptShop&) {}, {kScript + 2, "cf98(9);", 0});
    runVector(c, true, "op10/b", {0x10, 9, 0x10, 10}, [](ScriptedHost&, ScriptShop&) {},
              {kScript + 4, "cf98(9);cf98(10);", 0});
    runVector(c, true, "op11/a", {0x11, 3, 4}, [](ScriptedHost&, ScriptShop&) {},
              {kScript + 3, "cfsb(3,4,0);", 0});
    runVector(c, true, "op11/b", {0x11, 3, 4, 0x11, 5, 6}, [](ScriptedHost&, ScriptShop&) {},
              {kScript + 6, "cfsb(3,4,0);cfsb(5,6,0);", 0});
    // op 12: no operand; parent.done=1, parent.result=10.
    runVector(c, true, "op12/a", {0x12}, [](ScriptedHost&, ScriptShop&) {}, {kScript + 1, "", 0},
              [](const ScriptTask& t, const ScriptShop&) { return t.parentDone == 1 && t.parentResult == 10; });
    runVector(c, true, "op12/b", {0x12, 0x13, 4}, [](ScriptedHost&, ScriptShop&) {},
              {kScript + 3, "rtr(4);", 0},
              [](const ScriptTask& t, const ScriptShop&) { return t.parentDone == 1 && t.parentResult == 10; });
    runVector(c, true, "op13/a", {0x13, 4}, [](ScriptedHost&, ScriptShop&) {}, {kScript + 2, "rtr(4);", 0});
    runVector(c, true, "op13/b", {0x13, 4, 0x0F, 2}, [](ScriptedHost&, ScriptShop&) {},
              {kScript + 4, "rtr(4);sf98(2);", 0});
    // op 14: result 12 + done; ReleaseStampFlag sees the OLD (town,shop); then overwritten.
    runVector(c, true, "op14/a", {0x14, 3, 4}, [](ScriptedHost&, ScriptShop&) {},
              {kScript + 3, "rsf(5,9);", 0},
              [](const ScriptTask& t, const ScriptShop& s) {
                  return t.parentDone == 1 && t.parentResult == 12 && s.town == 3 && s.shop == 4;
              });
    runVector(c, true, "op14/b", {0x14, 0xF5, 0x20}, [](ScriptedHost&, ScriptShop&) {},
              {kScript + 3, "rsf(5,9);", 0},
              [](const ScriptTask& t, const ScriptShop& s) {
                  return t.parentDone == 1 && t.parentResult == 12 && s.town == -11 && s.shop == 0x20;
              });
    runVector(c, true, "op15/a", {0x15, 6}, [](ScriptedHost&, ScriptShop&) {}, {kScript + 2, "ssb88(6);", 0});
    runVector(c, true, "op15/b", {0x15, 6, 0x15, 7}, [](ScriptedHost&, ScriptShop&) {},
              {kScript + 4, "ssb88(6);ssb88(7);", 0});
    branchPair(c, "op16", {0x16, I}, 2, I, "tpfp", "tpfp(5,9);");
    runVector(c, true, "op17/a", {0x17, 8}, [](ScriptedHost&, ScriptShop&) {}, {kScript + 2, "spfq(8);", 0});
    runVector(c, true, "op17/b", {0x17, 8, 0x17, 9}, [](ScriptedHost&, ScriptShop&) {},
              {kScript + 4, "spfq(8);spfq(9);", 0});
    // op 18: in [54000, 198000] -> skip 2; outside -> table[idx].
    runVector(c, true, "op18/in", {0x18, I}, [](ScriptedHost&, ScriptShop& s) { s.word14 = 54000; },
              {kScript + 2, "", 0});
    runVector(c, true, "op18/out", {0x18, I}, [](ScriptedHost&, ScriptShop& s) { s.word14 = 53999; },
              {target(I), "", 1});
    branchPair(c, "op19", {0x19, 9, I}, 3, I, "tpf", "tpf(9);");
    // op 1A: n <= 100 - CountBits(0): n=30, count 70 -> taken; count 71 -> not.
    runVector(c, true, "op1A/taken", {0x1A, 30, I}, [](ScriptedHost& h, ScriptShop&) { h.countBits = 70; },
              {target(I), "cb(0);", 1});
    runVector(c, true, "op1A/not", {0x1A, 30, I}, [](ScriptedHost& h, ScriptShop&) { h.countBits = 71; },
              {kScript + 3, "cb(0);", 0});
    // op 1B: licence!=3 -> skip 2; licence==3 && state(100) -> branch.
    runVector(c, true, "op1B/skip", {0x1B, I}, [](ScriptedHost& h, ScriptShop&) { h.licence = 2; },
              {kScript + 2, "lic();", 0});
    runVector(c, true, "op1B/branch", {0x1B, I},
              [](ScriptedHost& h, ScriptShop&) { h.licence = 3; h.ret["tsb78"] = 1; },
              {target(I), "lic();tsb78(100);", 1});
    // op 1C: (s8)shop.town == id.
    runVector(c, true, "op1C/eq", {0x1C, 5, I}, [](ScriptedHost&, ScriptShop&) {}, {target(I), "", 1});
    runVector(c, true, "op1C/ne", {0x1C, 6, I}, [](ScriptedHost&, ScriptShop&) {}, {kScript + 3, "", 0});
    branchPair(c, "op1D", {0x1D, 3, 4, I}, 4, I, "toa", "toa(177f760,3,4);");
}

void extraVectors(VecCtx& c) {
    constexpr unsigned I = 7;
    // op 08 third path: no match, Fcb8 clear -> table[idxB].
    runVector(c, false, "op08/B", {0x08, 6, 9}, [](ScriptedHost&, ScriptShop&) {},
              {target(9), "msps(5,9);tfcb8(5,9);", 1});
    // op 18 boundaries: 54000+0x23280 = 198000 is in range, +1 is out.
    runVector(c, false, "op18/hi-in", {0x18, I}, [](ScriptedHost&, ScriptShop& s) { s.word14 = 198000; },
              {kScript + 2, "", 0});
    runVector(c, false, "op18/hi-out", {0x18, I}, [](ScriptedHost&, ScriptShop& s) { s.word14 = 198001; },
              {target(I), "", 1});
    runVector(c, false, "op18/neg", {0x18, I}, [](ScriptedHost&, ScriptShop& s) { s.word14 = -1; },
              {target(I), "", 1});
    // op 1B remaining paths.
    runVector(c, false, "op1B/flag57-skip", {0x1B, I},
              [](ScriptedHost& h, ScriptShop&) { h.licence = 3; h.ret["tsb78"] = 0; h.ret["tf98"] = 1; },
              {kScript + 2, "lic();tsb78(100);tf98(87);", 0});
    runVector(c, false, "op1B/flag57-clear-branch", {0x1B, I},
              [](ScriptedHost& h, ScriptShop&) { h.licence = 3; },
              {target(I), "lic();tsb78(100);tf98(87);", 1});
    // op 1C: lb vs lbu -- town -1 never equals byte 0xFF.
    runVector(c, false, "op1C/signed", {0x1C, 0xFF, I}, [](ScriptedHost&, ScriptShop& s) { s.town = -1; },
              {kScript + 3, "", 0});
    // op 1A boundary: n == 100 - count taken; n > taken? (n=101 never).
    runVector(c, false, "op1A/zero", {0x1A, 0, I}, [](ScriptedHost& h, ScriptShop&) { h.countBits = 100; },
              {target(I), "cb(0);", 1});
    // op 0B compares 64-bit: -1 stored never equals a byte.
    runVector(c, false, "op0B/neg", {0x0B, 0xFF, I}, [](ScriptedHost& h, ScriptShop&) { h.ret["gprr"] = -1; },
              {kScript + 3, "gprr(-1);", 0});
    // op 0E: per-task index 0 and 5 select different entries.
    runVector(c, false, "op0E/idx0", {0x0E, 2, 3, 4, 5, 6, 8},
              [](ScriptedHost& h, ScriptShop&) { h.ret["tf98"] = 1; h.taskIdx = 0; },
              {target(2), "tf98(80);fcti();", 1});
    runVector(c, false, "op0E/idx5", {0x0E, 2, 3, 4, 5, 6, 8},
              [](ScriptedHost& h, ScriptShop&) { h.ret["tf98"] = 1; h.taskIdx = 5; },
              {target(8), "tf98(80);fcti();", 1});
    // Chaining through a taken branch: op01 -> target containing another op.
    {
        Rig rig({0x01, 1, 4});
        rig.mem.put8(target(4), 0x0F);  // target 4: SetFlag 3 then text
        rig.mem.put8(target(4) + 1, 3);
        rig.mem.put8(target(4) + 2, kText);
        ScriptedHost host;
        host.ret["tf98"] = 1;
        ScriptShop shop;
        ScriptTask task;
        task.tableBase = kTable;
        const auto run = gameplay::runScriptPrefix(rig.mem, host, task, shop, kScript);
        const bool ok = run.status == gameplay::ScriptStatus::Ok && run.cursor == target(4) + 2 &&
                        host.str() == "tf98(1);sf98(3);" && run.branchesTaken == 1 && run.opsExecuted == 2;
        ++c.extraTotal;
        c.extraPass += ok ? 1 : 0;
        if (!ok) std::cerr << "VECTOR FAIL chain-through-branch: " << host.str() << "\n";
    }
    // Prefix end: bytes >= 0x20 return immediately (0x20, 0x7F, 0xFF).
    for (std::uint8_t b : {std::uint8_t(0x20), std::uint8_t(0x7F), std::uint8_t(0xFF)}) {
        Rig rig({b});
        ScriptedHost host;
        ScriptShop shop;
        ScriptTask task;
        task.tableBase = kTable;
        const auto run = gameplay::runScriptPrefix(rig.mem, host, task, shop, kScript);
        const bool ok = run.cursor == kScript && run.opsExecuted == 0 && host.str().empty();
        ++c.extraTotal;
        c.extraPass += ok ? 1 : 0;
    }
    // Step limit on a self-loop (original would hang): table[2] -> script start.
    {
        Rig rig({0x01, 1, 2});
        rig.mem.put32(kTable + 8, kScript);
        ScriptedHost host;
        host.ret["tf98"] = 1;
        ScriptShop shop;
        ScriptTask task;
        task.tableBase = kTable;
        const auto run = gameplay::runScriptPrefix(rig.mem, host, task, shop, kScript, 50);
        const bool ok = run.status == gameplay::ScriptStatus::StepLimit && run.opsExecuted == 50;
        ++c.extraTotal;
        c.extraPass += ok ? 1 : 0;
    }
    // Bad read: script running off the buffer.
    {
        Rig rig({0x0F});  // operand missing -> reads the text byte; fine. Use a table read OOB instead.
        rig.mem.put32(kTable + 4 * 3, 0xDEAD0000u);
        ScriptedHost host;
        host.ret["tf98"] = 1;
        rig.mem.putBytes(kScript, {0x01, 1, 3});
        ScriptShop shop;
        ScriptTask task;
        task.tableBase = kTable;
        // target 0xDEAD0000 is unmapped -> next opcode read fails
        const auto run = gameplay::runScriptPrefix(rig.mem, host, task, shop, kScript);
        const bool ok = run.status == gameplay::ScriptStatus::BadRead;
        ++c.extraTotal;
        c.extraPass += ok ? 1 : 0;
    }
}

// (e) unknown-op report + skip rule for ops 0, 9, 10, 12, 0x1E, 0x1F.
void unknownVectors(VecCtx& c, int& total, int& pass) {
    for (std::uint8_t op : {std::uint8_t(0), std::uint8_t(9), std::uint8_t(10), std::uint8_t(12),
                            std::uint8_t(0x1E), std::uint8_t(0x1F)}) {
        // Case 1: next byte < 0x20 -> skip every following control byte; the
        // would-be ops 0F 05 are swallowed (no SetFlag call).
        {
            Rig rig({op, 0x0F, 0x05, 0x03});
            ScriptedHost host;
            ScriptShop shop;
            shop.townLabel = 7;
            shop.shop = 9;
            ScriptTask task;
            task.tableBase = kTable;
            const auto run = gameplay::runScriptPrefix(rig.mem, host, task, shop, kScript);
            std::ostringstream want;
            want << "unk(" << int(op) << ",7,9);unkskip(" << std::hex << (kScript + 4) << ");";
            ++total;
            const bool ok = run.status == gameplay::ScriptStatus::Ok && run.cursor == kScript + 4 &&
                            host.str() == want.str() && run.unknownHits == 1;
            pass += ok ? 1 : 0;
            if (!ok) std::cerr << "UNKNOWN FAIL case1 op=" << int(op) << " log=" << host.str() << "\n";
        }
        // Case 2: next byte >= 0x20 -> cursor stays on it (opcode+1).
        {
            Rig rig({op});
            ScriptedHost host;
            ScriptShop shop;
            shop.townLabel = -2;
            shop.shop = 200;
            ScriptTask task;
            task.tableBase = kTable;
            const auto run = gameplay::runScriptPrefix(rig.mem, host, task, shop, kScript);
            std::ostringstream want;
            want << "unk(" << int(op) << ",-2,200);unkskip(" << std::hex << (kScript + 1) << ");";
            ++total;
            const bool ok = run.status == gameplay::ScriptStatus::Ok && run.cursor == kScript + 1 &&
                            host.str() == want.str();
            pass += ok ? 1 : 0;
            if (!ok) std::cerr << "UNKNOWN FAIL case2 op=" << int(op) << " log=" << host.str() << "\n";
        }
        c.hash.add(op, 1);
    }
    // Static decode classifies them as UnknownOp, not live.
    for (std::uint8_t op : {std::uint8_t(0), std::uint8_t(9), std::uint8_t(10), std::uint8_t(12),
                            std::uint8_t(0x1E), std::uint8_t(0x1F)}) {
        Rig rig({op, kText});
        const auto pre = gameplay::decodeScriptPrefix(rig.mem, kScript, kTableWords, false);
        ++total;
        pass += (pre.status == gameplay::ScriptPrefixStatus::UnknownOp && pre.unknownOp == op &&
                 !gameplay::scriptOpIsLive(op))
                    ? 1
                    : 0;
    }
}

// ---------------------------------------------------------------------------
// ELF-side checks.
// ---------------------------------------------------------------------------
struct ElfChecks {
    bool dispatcher = false;
    bool jumpTable = false;
    int liveHandlers = 0;
    int distinctTargets = 0;
    bool unknownAlias = false;
    bool caseVaddrsMatch = false;
};

ElfChecks checkJumpTable(const assets::ScriptImage& img) {
    ElfChecks r;
    // Dispatcher words (0x23AD28..0x23AD48): addiu v1,v1,-1 ; sltiu v0,v1,0x1d ;
    // beqz v0,0x23b0e0 ; addiu s0,s0,1 ; sll v0,v1,2 ; lui v1,0x2f ; addu ;
    // lw v1,-0x38b0(v1) ; jr v1.
    const std::array<std::pair<std::uint32_t, std::uint32_t>, 6> w = {{
        {0x23AD28, 0x2463FFFF}, {0x23AD2C, 0x2C62001D}, {0x23AD38, 0x00031080},
        {0x23AD3C, 0x3C03002F}, {0x23AD44, 0x8C63C750}, {0x23AD48, 0x00600008},
    }};
    r.dispatcher = true;
    for (const auto& [a, v] : w) {
        std::uint32_t got = 0;
        if (!img.readU32(a, got) || got != v) r.dispatcher = false;
    }
    r.jumpTable = true;
    std::set<std::uint32_t> distinct;
    r.caseVaddrsMatch = true;
    r.unknownAlias = true;
    for (std::uint32_t i = 0; i < 29; ++i) {
        std::uint32_t t = 0;
        if (!img.readU32(0x2EC750 + 4 * i, t) || t < 0x23AD50 || t > 0x23B0E0) r.jumpTable = false;
        distinct.insert(t);
        if (t != gameplay::kScriptOps[i].caseVaddr) r.caseVaddrsMatch = false;
        const bool stub = (t == 0x23B0E0);
        if (stub != (i == 8 || i == 9 || i == 11)) r.unknownAlias = false;
        if (!stub) ++r.liveHandlers;
    }
    r.distinctTargets = static_cast<int>(distinct.size());
    std::uint32_t after = 0xFFFFFFFF, nextWord = 0;
    img.readU32(0x2EC750 + 4 * 29, after);
    img.readU32(0x2EC750 + 4 * 30, nextWord);
    // Word 30 (1-based) is 0; word 31 is the first bytes of the next string
    // ("next msg #%d") -- an adjacent string, not a table terminator.
    if (after != 0 || nextWord != 0x7478656Eu) r.jumpTable = false;
    return r;
}

// ---------------------------------------------------------------------------
// Replay host: bit-accurate FC98 flag set/clear/test (0x23D778/7A8/7E0, 256
// bits), every other predicate answered by mode: 0 = all true, 1 = all false,
// 2 = seeded pseudo-random.  Every call + result is folded into `log`.
// ---------------------------------------------------------------------------
class ModelHost final : public IScriptHost {
public:
    ModelHost(int mode, std::uint64_t seed) : mode_(mode), state_(seed * 0x9E3779B97F4A7C15ULL + 1) {
        for (auto& w : f98_) w = mode == 0 ? ~0ULL : (mode == 1 ? 0ULL : next());
    }
    Fnv log;
    std::uint32_t calls = 0;
    std::uint32_t unknownReports = 0;

    std::int64_t testFlagBitFc98(std::uint8_t id) override {
        return rec(1, id, 0, (f98_[id >> 6] >> (id & 63)) & 1);
    }
    std::int64_t getFlaggedSlotCount(std::uint8_t s, std::uint8_t b, std::int32_t) override {
        return rec(2, s, b, pred() ? static_cast<std::int64_t>(next() % 6) : 0);
    }
    std::int64_t testStampFlag(std::int8_t t, std::uint8_t s) override { return rec(3, t, s, pred()); }
    std::int64_t testFlagFc68(std::uint8_t id) override { return rec(4, id, 0, pred()); }
    std::int64_t testStateBitFc78(std::uint8_t id) override { return rec(5, id, 0, pred()); }
    std::int64_t checkSlotPresence() override { return rec(6, 0, 0, pred() ? 1 + static_cast<std::int64_t>(next() % 2) : 0); }
    std::int64_t matchSlotPairState(std::int8_t t, std::uint8_t s) override { return rec(7, t, s, pred()); }
    std::int64_t testFlagFcb8(std::int8_t t, std::uint8_t s) override { return rec(8, t, s, pred()); }
    std::int64_t testProgressFlagForPair(std::int8_t t, std::uint8_t s) override { return rec(9, t, s, pred()); }
    std::int64_t getOrSetPresidentRaceResult(std::int64_t) override {
        return rec(10, 0, 0, mode_ == 0 ? 2 : (mode_ == 1 ? 0 : static_cast<std::int64_t>(next() % 3)));
    }
    std::int64_t testFlagFca8Clear(std::uint8_t id) override { return rec(11, id, 0, pred()); }
    std::int64_t testProgressFlag(std::uint8_t b) override { return rec(12, b, 0, pred()); }
    std::int32_t countBitsFca8Fcb0(std::int32_t) override {
        return static_cast<std::int32_t>(rec(13, 0, 0, mode_ == 0 ? 0 : (mode_ == 1 ? 100 : static_cast<std::int64_t>(next() % 101))));
    }
    std::int64_t testObjectAttribute(std::uint32_t, std::uint8_t a, std::uint8_t b) override { return rec(14, a, b, pred()); }
    std::int32_t findCurrentTaskIndex() override {
        return static_cast<std::int32_t>(rec(15, 0, 0, mode_ == 0 ? 5 : (mode_ == 1 ? 0 : static_cast<std::int64_t>(next() % 6))));
    }
    std::int8_t licenceByte() override {
        return static_cast<std::int8_t>(rec(16, 0, 0, mode_ == 0 ? 3 : (mode_ == 1 ? 0 : static_cast<std::int64_t>(next() % 4))));
    }
    void setFlagBitFc98(std::uint8_t id) override { f98_[id >> 6] |= 1ULL << (id & 63); rec(17, id, 0, 0); }
    void clearFlagBitFc98(std::uint8_t id) override { f98_[id >> 6] &= ~(1ULL << (id & 63)); rec(18, id, 0, 0); }
    void clearFlaggedSlotBit(std::uint8_t s, std::uint8_t b, std::int32_t) override { rec(19, s, b, 0); }
    void recordTaskResult(std::uint8_t c) override { rec(20, c, 0, 0); }
    void releaseStampFlag(const ScriptShop& s) override { rec(21, s.town, s.shop, 0); }
    void setStateBitFc88(std::uint8_t id) override { rec(22, id, 0, 0); }
    void setProgressFlagAndQueue(std::uint8_t id) override { rec(23, id, 0, 0); }
    void onUnknownPreControl(std::uint8_t c, std::int8_t tl, std::uint8_t s) override {
        ++unknownReports;
        rec(24, c, tl, s);
    }
    void onUnknownSkipped(std::uint32_t cur) override { rec(25, static_cast<std::int64_t>(cur), 0, 0); }

private:
    std::uint64_t next() {
        state_ += 0x9E3779B97F4A7C15ULL;
        std::uint64_t z = state_;
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        return z ^ (z >> 31);
    }
    std::int64_t pred() { return mode_ == 0 ? 1 : (mode_ == 1 ? 0 : static_cast<std::int64_t>(next() & 1)); }
    std::int64_t rec(int tag, std::int64_t a, std::int64_t b, std::int64_t result) {
        ++calls;
        log.add(static_cast<std::uint64_t>(tag), 1);
        log.add(static_cast<std::uint64_t>(a), 8);
        log.add(static_cast<std::uint64_t>(b), 8);
        log.add(static_cast<std::uint64_t>(result), 8);
        return result;
    }
    int mode_;
    std::uint64_t state_;
    std::array<std::uint64_t, 4> f98_{};
};

// ---------------------------------------------------------------------------
// ROM measurements.
// ---------------------------------------------------------------------------
struct Measured {
    Pins pins;
    // Not pinned individually (derived/informational) but folded into hashes.
    std::size_t entryUnknown = 0;     // entry scripts with an unknown op in their prefix
    std::uint32_t entryOob = 0;       // branch operands >= table length in entry prefixes
    std::uint32_t entryBleed = 0;
    std::uint32_t allOob = 0;         // strict out-of-bounds over every target prefix
    std::size_t reach = 0;            // (table,script) pairs reachable from entries via prefix branches
    std::size_t reachUnclean = 0;
    std::size_t shopLookupOk = 0;     // shopTable(town,slot) in census, plus terminator zero
    std::size_t shopLookupTotal = 0;
    std::uint64_t replaySlots = 0, replayOps = 0, replayBranches = 0, replayUnknown = 0;
    std::uint64_t replayStepLimit = 0, replayBadRead = 0, replayDone = 0;
    std::array<std::uint32_t, 32> opHist{};
    std::uint64_t modeOps[3] = {0, 0, 0}, modeBranches[3] = {0, 0, 0}, modeDone[3] = {0, 0, 0};
};

Measured measure(const assets::ScriptImage& img) {
    Measured m;
    Pins& p = m.pins;
    const assets::ScriptCensus c = img.census();
    const gameplay::ScriptImageMemory mem(img);

    p.towns = c.towns;
    p.level1Slots = c.level1Slots;
    p.level2Tables = c.tables.size();
    Fnv ch;
    for (std::uint32_t v : c.townTable) ch.add(v, 4);
    for (std::uint32_t v : c.slotsPerTown) ch.add(v, 4);
    std::set<std::uint32_t> tableSet;
    for (const auto& t : c.tables) {
        tableSet.insert(t.vaddr);
        p.sumTableWords += t.length;
        ch.add(t.vaddr, 4);
        ch.add(t.length, 4);
        ch.add(static_cast<std::uint8_t>(t.end), 1);
        switch (t.end) {
        case assets::ScriptTableEnd::NextTable: ++p.endNextTable; break;
        case assets::ScriptTableEnd::ZeroPad: ++p.endZeroPad; break;
        case assets::ScriptTableEnd::Foreign: ++p.endForeign; break;
        case assets::ScriptTableEnd::Last: ++p.endLast; break;
        }
    }
    p.censusHash = ch.h;

    // shopTable() must agree with the census (and stop at the zero word).
    for (std::size_t t = 1; t <= assets::kScriptTownCount; ++t) {
        const std::uint32_t n = c.slotsPerTown[t - 1];
        for (std::uint32_t s = 0; s < n; ++s) {
            ++m.shopLookupTotal;
            m.shopLookupOk += tableSet.count(img.shopTable(t, s)) ? 1 : 0;
        }
        ++m.shopLookupTotal;
        m.shopLookupOk += img.shopTable(t, n) == 0 ? 1 : 0;
    }

    Fnv th, oh;
    std::set<std::uint32_t> distinct;
    std::size_t reachTotal = 0, reachUnclean = 0;
    for (const auto& t : c.tables) {
        const bool adj = t.end == assets::ScriptTableEnd::NextTable;
        // Entry script (table[1]).
        std::uint32_t entry = 0;
        const bool haveEntry = t.length >= 2 && img.readU32(t.vaddr + 4, entry);
        if (haveEntry) {
            const auto pre = gameplay::decodeScriptPrefix(mem, entry, t.length, adj);
            if (pre.status == gameplay::ScriptPrefixStatus::Clean && pre.endByte >= 0x20) ++p.entryClean;
            if (pre.status == gameplay::ScriptPrefixStatus::UnknownOp) ++m.entryUnknown;
            m.entryOob += pre.oob;
            m.entryBleed += pre.bleed;
        }
        // Every script start [1..len) of the table.
        for (std::uint32_t i = 1; i < t.length; ++i) {
            std::uint32_t ptr = 0;
            if (!img.readU32(t.vaddr + 4 * i, ptr)) continue;
            ++p.targetRefs;
            distinct.insert(ptr);
            const auto pre = gameplay::decodeScriptPrefix(mem, ptr, t.length, adj);
            if (pre.status == gameplay::ScriptPrefixStatus::Clean) ++p.targetsClean;
            else ++p.targetsUnknown;
            th.add(t.vaddr, 4);
            th.add(i, 4);
            th.add(static_cast<std::uint8_t>(pre.status), 1);
            th.add(pre.ops.size(), 4);
            th.add(pre.endByte, 1);
            th.add(pre.endCursor - ptr, 4);
            p.opsDecoded += static_cast<std::uint32_t>(pre.ops.size());
            p.branchOperands += pre.branchOperands;
            p.bleed += pre.bleed;
            m.allOob += pre.oob;
            for (const auto& o : pre.ops) ++m.opHist[o.op];
        }
        // Reachability from the entry via prefix branch operands.
        if (haveEntry) {
            std::set<std::uint32_t> seen{entry};
            std::vector<std::uint32_t> work{entry};
            while (!work.empty()) {
                const std::uint32_t cur = work.back();
                work.pop_back();
                const auto pre = gameplay::decodeScriptPrefix(mem, cur, t.length, adj);
                ++reachTotal;
                if (pre.status != gameplay::ScriptPrefixStatus::Clean) ++reachUnclean;
                for (std::uint8_t idx : pre.branchIdx) {
                    std::uint32_t tgt = 0;
                    if (img.readU32(t.vaddr + 4u * idx, tgt) && seen.insert(tgt).second) work.push_back(tgt);
                }
            }
        }
    }
    p.targetsDistinct = distinct.size();
    p.targetHash = th.h;
    for (std::size_t op = 0; op < m.opHist.size(); ++op) {
        oh.add(op, 1);
        oh.add(m.opHist[op], 4);
    }
    p.opHistHash = oh.h;
    m.reach = reachTotal;
    m.reachUnclean = reachUnclean;

    // Replay every (town, slot) through the VM in three host modes.
    Fnv rh;
    for (std::size_t t = 1; t <= assets::kScriptTownCount; ++t) {
        for (std::uint32_t s = 0; s < c.slotsPerTown[t - 1]; ++s) {
            const std::uint32_t table = img.shopTable(t, s);
            for (int mode = 0; mode < 3; ++mode) {
                ModelHost host(mode, (static_cast<std::uint64_t>(t) << 8 | s) * 3 + mode);
                ScriptShop shop;
                shop.town = static_cast<std::int8_t>(t);
                shop.townLabel = static_cast<std::int8_t>(t);
                shop.shop = static_cast<std::uint8_t>(s);
                shop.word14 = mode == 0 ? 54000 : (mode == 1 ? 0 : 54000 + static_cast<std::int32_t>((t * 977 + s * 31) % 0x23281));
                ScriptTask task;
                std::uint32_t cursor = 0;
                if (!gameplay::beginScriptTable(mem, table, task, cursor)) {
                    ++m.replayBadRead;
                    continue;
                }
                const gameplay::ScriptRun run = gameplay::runScriptPrefix(mem, host, task, shop, cursor, 4096);
                ++m.replaySlots;
                m.replayOps += run.opsExecuted;
                m.replayBranches += run.branchesTaken;
                m.replayUnknown += run.unknownHits;
                m.replayStepLimit += run.status == gameplay::ScriptStatus::StepLimit;
                m.replayBadRead += run.status == gameplay::ScriptStatus::BadRead;
                m.replayDone += task.parentDone;
                m.modeOps[mode] += run.opsExecuted;
                m.modeBranches[mode] += run.branchesTaken;
                m.modeDone[mode] += task.parentDone;
                rh.add(table, 4);
                rh.add(static_cast<std::uint8_t>(run.status), 1);
                rh.add(run.cursor - cursor, 4);  // relative to the entry script start
                rh.add(run.cursor, 4);
                rh.add(run.opsExecuted, 4);
                rh.add(run.branchesTaken, 4);
                rh.add(host.log.h, 8);
                rh.add(static_cast<std::uint64_t>(task.parentResult), 4);
                rh.add(static_cast<std::uint8_t>(shop.town), 1);
                rh.add(shop.shop, 1);
            }
        }
    }
    p.replayHash = rh.h;
    return m;
}

struct Suite {
    Measured m;
    ElfChecks elf;
    VecCtx vec;
    int unkTotal = 0, unkPass = 0;
    std::uint64_t digest = 0;
};

Suite runSuite(const assets::ScriptImage& img) {
    Suite s;
    s.elf = checkJumpTable(img);
    coreVectors(s.vec);
    extraVectors(s.vec);
    unknownVectors(s.vec, s.unkTotal, s.unkPass);
    s.m = measure(img);
    Fnv d;
    const Pins& p = s.m.pins;
    for (std::uint64_t v : {std::uint64_t(p.towns), std::uint64_t(p.level1Slots), std::uint64_t(p.level2Tables),
                            p.sumTableWords, p.censusHash, std::uint64_t(p.entryClean), p.targetHash,
                            p.opHistHash, p.replayHash, s.vec.hash.h, std::uint64_t(s.vec.pass),
                            std::uint64_t(s.vec.extraPass), std::uint64_t(s.unkPass)}) {
        d.add(v, 8);
    }
    s.digest = d.h;
    return s;
}

// Pinned expected values.  Derived from the ELF by this test AND by an
// independent Python reference (docs/native-reference/script-vm.md §Repro);
// the replay hash (seeded mode 2) is C++-only, modes 0/1 are cross-checked.
const Pins kPins = {
    /*towns*/ 21, /*level1Slots*/ 330, /*level2Tables*/ 307, /*sumTableWords*/ 3174,
    /*endNextTable*/ 96, /*endZeroPad*/ 205, /*endForeign*/ 5, /*endLast*/ 1,
    /*censusHash*/ 0x2f5ffedee5bc99afULL,
    /*entryClean*/ 307, /*targetsDistinct*/ 2478, /*targetRefs*/ 2867, /*targetsClean*/ 2864,
    /*targetsUnknown*/ 3, /*targetHash*/ 0xecbb81a298a79c34ULL, /*opHistHash*/ 0xe30b5918821f704cULL,
    /*opsDecoded*/ 1431, /*branchOperands*/ 1096, /*bleed*/ 1,
    /*replayHash*/ 0x5ac718a076d1b868ULL,
};
// Replay totals over 330 slots x 3 host modes (990 runs); modes 0/1 re-derived in Python.
constexpr std::uint64_t kReplaySlots = 990, kReplayOps = 1604, kReplayBranches = 524;
constexpr std::uint64_t kModeOps[3] = {549, 537, 518}, kModeBranches[3] = {311, 39, 174};
constexpr std::size_t kReach = 1026;  // (table, script) pairs reachable from entries via prefix branches

}  // namespace

int main(int argc, char** argv) {
    std::string elf;
    bool allowSkip = false;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--elf" && i + 1 < argc) elf = argv[++i];
        else if (a == "--allow-skip") allowSkip = true;
        else if (a == "--verbose") gVerbose = true;
    }
    if (elf.empty()) {
        if (const char* e = std::getenv("RT_ELF")) elf = e;
    }
    if (elf.empty()) {
        if (const char* home = std::getenv("HOME")) {
            elf = std::string(home) + "/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
        }
    }
    std::error_code ec;
    if (elf.empty() || !std::filesystem::exists(elf, ec)) {
        if (allowSkip) {
            std::cout << "W2-01 SKIP NO_ELF (--allow-skip)\n";
            return 0;
        }
        std::cerr << "NO_ELF: " << elf << "\n";
        return 2;
    }
    const auto image = assets::ScriptImage::open(elf);
    if (!image) {
        std::cerr << "BAD_ELF: " << elf << "\n";
        return 2;
    }

    const Suite a = runSuite(*image);
    const Suite b = runSuite(*image);
    const bool deterministic = a.digest == b.digest;
    const Pins& p = a.m.pins;

    if (gVerbose) {
        std::cerr << std::dec << "MEASURED towns=" << p.towns << " l1=" << p.level1Slots << " l2=" << p.level2Tables
                  << " words=" << p.sumTableWords << " end(next/zero/foreign)=" << p.endNextTable << '/'
                  << p.endZeroPad << '/' << p.endForeign << " last=" << p.endLast << std::hex << " censusHash=0x" << p.censusHash << std::dec
                  << "\n entryClean=" << p.entryClean << " entryUnknown=" << a.m.entryUnknown
                  << " entryOob=" << a.m.entryOob << " entryBleed=" << a.m.entryBleed
                  << "\n targetRefs=" << p.targetRefs << " distinct=" << p.targetsDistinct << " clean=" << p.targetsClean
                  << " unknown=" << p.targetsUnknown << " opsDecoded=" << p.opsDecoded
                  << " branchOperands=" << p.branchOperands << " allOob=" << a.m.allOob << " bleed=" << p.bleed
                  << "\n reach=" << a.m.reach << " reachUnclean=" << a.m.reachUnclean
                  << " shopLookup=" << a.m.shopLookupOk << '/' << a.m.shopLookupTotal << std::hex
                  << "\n targetHash=0x" << p.targetHash << " opHistHash=0x" << p.opHistHash << " replayHash=0x"
                  << p.replayHash << std::dec << "\n replay slots=" << a.m.replaySlots << " ops=" << a.m.replayOps
                  << " branches=" << a.m.replayBranches << " unknown=" << a.m.replayUnknown
                  << " steplimit=" << a.m.replayStepLimit << " badread=" << a.m.replayBadRead
                  << " done=" << a.m.replayDone << "\n modeOps=" << a.m.modeOps[0] << '/' << a.m.modeOps[1] << '/' << a.m.modeOps[2]
                  << " modeBranches=" << a.m.modeBranches[0] << '/' << a.m.modeBranches[1] << '/' << a.m.modeBranches[2]
                  << " modeDone=" << a.m.modeDone[0] << '/' << a.m.modeDone[1] << '/' << a.m.modeDone[2] << "\n opHist:";
        for (std::size_t op = 0; op < a.m.opHist.size(); ++op) {
            if (a.m.opHist[op]) std::cerr << ' ' << std::hex << op << ':' << std::dec << a.m.opHist[op];
        }
        std::cerr << "\n";
    }

    const bool pinsOk =
        p.towns == kPins.towns && p.level1Slots == kPins.level1Slots && p.level2Tables == kPins.level2Tables &&
        p.sumTableWords == kPins.sumTableWords && p.endNextTable == kPins.endNextTable &&
        p.endZeroPad == kPins.endZeroPad && p.endForeign == kPins.endForeign && p.endLast == kPins.endLast && p.censusHash == kPins.censusHash &&
        p.targetRefs == kPins.targetRefs && p.targetsDistinct == kPins.targetsDistinct &&
        p.targetsClean == kPins.targetsClean && p.targetsUnknown == kPins.targetsUnknown &&
        p.targetHash == kPins.targetHash && p.opHistHash == kPins.opHistHash && p.opsDecoded == kPins.opsDecoded &&
        p.branchOperands == kPins.branchOperands && p.bleed == kPins.bleed && p.replayHash == kPins.replayHash;

    const bool entriesOk = p.entryClean == p.level2Tables && a.m.entryUnknown == 0;
    const bool oobOk = a.m.entryOob == 0 && a.m.allOob == 0;
    const bool elfOk = a.elf.dispatcher && a.elf.jumpTable && a.elf.liveHandlers == 26 && a.elf.distinctTargets == 27 &&
                       a.elf.unknownAlias && a.elf.caseVaddrsMatch;
    const bool vecOk = a.vec.pass == a.vec.total && a.vec.total == 52 && a.vec.extraPass == a.vec.extraTotal;
    const bool unkOk = a.unkPass == a.unkTotal && a.unkTotal == 18;  // 6 ops x {skip, no-skip} + 6 static
    const bool shopOk = a.m.shopLookupOk == a.m.shopLookupTotal;
    const bool replayOk = a.m.replayBadRead == 0 && a.m.replayStepLimit == 0 && a.m.replayUnknown == 0 &&
                          a.m.replaySlots == kReplaySlots && a.m.replayOps == kReplayOps &&
                          a.m.replayBranches == kReplayBranches && a.m.reach == kReach && a.m.reachUnclean == 0 &&
                          a.m.modeOps[0] == kModeOps[0] && a.m.modeOps[1] == kModeOps[1] &&
                          a.m.modeOps[2] == kModeOps[2] && a.m.modeBranches[0] == kModeBranches[0] &&
                          a.m.modeBranches[1] == kModeBranches[1] && a.m.modeBranches[2] == kModeBranches[2];
    const bool pass = pinsOk && entriesOk && oobOk && elfOk && vecOk && unkOk && shopOk && replayOk && deterministic;

    std::cout << "W2-01 SCORE tables=" << p.towns << '/' << p.level1Slots << '/' << p.level2Tables
              << " entry_clean=" << p.entryClean << '/' << p.level2Tables << " unknown_ops=" << a.m.entryUnknown
              << " branch_oob=" << (a.m.entryOob + a.m.allOob) << " op_vectors=" << a.vec.pass << '/' << 52
              << " deterministic=" << (deterministic ? 1 : 0) << " branch_bleed=" << p.bleed
              << " extra_vectors=" << a.vec.extraPass << '/' << a.vec.extraTotal << ' ' << (pass ? "PASS" : "FAIL")
              << '\n';
    if (!pass) {
        std::cerr << "gates: pins=" << pinsOk << " entries=" << entriesOk << " oob=" << oobOk << " elf=" << elfOk
                  << " vec=" << vecOk << " (core " << a.vec.pass << '/' << a.vec.total << " extra " << a.vec.extraPass
                  << '/' << a.vec.extraTotal << ") unknown=" << unkOk << " (" << a.unkPass << '/' << a.unkTotal << ")"
                  << " shop=" << shopOk << " replay=" << replayOk << "\n";
    }
    return pass ? 0 : 1;
}
