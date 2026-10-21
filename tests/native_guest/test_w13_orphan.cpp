// W13-ORPHAN pins: the 20 jump-table handlers carved at codeptr_* (package w13-orphan).
// Expected values are computed by hand from the Ghidra pseudocode / MIPS cited per case
// (build/scratch/w13-orphan/REPORT.md); the HUD table at 0x2a8fb0 is checked against the ELF.
#undef NDEBUG
#include "../../src/native/guest/guest_shim.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

using namespace rtguest;
static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); ++g_fail; } } while (0)

template <class T> static T& at(uint32_t a) { return *(T*)G(a); }
static uint64_t call(uint32_t fn, uint64_t a0 = 0, uint64_t a1 = 0, uint64_t a2 = 0, uint64_t a3 = 0,
                     uint64_t a4 = 0, uint64_t a5 = 0, uint64_t a6 = 0) {
  return callGuest(fn, a0, a1, a2, a3, a4, a5, a6);
}

static const uint32_t kAll[] = {0x002010a0, 0x00201118, 0x00201130, 0x002335a8, 0x00234524, 0x0023c498,
                                0x0023c558, 0x002496c8, 0x00249fa0, 0x0024a6a8, 0x0024ab38, 0x0024b3a0,
                                0x0024ba78, 0x0024c320, 0x0024cb70, 0x0024d3e0, 0x0024da50};

static void registered() {
  for (uint32_t a : kAll) CHECK(fnEntry(a) != 0);
  // 0x21fe70 / 0x280400 / 0x280800 are deliberately NOT ported (REPORT.md): interior of a parent.
  CHECK(fnEntry(0x0021fe70) == 0 && fnEntry(0x00280400) == 0 && fnEntry(0x00280800) == 0);
}

// uiKanaGlyphUV fragments: t1 = &u, t0 = &v (a0..a3,t0,t1 positional).
static void kana() {
  const uint32_t U = 0x00300000, V = 0x00300010;
  at<int16_t>(U) = 77; at<int16_t>(V) = 100;
  call(0x002010a0, 0, 0, 0, 0, V, U);            // MIPS 0x2010a0: u=0, v+=0xb4
  CHECK(at<int16_t>(U) == 0 && at<int16_t>(V) == 100 + 0xb4);
  at<int16_t>(V) = 100;
  call(0x00201118, 0, 0, 0, 0, V, U);            // MIPS 0x201118: u=0xc0, v+=0x60
  CHECK(at<int16_t>(U) == 0xc0 && at<int16_t>(V) == 100 + 0x60);
  struct { uint32_t code; int u, vadd; } k[] = {
      {0x8754, 0x78, 0xb4},                        // (short)0x8754 + 0x78ac = 0 -> 0*0x14 + 0x78
      {0x8755, 0x8c, 0xb4},                        // 1*0x14 + 0x78
      {0x8260, 0x50, 0x8c},                        // (0x8260+0x7da4)&0xffff = 4 -> 4%25*0x14, 4/25*0x14 + 0x8c
      {0x8250, 0x190, 0x78},                       // (0x8250+0x7dc4)&0xffff = 20: 20%25*20, 20/25*20 + 0x78
      {0x829f, 0x0, 0x0},                          // (0x829f+0x7d61)&0xffff = 0: u=0, v += 0
      {0x8340, 0xa0, 0x3c}};                       // (0x8340+0x7cc8)&0xffff = 8: 8*20, 0*20 + 0x3c
  for (auto& c : k) {
    at<int16_t>(U) = 0; at<int16_t>(V) = 1000;
    call(0x00201130, 0, 0, c.code, 0, V, U);
    if (!(at<int16_t>(U) == (int16_t)c.u && at<int16_t>(V) == (int16_t)(1000 + c.vadd))) std::fprintf(stderr, "code %x: u=%d v=%d\n", c.code, at<int16_t>(U), at<int16_t>(V));
    CHECK(at<int16_t>(U) == (int16_t)c.u && at<int16_t>(V) == (int16_t)(1000 + c.vadd));
  }
}

static void blocks() {
  const uint32_t D = 0x00310000;
  std::memset(kRam + D, 0xAA, 0x40);
  call(0x002335a8, 0, 0, D);  // MIPS 0x2335a8..0x23384c: 0x2c-byte template, +0x28 = 2.0f, +0x24 = 0x1e
  CHECK(at<uint64_t>(D) == 0x14000000003ull && at<uint64_t>(D + 8) == 0xe0 &&
        at<uint64_t>(D + 16) == 0x29fb8800000000ull && at<uint64_t>(D + 24) == 1 &&
        at<uint32_t>(D + 32) == 0 && at<uint32_t>(D + 0x24) == 0x1e && at<uint32_t>(D + 0x28) == 0x40000000u);
  CHECK(kRam[D + 0x2c] == 0xAA);

  const uint32_t B = 0x00320000;  // 0x234524 = nop + ResetStateBlock (0x234528)
  std::memset(kRam + B, 0xAA, 0x8cb0 + 16);
  call(0x00234524, B);
  CHECK(kRam[B] == 0 && kRam[B + 0x8cae] == 0 && kRam[B + 0x8cb0] == 0xAA);
  CHECK(at<uint32_t>(B + 0x8004) == 0 && at<uint64_t>(B + 0x80a4) == 0x6400000001ull &&
        at<uint64_t>(B + 0x80ac) == 0x10000002cull && at<uint64_t>(B + 0x80b4) == 0x29f74800000004ull &&
        at<uint64_t>(B + 0x80bc) == 1 && at<uint64_t>(B + 0x80c4) == 0 && at<uint32_t>(B + 0x80cc) == 0);
}

// switchD_0023c440 arms: a2 = index, a3 = record.
static void switchArms() {
  const uint32_t R = 0x00330000;
  const uint32_t T1 = 0x002eca8a + 3 * 0x1c;
  at<uint16_t>(R + 8) = 0x1234;
  at<int16_t>(T1) = 1;  call(0x0023c498, 0, 0, 3, R);
  CHECK(at<uint16_t>(R + 8) == (0x1234 | 0x400));
  at<int16_t>(T1) = 0;  call(0x0023c498, 0, 0, 3, R);
  CHECK(at<uint16_t>(R + 8) == (0x1234 & 0xfbff));
  const uint32_t H = 0x002f03ec + 2 * 0x14, Bt = 0x002f03f0 + 2 * 0x14;  // stride 0x14 (Ghidra's *5 is words)
  at<uint16_t>(H) = 0x0124; at<uint8_t>(Bt) = 0x5a; at<uint16_t>(R + 8) = 0xffff;
  call(0x0023c558, 0, 0, 2, R);
  CHECK(at<uint8_t>(R + 7) == 0x5a && at<uint16_t>(R + 8) == ((0xffff & 0xfcdb) | 0x0124));
}

// HUD table (UpdateRaceHud 0x2490a0 -> *(0x2a8fb0 + 4*i)) holds exactly our 10 handlers + 0x24e1a0.
static void table() {
  static const uint32_t want[] = {0x2496c8, 0x249fa0, 0x24a6a8, 0x24ab38, 0x24b3a0, 0x24ba78,
                                  0x24c320, 0x24cb70, 0x24d3e0, 0x24da50, 0x24e1a0};
  std::vector<uint8_t> elf;
  const char* p = std::getenv("RT_ELF");
  std::string path = p ? p : "build/rom/SLUS_203.98";
  FILE* f = std::fopen(path.c_str(), "rb");
  if (!f) { std::printf("table: ELF %s missing, ELF-backed pin skipped\n", path.c_str()); return; }
  std::fseek(f, 0, SEEK_END); elf.resize(std::ftell(f)); std::fseek(f, 0, SEEK_SET);
  CHECK(std::fread(elf.data(), 1, elf.size(), f) == elf.size());
  std::fclose(f);
  CHECK(loadImage(elf.data(), elf.size()));
  for (int i = 0; i < 11; ++i) {
    CHECK(at<uint32_t>(0x2a8fb0 + 4 * i) == want[i]);
    CHECK(fnEntry(want[i]) != 0);                         // the census fault: handler 0 x71
  }
  // Handlers run end to end only up to uiArenaSubmit (vifDirect -> fatal without a DMA environment): not pinned here.
}

int main() {
  registered(); kana(); blocks(); switchArms(); table();
  if (g_fail) { std::fprintf(stderr, "%d failures\n", g_fail); return 1; }
  std::printf("w13_orphan OK\n");
  return 0;
}
