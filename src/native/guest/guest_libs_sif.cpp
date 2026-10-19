// W12 host services, wave 1 (SIF / RPC / libload / libkernel helpers / libc
// thin ports). These mirror the ORACLE's host implementations of the guest
// library functions the recomp stubs out (config/native_host_stubs.txt) —
// semantics ported from ps2xRuntime Kernel/Stubs/SIF.cpp, Syscalls/RPC.cpp,
// Syscalls/System.cpp, Syscalls/Thread.cpp, Syscalls/Sync.cpp, Stubs/LibC.cpp.
// Nothing here is compiled or linked from ps2xRuntime itself (decomp hard
// rule): each body is authored against rtguest::kRam + the kernel ops.
//
// The oracle answers SIF at the host-RPC layer (no guest libsd protocol runs
// there): BindRpc publishes a dummy server descriptor for unknown sids and
// CallRpc dispatches synchronously, falling back to copy/zero of the receive
// buffer for unanswered services. Native mirrors that; real IOP-CPU behavior
// is out of scope — the (sid,fno) service checklist (W12 "51 pairs") fills
// answers where the game needs them.
//
// Deviation ledger: docs/native-reference/w12-kernel-design.md.
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <vector>
#include "guest_kernel.h"
#include "guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// Unique-per-file registration (RT_GUEST_FN's struct name would collide).
#define RT_STUB_FN(addr, name)                                       \
    namespace { struct RtReg_##addr { RtReg_##addr() {               \
        rtguest::registerGuest(addr,                                 \
            reinterpret_cast<void*>(&name)); } } rt_reg_##addr##_; }

namespace {

// ---- host RPC state (mirrors RPC.cpp g_rpc_*; no locks: green threads) ----
struct RpcClient { uint32_t sid = 0; bool busy = false; uint32_t lastRpc = 0; };
std::map<uint32_t, RpcClient> g_rpcClients;      // clientPtr -> state
std::map<uint32_t, uint32_t> g_rpcServers;       // sid -> serverData ptr
uint32_t g_rpcNextId = 1;
bool g_rpcInit = false;
uint32_t g_rpcActiveQueue = 0;
std::map<uint32_t, std::string> g_modulesById;   // module id -> path
uint32_t g_moduleNextId = 1;

// Fake IOP RAM substrate: host services may stage payloads here; no IOP CPU
// is ever run (W12 design). shortcut: unpinned allocation base, nothing
// reads module text here; W13 parity will pin if an answer needs contents.
uint8_t* iopRam() { static std::vector<uint8_t> r(2u << 20); return r.data(); }
uint32_t g_iopHeapBump = 0x00002000;

// ---- RAM access (oracle getMemPtr: null outside physical RAM) ----
void* mem(uint32_t a) {
    const uint32_t p = a & 0x1FFFFFFFu;
    return p < RT_GUEST_RAM_SIZE ? rtguest::kRam + p : nullptr;
}
uint32_t rdW(uint32_t a) {
    uint32_t v = 0;
    std::memcpy(&v, rtguest::kRam + (a & 0x1FFFFFFFu), 4);
    return v;
}
void wrW(uint32_t a, uint32_t v) {
    std::memcpy(rtguest::kRam + (a & 0x1FFFFFFFu), &v, 4);
}

// t_SifRpcClientData / t_SifRpcServerData / t_SifRpcDataQueue field offsets
// (ps2xRuntime Helpers/State.h 41-87; static layouts, no structs over kRam
// to keep host endian/size assumptions explicit).
enum RpcClientOff { kClPkt = 0x00, kClRpcId = 0x04, kClSema = 0x08, kClMode = 0x0C,
    kClCmd = 0x10, kClBuf = 0x14, kClCbuf = 0x18, kClEndFn = 0x1C, kClEndPrm = 0x20,
    kClServer = 0x24 };
enum RpcSdOff { kSdSid = 0x00, kSdFunc = 0x04, kSdBuf = 0x08, kSdSize = 0x0C,
    kSdCfunc = 0x10, kSdCbuf = 0x14, kSdSize2 = 0x18, kSdClient = 0x1C, kSdPkt = 0x20,
    kSdRpcNum = 0x24, kSdRecvBuf = 0x28, kSdRsize = 0x2C, kSdRmode = 0x30, kSdRid = 0x34,
    kSdLink = 0x38, kSdNext = 0x3C, kSdBase = 0x40 };
constexpr uint32_t kSdStructSize = 0x44;
enum RpcQdOff { kQdThread = 0x00, kQdActive = 0x04, kQdLink = 0x08, kQdStart = 0x0C,
    kQdEnd = 0x10, kQdNext = 0x14 };

uint32_t iopAlloc(uint32_t bytes) {
    g_iopHeapBump = (g_iopHeapBump + 63u) & ~63u;
    const uint32_t a = g_iopHeapBump;
    g_iopHeapBump += (bytes + 63u) & ~63u;
    return g_iopHeapBump <= (2u << 20) ? a : 0u;
}

std::string cstrGuest(uint32_t a, uint32_t cap) {
    if (!mem(a)) return {};
    const uint32_t p = a & 0x1FFFFFFFu;
    std::string s;
    for (uint32_t i = 0; i < cap && rtguest::kRam[p + i]; ++i) s += (char)rtguest::kRam[p + i];
    return s;
}

void sifSendCmdLogged(uint32_t cid) {
    static int shown = 0;
    if (shown < 8)
        std::fprintf(stderr, "[hostlib] SifSendCmd cid=0x%x (no IOP: not delivered)\n", cid);
    if (shown < 1000) ++shown;
}

}  // namespace

// ---- SIF cmd layer (Stubs/SIF.cpp: host flag only; regs reseeded on exit) ----
uint32_t sceSifInitCmd(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x00286F98, guest::sceSifInitCmd)

uint32_t sceSifExitCmd(uint32_t q0, ...) { rtguest::kResetSifRegs(); return 0; }
RT_STUB_FN(0x00287218, guest::sceSifExitCmd)

uint32_t sceSifAddCmdHandler(uint32_t cid, uint32_t fn, uint32_t arg, ...) {
    rtguest::kSetCmdHandler(cid, fn, arg);
    return 0;
}
RT_STUB_FN(0x00287280, guest::sceSifAddCmdHandler)

uint32_t sceSifRemoveCmdHandler(uint32_t cid, ...) {
    rtguest::kEraseCmdHandler(cid);
    return 0;
}
RT_STUB_FN(0x002872B0, guest::sceSifRemoveCmdHandler)

uint32_t _sceSifSendCmd(uint32_t cid, uint64_t src, uint64_t size, uint64_t dstFn, uint32_t dstBuf, uint32_t qos64, uint64_t param_7, ...) {
    sifSendCmdLogged(cid);
    return 1;
}
RT_STUB_FN(0x002872D8, guest::_sceSifSendCmd)

uint32_t sceSifSendCmd(uint64_t cid, uint64_t src, uint64_t size, uint64_t dstFn,
                      uint64_t dstBuf, uint64_t qos, ...) {
    sifSendCmdLogged((uint32_t)cid);
    return 1;
}
RT_STUB_FN(0x00287410, guest::sceSifSendCmd)

uint32_t sceSifWriteBackDCache__002875b8(uint32_t param_1, uint32_t param_2, ...) {
    return 0;
}
RT_STUB_FN(0x002875B8, guest::sceSifWriteBackDCache__002875b8)

// ---- SIF RPC (Syscalls/RPC.cpp semantics; sync completion, no IOP CPU) ----
uint32_t sceSifInitRpc(uint32_t q0, ...) {
    if (!g_rpcInit) {
        g_rpcClients.clear(); g_rpcServers.clear();
        g_rpcNextId = 1; g_rpcActiveQueue = 0; g_rpcInit = true;
        std::fprintf(stderr, "[SifInitRpc] Initialized\n");
    }
    return 0;
}
RT_STUB_FN(0x00287668, guest::sceSifInitRpc)

uint32_t sceSifGetOtherData(uint32_t rdAddr, uint32_t srcAddr, uint32_t dstAddr,
                            uint32_t size, uint64_t param_5, ...) {
    if ((int32_t)size <= 0) return 0;
    if (size >= RT_GUEST_RAM_SIZE) return (uint32_t)-1;
    if (srcAddr >= (2u << 20) || !mem(dstAddr) || dstAddr + size > RT_GUEST_RAM_SIZE)
        return (uint32_t)-1;
    std::memcpy(mem(dstAddr), iopRam() + (srcAddr & 0x1FFFFFFFu), size);
    if (mem(rdAddr)) {  // SifRpcReceiveData_t src/dest/size at +0x10/+0x14/+0x18
        wrW(rdAddr + 0x10, srcAddr); wrW(rdAddr + 0x14, dstAddr); wrW(rdAddr + 0x18, size);
    }
    return 0;
}
RT_STUB_FN(0x00287A80, guest::sceSifGetOtherData)

uint32_t sceSifBindRpc(uint32_t clientPtr, uint32_t rpcId, uint64_t mode64, ...) {
    const uint32_t mode = (uint32_t)mode64;
    if (!mem(clientPtr)) return (uint32_t)-1;
    wrW(clientPtr + kClCmd, 0); wrW(clientPtr + kClBuf, 0); wrW(clientPtr + kClCbuf, 0);
    wrW(clientPtr + kClEndFn, 0); wrW(clientPtr + kClEndPrm, 0); wrW(clientPtr + kClServer, 0);
    wrW(clientPtr + kClPkt, 0); wrW(clientPtr + kClSema, 0xFFFFFFFFu);
    wrW(clientPtr + kClMode, mode);
    wrW(clientPtr + kClRpcId, g_rpcNextId++);
    uint32_t serverPtr = 0;
    auto it = g_rpcServers.find(rpcId);
    if (it != g_rpcServers.end()) serverPtr = it->second;
    auto& cl = g_rpcClients[clientPtr];
    cl.sid = rpcId; cl.busy = false;
    if (!serverPtr && !g_modulesById.empty()) {
        // Unknown sid: publish a dummy server descriptor (oracle
        // rpcAllocServerAddr + memset + sid; client->server != 0 is the SDK's
        // "bound" signal).
        serverPtr = rtguest::kHeapAlloc(kSdStructSize);
        if (serverPtr < RT_GUEST_RAM_SIZE && mem(serverPtr)) {
            std::memset(mem(serverPtr), 0, kSdStructSize);
            wrW(serverPtr + kSdSid, rpcId);
            g_rpcServers[rpcId] = serverPtr;
        } else {
            serverPtr = 0;
        }
    }
    if (serverPtr) {
        wrW(clientPtr + kClServer, serverPtr);
        wrW(clientPtr + kClBuf, rdW(serverPtr + kSdBuf));
        wrW(clientPtr + kClCbuf, rdW(serverPtr + kSdCbuf));
    }
    return 0;
}
RT_STUB_FN(0x00287CD8, guest::sceSifBindRpc)

uint32_t sceSifCallRpc(uint32_t clientPtr, uint32_t rpcNum, uint64_t mode, uint64_t sendBuf, uint64_t sendSize64, uint64_t recvBuf64, uint64_t recvSize64, uint64_t endFn64, uint32_t endParam, ...) {
    const uint32_t sendSize = (uint32_t)sendSize64, receiveBuffer = (uint32_t)recvBuf64,
                   receiveSize = (uint32_t)recvSize64, endFunction = (uint32_t)endFn64;
    if (!mem(clientPtr)) return (uint32_t)-1;
    wrW(clientPtr + kClCmd, rpcNum);
    wrW(clientPtr + kClEndFn, endFunction);
    wrW(clientPtr + kClEndPrm, endParam);
    wrW(clientPtr + kClMode, mode);

    auto& cl = g_rpcClients[clientPtr];
    cl.busy = true; cl.lastRpc = rpcNum;
    if (cl.sid && g_rpcServers.count(cl.sid))
        wrW(clientPtr + kClServer, g_rpcServers[cl.sid]);

    const uint32_t serverPtr = rdW(clientPtr + kClServer);
    if (serverPtr && mem(serverPtr)) {
        wrW(serverPtr + kSdClient, clientPtr);
        wrW(serverPtr + kSdPkt, rdW(clientPtr + kClPkt));
        wrW(serverPtr + kSdRpcNum, rpcNum);
        wrW(serverPtr + kSdSize, sendSize);
        wrW(serverPtr + kSdRecvBuf, receiveBuffer);
        wrW(serverPtr + kSdRsize, receiveSize);
        wrW(serverPtr + kSdRmode, (mode & 2u) && endFunction == 0u ? 0u : 1u);
        wrW(serverPtr + kSdRid, 0);
        const uint32_t serverBuf = rdW(serverPtr + kSdBuf);
        if (serverBuf && sendBuf && sendSize && sendSize <= RT_GUEST_RAM_SIZE &&
            mem(serverBuf) && mem(sendBuf) && serverBuf + sendSize <= RT_GUEST_RAM_SIZE)
            std::memcpy(mem(serverBuf), mem(sendBuf), sendSize);
    }

    // Answer the request: EE-registered guest server functions dispatch
    // directly (the old RegisterRpc path); IOP services come from the
    // (sid,fno) host checklist (W12) — unanswered falls to the oracle's
    // copy-send / zero-receive fallback.
    uint32_t resultPtr = 0;
    bool handled = false;
    const uint32_t sFunc = serverPtr ? rdW(serverPtr + kSdFunc) : 0;
    const uint32_t sBuf = serverPtr ? rdW(serverPtr + kSdBuf) : 0;
    if (sFunc && rtguest::fnEntry(sFunc)) {
        resultPtr = (uint32_t)rtCall(sFunc, rpcNum, sBuf, sendSize, receiveBuffer);
        handled = true;
        if (!resultPtr) resultPtr = sBuf ? sBuf : receiveBuffer;
    } else if (!handled) {
        static int shown = 0;
        if (shown < 8)
            std::fprintf(stderr, "[hostlib] CallRpc sid=0x%x fno=0x%x unanswered\n",
                         cl.sid, rpcNum);
        if (shown < 1000) ++shown;
    }
    if (receiveBuffer && receiveSize && mem(receiveBuffer) &&
        receiveBuffer + receiveSize <= RT_GUEST_RAM_SIZE) {
        if (handled && resultPtr && mem(resultPtr) && resultPtr != receiveBuffer) {
            std::memcpy(mem(receiveBuffer), mem(resultPtr), receiveSize);
        } else if (!handled && sendBuf && sendSize && sendBuf != receiveBuffer &&
                   mem(sendBuf)) {
            std::memcpy(mem(receiveBuffer), mem(sendBuf),
                        sendSize < receiveSize ? sendSize : receiveSize);
        } else if (!handled) {
            std::memset(mem(receiveBuffer), 0, receiveSize);
        }
    }
    cl.busy = false;

    // end_function (oracle RpcCallback invocation; ours synchronous on the
    // same green thread — the pump's switchpoints make that equivalent).
    if (endFunction && rtguest::fnEntry(endFunction)) {
        rtCall(endFunction, endParam);
    } else {
        const uint32_t sema = rdW(clientPtr + kClSema);
        if (sema != 0xFFFFFFFFu && sema != 0u) rtguest::kSignalSema((int)sema, true);
    }
    return 0;
}
RT_STUB_FN(0x00287EA8, guest::sceSifCallRpc)

uint32_t sceSifCheckStatRpc(uint32_t clientPtr, ...) {
    auto it = g_rpcClients.find(clientPtr);
    return it != g_rpcClients.end() && it->second.busy ? 1u : 0u;
}
RT_STUB_FN(0x00288098, guest::sceSifCheckStatRpc)

uint32_t sceSifSetRpcQueue(uint32_t qdPtr, uint32_t threadId, ...) {
    if (!mem(qdPtr)) return (uint32_t)-1;
    wrW(qdPtr + kQdThread, threadId);
    wrW(qdPtr + kQdActive, 0); wrW(qdPtr + kQdLink, 0); wrW(qdPtr + kQdStart, 0);
    wrW(qdPtr + kQdEnd, 0); wrW(qdPtr + kQdNext, 0);
    if (!g_rpcActiveQueue) {
        g_rpcActiveQueue = qdPtr;
    } else {
        uint32_t curPtr = g_rpcActiveQueue;
        for (int guard = 0; guard < 1024 && curPtr; ++guard) {
            if (curPtr == qdPtr) break;
            if (!mem(curPtr)) break;
            if (!rdW(curPtr + kQdNext)) { wrW(curPtr + kQdNext, qdPtr); break; }
            curPtr = rdW(curPtr + kQdNext);
        }
    }
    return 0;
}
RT_STUB_FN(0x002880D8, guest::sceSifSetRpcQueue)

uint32_t sceSifRegisterRpc(uint32_t sdPtr, uint32_t sid, uint32_t func, uint32_t buf,
                           uint32_t cfunc, uint32_t cbuf, uint32_t qd, ...) {
    if (!mem(sdPtr)) return (uint32_t)-1;
    wrW(sdPtr + kSdSid, sid); wrW(sdPtr + kSdFunc, func); wrW(sdPtr + kSdBuf, buf);
    wrW(sdPtr + kSdSize, 0); wrW(sdPtr + kSdCfunc, cfunc); wrW(sdPtr + kSdCbuf, cbuf);
    wrW(sdPtr + kSdSize2, 0); wrW(sdPtr + kSdClient, 0); wrW(sdPtr + kSdPkt, 0);
    wrW(sdPtr + kSdRpcNum, 0); wrW(sdPtr + kSdRecvBuf, 0); wrW(sdPtr + kSdRsize, 0);
    wrW(sdPtr + kSdRmode, 0); wrW(sdPtr + kSdRid, 0); wrW(sdPtr + kSdBase, qd);
    wrW(sdPtr + kSdLink, 0); wrW(sdPtr + kSdNext, 0);
    if (qd && mem(qd)) {
        if (!rdW(qd + kQdLink)) {
            wrW(qd + kQdLink, sdPtr);
        } else {
            uint32_t curPtr = rdW(qd + kQdLink);
            for (int guard = 0; guard < 1024 && curPtr; ++guard) {
                if (!mem(curPtr)) break;
                if (!rdW(curPtr + kSdLink)) { wrW(curPtr + kSdLink, sdPtr); break; }
                if (rdW(curPtr + kSdLink) == sdPtr) break;
                curPtr = rdW(curPtr + kSdLink);
            }
        }
    }
    g_rpcServers[sid] = sdPtr;
    for (auto& entry : g_rpcClients) {
        if (entry.second.sid == sid && mem(entry.first)) {
            wrW(entry.first + kClServer, sdPtr);
            wrW(entry.first + kClBuf, rdW(sdPtr + kSdBuf));
            wrW(entry.first + kClCbuf, rdW(sdPtr + kSdCbuf));
        }
    }
    std::fprintf(stderr, "[SifRegisterRpc] sid=0x%x sd=0x%x\n", sid, sdPtr);
    return 0;
}
RT_STUB_FN(0x00288170, guest::sceSifRegisterRpc)

uint32_t sceSifRemoveRpc(uint32_t sdPtr, uint32_t qdPtr, ...) {
    if (!mem(qdPtr) || !sdPtr) return 0;
    if (rdW(qdPtr + kQdLink) == sdPtr) {
        wrW(qdPtr + kQdLink, mem(sdPtr) ? rdW(sdPtr + kSdLink) : 0);
        if (mem(sdPtr)) wrW(sdPtr + kSdLink, 0);
        return sdPtr;
    }
    uint32_t curPtr = rdW(qdPtr + kQdLink);
    for (int guard = 0; guard < 1024 && curPtr; ++guard) {
        if (!mem(curPtr)) break;
        if (rdW(curPtr + kSdLink) == sdPtr) {
            wrW(curPtr + kSdLink, mem(sdPtr) ? rdW(sdPtr + kSdLink) : 0);
            if (mem(sdPtr)) wrW(sdPtr + kSdLink, 0);
            return sdPtr;
        }
        curPtr = rdW(curPtr + kSdLink);
    }
    return 0;
}
RT_STUB_FN(0x00288240, guest::sceSifRemoveRpc)

uint32_t sceSifRemoveRpcQueue(uint32_t qdPtr, ...) {
    if (!qdPtr || !g_rpcActiveQueue) return 0;
    if (g_rpcActiveQueue == qdPtr) {
        g_rpcActiveQueue = mem(qdPtr) ? rdW(qdPtr + kQdNext) : 0;
        return qdPtr;
    }
    uint32_t curPtr = g_rpcActiveQueue;
    for (int guard = 0; guard < 1024 && curPtr; ++guard) {
        if (!mem(curPtr)) break;
        if (rdW(curPtr + kQdNext) == qdPtr) {
            wrW(curPtr + kQdNext, mem(qdPtr) ? rdW(qdPtr + kQdNext) : 0);
            return qdPtr;
        }
        curPtr = rdW(curPtr + kQdNext);
    }
    return 0;
}
RT_STUB_FN(0x002882D8, guest::sceSifRemoveRpcQueue)

uint32_t sceSifGetNextRequest(uint32_t param_1, ...) { return 0; }
RT_STUB_FN(0x00288368, guest::sceSifGetNextRequest)

uint32_t sceSifExecRequest(uint32_t param_1, ...) { return 0; }
RT_STUB_FN(0x002883C0, guest::sceSifExecRequest)

uint32_t sceSifRpcLoop(uint64_t param_1, ...) { return 0; }
RT_STUB_FN(0x00288588, guest::sceSifRpcLoop)

// ---- libload / IOP heap (Stubs/SIF.cpp: load is tracked, not executed) ----
uint32_t sceSifInitIopHeap(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x0028BC50, guest::sceSifInitIopHeap)

uint32_t sceSifAllocIopHeap(uint32_t size, ...) { return iopAlloc(size); }
RT_STUB_FN(0x0028BCD8, guest::sceSifAllocIopHeap)

uint32_t sceSifAllocSysMemory(uint32_t param_1, uint32_t size, uint32_t param_3, ...) {
    return iopAlloc(size);  // oracle: allocateIopMemory($a1, 64)
}
RT_STUB_FN(0x0028BD48, guest::sceSifAllocSysMemory)

uint32_t sceSifLoadIopHeap(uint32_t param_1, uint32_t param_2, ...) { return 0; }
RT_STUB_FN(0x0028BE60, guest::sceSifLoadIopHeap)

uint32_t sceSifLoadFileReset(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x0028C0E0, guest::sceSifLoadFileReset)

uint32_t _sceSifLoadModuleBuffer(uint32_t bufferAddr, uint32_t argSize,
                                 uint32_t argAddr, uint32_t param_4, ...) {
    if (!bufferAddr || !mem(bufferAddr)) return (uint32_t)-1;
    const uint32_t id = g_moduleNextId++;
    std::fprintf(stderr, "[SIF module] load-buffer-emulated id=%u\n", id);
    return id;
}
RT_STUB_FN(0x0028C118, guest::_sceSifLoadModuleBuffer)

uint32_t _sceSifLoadModule(uint32_t pathAddr, uint32_t argSize, uint32_t argAddr,
                          uint32_t param_4, uint64_t param_5, ...) {
    const std::string path = cstrGuest(pathAddr, 255);
    if (path.empty()) return (uint32_t)-1;
    if (argSize > 64u * 1024u) return (uint32_t)-1;
    // The oracle loads the IRX onto its emulated IOP; native answers through
    // host RPC services instead (never runs IOP code), so a disc-named module
    // always "loads" and only its bookkeeping matters (module ids, bind gate).
    const uint32_t id = g_moduleNextId++;
    g_modulesById[id] = path;
    std::fprintf(stderr, "[SIF module] load-emulated id=%u ref=1 path=\"%s\"\n",
                 id, path.c_str());
    return id;
}
RT_STUB_FN(0x0028C728, guest::_sceSifLoadModule)

uint32_t sceSifGetIopAddr(uint32_t param_1, uint32_t param_2, uint64_t param_3, ...) {
    return param_1;  // oracle verbatim (returns $a0)
}
RT_STUB_FN(0x0028CAE0, guest::sceSifGetIopAddr)

uint32_t sceSifSetIopAddr(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
    return param_2;
}
RT_STUB_FN(0x0028CBD0, guest::sceSifSetIopAddr)

uint32_t sceSifResetIop(uint32_t param_1, uint32_t param_2, ...) {
    rtguest::kResetSifRegs();
    return 1;
}
RT_STUB_FN(0x0028CCB0, guest::sceSifResetIop)

uint32_t sceSifIsAliveIop(uint32_t q0, ...) { return 1; }
RT_STUB_FN(0x0028CDF0, guest::sceSifIsAliveIop)

uint32_t sceSifSyncIop(uint32_t q0, ...) { return 1; }
RT_STUB_FN(0x0028CE18, guest::sceSifSyncIop)

uint64_t sceSifRebootIop(uint32_t imageAddr, ...) {
    rtguest::kResetSifRegs();  // oracle PS2IopTransport::reset
    std::fprintf(stderr, "[hostlib] SifRebootIop image=0x%x (emulated)\n", imageAddr);
    return 1;
}
RT_STUB_FN(0x0028CE50, guest::sceSifRebootIop)

// ---- libkernel helpers (Syscalls/Thread.cpp InitThread -> main tid,
// Sync.cpp InitAlarm -> KE_OK; iWakeupThread/iRotateThreadReadyQueue reuse
// the syscall dispatcher with the i-variant keys, as the stubs do) ----
uint32_t InitAlarm(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x00285C50, guest::InitAlarm)

uint32_t InitThread(uint32_t q0, ...) { return 1; }  // kMainThreadId
RT_STUB_FN(0x00285E00, guest::InitThread)

uint32_t iWakeupThread(uint32_t thid, ...) {
    return (uint32_t)syscall(0xffffffccull, thid);
}
RT_STUB_FN(0x00285ED8, guest::iWakeupThread)

uint32_t iRotateThreadReadyQueue(uint32_t tid, ...) {
    return (uint32_t)syscall(0xffffffd4ull, tid);
}
RT_STUB_FN(0x00285F70, guest::iRotateThreadReadyQueue)

// ---- newlib thin ports (Stubs/LibC.cpp: host libc over guest RAM) ----
uint32_t memcmp(uint32_t s1, uint32_t s2, uint32_t n, ...) {
    return (uint32_t)std::memcmp(mem(s1), mem(s2), n);
}
RT_STUB_FN(0x0027E5F8, guest::memcmp)

uint32_t memcpy(uint32_t dest, uint32_t src, uint32_t n, ...) {
    std::memcpy(mem(dest), mem(src), n);
    return dest;
}
RT_STUB_FN(0x0027E68C, guest::memcpy)

uint32_t memmove(uint32_t dest, uint32_t src, uint32_t n, ...) {
    std::memmove(mem(dest), mem(src), n);
    return dest;
}
RT_STUB_FN(0x0027E73C, guest::memmove)

uint32_t memset(uint32_t s, uint32_t c, uint32_t n, ...) {
    std::memset(mem(s), (int)c, n);
    return s;
}
RT_STUB_FN(0x0027E840, guest::memset)

uint32_t strcat(uint32_t dest, uint32_t src, ...) {
    std::strcat((char*)mem(dest), (const char*)mem(src));
    return dest;
}
RT_STUB_FN(0x0027EF70, guest::strcat)

uint32_t strcmp(uint32_t s1, uint32_t s2, ...) {
    return (uint32_t)std::strcmp((const char*)mem(s1), (const char*)mem(s2));
}
RT_STUB_FN(0x0027F09C, guest::strcmp)

uint32_t strcpy(uint32_t dest, uint32_t src, ...) {
    std::strcpy((char*)mem(dest), (const char*)mem(src));
    return dest;
}
RT_STUB_FN(0x0027F1E0, guest::strcpy)

uint32_t strlen(uint32_t s, ...) {
    return (uint32_t)std::strlen((const char*)mem(s));
}
RT_STUB_FN(0x0027F2F4, guest::strlen)

uint32_t strncpy(uint32_t dest, uint32_t src, uint32_t n, ...) {
    std::strncpy((char*)mem(dest), (const char*)mem(src), n);
    return dest;
}
RT_STUB_FN(0x0027F430, guest::strncpy)

uint64_t __divdi3(int64_t num, int64_t den, ...) {
    return den ? (uint64_t)(num / den) : 0u;
}
RT_STUB_FN(0x0028D210, guest::__divdi3)

}  // namespace guest
