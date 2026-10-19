// guest:0x00272868 translated from Ghidra pseudocode and VU0 macro disassembly
#include "../guest_vu0.h"
namespace guest { using namespace rtguest::vu0;
void mat4InvertAffine(uint32_t out, uint32_t in) {
 const Vu4 r0=lqc2(in),r1=lqc2(in+16),r2=lqc2(in+32),t=lqc2(in+48);
 const Vu4 c0{{r0.v[0],r1.v[0],r2.v[0],0}},c1{{r0.v[1],r1.v[1],r2.v[1],0}},c2{{r0.v[2],r1.v[2],r2.v[2],0}};
 sqc2(out,c0); sqc2(out+16,c1); sqc2(out+32,c2); S.vf[5]=t; S.vf[6]=c0; S.vf[7]=c1; S.vf[8]=c2; S.vf[9]={{0,0,0,t.v[3]}};
 vmulabc(0b1110,0,6,5); vmaddabc(0b1110,1,7,5); vmaddbc(0b1110,2,4,8,5); vsub(0b1110,4,9,4); sqc2(out+48,S.vf[4]); }
RT_GUEST_FN(0x00272868, guest::mat4InvertAffine) }
