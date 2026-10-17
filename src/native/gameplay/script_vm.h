// Host interpreter for the town/shop event-script control prefix: a
// bit-faithful port of InterpretScriptCommands @ 0x0023AD00..0x0023B167
// (docs/native-reference/script-vm.md).  The VM is pure: it reads script
// bytes and branch tables through IScriptMemory, reads/writes the game state
// it touches through IScriptHost (one method per named callee), and mutates
// the two small records the original touches directly (shop record, parent
// task record).  No SDL/GPU/save dependencies; later waves bind IScriptHost to
// SaveData/economy without changing the VM.
//
// Tick semantics (deterministic, no yielding): one call runs the control
// prefix of one message to completion and returns the cursor of the first
// byte >= 0x20 (text follows).  dialogueTask 0x23B168 calls it once per
// message start (decomp 0023b168.c: `*(task+0x28) = Interpret(task, shop,
// *(task+0x28))`), never mid-message.  There is no timer op in this VM.
// The original has no step bound (a branch loop would hang the game);
// runScriptPrefix adds `maxOps` and reports StepLimit instead.
//
// Every constant is grounded to build/decomp/fn/0023ad00.c and to the
// disassembly of 0x23AD00..0x23B167 (re-read and cited in script-vm.md).
#pragma once

#include "assets/script_image.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rt::native::gameplay {

// ---------------------------------------------------------------------------
// Guest memory the VM reads: script bytes and the u32 branch-pointer tables.
// ---------------------------------------------------------------------------
class IScriptMemory {
public:
    virtual ~IScriptMemory() = default;
    virtual bool readU8(std::uint32_t vaddr, std::uint8_t& out) const = 0;
    virtual bool readU32(std::uint32_t vaddr, std::uint32_t& out) const = 0;
};

// Adapter over the ELF-resident image (does not own it).
class ScriptImageMemory final : public IScriptMemory {
public:
    explicit ScriptImageMemory(const assets::ScriptImage& image) : image_(image) {}
    bool readU8(std::uint32_t v, std::uint8_t& o) const override { return image_.readU8(v, o); }
    bool readU32(std::uint32_t v, std::uint32_t& o) const override { return image_.readU32(v, o); }

private:
    const assets::ScriptImage& image_;
};

// ---------------------------------------------------------------------------
// Game-state callees.  Names are the symbols in build/decomp/symbols.tsv; the
// address is the callee's vaddr.  Parameter widths follow the call sites
// (lbu = u8, lb = s8).  Return values the original tests with `!= 0` are
// int64_t (the decomp types them `long`).
// ---------------------------------------------------------------------------
struct ScriptShop;

class IScriptHost {
public:
    virtual ~IScriptHost() = default;

    // Predicates.
    virtual std::int64_t testFlagBitFc98(std::uint8_t id) = 0;                 // 0x23D778
    virtual std::int64_t getFlaggedSlotCount(std::uint8_t slot, std::uint8_t bit,
                                             std::int32_t zero) = 0;           // 0x23D488
    virtual std::int64_t testStampFlag(std::int8_t town, std::uint8_t shop) = 0;   // 0x23D5F8
    virtual std::int64_t testFlagFc68(std::uint8_t id) = 0;                    // 0x23D8E8
    virtual std::int64_t testStateBitFc78(std::uint8_t id) = 0;                // 0x23D628
    virtual std::int64_t checkSlotPresence() = 0;                              // 0x23DBC8
    virtual std::int64_t matchSlotPairState(std::int8_t town, std::uint8_t shop) = 0;  // 0x23DB80
    virtual std::int64_t testFlagFcb8(std::int8_t town, std::uint8_t shop) = 0;        // 0x23DCC0
    virtual std::int64_t testProgressFlagForPair(std::int8_t town, std::uint8_t shop) = 0;  // 0x23DE50
    virtual std::int64_t getOrSetPresidentRaceResult(std::int64_t arg) = 0;    // 0x23D8C8 (op 0xB passes -1)
    virtual std::int64_t testFlagFca8Clear(std::uint8_t id) = 0;               // 0x23DBF0
    virtual std::int64_t testProgressFlag(std::uint8_t bit) = 0;               // 0x23DE30
    virtual std::int32_t countBitsFca8Fcb0(std::int32_t slot) = 0;             // 0x23D968
    virtual std::int64_t testObjectAttribute(std::uint32_t objVaddr, std::uint8_t a,
                                             std::uint8_t b) = 0;              // 0x23D5E0
    virtual std::int32_t findCurrentTaskIndex() = 0;                           // 0x23DDB8
    virtual std::int8_t licenceByte() = 0;  // DAT_0177FDB1 (progress+0x651, save-layout.md "licence")

    // Mutators.
    virtual void setFlagBitFc98(std::uint8_t id) = 0;                          // 0x23D7A8
    virtual void clearFlagBitFc98(std::uint8_t id) = 0;                        // 0x23D7E0
    virtual void clearFlaggedSlotBit(std::uint8_t slot, std::uint8_t bit, std::int32_t zero) = 0;  // 0x23D390
    virtual void recordTaskResult(std::uint8_t code) = 0;                      // 0x23DD30
    virtual void releaseStampFlag(const ScriptShop& shop) = 0;                 // 0x23C7B0 (a0 = shop record; called before op 0x14 overwrites +0x23/+0x21)
    virtual void setStateBitFc88(std::uint8_t id) = 0;                         // 0x23D730
    virtual void setProgressFlagAndQueue(std::uint8_t id) = 0;                 // 0x23CB68

    // printf("Unknown pre-control code %d in town T%02d shop %d\n", code,
    // shop+0x22, shop+0x21) @0x23B0E0..0x23B0F4.
    virtual void onUnknownPreControl(std::uint8_t code, std::int8_t townLabel,
                                     std::uint8_t shop) = 0;
    // Second printf of the unknown path (format pointer at 0x333928) with the
    // post-skip cursor as argument, @0x23B130..0x23B138.
    virtual void onUnknownSkipped(std::uint32_t cursorAfterSkip) = 0;
};

// ---------------------------------------------------------------------------
// The two records the interpreter touches directly.
// ---------------------------------------------------------------------------
// Shop/exploration record = `param_2` of InterpretScriptCommands.
struct ScriptShop {
    std::int32_t word14 = 0;     // +0x14: op 0x18 money-range gate (meaning OPEN)
    std::uint8_t shop = 0;       // +0x21: shop slot, index into the level-1 table
    std::int8_t townLabel = 0;   // +0x22: printed as "T%02d" in the unknown-op report
    std::int8_t town = 0;        // +0x23: town index into 0x2A7ED0 (spawners use (s8) of it)
};

// Dialogue task record (`param_1`) + the parent task it reports to (+0x18).
struct ScriptTask {
    std::uint32_t tableBase = 0;   // +0x20: current level-2 table (branch base)
    std::uint8_t parentDone = 0;   // parent+0x24, set to 1 by ops 0x12/0x14
    std::int32_t parentResult = 0; // parent+0x04, set to 10 (op 0x12) / 12 (op 0x14)
};

// Result codes written by ops 0x12 / 0x14 (0x23B048..0x23B05C, 0x23B078..0x23B090).
inline constexpr std::int32_t kScriptResultOp12 = 10;
inline constexpr std::int32_t kScriptResultOp14 = 12;
// Op 0x18: continue when (u32)(word14 - 54000) < 0x23281, else branch
// (0x23AE78..0x23AE94: addu -0xD2F0; sltu 0x23280 < a0).
inline constexpr std::uint32_t kScriptMoneyGateBase = 54000;
inline constexpr std::uint32_t kScriptMoneyGateSpan = 0x23281;
// Op 0x1D reads the progress record at this vaddr (lui 0x178; addiu -0x8A0).
inline constexpr std::uint32_t kScriptProgressObjVaddr = 0x0177F760;
// Op 0x0E tests this flag (jal 0x23D778, a0 = 0x50) then indexes by task idx.
inline constexpr std::uint8_t kScriptFlagTaskReload = 0x50;
// Op 0x1B: licence byte value that opens the president gate; state bit; flag.
inline constexpr std::int8_t kScriptLicenceGate = 3;
inline constexpr std::uint8_t kScriptStateBitPresident = 100;
inline constexpr std::uint8_t kScriptFlagPresident = 0x57;
// Op 0x1A: n <= 100 - countBits(0).
inline constexpr std::int32_t kScriptCountCeiling = 100;
// Control prefix ends at the first byte >= this (0x23B140 sltiu 0x20).
inline constexpr std::uint8_t kScriptPrefixEnd = 0x20;

// ---------------------------------------------------------------------------
// Op table (op -> width/behaviour/grounding).  Index = op - 1, ops 1..29.
// ---------------------------------------------------------------------------
enum class ScriptOpClass : std::uint8_t {
    Branch,    // conditional table branch (taken -> table[idx], else skip)
    Effect,    // host side effect, then continue (fall through)
    Redirect,  // 0x0E: flag-gated reload through a per-task index table
    Unknown,   // ops 9/10/12: alias of the unknown-op stub 0x23B0E0
};

struct ScriptOpInfo {
    std::uint8_t op;
    const char* name;        // host callee / behaviour
    std::uint8_t width;      // bytes incl. opcode on the fall-through path (0 = unknown stub)
    std::uint8_t branchMask; // bit k (1..6) set -> byte at opc+k is a table index
    ScriptOpClass kind;
    std::uint32_t caseVaddr; // jump-table target (0x2EC750 + (op-1)*4)
    const char* behaviour;
};

inline constexpr std::size_t kScriptOpCount = 29;
extern const ScriptOpInfo kScriptOps[kScriptOpCount];

// nullptr for op 0, ops >= 0x1E.
const ScriptOpInfo* scriptOpInfo(std::uint8_t op);
// True for the 26 live handlers (1..8, 0xB, 0xD..0x1D); false for 0, 9, 10, 12, >= 0x1E.
bool scriptOpIsLive(std::uint8_t op);

// ---------------------------------------------------------------------------
// Interpreter.
// ---------------------------------------------------------------------------
enum class ScriptStatus : std::uint8_t {
    Ok,         // stopped on a byte >= 0x20
    BadRead,    // cursor/table read outside guest memory (not reachable in-game)
    StepLimit,  // maxOps exceeded (branch loop; original would hang)
};

struct ScriptRun {
    ScriptStatus status = ScriptStatus::Ok;
    std::uint32_t cursor = 0;        // returned script pointer
    std::uint32_t opsExecuted = 0;   // live ops + unknown-op skips
    std::uint32_t branchesTaken = 0; // table redirects (incl. op 0x0E reload)
    std::uint32_t unknownHits = 0;   // default-path entries (ops 0, 9, 10, 12, 0x1E, 0x1F)
};

inline constexpr std::uint32_t kScriptDefaultMaxOps = 1u << 16;

ScriptRun runScriptPrefix(const IScriptMemory& mem, IScriptHost& host, ScriptTask& task,
                          ScriptShop& shop, std::uint32_t cursor,
                          std::uint32_t maxOps = kScriptDefaultMaxOps);

// dialogueTask frame-0 setup (0023b168.c:34-39): +0x20 = +0x1c (the level-2
// table), cursor = table[1].  Returns false on a bad read.
bool beginScriptTable(const IScriptMemory& mem, std::uint32_t tableVaddr, ScriptTask& task,
                      std::uint32_t& cursorOut);

// Escape-handler style jump: cursor = table[idx] (0023b168.c:0x23b878 etc.).
bool scriptTableTarget(const IScriptMemory& mem, const ScriptTask& task, std::uint8_t idx,
                       std::uint32_t& out);

// ---------------------------------------------------------------------------
// Static prefix decode (no host): follow the fall-through path of the prefix.
// ---------------------------------------------------------------------------
struct ScriptPrefixOp {
    std::uint8_t op = 0;
    std::uint32_t at = 0;
};

enum class ScriptPrefixStatus : std::uint8_t {
    Clean,      // ended on a byte >= 0x20 with only live ops
    UnknownOp,  // hit op 0, 9, 10, 12, 0x1E or 0x1F
    BadRead,
};

struct ScriptPrefix {
    ScriptPrefixStatus status = ScriptPrefixStatus::Clean;
    std::uint32_t endCursor = 0;   // first byte >= 0x20 (Clean) or the offending opcode
    std::uint8_t endByte = 0;
    std::uint8_t unknownOp = 0;
    std::vector<ScriptPrefixOp> ops;
    // Branch operands seen (all table indices named by the prefix ops, incl.
    // every one of op 0x0E's six per-task entries); `oob` counts those
    // >= tableLength, `bleed` those == tableLength where the table is
    // directly followed by another table (reads that table's name word).
    std::uint32_t branchOperands = 0;
    std::uint32_t oob = 0;
    std::uint32_t bleed = 0;
    std::vector<std::uint8_t> branchIdx;  // every operand, in decode order
};

ScriptPrefix decodeScriptPrefix(const IScriptMemory& mem, std::uint32_t cursor,
                                std::uint32_t tableLength, bool nextTableAdjacent);

}  // namespace rt::native::gameplay
