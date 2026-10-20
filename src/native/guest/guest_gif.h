#ifndef RT_NATIVE_GUEST_GIF_H
#define RT_NATIVE_GUEST_GIF_H
#include <cstdint>
namespace rtguest {
// W3 step 3a GIF front end (docs/native-reference/w13-gs-sink-design.md).
// Decodes GIF tag streams the way the oracle front end does
// (gs_frontend.cpp:919-1030: NLOOP lo&0x7fff, FLG lo 59:58, NREG lo 63:60
// with 0=16, PRE lo bit 46 with A+D prim 57:47, REGS = tag upper qword
// nibbles; ADK clear on XYZ(F)2 = vertex kick). fed by XGKICK kicks
// (vu1SetKick) and the GIF DMA channel payload. Slice 3a decodes and
// censuses; GS register state and the renderer feed land with the W2
// preview chain.
// gifSink decodes a GIF packet stream (tag + payload qwords) into GS register
// writes; gifPath2Direct forwards VIF1 DIRECT payloads (PATH2, splitting kept).
void gifSink(const uint8_t* data, uint32_t sizeBytes);
uint32_t gifSinkConsume(const uint8_t* data, uint32_t sizeBytes);
void gifPath2Direct(const uint8_t* data, uint32_t sizeBytes, bool directHl);
bool gifCensusOn();
void gifCensusDump();
// census counters (test-visible)
uint64_t gifTagCount(uint32_t flg);
uint64_t gifVertsCount();
uint64_t gifKicksCount();
uint64_t gifImageBytesCount();
}  // namespace rtguest
#endif
