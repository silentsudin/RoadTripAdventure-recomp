#include "gameplay/event_bridge.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

using namespace rt::native::gameplay::events;
namespace fs = std::filesystem;
static int checks = 0;
static void require(bool b, const char* what) {
    ++checks;
    if (!b) { std::cerr << "FAIL " << what << '\n'; std::exit(1); }
}
static fs::path findRom() {
    if (const char* p = std::getenv("RT_ELF"); p && fs::is_regular_file(p)) return p;
    if (const char* h = std::getenv("HOME")) {
        auto p = fs::path(h) / "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
        if (fs::is_regular_file(p)) return p;
    }
    if (fs::is_directory("build/scratch")) {
        std::vector<fs::path> candidates;
        for (const auto& e : fs::directory_iterator("build/scratch")) {
            auto p = e.path() / "disc/SLUS_203.98";
            if (fs::is_regular_file(p)) candidates.push_back(p);
        }
        std::sort(candidates.begin(), candidates.end());
        if (!candidates.empty()) return candidates.front();
    }
    return {};
}
int main() {
    // Changing 0x82's width, interpreting a hold as radio, replaying drained
    // events, or advancing on backpressure must fail these contracts.
    const std::uint8_t authored[] = {0x80,0x82,0x91,0x83,0x84,0x85,0x8a,0x8b,0xff};
    const Type expected[] = {Type::SmallOn,Type::LargeValue,Type::LargeOff,
        Type::LargeInitial,Type::Hold,Type::Hold,Type::Hold,Type::End};
    EventBridge q;
    std::size_t cursor = 0;
    auto result = q.pump(Dialect::Rumble, authored, cursor, 0x1000);
    require(result.status == Status::End && cursor == sizeof(authored), "consume stream");
    std::vector<Event> got;
    Callbacks cb;
    cb.onEvent = [&](const Event& e) { got.push_back(e); };
    require(q.drain(cb) == 8, "drain count");
    require(q.drain(cb) == 0, "dispatch once");
    for (std::size_t i=0;i<got.size();++i) require(got[i].type == expected[i], "event sequence");
    require(got[1].width == 2 && got[1].payloadSize == 1 && got[1].payload[0] == 0x91,
            "raw operand");
    require(got[1].valueKnown && !got[3].valueKnown, "literal known, initial strength UNKNOWN");
    require(got[5].opcode == 0x8a && got[5].width == 1, "0x8a is hold, not radio");
    const std::uint8_t truncated[] = {0x82};
    cursor=0;
    auto bad = q.pump(Dialect::Rumble,truncated,cursor);
    require(bad.status == Status::Truncated && cursor==0 && bad.logOnce, "truncated stops");
    require(!q.pump(Dialect::Rumble,truncated,cursor).logOnce, "diagnostic once");
    cursor=0;
    auto unknown = q.pump(Dialect::TownPrefix,authored,cursor);
    require(unknown.status == Status::Unsupported && cursor==0 && unknown.logOnce,
            "town high byte never executed");
    require(unknown.record.type == Type::Unknown && unknown.record.width == 0,
            "unknown width not guessed");
    require(!q.pump(Dialect::TownPrefix,authored,cursor).logOnce, "unknown logs once");
    for (std::size_t i=0;i<EventBridge::kCapacity;++i)
        require(q.enqueue(Type::Radio,static_cast<std::uint32_t>(i%3)), "bounded fill");
    cursor=0;
    require(q.pump(Dialect::Rumble,authored,cursor).status == Status::QueueFull && cursor==0,
            "backpressure retains cursor");
    require(!q.enqueue(Type::Countdown,1), "overflow rejected");
    int radio=0, countdown=0, camera=0;
    cb.onRadio=[&](std::uint32_t){++radio;};
    cb.onCountdown=[&](bool b){if(b) ++countdown;};
    cb.onCameraPreset=[&](std::uint32_t k){if(k==2) ++camera;};
    q.drain(cb);
    require(radio==static_cast<int>(EventBridge::kCapacity), "radio callback explicit native events");
    require(q.enqueue(Type::Countdown,1) && q.enqueue(Type::CameraPreset,2), "native triggers");
    const std::uint8_t subtitle[]={'G','O'};
    require(q.enqueue(Type::Subtitle,0,subtitle), "subtitle owned bytes");
    require(q.enqueue(Type::FlagSet,42) && q.enqueue(Type::FlagClear,42), "flag records");
    require(q.drain(cb)==5 && countdown==1 && camera==1, "native callback sequence");
    std::vector<std::uint8_t> tooBig(Event::kMaxPayload+1);
    require(!q.enqueue(Type::Subtitle,0,tooBig), "oversized payload rejected");
    // Drain snapshots its initial size and rejects recursive drain calls.
    q.enqueue(Type::Radio,1);
    cb.onRadio=[&](std::uint32_t){ require(q.drain(cb)==0,"no recursive dispatch");
        require(q.enqueue(Type::Countdown,1),"callback enqueue"); };
    require(q.drain(cb)==1 && q.size()==1, "callback enqueue deferred to next frame");
    q.drain(cb);
    cursor=0;
    require(q.pump(Dialect::Rumble,authored,cursor,0,1).status==Status::StepLimit && cursor==1,
            "bounded operation budget");
    q.drain({});
    for (unsigned op=0x80;op<=0x8b;++op) {
        const std::uint8_t command[]={static_cast<std::uint8_t>(op),0x42};
        cursor=0;
        auto one=q.pump(Dialect::Rumble,command,cursor,0,1);
        require(one.records==1 && cursor==(op==0x82 ? 2u : 1u), "all high-op widths");
        Event decoded;
        Callbacks sink; sink.onEvent=[&](const Event&e){decoded=e;};
        require(q.drain(sink)==1,"all high-op records");
        if(op>=0x85) require(decoded.type==Type::Hold && decoded.value==op,
                            "85..8b are raw hold bytes");
    }

    auto path=findRom();
    if(path.empty()) {
        std::cout << "SCORE w3_script_bridge_test synthetic=PASS ROM=NO_ELF SKIP\n";
        return 0;
    }
    std::ifstream f(path,std::ios::binary);
    std::vector<std::uint8_t> rom((std::istreambuf_iterator<char>(f)),{});
    auto byte=[&](std::uint32_t a){require(a>=0x1ff000 && a-0x1ff000<rom.size(),"ROM range");
        return rom[a-0x1ff000];};
    auto word=[&](std::uint32_t a){std::uint32_t w=0;for(int k=0;k<4;++k) w|=std::uint32_t(byte(a+k))<<(8*k);return w;};
    require(rom.size()==1280576,"ROM size");
    require(word(0x23b140)==0x2c420020,"town VM stops at >=0x20");
    require(word(0x20a710)==0x1072001b && word(0x20a784)==0xacc20000,
            "rumble 82 handler advances extra byte");
    require(word(0x20a7e0)==0x90430000 && word(0x20a7e4)==0xa1230000,
            "rumble default is hold");
    require(word(0x21ce48)==0x30c20004 && word(0x21ce94)==0x0082001a,
            "purported countdown is ratio logic");
    // All eight actual pattern pointers @0x29AD60, live extraction only.
    // First real sequence @0x2EA3D8: small-on, initial large, hold 2,
    // small-off, hold 6, end. Other patterns cover literal 0x82 payloads.
    std::size_t records=0;
    for(unsigned pattern=0;pattern<8;++pattern) {
        const auto start=word(0x29ad60+4*pattern);
        require(start==0x2ea3d0+8*pattern,"real pattern address");
        std::span<const std::uint8_t> stream(rom.data()+start-0x1ff000,8);
        cursor=0;
        EventBridge real;
        require(real.pump(Dialect::Rumble,stream,cursor,start).status==Status::End,"real pattern end");
        std::vector<Event> events;
        Callbacks sink; sink.onEvent=[&](const Event&e){events.push_back(e);};
        records+=real.drain(sink);
        require(real.drain(sink)==0,"real dispatch once");
        std::size_t off=0;
        for(const auto&e:events) {
            require(e.address==start+off && e.opcode==byte(start+off),"real address/opcode");
            off+=e.width;
        }
        require(off==cursor && events.back().type==Type::End,"real exact consumed width");
        if(pattern==1) {
            const Type types[]={Type::SmallOn,Type::LargeInitial,Type::Hold,Type::SmallOff,Type::Hold,Type::End};
            require(events.size()==6 && cursor==6,"oracle sequence length @2ea3d8");
            for(unsigned i=0;i<6;++i) require(events[i].type==types[i],"real oracle sequence");
        }
        if(pattern==7) require(events[0].payload[0]==0x7f && events[0].width==2 &&
            events[1].opcode==30,"real literal large intensity/hold @2ea408");
    }
    std::cout << "SCORE w3_script_bridge_test checks=" << checks
              << " patterns=8 records=" << records
              << " vaddrs=0x2EA3D0..0x2EA40B deterministic=1 PASS\n";
}
