#include "gameplay/script_vm.h"

namespace rt::native::gameplay {

// Jump table 0x2EC750 (29 words, ELF-verified): case vaddrs below are the
// table words; ops 9, 10, 12 alias the unknown-op stub 0x23B0E0.
// Widths/branch operands: disassembly of 0x23AD50..0x23B0DC (see script-vm.md).
const ScriptOpInfo kScriptOps[kScriptOpCount] = {
    {0x01, "TestFlagBitFc98", 3, 0x04, ScriptOpClass::Branch, 0x23AD50,
     "u8 flag,u8 idx: branch table[idx] if flag set, else skip 3"},
    {0x02, "TestFlagBitFc98", 3, 0x04, ScriptOpClass::Branch, 0x23AD70,
     "u8 flag,u8 idx: branch table[idx] if flag CLEAR, else skip 3"},
    {0x03, "GetFlaggedSlotCount", 4, 0x08, ScriptOpClass::Branch, 0x23AD90,
     "u8 slot,u8 bit,u8 idx: count(slot,bit,0)!=0 -> branch, else skip 4"},
    {0x04, "TestStampFlag", 2, 0x02, ScriptOpClass::Branch, 0x23ADA8,
     "u8 idx: TestStampFlag(shop.town,shop.shop) -> branch, else skip 2"},
    {0x05, "TestFlagFc68", 3, 0x04, ScriptOpClass::Branch, 0x23ADC8,
     "u8 id,u8 idx: true -> branch, else skip 3"},
    {0x06, "TestStateBitFc78", 3, 0x04, ScriptOpClass::Branch, 0x23ADE8,
     "u8 bit,u8 idx: true -> branch, else skip 3"},
    {0x07, "CheckSlotPresence", 2, 0x02, ScriptOpClass::Branch, 0x23AE08,
     "u8 idx: CheckSlotPresence()!=0 -> branch, else skip 2"},
    {0x08, "MatchSlotPairState/TestFlagFcb8", 3, 0x06, ScriptOpClass::Branch, 0x23AE20,
     "u8 idxA,u8 idxB: Match(town,shop)!=0 -> table[idxA]; elif Fcb8(town,shop)!=0 -> skip 3; else table[idxB]"},
    {0x09, "unknown-stub", 0, 0x00, ScriptOpClass::Unknown, 0x23B0E0, "alias of 0x23B0E0 (report + skip)"},
    {0x0A, "unknown-stub", 0, 0x00, ScriptOpClass::Unknown, 0x23B0E0, "alias of 0x23B0E0 (report + skip)"},
    {0x0B, "GetOrSetPresidentRaceResult", 3, 0x04, ScriptOpClass::Branch, 0x23AEC0,
     "u8 result,u8 idx: Get(-1)==result -> branch, else skip 3"},
    {0x0C, "unknown-stub", 0, 0x00, ScriptOpClass::Unknown, 0x23B0E0, "alias of 0x23B0E0 (report + skip)"},
    {0x0D, "TestFlagFca8Clear", 3, 0x04, ScriptOpClass::Branch, 0x23AEE0,
     "u8 id,u8 idx: bit clear(true) -> branch, else skip 3"},
    {0x0E, "TestFlagBitFc98(0x50)/FindCurrentTaskIndex", 7, 0x7E, ScriptOpClass::Redirect, 0x23AF00,
     "6 u8 idx[taskIndex]: flag 0x50 set -> cursor=table[idx[FindCurrentTaskIndex()]]; else skip 7"},
    {0x0F, "SetFlagBitFc98", 2, 0x00, ScriptOpClass::Effect, 0x23AFF8, "u8 id: set bit"},
    {0x10, "ClearFlagBitFc98", 2, 0x00, ScriptOpClass::Effect, 0x23B010, "u8 id: clear bit"},
    {0x11, "ClearFlaggedSlotBit", 3, 0x00, ScriptOpClass::Effect, 0x23B028,
     "u8 slot,u8 bit: ClearFlaggedSlotBit(slot,bit,0)"},
    {0x12, "task-result-10", 1, 0x00, ScriptOpClass::Effect, 0x23B048,
     "no operands: parent.done=1, parent.result=10"},
    {0x13, "RecordTaskResult", 2, 0x00, ScriptOpClass::Effect, 0x23B060, "u8 code: RecordTaskResult(code)"},
    {0x14, "ReleaseStampFlag+warp", 3, 0x00, ScriptOpClass::Effect, 0x23B078,
     "u8 town,u8 shop: parent.done=1,result=12; ReleaseStampFlag(shop); shop.town=a; shop.shop=b"},
    {0x15, "SetStateBitFc88", 2, 0x00, ScriptOpClass::Effect, 0x23B0B0, "u8 id: set state bit"},
    {0x16, "TestProgressFlagForPair", 2, 0x02, ScriptOpClass::Branch, 0x23AE58,
     "u8 idx: pair progress bit set -> branch, else skip 2"},
    {0x17, "SetProgressFlagAndQueue", 2, 0x00, ScriptOpClass::Effect, 0x23B0C8, "u8 id: set progress flag + queue"},
    {0x18, "money-range-gate", 2, 0x02, ScriptOpClass::Branch, 0x23AE78,
     "u8 idx: 54000<=shop.word14<=198000 -> skip 2; else table[idx]"},
    {0x19, "TestProgressFlag", 3, 0x04, ScriptOpClass::Branch, 0x23AEA8,
     "u8 bit,u8 idx: set -> branch, else skip 3"},
    {0x1A, "CountBitsFca8Fcb0", 3, 0x04, ScriptOpClass::Branch, 0x23AF38,
     "u8 n,u8 idx: n <= 100-CountBits(0) -> branch, else skip 3"},
    {0x1B, "licence/president-gate", 2, 0x02, ScriptOpClass::Branch, 0x23AF60,
     "u8 idx: licence!=3 -> skip 2; else state100 set -> branch; else flag 0x57 set -> skip 2; else branch"},
    {0x1C, "shop.town==id", 3, 0x04, ScriptOpClass::Branch, 0x23AFA0,
     "u8 id,u8 idx: (s8)shop.town==id -> branch, else skip 3"},
    {0x1D, "TestObjectAttribute", 4, 0x08, ScriptOpClass::Branch, 0x23AFC0,
     "u8 a,u8 b,u8 idx: TestObjectAttribute(0x177F760,a,b) -> branch, else skip 4"},
};

const ScriptOpInfo* scriptOpInfo(std::uint8_t op) {
    return (op >= 1 && op <= kScriptOpCount) ? &kScriptOps[op - 1] : nullptr;
}

bool scriptOpIsLive(std::uint8_t op) {
    const ScriptOpInfo* info = scriptOpInfo(op);
    return info != nullptr && info->kind != ScriptOpClass::Unknown;
}

bool scriptTableTarget(const IScriptMemory& mem, const ScriptTask& task, std::uint8_t idx,
                       std::uint32_t& out) {
    return mem.readU32(task.tableBase + static_cast<std::uint32_t>(idx) * 4, out);
}

bool beginScriptTable(const IScriptMemory& mem, std::uint32_t tableVaddr, ScriptTask& task,
                      std::uint32_t& cursorOut) {
    task.tableBase = tableVaddr;
    return mem.readU32(tableVaddr + 4, cursorOut);
}

ScriptRun runScriptPrefix(const IScriptMemory& mem, IScriptHost& host, ScriptTask& task,
                          ScriptShop& shop, std::uint32_t cursor, std::uint32_t maxOps) {
    ScriptRun r;
    std::uint32_t p = cursor;
    bool bad = false;

    const auto finish = [&](ScriptStatus status) {
        r.status = status;
        r.cursor = p;
        return r;
    };

    for (;;) {
        std::uint8_t op = 0;
        if (!mem.readU8(p, op)) return finish(ScriptStatus::BadRead);
        if (op >= kScriptPrefixEnd) return finish(ScriptStatus::Ok);
        if (r.opsExecuted >= maxOps) return finish(ScriptStatus::StepLimit);
        ++r.opsExecuted;

        const auto arg = [&](std::uint32_t k) -> std::uint8_t {
            std::uint8_t v = 0;
            if (!mem.readU8(p + k, v)) bad = true;
            return v;
        };
        const auto jump = [&](std::uint8_t idx) {
            std::uint32_t t = 0;
            if (!scriptTableTarget(mem, task, idx, t)) {
                bad = true;
                return;
            }
            p = t;
            ++r.branchesTaken;
        };
        // taken -> table[arg(idxOff)], otherwise fall through `width` bytes.
        const auto branchIf = [&](bool taken, std::uint32_t idxOff, std::uint32_t width) {
            if (taken) {
                jump(arg(idxOff));
            } else {
                p += width;
            }
        };

        switch (op) {
        case 0x01: branchIf(host.testFlagBitFc98(arg(1)) != 0, 2, 3); break;
        case 0x02: branchIf(host.testFlagBitFc98(arg(1)) == 0, 2, 3); break;
        case 0x03: branchIf(host.getFlaggedSlotCount(arg(1), arg(2), 0) != 0, 3, 4); break;
        case 0x04: branchIf(host.testStampFlag(shop.town, shop.shop) != 0, 1, 2); break;
        case 0x05: branchIf(host.testFlagFc68(arg(1)) != 0, 2, 3); break;
        case 0x06: branchIf(host.testStateBitFc78(arg(1)) != 0, 2, 3); break;
        case 0x07: branchIf(host.checkSlotPresence() != 0, 1, 2); break;
        case 0x08:
            if (host.matchSlotPairState(shop.town, shop.shop) != 0) {
                jump(arg(1));
            } else if (host.testFlagFcb8(shop.town, shop.shop) != 0) {
                p += 3;
            } else {
                jump(arg(2));
            }
            break;
        case 0x0B:
            branchIf(host.getOrSetPresidentRaceResult(-1) == static_cast<std::int64_t>(arg(1)), 2, 3);
            break;
        case 0x0D: branchIf(host.testFlagFca8Clear(arg(1)) != 0, 2, 3); break;
        case 0x0E:
            if (host.testFlagBitFc98(kScriptFlagTaskReload) == 0) {
                p += 7;
            } else {
                const std::int32_t taskIdx = host.findCurrentTaskIndex();
                std::uint8_t idx = 0;
                if (!mem.readU8(p + 1 + static_cast<std::uint32_t>(taskIdx), idx)) {
                    bad = true;
                } else {
                    jump(idx);
                }
            }
            break;
        case 0x0F: host.setFlagBitFc98(arg(1)); p += 2; break;
        case 0x10: host.clearFlagBitFc98(arg(1)); p += 2; break;
        case 0x11: host.clearFlaggedSlotBit(arg(1), arg(2), 0); p += 3; break;
        case 0x12:
            task.parentDone = 1;
            task.parentResult = kScriptResultOp12;
            p += 1;
            break;
        case 0x13: host.recordTaskResult(arg(1)); p += 2; break;
        case 0x14: {
            task.parentDone = 1;
            task.parentResult = kScriptResultOp14;
            host.releaseStampFlag(shop);
            const std::uint8_t town = arg(1);
            const std::uint8_t slot = arg(2);
            shop.town = static_cast<std::int8_t>(town);
            shop.shop = slot;
            p += 3;
            break;
        }
        case 0x15: host.setStateBitFc88(arg(1)); p += 2; break;
        case 0x16: branchIf(host.testProgressFlagForPair(shop.town, shop.shop) != 0, 1, 2); break;
        case 0x17: host.setProgressFlagAndQueue(arg(1)); p += 2; break;
        case 0x18: {
            const std::uint32_t d = static_cast<std::uint32_t>(shop.word14) - kScriptMoneyGateBase;
            branchIf(!(d < kScriptMoneyGateSpan), 1, 2);
            break;
        }
        case 0x19: branchIf(host.testProgressFlag(arg(1)) != 0, 2, 3); break;
        case 0x1A:
            branchIf(static_cast<std::int32_t>(arg(1)) <=
                         kScriptCountCeiling - host.countBitsFca8Fcb0(0),
                     2, 3);
            break;
        case 0x1B:
            if (host.licenceByte() != kScriptLicenceGate) {
                p += 2;
            } else if (host.testStateBitFc78(kScriptStateBitPresident) != 0) {
                jump(arg(1));
            } else if (host.testFlagBitFc98(kScriptFlagPresident) != 0) {
                p += 2;
            } else {
                jump(arg(1));
            }
            break;
        case 0x1C:
            branchIf(static_cast<std::int32_t>(shop.town) == static_cast<std::int32_t>(arg(1)), 2, 3);
            break;
        case 0x1D:
            branchIf(host.testObjectAttribute(kScriptProgressObjVaddr, arg(1), arg(2)) != 0, 3, 4);
            break;
        default: {
            // 0x23B0E0: report, then skip. `p` is the opcode here (the
            // original has already advanced s0 by one).
            ++r.unknownHits;
            host.onUnknownPreControl(op, shop.townLabel, shop.shop);
            std::uint32_t q = p + 1;
            std::uint8_t b = 0;
            if (!mem.readU8(q, b)) {
                bad = true;
                break;
            }
            if (b < kScriptPrefixEnd) {
                q = p + 2;
                for (;;) {
                    if (!mem.readU8(q, b)) {
                        bad = true;
                        break;
                    }
                    if (b >= kScriptPrefixEnd) break;
                    ++q;
                }
                if (bad) break;
            }
            host.onUnknownSkipped(q);
            p = q;
            break;
        }
        }
        if (bad) return finish(ScriptStatus::BadRead);
    }
}

ScriptPrefix decodeScriptPrefix(const IScriptMemory& mem, std::uint32_t cursor,
                                std::uint32_t tableLength, bool nextTableAdjacent) {
    ScriptPrefix out;
    std::uint32_t p = cursor;
    for (;;) {
        std::uint8_t op = 0;
        if (!mem.readU8(p, op)) {
            out.status = ScriptPrefixStatus::BadRead;
            out.endCursor = p;
            return out;
        }
        out.endCursor = p;
        out.endByte = op;
        if (op >= kScriptPrefixEnd) {
            out.status = ScriptPrefixStatus::Clean;
            return out;
        }
        if (!scriptOpIsLive(op)) {
            out.status = ScriptPrefixStatus::UnknownOp;
            out.unknownOp = op;
            return out;
        }
        const ScriptOpInfo& info = kScriptOps[op - 1];
        out.ops.push_back({op, p});
        for (std::uint32_t k = 1; k <= 6; ++k) {
            if ((info.branchMask & (1u << k)) == 0) continue;
            std::uint8_t idx = 0;
            if (!mem.readU8(p + k, idx)) {
                out.status = ScriptPrefixStatus::BadRead;
                return out;
            }
            ++out.branchOperands;
            out.branchIdx.push_back(idx);
            if (idx >= tableLength) {
                if (idx == tableLength && nextTableAdjacent) {
                    ++out.bleed;
                } else {
                    ++out.oob;
                }
            }
        }
        p += info.width;
    }
}

}  // namespace rt::native::gameplay
