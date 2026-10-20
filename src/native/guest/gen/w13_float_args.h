// W13 float-param calls: callees take f12.. float args as uint32 bit patterns (decl stubs are uint32);
// a float-typed caller expression would value-truncate, so these exact-match templates carry its bits.
// Integer args pass through unchanged (raw-bit loads/literals), so only float/double exprs change.
#pragma once
#include "../guest_shim.h"

namespace guest {
namespace w13f {
inline uint32_t fb(float v) { return rtguest::f2bits(v); }
inline uint32_t fb(double v) { return rtguest::f2bits(static_cast<float>(v)); }
template <class T> inline uint32_t fb(T v) { return static_cast<uint32_t>(v); }
}  // namespace w13f

#define W13F_U uint32_t
template <class A> inline uint32_t SteerHeadingVector(A a, W13F_U b = 0, W13F_U c = 0) { return ::guest::SteerHeadingVector(w13f::fb(a), b, c); }
template <class A> inline uint32_t TestQuadLateralMask(A a, W13F_U b = 0, W13F_U c = 0, W13F_U d = 0, W13F_U e = 0) { return ::guest::TestQuadLateralMask(w13f::fb(a), b, c, d, e); }
template <class A, class B, class C> inline uint32_t StoreIndexedPosition(A a, B b, C c, W13F_U d = 0, W13F_U e = 0) { return ::guest::StoreIndexedPosition(w13f::fb(a), w13f::fb(b), w13f::fb(c), d, e); }
template <class A, class B, class C> inline uint32_t mapViewMatrix(A a, B b, C c, W13F_U d = 0) { return ::guest::mapViewMatrix(w13f::fb(a), w13f::fb(b), w13f::fb(c), d); }
template <class A, class B> inline uint32_t drawCar(A a, B b, W13F_U c = 0, W13F_U d = 0, W13F_U e = 0) { return ::guest::drawCar(w13f::fb(a), w13f::fb(b), c, d, e); }
template <class A> inline uint32_t AdvanceCarEmitterStep(A a, W13F_U b = 0, W13F_U c = 0, W13F_U d = 0, W13F_U e = 0, W13F_U f = 0, W13F_U g = 0) { return ::guest::AdvanceCarEmitterStep(w13f::fb(a), b, c, d, e, f, g); }
template <class A> inline uint32_t updateTimeOfDayLight(A a, W13F_U b = 0, W13F_U c = 0) { return ::guest::updateTimeOfDayLight(w13f::fb(a), b, c); }
template <class A> inline uint32_t ScaleCarLightMatrix(A a, W13F_U b = 0) { return ::guest::ScaleCarLightMatrix(w13f::fb(a), b); }
template <class A> inline uint32_t BuildCarouselQuadA(A a, W13F_U b = 0, W13F_U c = 0, W13F_U d = 0, W13F_U e = 0, W13F_U f = 0) { return ::guest::BuildCarouselQuadA(w13f::fb(a), b, c, d, e, f); }
// QuadB reads only f12+a0..a3 (disasm 0x244538: no $t0 use); callers' 6th arg (t0) is dead. Forwarding it
// would pick this template over the 5-named+ellipsis decl -> self-recursion UB (clang brk in drawCarouselScene).
template <class A> inline uint32_t BuildCarouselQuadB(A a, W13F_U b = 0, W13F_U c = 0, W13F_U d = 0, W13F_U e = 0, W13F_U = 0) { return ::guest::BuildCarouselQuadB(w13f::fb(a), b, c, d, e); }
// QuadC reads only f12+a0..a3 (disasm 0x2446a8: no $t0 use); callers' 6th arg (t0) is dead. Forwarding it
// would pick this template over the 5-named+ellipsis decl -> self-recursion UB (clang brk in drawCarouselScene).
template <class A> inline uint32_t BuildCarouselQuadC(A a, W13F_U b = 0, W13F_U c = 0, W13F_U d = 0, W13F_U e = 0, W13F_U = 0) { return ::guest::BuildCarouselQuadC(w13f::fb(a), b, c, d, e); }
template <class A> inline uint32_t BuildCarouselQuadD(A a, W13F_U b = 0, W13F_U c = 0, W13F_U d = 0, W13F_U e = 0, W13F_U f = 0) { return ::guest::BuildCarouselQuadD(w13f::fb(a), b, c, d, e, f); }
template <class A> inline uint32_t BuildCarouselDrawPacket(A a, W13F_U b = 0, W13F_U c = 0, W13F_U d = 0, W13F_U e = 0, W13F_U f = 0) { return ::guest::BuildCarouselDrawPacket(w13f::fb(a), b, c, d, e, f); }
template <class A> inline void vu0RotMatrixX(A a, W13F_U o = 0, W13F_U i = 0) { ::guest::vu0RotMatrixX(w13f::fb(a), o, i); }
template <class A> inline void vu0RotMatrixY(A a, W13F_U o = 0, W13F_U i = 0) { ::guest::vu0RotMatrixY(w13f::fb(a), o, i); }
template <class A> inline void vu0RotMatrixZ(A a, W13F_U o = 0, W13F_U i = 0) { ::guest::vu0RotMatrixZ(w13f::fb(a), o, i); }
template <class A, class B, class C> inline uint32_t vu0DropShadowMatrix(A a, B b, C c, W13F_U d = 0, W13F_U e = 0, W13F_U f = 0) { return ::guest::vu0DropShadowMatrix(w13f::fb(a), w13f::fb(b), w13f::fb(c), d, e, f); }
template <class A> inline uint32_t __ieee754_rem_pio2f(A a, W13F_U b = 0) { return ::guest::__ieee754_rem_pio2f(w13f::fb(a), b); }
template <class A> inline uint32_t __ieee754_sqrtf(A a) { return ::guest::__ieee754_sqrtf(w13f::fb(a)); }
template <class A, class B = uint32_t> inline uint32_t __kernel_cosf(A a, B b = 0) { return ::guest::__kernel_cosf(w13f::fb(a), w13f::fb(b)); }
template <class A, class B = uint32_t> inline uint32_t __kernel_sinf(A a, B b = 0, W13F_U c = 0) { return ::guest::__kernel_sinf(w13f::fb(a), w13f::fb(b), c); }
template <class A> inline uint32_t floorf(A a) { return ::guest::floorf(w13f::fb(a)); }
template <class A> inline uint32_t scalbnf(A a, W13F_U b = 0) { return ::guest::scalbnf(w13f::fb(a), b); }
#undef W13F_U
}  // namespace guest
