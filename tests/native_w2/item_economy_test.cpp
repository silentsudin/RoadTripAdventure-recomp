// W2-06 acceptance: item economy (purse funnel + shop buy/sell + price tables).
//   w2_item_economy_test [--elf PATH] [--verbose]
// ELF: $RT_ELF (overrides), else --elf, else ~/Library/Application Support/
// RoadTripRecomp/disc/SLUS_203.98.  Without an ELF the fixture tables run
// (traces, invariant) and the test prints NO_ELF and exits 0; the ELF pins are
// then reported as 0/N and not counted.  With an ELF, exit 0 only if every pin
// matches, 6/6 traces match, the invariant has 0 violations and the two-run
// transcript hash is equal.  Prints exactly one `W2-06 SCORE ...` line.
// No ROM bytes are embedded: only addresses, raw words that are instruction
// encodings or pointers, and derived prices.

#include "gameplay/item_economy.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

using namespace rt::native::gameplay::item_economy;

namespace {

bool gVerbose = false;

constexpr std::uint64_t kFnvOffset = 0xcbf29ce484222325ULL;
constexpr std::uint64_t kFnvPrime = 0x100000001b3ULL;

struct Fnv {
    std::uint64_t h = kFnvOffset;
    void add(std::uint64_t v, int bytes = 8) {
        for (int i = 0; i < bytes; ++i) h = (h ^ ((v >> (8 * i)) & 0xff)) * kFnvPrime;
    }
};

// ---------------------------------------------------------------------------
// Minimal ELF32 PT_LOAD reader (virtual address -> file bytes).
// ---------------------------------------------------------------------------
class Elf {
public:
    static std::optional<Elf> open(const std::filesystem::path& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in) return std::nullopt;
        Elf e;
        e.bytes_.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
        if (e.bytes_.size() < 0x34 || std::memcmp(e.bytes_.data(), "\x7f" "ELF", 4) != 0) {
            return std::nullopt;
        }
        const std::uint32_t phoff = e.u32At(0x1C);
        const std::uint16_t phnum = e.u16At(0x2C);
        for (std::uint16_t i = 0; i < phnum; ++i) {
            const std::size_t p = phoff + 32u * i;
            if (p + 32 > e.bytes_.size()) return std::nullopt;
            if (e.u32At(p) != 1) continue;  // PT_LOAD
            Seg s{e.u32At(p + 8), e.u32At(p + 4), e.u32At(p + 16)};
            e.segs_.push_back(s);
        }
        return e;
    }

    bool readU32(std::uint32_t va, std::uint32_t& out) const {
        for (const auto& s : segs_) {
            if (va >= s.vaddr && static_cast<std::uint64_t>(va) + 4 <= s.vaddr + s.filesz) {
                const std::size_t off = s.offset + (va - s.vaddr);
                if (off + 4 > bytes_.size()) return false;
                out = u32At(off);
                return true;
            }
        }
        return false;
    }

    std::uint32_t codeEnd() const { return segs_.empty() ? 0 : segs_.front().vaddr + segs_.front().filesz; }

private:
    struct Seg {
        std::uint32_t vaddr, offset, filesz;
    };
    std::uint32_t u32At(std::size_t o) const {
        return static_cast<std::uint32_t>(bytes_[o]) | (static_cast<std::uint32_t>(bytes_[o + 1]) << 8) |
               (static_cast<std::uint32_t>(bytes_[o + 2]) << 16) |
               (static_cast<std::uint32_t>(bytes_[o + 3]) << 24);
    }
    std::uint16_t u16At(std::size_t o) const {
        return static_cast<std::uint16_t>(bytes_[o] | (bytes_[o + 1] << 8));
    }
    std::vector<std::uint8_t> bytes_;
    std::vector<Seg> segs_;
};

// ---------------------------------------------------------------------------
// ELF pins. Each check = one pin; the expected values were read from the ELF
// on this pass (vaddr -> raw word recorded in docs/native-reference/item-economy.md).
// ---------------------------------------------------------------------------
struct PinCount {
    int ok = 0;
    int total = 0;
    void check(bool cond, const char* what, std::uint32_t va = 0) {
        ++total;
        if (cond) {
            ++ok;
        } else if (gVerbose) {
            std::cerr << "PIN FAIL " << what << " @0x" << std::hex << va << std::dec << "\n";
        }
    }
};

constexpr std::uint32_t kSelectorVa = 0x002DC8A8;   // 14 classes used
constexpr std::uint32_t kGetterVa = 0x002F1280;     // 15 pointers
constexpr std::uint32_t kGetterBodies[15] = {
    0x00243698, 0x002436A0, 0x002436C0, 0x002436E0, 0x002436F8, 0x00243718, 0x00243730, 0x00243758,
    0x00243778, 0x00243790, 0x002437B0, 0x002437C8, 0x002437E8, 0x00243808, 0x00243820,
};
// Brake getter 0x243730 (class 6): sll/addu/sll/subu/sll, lui v0,0x2f,
// addu, lw v0,-0x2a28(v0): reads word at 0x2ED5D8 + 0x2C*slot.
constexpr std::uint32_t kBrakeGetterWords[8] = {
    0x00051840, 0x00651821, 0x00031880, 0x00651823, 0x00031880, 0x3C02002F, 0x00431021, 0x8C42D5D8,
};
constexpr std::uint32_t kSpendVa = 0x0023DC58;
constexpr std::uint32_t kCapHiVa = 0x0023DC88;   // lui v1,0xf
constexpr std::uint32_t kCapLoVa = 0x0023DC8C;   // ori v1,v1,0x423f
constexpr std::uint32_t kSkipSfxVa = 0x0023DCA0; // beq a0,zero,+4 (no SFX when amount==0)
constexpr std::uint32_t kSfxVa = 0x0023DCAC;     // addiu a0,zero,0x305
constexpr std::uint32_t kSeedVa = 0x00229FFC;    // addiu v1,zero,1000
constexpr std::uint32_t kSeedStoreVa = 0x0022A004;  // sw v1,0x654(v0)

// The 13 documented SpendTimerBudget callers (rules-tables.md §1.4).
constexpr std::uint32_t kSpendCallers[13] = {
    0x002366F0, 0x00236C84, 0x0023A220, 0x0023A7E0, 0x0023A850, 0x00256584, 0x00261934,
    0x00262288, 0x00269244, 0x00269284, 0x002692B4, 0x002697E4, 0x00269AFC,
};

void pinElf(const Elf& elf, PinCount& pins) {
    for (std::uint32_t i = 0; i < 14; ++i) {
        std::uint32_t w = 0;
        const std::uint32_t va = kSelectorVa + 4 * i;
        pins.check(elf.readU32(va, w) && w == i + 1, "selector word == class", va);
    }
    for (std::uint32_t i = 0; i < 15; ++i) {
        std::uint32_t w = 0;
        const std::uint32_t va = kGetterVa + 4 * i;
        pins.check(elf.readU32(va, w) && w == kGetterBodies[i], "getter pointer", va);
    }
    std::uint32_t w = 0;
    pins.check(elf.readU32(kGetterBodies[0], w) && w == 0x03E00008, "GetConst500 jr ra", kGetterBodies[0]);
    pins.check(elf.readU32(kGetterBodies[0] + 4, w) && w == 0x240201F4, "GetConst500 delay slot li 500", kGetterBodies[0] + 4);
    for (std::uint32_t i = 0; i < 8; ++i) {
        const std::uint32_t va = kGetterBodies[6] + 4 * i;
        pins.check(elf.readU32(va, w) && w == kBrakeGetterWords[i], "brake getter word", va);
    }
    for (const auto& t : priceTables()) {
        for (std::uint8_t slot = 0; slot < t.count; ++slot) {
            const std::uint32_t va = t.getterBase + t.stride * slot;
            const auto price = priceOf(ItemRef{t.category, slot});
            pins.check(elf.readU32(va, w) && price && w == *price, "price word", va);
        }
        const std::uint32_t tva = t.getterBase + t.stride * t.count;
        pins.check(elf.readU32(tva, w) && w == t.terminatorWord, "record terminator", tva);
    }
    pins.check(elf.readU32(kSpendVa, w) && w == 0x24023448, "SpendTimerBudget 0x3448 slot stride", kSpendVa);
    pins.check(elf.readU32(kCapHiVa, w) && w == 0x3C03000F, "cap lui 0xf", kCapHiVa);
    pins.check(elf.readU32(kCapLoVa, w) && w == 0x3463423F, "cap ori 0x423f", kCapLoVa);
    pins.check(elf.readU32(kSkipSfxVa, w) && w == 0x10800004, "amount==0 skips SFX", kSkipSfxVa);
    pins.check(elf.readU32(kSfxVa, w) && w == 0x24040305, "cash SFX 0x305", kSfxVa);
    pins.check(elf.readU32(kSeedVa, w) && w == 0x240303E8, "seed li 1000", kSeedVa);
    pins.check(elf.readU32(kSeedStoreVa, w) && w == 0xAC430654, "seed sw 0x654", kSeedStoreVa);

    // Every jal to SpendTimerBudget in the code segment must be a documented caller.
    const std::uint32_t jal = 0x0C000000u | (kSpendVa >> 2);
    std::vector<std::uint32_t> callers;
    for (std::uint32_t va = 0x00200000; va + 4 <= elf.codeEnd(); va += 4) {
        if (elf.readU32(va, w) && w == jal) callers.push_back(va);
    }
    int matched = 0;
    for (std::uint32_t expected : kSpendCallers) {
        bool found = false;
        for (std::uint32_t c : callers) found = found || (c == expected);
        matched += found ? 1 : 0;
        pins.check(found, "documented SpendTimerBudget caller", expected);
    }
    pins.check(callers.size() == 13 && matched == 13, "exactly 13 SpendTimerBudget jal sites");
}

// ---------------------------------------------------------------------------
// Golden purchase traces (hand-verified: ELF prices, funnel rules, sell = price/2).
// ---------------------------------------------------------------------------
struct Step {
    bool buy;
    ItemRef item;
    ShopStatus status;
    std::int32_t money;
    bool sfx;
    bool owned;
};

struct Trace {
    const char* name;
    std::int32_t startMoney;
    std::vector<ItemRef> preset;
    std::vector<Step> steps;
};

std::vector<Trace> goldenTraces() {
    const ItemRef tireSports{1, 1};       // 1000
    const ItemRef tireDevil{1, 12};       // 200000
    const ItemRef gearbox1500{2, 3};      // 1500
    const ItemRef chassis500{3, 1};       // 500
    const ItemRef horn0{13, 0};           // 0 (stock)
    return {
        {"G1 buy Sports tire at exact money",
         1000,
         {},
         {{true, tireSports, ShopStatus::Bought, 0, true, true}}},
        {"G2 buy Sports tire with 999 refused",
         999,
         {},
         {{true, tireSports, ShopStatus::Insufficient, 999, false, false}}},
        {"G3 buy gearbox 1500 then sell back for 750",
         5000,
         {},
         {{true, gearbox1500, ShopStatus::Bought, 3500, true, true},
          {false, gearbox1500, ShopStatus::Sold, 4250, true, false}}},
        {"G4 buy chassis 500 then second buy is AlreadyOwned",
         5000,
         {},
         {{true, chassis500, ShopStatus::Bought, 4500, true, true},
          {true, chassis500, ShopStatus::AlreadyOwned, 4500, false, true}}},
        {"G5 sell Devil tire refund 100000 clamps at 999999",
         950000,
         {tireDevil},
         {{false, tireDevil, ShopStatus::Sold, 999999, true, false}}},
        {"G6 zero-price horn buys and sells with no SFX",
         0,
         {},
         {{true, horn0, ShopStatus::Bought, 0, false, true},
          {false, horn0, ShopStatus::Sold, 0, false, false}}},
    };
}

bool runTrace(const Trace& trace, Fnv& transcript, bool& balanced) {
    Purse purse;
    purse.startMoney = trace.startMoney;
    purse.money = trace.startMoney;
    Inventory inv(catalog().size());
    for (const auto& item : trace.preset) inv.setOwned(item, true);
    bool ok = true;
    for (const auto& step : trace.steps) {
        const std::int32_t before = purse.money;
        const ShopResult r = step.buy ? buyItem(purse, inv, step.item) : sellItem(purse, inv, step.item);
        ok = ok && r.status == step.status && purse.money == step.money && r.sfx == step.sfx &&
             inv.owned(step.item) == step.owned;
        if (r.status == ShopStatus::Insufficient || r.status == ShopStatus::AlreadyOwned ||
            r.status == ShopStatus::NotOwned) {
            ok = ok && purse.money == before;
        }
        balanced = balanced && purseBalanced(purse);
        transcript.add(static_cast<std::uint64_t>(r.status), 1);
        transcript.add(static_cast<std::uint64_t>(static_cast<std::uint32_t>(purse.money)), 4);
        transcript.add(r.sfx ? 1 : 0, 1);
        if (gVerbose) {
            std::cerr << trace.name << " money=" << purse.money << " status=" << static_cast<int>(r.status)
                      << " sfx=" << r.sfx << "\n";
        }
    }
    return ok;
}

// ---------------------------------------------------------------------------
// Balance invariant over seeded random operation sequences.
//   (I1) money == startMoney + credited - spent - clamped and 0 <= money <= cap
//        after every operation (purseBalanced).
//   (I2) for every catalog item, owned == buys - sells and owned in {0,1}
//        (checked against an independent tracker).
//   (I3) refused operations (insufficient / already owned / not owned) leave
//        money and ownership unchanged.
// ---------------------------------------------------------------------------
struct Xorshift {
    std::uint64_t s;
    std::uint64_t next() {
        s ^= s >> 12;
        s ^= s << 25;
        s ^= s >> 27;
        return s * 0x2545F4914F6CDD1DULL;
    }
};

struct InvariantResult {
    int runs = 0;
    std::uint64_t ops = 0;
    std::uint64_t violations = 0;
};

InvariantResult runInvariant(std::uint64_t seed, int sequences, int opsPerSequence, Fnv& transcript) {
    InvariantResult res;
    Xorshift rng{seed};
    const auto& cat = catalog();
    for (int seq = 0; seq < sequences; ++seq) {
        Purse purse;
        purse.startMoney = static_cast<std::int32_t>(rng.next() % (kMoneyCap + 1));
        purse.money = purse.startMoney;
        Inventory inv(cat.size());
        std::vector<int> tracker(cat.size(), 0);  // buys - sells
        for (int op = 0; op < opsPerSequence; ++op) {
            const std::size_t idx = rng.next() % cat.size();
            const bool buy = (rng.next() & 1) != 0;
            const ItemRef item = cat[idx].item;
            const std::int32_t before = purse.money;
            const bool ownedBefore = inv.owned(item);
            const ShopResult r = buy ? buyItem(purse, inv, item) : sellItem(purse, inv, item);
            ++res.ops;
            if (r.status == ShopStatus::Bought) tracker[idx] += 1;
            if (r.status == ShopStatus::Sold) tracker[idx] -= 1;
            const bool refused = r.status == ShopStatus::Insufficient ||
                                 r.status == ShopStatus::AlreadyOwned ||
                                 r.status == ShopStatus::NotOwned;
            if (!purseBalanced(purse)) ++res.violations;                         // I1
            if (refused && (purse.money != before || inv.owned(item) != ownedBefore)) {
                ++res.violations;                                                // I3
            }
            if (inv.owned(item) != (tracker[idx] == 1) || tracker[idx] < 0 || tracker[idx] > 1) {
                ++res.violations;                                                // I2
            }
            transcript.add(static_cast<std::uint64_t>(r.status), 1);
            transcript.add(static_cast<std::uint64_t>(static_cast<std::uint32_t>(purse.money)), 4);
        }
        std::size_t owned = 0;
        for (int t : tracker) owned += t == 1 ? 1 : 0;
        if (owned != inv.ownedCount()) ++res.violations;                         // I2 (count)
        ++res.runs;
    }
    return res;
}

}  // namespace

int main(int argc, char** argv) {
    std::string elfArg;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--verbose") {
            gVerbose = true;
        } else if (a == "--elf" && i + 1 < argc) {
            elfArg = argv[++i];
        }
    }
    std::string elfPath;
    if (const char* e = std::getenv("RT_ELF")) {
        elfPath = e;
    } else if (!elfArg.empty()) {
        elfPath = elfArg;
    } else if (const char* home = std::getenv("HOME")) {
        elfPath = std::string(home) + "/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
    }
    std::optional<Elf> elf;
    if (!elfPath.empty()) elf = Elf::open(elfPath);

    const std::size_t kPinTotal = 125;
    auto runSuite = [&](PinCount& pins, Fnv& transcript, int& tracesOk, bool& balanced,
                        InvariantResult& inv) {
        if (elf) pinElf(*elf, pins);
        tracesOk = 0;
        balanced = true;
        for (const auto& t : goldenTraces()) {
            if (runTrace(t, transcript, balanced)) ++tracesOk;
        }
        inv = runInvariant(0x9E3779B97F4A7C15ULL, 1000, 200, transcript);
    };

    PinCount pins1, pins2;
    Fnv t1, t2;
    int tr1 = 0, tr2 = 0;
    bool bal1 = true, bal2 = true;
    InvariantResult inv1, inv2;
    runSuite(pins1, t1, tr1, bal1, inv1);
    runSuite(pins2, t2, tr2, bal2, inv2);
    const bool deterministic = t1.h == t2.h && pins1.ok == pins2.ok && tr1 == tr2 &&
                               inv1.violations == inv2.violations;

    const int pinsOk = elf ? pins1.ok : 0;
    const int pinsTotal = elf ? pins1.total : 0;
    if (!elf) {
        std::cout << "NO_ELF (" << (elfPath.empty() ? "no path" : elfPath)
                  << "): fixture tables only; ELF pins not evaluated\n";
    }
    const std::size_t catalogSize = catalog().size();
    std::cout << "W2-06 SCORE pins=" << pinsOk << "/" << (elf ? pinsTotal : static_cast<int>(kPinTotal))
              << " traces=" << tr1 << "/" << goldenTraces().size()
              << " invariant_runs=" << inv1.runs << " invariant_violations=" << inv1.violations
              << " catalog=" << catalogSize << " unbounded_categories=" << unboundedCategories().size()
              << " deterministic=" << (deterministic ? 1 : 0) << "\n";

    if (!elf) return 0;
    const bool pass = pinsOk == pinsTotal && tr1 == static_cast<int>(goldenTraces().size()) &&
                      inv1.violations == 0 && bal1 && bal2 && deterministic;
    return pass ? 0 : 1;
}
