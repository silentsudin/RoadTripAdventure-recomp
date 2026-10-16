#include "runtime/testsocket.h"

#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <regex>
#include <string>
#include <thread>

#ifndef _WIN32
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

namespace rt::native::runtime {
namespace {
std::string field(const std::string& s, const char* name, const std::string& fallback = {}) {
    const std::regex r(std::string("\"") + name + "\"\\s*:\\s*(\"([^\"]*)\"|[-0-9]+|true|false)");
    std::smatch m; if (!std::regex_search(s, m, r)) return fallback;
    return m[2].matched ? m[2].str() : m[1].str();
}
long number(const std::string& s, const char* name, long fallback = 0) {
    try { return std::stol(field(s, name, std::to_string(fallback)), nullptr, 0); } catch (...) { return fallback; }
}
std::string ok(const std::string& extra = {}) { return std::string("{\"ok\":true") + extra + "}\n"; }
std::string error(const char* message) { return std::string("{\"ok\":false,\"error\":\"") + message + "\"}\n"; }
}

struct TestSocket::Impl {
    std::atomic<bool> enabled{false}, quit{false};
    std::atomic<int> requested{0};
    std::atomic<std::uint64_t> vblank{0};
    mutable std::mutex mutex;
    std::condition_variable stepped;
    TestPad pad{};
    Command command;
    std::thread thread;
#ifndef _WIN32
    int listener = -1;
#endif
};

TestSocket::TestSocket() : impl_(new Impl) {}
TestSocket::~TestSocket() { stop(); delete impl_; }
bool TestSocket::enabled() const { return impl_->enabled.load(); }
bool TestSocket::lockstep() const { return enabled(); }
std::uint64_t TestSocket::vblank() const { return impl_->vblank.load(); }
TestPad TestSocket::pad() const { std::lock_guard lock(impl_->mutex); return impl_->pad; }

bool TestSocket::startIfRequested(Command command) {
    const char* env = std::getenv("RT_TEST_SOCKET");
#ifdef __ANDROID__
    const std::string spec = env ? env : "@roadtrip-test";
#else
    if (!env || !*env) return false;
    const std::string spec(env);
#endif
    impl_->command = std::move(command);
#ifdef _WIN32
    (void)spec; return false;
#else
    int fd = ::socket(AF_INET, SOCK_STREAM, 0); int port = 0;
    if (spec.rfind("tcp:", 0) == 0) {
        port = std::atoi(spec.c_str() + 4);
        sockaddr_in a{}; a.sin_family = AF_INET; a.sin_addr.s_addr = htonl(INADDR_LOOPBACK); a.sin_port = htons(port);
        int one = 1; setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
        if (bind(fd, reinterpret_cast<sockaddr*>(&a), sizeof(a)) != 0) { close(fd); return false; }
    } else if (!spec.empty() && spec[0] == '@') {
        sockaddr_un a{}; a.sun_family = AF_UNIX; a.sun_path[0] = '\0';
        std::strncpy(a.sun_path + 1, spec.c_str() + 1, sizeof(a.sun_path) - 2);
        if (bind(fd, reinterpret_cast<sockaddr*>(&a), sizeof(sa_family_t) + spec.size() - 1) != 0) { close(fd); return false; }
    } else { close(fd); return false; }
    if (listen(fd, 1) != 0) { close(fd); return false; }
    impl_->listener = fd; impl_->enabled = true;
    impl_->thread = std::thread([this] {
        while (!impl_->quit) {
            const int client = accept(impl_->listener, nullptr, nullptr); if (client < 0) break;
            std::string line; char c;
            while (!impl_->quit && recv(client, &c, 1, 0) == 1) {
                if (c != '\n') { if (line.size() < 65536) line += c; continue; }
                const std::string cmd = field(line, "cmd"); std::string reply;
                if (cmd == "hello" || cmd == "version") reply = ok(",\"version\":\"native-facade-1\"");
                else if (cmd == "pad") { std::lock_guard lock(impl_->mutex); impl_->pad.buttons = static_cast<std::uint16_t>(number(line,"buttons",0xffff)); impl_->pad.lx=number(line,"lx",128); impl_->pad.ly=number(line,"ly",128); impl_->pad.rx=number(line,"rx",128); impl_->pad.ry=number(line,"ry",128); impl_->pad.connected=number(line,"connected",1)!=0; reply=ok(); }
                else if (cmd == "release_pad") { std::lock_guard lock(impl_->mutex); impl_->pad = {}; reply=ok(); }
                else if (cmd == "run" || cmd == "step") { const int n=std::max(0L,number(line,"vblanks",1)); const auto goal=impl_->vblank.load()+static_cast<std::uint64_t>(n); impl_->requested.fetch_add(n); std::unique_lock lock(impl_->mutex); impl_->stepped.wait(lock,[&]{return impl_->quit || impl_->vblank.load()>=goal;}); reply=ok(",\"vblank\":"+std::to_string(impl_->vblank.load())+(cmd=="step" ? ",\"data\":[]" : "")); }
                else if (cmd == "quit") { reply=ok(); impl_->quit=true; }
                else if (impl_->command) reply=impl_->command(cmd,line);
                else reply=error("unknown command");
                send(client, reply.data(), reply.size(), 0); line.clear();
            }
            close(client);
        }
    });
    return true;
#endif
}
bool TestSocket::takeStep() { int n=impl_->requested.load(); while(n>0 && !impl_->requested.compare_exchange_weak(n,n-1)){} return n>0; }
void TestSocket::completeStep() { impl_->vblank.fetch_add(1); impl_->stepped.notify_all(); }
void TestSocket::stop() {
    if (!impl_ || !impl_->enabled.exchange(false)) return;
    impl_->quit=true; impl_->stepped.notify_all();
#ifndef _WIN32
    if (impl_->listener>=0) { shutdown(impl_->listener, SHUT_RDWR); close(impl_->listener); impl_->listener=-1; }
#endif
    if (impl_->thread.joinable()) impl_->thread.join();
}
} // namespace rt::native::runtime
