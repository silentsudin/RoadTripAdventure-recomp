// Pins below are computed by hand from the cited Ghidra pseudocode bodies
// (build/decomp/fn/<addr>.c) with zeroed kRam and the listed seeds.
#include "guest/guest_shim.h"

#include <cassert>
#include <cstring>

namespace guest {
uint32_t memclr(uint32_t, uint64_t, ...);
uint32_t sifCmdGetFlag(uint32_t, ...);
uint32_t sifFreeRpcPacket(uint32_t, ...);
uint32_t sifCmd0StoreWord(uint32_t, uint32_t, ...);
uint32_t sifCmd1StoreIndexed(uint32_t, uint32_t, ...);
uint32_t sifFindRpcServer(uint32_t, uint32_t, ...);
uint32_t gs_stamps_award_function(uint64_t, ...);
uint32_t gs_photos_take_function(uint64_t, ...);
}

template <class T> static T& at(uint32_t addr) {
  return *reinterpret_cast<T*>(rtguest::G(addr));
}

int main() {
  std::memset(rtguest::kRam, 0, RT_GUEST_RAM_SIZE);

  // 0x00270148 memclr(addr, 5): zero exactly the first 5 of 8 seeded bytes.
  constexpr uint32_t clr = 0x100000;
  std::memset(rtguest::G(clr), 0xa5, 8);
  guest::memclr(clr, 5);
  for (int i = 0; i < 8; ++i)
    assert(at<uint8_t>(clr + i) == (i < 5 ? 0 : 0xa5));

  // 0x00286f50 sifCmdGetFlag(2): returns word at 0x1776fc0 + 2*4.
  at<uint32_t>(0x1776fc0 + 2 * 4) = 0x12345678;
  assert(guest::sifCmdGetFlag(2) == 0x12345678);

  // 0x002878d8 sifFreeRpcPacket(p): clears +0x18 and bit0 of the +0x10 flag.
  constexpr uint32_t pkt = 0x110000;
  at<uint32_t>(pkt + 0x10) = 0x00030005;
  at<uint32_t>(pkt + 0x18) = 7;
  guest::sifFreeRpcPacket(pkt);
  assert(at<uint32_t>(pkt + 0x10) == 0x00030004);
  assert(at<uint32_t>(pkt + 0x18) == 0);

  // 0x00286f40 sifCmd0StoreWord(p, q): *(u32*)(q+8) = *(u32*)(p+0x10).
  at<uint32_t>(0x2000 + 0x10) = 0xabcd1234;
  guest::sifCmd0StoreWord(0x2000, 0x2100);
  assert(at<uint32_t>(0x2100 + 8) == 0xabcd1234);

  // 0x00286f20 sifCmd1StoreIndexed(p, q): *(u32*)(*(int*)(p+0x10)*4 + *(int*)(q+0x1c))
  //                                    = *(u32*)(p+0x14).
  at<int32_t>(0x3000 + 0x10) = 3;
  at<int32_t>(0x3100 + 0x1c) = 0x2000;
  at<uint32_t>(0x3000 + 0x14) = 0x55667788;
  guest::sifCmd1StoreIndexed(0x3000, 0x3100);
  assert(at<uint32_t>(0x2000 + 3 * 4) == 0x55667788);

  // 0x00287bd8 sifFindRpcServer(id, cmddata): node=*(cmddata+0x28), first entry
  // *(node+8), next entry entry[0xe], next node node+0x14; NULL when unregistered.
  at<uint32_t>(0x2100 + 0x28) = 0x2000;              // node
  at<uint32_t>(0x2000 + 8) = 0x2200;                 // entry 1
  at<uint32_t>(0x2200) = 0x80000101;                 // entry id
  at<uint32_t>(0x2200 + 0xe * 4) = 0x2300;           // entry 2 via +0x38
  at<uint32_t>(0x2300) = 0xdead;                     // different id
  at<uint32_t>(0x2300 + 0xe * 4) = 0;                // end of entries
  at<uint32_t>(0x2000 + 0x14) = 0;                   // end of nodes
  assert(guest::sifFindRpcServer(0x80000101, 0x2100) == 0x2200);
  assert(guest::sifFindRpcServer(0x99999999, 0x2100) == 0);

  // 0x0023c8a8 gs_stamps_award_function(0x11): queue empty, bitmap clear, char
  // window not full (0x1780a64 queue, 0x177fd08 char row, count 0x177fd88).
  // Stamp 0x11 is the else-branch (uVar7 = 0x7c) appended at row[0], count -> 1.
  // A second award of the same stamp short-circuits in the queue scan (returns 0).
  assert(guest::gs_stamps_award_function(0x11) == 1);
  assert(at<uint8_t>(0x1782a64) == 0x11);
  assert(at<int32_t>(0x1782a60) == 1);
  assert(at<uint8_t>(0x177fd08) == 0x7c);
  assert(at<int32_t>(0x177fd88) == 1);
  assert(guest::gs_stamps_award_function(0x11) == 0);

  // 0x0023c5c0 gs_photos_take_function(3): photo index 2 records the progress
  // pointer (u64 at 0x177f760) and town id (u32 at 0x177f768), sets bitmap bit 2
  // in the u64 at 0x177fc68, the u16 photo index at 0x1780a42, and ORs
  // 0x80000000 into the u64 at 0x177fc90.
  at<uint64_t>(0x177f760) = 0x1122334455667788;
  at<uint32_t>(0x177f768) = 0x1a2b3c4d;
  at<uint64_t>(0x177fc90) = 1;
  guest::gs_photos_take_function(3);
  assert(at<uint64_t>(0x177f7b4 + 2 * 0xc) == 0x1122334455667788);
  assert(at<uint32_t>(0x177f7bc + 2 * 0xc) == 0x1a2b3c4d);
  assert(at<uint64_t>(0x177fc68) == 4);
  assert(at<uint16_t>(0x1780a42) == 2);
  assert(at<uint64_t>(0x177fc90) == 0x80000001);
  return 0;
}
