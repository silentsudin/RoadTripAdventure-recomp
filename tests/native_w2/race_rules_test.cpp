// W2-05 acceptance. ROM is read only to pin payout table 0x29FCB8.
#include "gameplay/race_rules.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <string>
using namespace rt::native::gameplay;
namespace rr=rt::native::gameplay::race_rules;
namespace {
std::string elfPath(int argc,char**argv){ if(const char* e=std::getenv("RT_ELF");e&&*e)return e; for(int i=1;i+1<argc;++i)if(std::string(argv[i])=="--elf")return argv[i+1];return {}; }
std::uint32_t le32(const std::vector<std::uint8_t>& b,std::size_t o){return std::uint32_t(b[o])|(std::uint32_t(b[o+1])<<8)|(std::uint32_t(b[o+2])<<16)|(std::uint32_t(b[o+3])<<24);}
std::uint64_t hash(std::uint64_t h,std::uint64_t v){for(int i=0;i<8;++i){h^=v>>(8*i);h*=1099511628211ull;}return h;}
struct Result {int payout=0,licence=0,records=0,wgp=0,rank=0,bands=0;std::uint64_t h=1469598103934665603ull;};
Result run(const std::vector<std::uint8_t>& elf){ Result x;
 for(std::size_t t=0;t<5;++t)for(std::size_t p=0;p<6;++p){ if(le32(elf,0x29fcb8-0x1ff000+4*(t*6+p))==rr::kPayoutLadder[t][p])++x.payout; x.h=hash(x.h,rr::payout(t,p)); }
 // 3 tiers x {all top-six, one seventh, one not raced}.
 for(std::uint8_t tier=0;tier<3;++tier)for(int vector=0;vector<3;++vector){std::array<std::uint8_t,26> a{};a.fill(0); for(std::size_t i=0;i<24;++i)if(rr::kRecords[i].tier==tier)a[i]=vector==1?6:vector==2?255:5; SaveData s;s.licence=tier; bool got=rr::awardLicenceIfEligible(s,a); if(got==(vector==0)&&s.licence==tier+(vector==0))++x.licence;x.h=hash(x.h,s.licence);}
 // Records: 24 licence + 2 WGP; IDs 26/27 must remap to 24/25.  Each player band is independent.
 SaveData save;save.bestLap.fill(kBestTimeNone);save.bestTotal.fill(kBestTimeNone); for(std::uint8_t b=0;b<3;++b)for(int id=0;id<28;++id){rr::writeResultAndTimes(save,b,id,static_cast<std::uint8_t>(id%6),1000+b*100+id,2000+b*100+id);int slot=rr::recordSlotForRaceId(id);if(slot>=0&&save.bestLap[b*26+slot]==1000u+b*100+id)++x.bands;}
 for(int i=0;i<24;++i)if(rr::kRecords[i].tier<3&&elf[0x2bddd0-0x1ff000+i*16]==rr::kRecords[i].course&&elf[0x2bddd0-0x1ff000+i*16+3]==rr::kRecords[i].tier)++x.records; x.records += rr::kRecords[24].course==17&&rr::kRecords[25].course==15&&rr::kRecords[24].tier==4&&rr::kRecords[25].tier==4&&elf[0x2bddd0-0x1ff000+24*16]==17&&elf[0x2bddd0-0x1ff000+24*16+3]==4&&elf[0x2bddd0-0x1ff000+25*16]==15&&elf[0x2bddd0-0x1ff000+25*16+3]==4?2:0;
 // Independent reference: sums then stable insertion by strictly-greater score.
 std::uint32_t seed=0x4a17u;for(int it=0;it<1000;++it){std::array<std::array<std::uint8_t,8>,7> rounds{};for(auto& row:rounds)for(auto& q:row){seed=seed*1664525u+1013904223u;q=seed%101;}auto got=rr::rankWgp(rounds,(seed%7)+1);std::array<rr::Standing,8> ref{};auto n=(seed%7)+1;for(int d=0;d<8;++d){ref[d].driver=d;for(unsigned z=0;z<n;++z)ref[d].points+=rounds[z][d];}for(int i=1;i<8;++i)for(int j=i;j>0&&ref[j].points>ref[j-1].points;--j)std::swap(ref[j],ref[j-1]);if(got==ref)++x.wgp;x.h=hash(x.h,got[0].points);}
 // Payout rank fuzz includes out-of-range, independently bounded to 0 outside table.
 for(int i=0;i<1000;++i){seed=seed*1103515245u+12345u;auto t=std::uint8_t(seed%8),p=std::uint8_t((seed>>8)%9);auto ref=t<5&&p<6?rr::kPayoutLadder[t][p]:0;if(rr::payout(t,p)==ref)++x.rank;}
 return x; }
}
int main(int argc,char**argv){auto path=elfPath(argc,argv);if(path.empty()){std::puts("NO_ELF");return 0;}std::ifstream f(path,std::ios::binary);std::vector<std::uint8_t> elf((std::istreambuf_iterator<char>(f)),{});if(elf.size()<0xa1000){std::puts("NO_ELF");return 0;}auto a=run(elf),b=run(elf);rr::LapState lap;lap.lapTimer=120;auto one=rr::recordLap(lap);lap.lapTimer=140;auto two=rr::recordLap(lap);lap.lapTimer=100;auto three=rr::recordLap(lap);const bool lapOk=one.newBest&&!two.newBest&&three.newBest&&lap.lap==3&&lap.best==100&&lap.recent==std::array<std::uint32_t,3>{120,140,100};bool deterministic=a.h==b.h;bool ok=a.payout==30&&a.licence==9&&a.records==26&&a.wgp==1000&&a.rank==1000&&a.bands==78&&lapOk&&deterministic;std::printf("W2-05 SCORE payout=%d/30 licence_vectors=%d/9 records=24+2 wgp_fuzz=%d/1000 rank_fuzz=%d/1000 bands=%d/78 deterministic=%d\n",a.payout,a.licence,a.wgp,a.rank,a.bands,deterministic?1:0);return ok?0:1;}
