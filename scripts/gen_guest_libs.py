#!/usr/bin/env python3
"""W12: regenerate src/native/guest/guest_libs.cpp (+ report hand sigs).

The oracle recompiler replaces 192 guest library functions with host
implementations (config/roadtrip.toml [general].stubs -> mirrored list in
config/native_host_stubs.txt). The native build mirrors that split: the gen
TU stays on disk for coverage, and a host body serves the symbol.

Authoritative name for an address = the gen TU's RT_GUEST_FN line (may carry
a __address disambiguation suffix). Authoritative signature = the exact
build/decomp/guest_decls.inc line for that name (what every caller TU
compiles against; it flattens some Ghidra u64 folds to positional u32s),
falling back to the normalized gen TU def when the .inc has no such line
(suffixed names are not declared in the .inc).

Functions whose host bodies are hand-written (HAND below, in
guest/guest_libs_sif.cpp) are skipped; every other stub gets a pending body
(log once + return 0) until its ps2xRuntime semantics are ported in later
W12 waves (cdvd/fs/mc/pad/sd/gs/dma/vpu/newlib...). Each stub keeps its
function-table registration so indirect guest calls reach the host body too.

Usage: python3 scripts/gen_guest_libs.py [--print-hand-defs]
"""
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
CFG = ROOT / "config/native_host_stubs.txt"
INC = (ROOT / "build/decomp/guest_decls.inc").read_text()
OUT = ROOT / "src/native/guest/guest_libs.cpp"
GEN = ROOT / "src/native/guest/gen"

# Host bodies hand-written in guest/guest_libs_sif.cpp (wave 1: SIF/RPC/
# libload/libkernel helpers + the newlib mem/str/compiler-rt thin ports).
HAND = {
    "sceSifInitCmd", "sceSifExitCmd", "sceSifAddCmdHandler", "sceSifRemoveCmdHandler",
    "_sceSifSendCmd", "sceSifSendCmd", "sceSifWriteBackDCache",
    "sceSifInitRpc", "sceSifGetOtherData", "sceSifBindRpc", "sceSifCallRpc",
    "sceSifCheckStatRpc", "sceSifSetRpcQueue", "sceSifRegisterRpc", "sceSifRemoveRpc",
    "sceSifRemoveRpcQueue", "sceSifGetNextRequest", "sceSifExecRequest", "sceSifRpcLoop",
    "sceSifInitIopHeap", "sceSifAllocIopHeap", "sceSifAllocSysMemory", "sceSifLoadIopHeap",
    "sceSifLoadFileReset", "_sceSifLoadModuleBuffer", "_sceSifLoadModule",
    "sceSifGetIopAddr", "sceSifSetIopAddr", "sceSifResetIop", "sceSifIsAliveIop",
    "sceSifSyncIop", "sceSifRebootIop",
    "InitThread", "InitAlarm", "iWakeupThread", "iRotateThreadReadyQueue",
    "memcmp", "memcpy", "memmove", "memset", "strcat", "strcmp", "strcpy",
    "strlen", "strncpy", "__divdi3",
}
# Wave 2 (guest_libs_pad.cpp): the libpad family.
HAND |= {
    "scePadInit", "scePadInit2", "scePadEnd", "scePadPortOpen", "scePadPortClose",
    "scePadGetDmaStr", "scePadGetFrameCount", "scePadRead", "scePadGetState",
    "scePadStateIntToStr", "scePadSetReqState", "scePadGetReqState", "scePadInfoAct",
    "scePadInfoComb", "scePadInfoMode", "scePadSetMainMode", "scePadSetActDirect",
    "scePadSetActAlign", "scePadGetButtonMask", "scePadSetButtonInfo",
    "scePadInfoPressMode", "scePadEnterPressMode", "scePadExitPressMode",
    "scePadSetVrefParam", "scePadGetPortMax", "scePadGetSlotMax",
    "scePadGetModVersion", "scePadSetWarningLevel",
}
# Wave 3 (guest_libs_dma.cpp): libdma send/sync/register model.
HAND |= {
    "sceDmaGetChan", "sceDmaReset", "sceDmaPutEnv", "sceDmaGetEnv",
    "sceDmaPutStallAddr", "sceDmaSend", "sceDmaSendI", "sceDmaSendN",
    "sceDmaSync",
}
# Wave 4 (guest_libs_cd.cpp / guest_libs_fs.cpp): libcdvd + libfs host bodies.
HAND |= {
    "sceCdDelayThread", "sceCdCallback", "sceCdInitEeCB", "sceCdSearchFile",
    "sceCdNcmdDiskReady", "sceCdSync", "sceCdSyncS", "sceCdInit", "sceCdMmode",
    "sceCdRead", "sceCdGetError", "sceCdBreak", "sceSdTransToIOP",
    "_sceFsSemInit", "sceFsInit", "sceFsReset", "sceOpen", "sceClose",
    "sceLseek", "sceRead", "sceWrite", "sceIoctl",
}
# Wave 5 (guest_libs_gs.cpp): libgraph state bodies + vblank park.
HAND |= {
    "sceGsResetGraph", "sceGsResetPath", "sceGsSyncV", "sceGsSyncPath",
    "sceGsSetDefLoadImage", "sceGsSetDefStoreImage", "sceGsExecLoadImage",
    "sceGsExecStoreImage", "sceGsSetDefDBuffDc", "sceGsSwapDBuffDc",
    "sceGsSetDefDispEnv", "sceGsPutDispEnv", "sceGszbufaddr",
    "sceGsSetDefDrawEnv", "sceGsSetDefClear", "sceGsPutDrawEnv",
    "sceGsSetDefDrawEnv2",
}
# Wave 6 (guest_libs_mc.cpp): libmc host-emulated memory card.
HAND |= {
    "sceMcInit", "sceMcChangeThreadPriority", "sceMcGetSlotMax", "sceMcOpen",
    "sceMcMkdir", "sceMcClose", "sceMcSeek", "mceIntrReadFixAlign", "sceMcRead",
    "sceMcWrite", "sceMcSync", "mceGetInfoApdx", "sceMcGetInfo", "sceMcGetDir",
    "mceStorePwd", "sceMcChdir", "sceMcFormat", "sceMcDelete", "sceMcFlush",
    "sceMcSetFileInfo", "sceMcRename", "sceMcUnformat", "sceMcGetEntSpace",
}


def def_name(addr: str) -> str:
    """Name the linker sees for this address: its gen TU's registration."""
    text = (GEN / f"{addr}.cpp").read_text()
    m = re.search(rf"RT_GUEST_FN\(0x{addr}, guest::(\w+)\)", text)
    if not m:
        raise SystemExit(f"RT_GUEST_FN line not found @ {addr}")
    return m.group(1)


def signature(addr: str, name: str) -> str:
    """Normalized '<ret> <name>(<types>)' — .inc line wins over the gen def."""
    m = re.search(rf"^([A-Za-z0-9_]+) {re.escape(name)}\((.*)\);$", INC, re.M)
    if m:
        params = ", ".join(p.split(" =")[0].strip() for p in m.group(2).split(","))
        return f"{m.group(1)} {name}({params})"
    text = (GEN / f"{addr}.cpp").read_text()
    d = re.search(rf"^[A-Za-z0-9_]+ {re.escape(name)}\([^)]*\) \{{$", text, re.M)
    if not d:
        raise SystemExit(f"no .inc decl and no gen def for {name} @ {addr}")
    return re.sub(r"\s+", " ", d.group(0))[:-2]


def main() -> None:
    entries = []
    for line in CFG.read_text().splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        addr, name = line.split()
        entries.append((addr, name))

    hand_lines = []
    generating = "--print-hand-defs" not in sys.argv
    out = []
    if generating:
        out = [
            "// GENERATED by scripts/gen_guest_libs.py - DO NOT hand-edit.",
            "// Pending host bodies for the oracle-stubbed guest library",
            "// functions (config/native_host_stubs.txt; names+signatures from",
            "// the gen TU registration + build/decomp/guest_decls.inc). Hand-",
            "// written ports live in guest_libs_sif.cpp (later wave files",
            "// added to HAND there). A pending body logs once and returns 0:",
            "// a port gap (the oracle has a real host body), not TODO_NAMED.",
            "#include <cstdio>",
            "#include <set>",
            "#include <string>",
            '#include "guest_shim.h"',
            '#include "guest_decls.inc"',
            "",
            "namespace guest {",
            "",
            "namespace {",
            "std::set<std::string> g_pending_logged;",
            "}  // namespace",
            "",
            "// Like RT_GUEST_FN but unique per TU (many registrations/file).",
            "#define RT_STUB_FN(addr, name)                                   \\",
            "    namespace { struct RtReg_##addr { RtReg_##addr() {           \\",
            "        rtguest::registerGuest(addr,                             \\",
            "            reinterpret_cast<void*>(&name)); } } rt_reg_##addr##_; }",
            "",
            "static void rtPending(const char* name) {",
            "    if (g_pending_logged.insert(name).second)",
            '        std::fprintf(stderr, "[hostlib] pending host port: %s -> 0\\n", name);',
            "}",
            "",
        ]
    n = 0
    seenHand = set()
    for addr, name in entries:
        dname = def_name(addr)
        sig = signature(addr, dname)
        if name in HAND:
            seenHand.add(name)
            hand_lines.append(sig)
            continue
        ret = sig.split(" ", 1)[0]
        body = "rtPending(\"%s\");" % dname + ("" if ret == "void" else " return 0;")
        out.append(f"{sig} {{ {body} }}")
        out.append(f"RT_STUB_FN(0x{addr.upper()}, guest::{dname})")
        out.append("")
        n += 1
    missing = sorted(HAND - seenHand)
    if missing:
        print(f"WARNING: HAND names not in stub list: {missing}")
    if not generating:
        print("\n".join(hand_lines))
        return
    out.append("}  // namespace guest")
    OUT.write_text("\n".join(out) + "\n")
    print(f"wrote {OUT.relative_to(ROOT)}: {n} pending bodies "
          f"({len(seenHand)} hand-written ports excluded)")


if __name__ == "__main__":
    main()
