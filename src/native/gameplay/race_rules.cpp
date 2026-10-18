#include "gameplay/race_rules.h"
#include <algorithm>
namespace rt::native::gameplay::race_rules {
// PIN 0x29FCB8: u32[5][6], consumed by 0x236238 / 0x236858.
const std::array<std::array<std::uint32_t, kPayoutPlaces>, kPayoutTiers> kPayoutLadder{{
 {{800,500,400,300,200,100}}, {{1500,1200,1000,800,600,500}}, {{2500,2000,1600,1200,1000,800}},
 {{80000,60000,40000,30000,20000,10000}}, {{10000,7500,5000,3000,2000,1000}}
}};
// PIN 0x2BDDD0, record stride 0x10; only course and byte +3 tier are persistent rules inputs.
const std::array<RaceRecord, kRaceRecords> kRecords{
 RaceRecord{0,0},RaceRecord{0,1},RaceRecord{1,2},RaceRecord{3,0},RaceRecord{2,1},RaceRecord{2,2},RaceRecord{4,0},RaceRecord{4,1},RaceRecord{5,1},RaceRecord{5,2},RaceRecord{7,0},RaceRecord{7,2},
 RaceRecord{12,1},RaceRecord{12,2},RaceRecord{10,0},RaceRecord{8,0},RaceRecord{9,1},RaceRecord{8,2},RaceRecord{6,1},RaceRecord{11,2},RaceRecord{13,1},RaceRecord{13,2},RaceRecord{14,1},RaceRecord{14,2},RaceRecord{17,4},RaceRecord{15,4}
};
int recordSlotForRaceId(int id) { if (id == 26) return 24; if (id == 27) return 25; return id >= 0 && id < 26 ? id : -1; }
std::uint32_t payout(std::uint8_t tier, std::uint8_t place) { return tier < kPayoutTiers && place < kPayoutPlaces ? kPayoutLadder[tier][place] : 0; }
LapSplit recordLap(LapState& s) {
  LapSplit out; out.elapsed=s.lapTimer; out.count=std::min<std::uint8_t>(s.lap,3); out.splits=s.recent;
  out.newBest=s.lapTimer < s.best; if(out.newBest) s.best=s.lapTimer;
  if(s.lap < 3) s.recent[s.lap]=s.lapTimer; else { s.recent[0]=s.recent[1]; s.recent[1]=s.recent[2]; s.recent[2]=s.lapTimer; }
  ++s.lap; s.lapTimer=0; return out;
}
bool eligibleForLicence(std::uint8_t licence, std::span<const std::uint8_t> slots) {
  if (licence >= 3 || slots.size() < kLicenceRecords) return false;
  for(std::size_t i=0;i<kLicenceRecords;++i) if(kRecords[i].tier==licence && slots[i] > 5) return false;
  return true;
}
bool awardLicenceIfEligible(SaveData& save, std::span<const std::uint8_t> slots) {
  if(!eligibleForLicence(save.licence,slots)) return false; ++save.licence; return true;
}
void writeResultAndTimes(SaveData& save, std::uint8_t band, int id, std::uint8_t place, std::uint32_t lap, std::uint32_t total) {
 const int slot=recordSlotForRaceId(id); if(band>=kPlayerBands||slot<0) return; const auto row=static_cast<std::size_t>(band)*kBandEntries+slot;
 save.raceResults[slot]=place; if(lap<save.bestLap[row]) save.bestLap[row]=lap; if(total<save.bestTotal[row]) save.bestTotal[row]=total;
}
std::array<Standing,8> rankWgp(const std::array<std::array<std::uint8_t,8>,7>& r,std::size_t n) {
 std::array<Standing,8> v{}; n=std::min<std::size_t>(n,7); for(std::size_t d=0;d<8;++d){v[d].driver=d;for(std::size_t round=0;round<n;++round)v[d].points+=r[round][d];}
 // guest selection sort swaps only on strictly greater score (0x236858); stable ties retain driver order.
 std::stable_sort(v.begin(),v.end(),[](auto a,auto b){return a.points>b.points;}); return v;
}
std::uint32_t wgpPurse(std::uint8_t tier,const std::array<Standing,8>& s,std::uint8_t player) { for(std::size_t i=0;i<s.size();++i)if(s[i].driver==player)return payout(tier,static_cast<std::uint8_t>(i)); return 0; }
} // namespace rt::native::gameplay::race_rules
