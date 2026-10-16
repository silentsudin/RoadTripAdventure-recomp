#include "runtime/guest_facade.h"
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <regex>
#include <sstream>
namespace rt::native::runtime {
GuestFacade::GuestFacade(gameplay::GameSession& s, gameplay::FrameSnapshot& f, std::function<void(int)> w) : session_(s), snapshot_(f), warp_(std::move(w)) {}
static void put(std::vector<std::uint8_t>& out,std::size_t at,const void* p,std::size_t n){if(at<out.size())std::memcpy(out.data()+at,p,std::min(n,out.size()-at));}
std::vector<std::uint8_t> GuestFacade::read(std::uint32_t a,std::size_t n,std::string& e) const {
 std::vector<std::uint8_t> out(n); e.clear();
 if(a==kRacePlace){std::uint32_t v=static_cast<std::uint32_t>(snapshot_.rank);put(out,0,&v,4);return out;}
 if(a==kClock){put(out,0,&snapshot_.clock,4);return out;}
 if(a==kGravity){const std::int8_t g=-0x59;put(out,0,&g,1);return out;}
 if(a==kVibration){put(out,0,&rumble_,1);return out;}
 if(a>=kCars && a<kCars+0x270){ const std::uint32_t off=a-kCars; if(off<12){float p[3]={snapshot_.carPosition.x,0.f,snapshot_.carPosition.z};put(out,0,reinterpret_cast<const std::uint8_t*>(p)+off,std::min(n,static_cast<std::size_t>(12u-off)));} return out; }
 e="unmapped guest address"; return out;
}
bool GuestFacade::write(std::uint32_t a,const std::vector<std::uint8_t>& d,std::string& e){e.clear();if(a==kVibration&&!d.empty()){rumble_=d[0];return true;} if(a==0x00335910&&d.size()>=4){int id;std::memcpy(&id,d.data(),4);setTile(id);return true;} e="write rejected: no native model route";return false;}
void GuestFacade::setTile(int id){if(id!=11&&id!=43)return;tile_=id;if(warp_)warp_(id);}
std::string GuestFacade::command(const std::string& cmd, const std::string& line) {
 auto text=[&](const char* key){std::smatch m;std::regex r(std::string("\"")+key+"\"\\s*:\\s*(\"([^\"]*)\"|[0-9]+)");return std::regex_search(line,m,r)?(m[2].matched?m[2].str():m[1].str()):std::string{};};
 auto num=[&](const char* key){try{return static_cast<std::uint32_t>(std::stoul(text(key),nullptr,0));}catch(...){return 0u;}};
 if(cmd=="read") { std::string err;auto bytes=read(num("addr"),num("len"),err);std::ostringstream h;for(auto b:bytes)h<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(b);return err.empty()?"{\"ok\":true,\"data\":\""+h.str()+"\"}\n":"{\"ok\":false,\"error\":\""+err+"\"}\n"; }
 if(cmd=="write") { std::string hex=text("data"),err;std::vector<std::uint8_t>b; if(hex.size()%2)return "{\"ok\":false,\"error\":\"bad hex\"}\n";for(size_t i=0;i<hex.size();i+=2)b.push_back(static_cast<std::uint8_t>(std::stoul(hex.substr(i,2),nullptr,16)));return write(num("addr"),b,err)?"{\"ok\":true}\n":"{\"ok\":false,\"error\":\""+err+"\"}\n"; }
 if(cmd=="map") return "{\"ok\":true,\"tile\":"+std::to_string(tile_)+"}\n";
 if(cmd=="state_slot" || cmd=="state_slots" || cmd=="texture_pack") return "{\"ok\":false,\"error\":\"native facade feature not implemented\"}\n";
 return "{\"ok\":false,\"error\":\"unknown native command\"}\n";
}
} // namespace rt::native::runtime
