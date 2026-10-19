// Pins W11-39: sifRpcCallRequest (0x287e18) — SIF RPC client queue link +
// field copies + the iWakeupThread(*queue) forward (lw $4,0x0($6) proven at
// 287e7c). Queue/thread machinery stays on the zero-RAM else path.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t sifRpcCallRequest(uint32_t q0 = 0, ...);
}  // namespace guest

int main() {
  std::memset(rtguest::kRam, 0, sizeof(rtguest::kRam));

  const uint32_t blk = 0x00400000, req = 0x00400100, q = 0x00400200;
  const uint32_t req2 = 0x00400300;
  *(uint32_t*)rtguest::G(blk + 0x34) = req;
  *(uint32_t*)rtguest::G(req + 0x40) = q;
  *(uint32_t*)rtguest::G(q) = 3;              // queue owner thread id
  *(uint32_t*)rtguest::G(blk + 0x14) = 0x1111;
  *(uint32_t*)rtguest::G(blk + 0x1c) = 0x2222;
  *(uint32_t*)rtguest::G(blk + 0x20) = 0x3333;

  guest::sifRpcCallRequest(blk);
  assert(*(uint32_t*)rtguest::G(q + 0xc) == req);   // empty queue: tail=req
  assert(*(uint32_t*)rtguest::G(q + 0x10) == req);  // head->last
  assert(*(uint32_t*)rtguest::G(req + 0x1c) == 0x2222);
  assert(*(uint32_t*)rtguest::G(req + 0x20) == 0x1111);
  assert(*(uint32_t*)rtguest::G(req + 0x24) == 0x3333);

  // second request: links off the old tail (+0x3c) and moves last
  *(uint32_t*)rtguest::G(blk + 0x34) = req2;
  *(uint32_t*)rtguest::G(req2 + 0x40) = q;
  guest::sifRpcCallRequest(blk);
  assert(*(uint32_t*)rtguest::G(req + 0x3c) == req2);
  assert(*(uint32_t*)rtguest::G(q + 0x10) == req2);
  assert(*(uint32_t*)rtguest::G(q + 0xc) == req);  // tail unchanged
  return 0;
}
