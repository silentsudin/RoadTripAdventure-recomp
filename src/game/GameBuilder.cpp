#include "GameBuilder.h"

#include "platform/Paths.h"
#include "ps2_runtime.h"

#include <dlfcn.h>
#include <fcntl.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <thread>
#include <vector>

extern char **environ;

namespace rt::game
{
    namespace fs = std::filesystem;

    namespace
    {
        constexpr const char *kLibName = "libroadtrip_game.dylib";
        constexpr size_t kUnityBatch = 24;

        fs::path libPath() { return paths::gameDir() / kLibName; }
        fs::path stampPath() { return paths::gameDir() / "build_id"; }
        fs::path sdkDir() { return paths::bundleResources() / "sdk"; }
        fs::path recompDir() { return paths::bundleResources() / "recomp"; }

        std::string readFile(const fs::path &p)
        {
            std::ifstream in(p, std::ios::binary);
            std::stringstream ss;
            ss << in.rdbuf();
            return ss.str();
        }

        std::string trim(std::string s)
        {
            while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
                s.pop_back();
            return s;
        }

        std::string bundleBuildId() { return trim(readFile(sdkDir() / "build_id")); }

        // Runs argv with stdout/stderr appended to `log`; returns the exit status (or -1).
        int run(const std::vector<std::string> &argv, const fs::path &log, const fs::path &cwd = {})
        {
            posix_spawn_file_actions_t actions;
            posix_spawn_file_actions_init(&actions);
            posix_spawn_file_actions_addopen(&actions, 1, log.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
            posix_spawn_file_actions_adddup2(&actions, 1, 2);
            if (!cwd.empty())
                posix_spawn_file_actions_addchdir_np(&actions, cwd.c_str());

            std::vector<char *> args;
            for (const auto &a : argv)
                args.push_back(const_cast<char *>(a.c_str()));
            args.push_back(nullptr);

            pid_t pid = 0;
            const int rc = posix_spawn(&pid, args[0], &actions, nullptr, args.data(), environ);
            posix_spawn_file_actions_destroy(&actions);
            if (rc != 0)
                return -1;
            int status = 0;
            if (waitpid(pid, &status, 0) < 0)
                return -1;
            return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        }

        std::string capture(const std::vector<std::string> &argv)
        {
            const fs::path tmp = fs::temp_directory_path() / ("rt_capture_" + std::to_string(getpid()));
            fs::remove(tmp);
            const int rc = run(argv, tmp);
            std::string out = trim(readFile(tmp));
            fs::remove(tmp);
            return rc == 0 ? out : std::string{};
        }

        // flags.json is written by scripts/bundle_sdk.py: {"flags": [...], "includes": [...]}.
        std::vector<std::string> jsonStringArray(const std::string &json, const std::string &key)
        {
            std::vector<std::string> out;
            const size_t k = json.find("\"" + key + "\"");
            if (k == std::string::npos)
                return out;
            const size_t open = json.find('[', k), close = json.find(']', k);
            static const std::regex str(R"re("((?:[^"\\]|\\.)*)")re");
            const std::string body = json.substr(open + 1, close - open - 1);
            for (std::sregex_iterator it(body.begin(), body.end(), str), end; it != end; ++it)
            {
                std::string v = (*it)[1].str();
                v = std::regex_replace(v, std::regex(R"(\\(.))"), "$1");
                out.push_back(v);
            }
            return out;
        }

        std::string tomlString(const fs::path &p)
        {
            std::string s = p.string(), out = "\"";
            for (char c : s)
            {
                if (c == '\\' || c == '"')
                    out += '\\';
                out += c;
            }
            return out + "\"";
        }

        // Points the bundled config at the user's ELF and our output dir.
        bool writeConfig(const fs::path &elf, const fs::path &outDir, const fs::path &dst)
        {
            std::istringstream in(readFile(recompDir() / "roadtrip.toml"));
            std::ofstream out(dst, std::ios::trunc);
            std::string line;
            bool sawInput = false, sawOutput = false;
            while (std::getline(in, line))
            {
                if (!sawInput && line.rfind("input =", 0) == 0)
                {
                    line = "input = " + tomlString(elf);
                    sawInput = true;
                }
                else if (!sawOutput && line.rfind("output =", 0) == 0)
                {
                    line = "output = " + tomlString(outDir.string() + "/");
                    sawOutput = true;
                }
                out << line << '\n';
            }
            return sawInput && sawOutput && static_cast<bool>(out);
        }
    }

    fs::path buildLog() { return paths::gameDir() / "build.log"; }

    namespace
    {
        void *g_vu1Entry = nullptr;
        uint64_t g_vu1ImageHash = 0;
    }

    void *vu1NativeEntry(uint64_t &imageHash)
    {
        imageHash = g_vu1ImageHash;
        return g_vu1Entry;
    }

    std::optional<fs::path> findCompiler()
    {
        const std::string p = capture({"/usr/bin/xcrun", "--find", "clang++"});
        if (p.empty() || !fs::exists(p))
            return std::nullopt;
        return fs::path(p);
    }

    void requestCommandLineTools()
    {
        run({"/usr/bin/xcode-select", "--install"}, "/dev/null");
    }

    bool isBuilt()
    {
        std::error_code ec;
        return fs::exists(libPath(), ec) && trim(readFile(stampPath())) == bundleBuildId() &&
               !bundleBuildId().empty();
    }

    bool build(const fs::path &elf, TaskProgress &progress)
    {
        auto fail = [&](std::string m)
        { return progress.fail(std::move(m) + "\n\nDetails: " + buildLog().string()); };

        const std::string buildId = bundleBuildId();
        if (buildId.empty())
            return fail("The app bundle is missing its build kit (Contents/Resources/sdk).");
        const auto compiler = findCompiler();
        const std::string sysroot = capture({"/usr/bin/xcrun", "--show-sdk-path"});
        if (!compiler || sysroot.empty())
            return fail("Xcode Command Line Tools are required to build the game.");

        const fs::path work = paths::gameDir() / "work";
        const fs::path generated = work / "generated";
        const fs::path objDir = work / "obj";
        std::error_code ec;
        fs::remove_all(work, ec);
        fs::create_directories(generated, ec);
        fs::create_directories(objDir, ec);
        fs::remove(buildLog(), ec);
        if (ec)
            return fail("Cannot create " + work.string() + ": " + ec.message());

        // 1. MIPS -> C++
        progress.setPhase("Translating the game's code (one-time setup)");
        progress.done = 0;
        progress.total = 1;
        const fs::path config = work / "roadtrip.toml";
        if (!writeConfig(elf, generated, config))
            return fail("Cannot write recompiler config.");
        if (run({(recompDir() / "ps2_recomp").string(), config.string()}, buildLog(), work) != 0)
            return fail("The recompiler failed.");

        // VU1 microcode -> C++. Optional: without it the VU1 interpreter runs the 3D code.
        const fs::path vu1Source = work / "vu1_native.cpp";
        const bool haveVu1 =
            fs::exists(recompDir() / "ps2_vu1_recomp") &&
            run({(recompDir() / "ps2_vu1_recomp").string(), "--elf", elf.string(), "--out", vu1Source.string()},
                buildLog(), work) == 0 &&
            fs::exists(vu1Source);

        // 2. Group generated sources into unity files (much faster to compile).
        std::vector<fs::path> sources;
        for (const auto &e : fs::directory_iterator(generated))
            if (e.path().extension() == ".cpp")
                sources.push_back(e.path());
        std::sort(sources.begin(), sources.end());
        if (sources.empty())
            return fail("The recompiler produced no code.");

        std::vector<fs::path> units;
        for (size_t i = 0; i < sources.size(); i += kUnityBatch)
        {
            const fs::path unit = work / ("unity_" + std::to_string(units.size()) + ".cpp");
            std::ofstream u(unit);
            for (size_t j = i; j < std::min(sources.size(), i + kUnityBatch); ++j)
                u << "#include " << tomlString(sources[j]) << "\n";
            units.push_back(unit);
        }
        units.push_back(sdkDir() / "game_shim.cpp");
        if (haveVu1)
            units.push_back(vu1Source);

        // 3. Compile.
        const std::string sdkJson = readFile(sdkDir() / "flags.json");
        std::vector<std::string> base{compiler->string(), "-isysroot", sysroot};
        for (auto &f : jsonStringArray(sdkJson, "flags"))
            base.push_back(f);
        for (auto &inc : jsonStringArray(sdkJson, "includes"))
            base.push_back("-I" + (sdkDir() / inc).string());
        base.push_back("-I" + generated.string());
        base.push_back("-w");
        for (const char *sym : {"g_ps2RecompiledFunctionTable", "g_ps2RecompiledFunctionTableBase",
                                "g_ps2RecompiledFunctionTableEnd", "g_ps2RecompiledFunctionTableSlotCount"})
            base.push_back(std::string("-D") + sym + "=rt_game_" + (sym + 2));
        base.push_back("-DRT_GAME_BUILD_ID=\"" + buildId + "\"");
        // Debugging: RT_GAME_EXTRA_CFLAGS adds compiler flags (space-separated), e.g. to build
        // with -DPS2_FUNCTION_LOG_TRACKER and a custom PS_LOG_ENTRY via -include.
        if (const char *extra = std::getenv("RT_GAME_EXTRA_CFLAGS"))
        {
            std::istringstream words(extra);
            for (std::string w; words >> w;)
                base.push_back(w);
        }

        progress.setPhase("Compiling the game for your Mac (one-time setup)");
        progress.done = 0;
        progress.total = units.size();
        std::atomic<size_t> next{0};
        std::atomic<bool> compileFailed{false};
        std::vector<fs::path> objects(units.size());
        auto worker = [&]
        {
            for (size_t i; !compileFailed && (i = next++) < units.size();)
            {
                objects[i] = objDir / ("u" + std::to_string(i) + ".o");
                std::vector<std::string> argv = base;
                argv.insert(argv.end(), {"-c", units[i].string(), "-o", objects[i].string()});
                if (run(argv, buildLog()) != 0)
                    compileFailed = true;
                ++progress.done;
            }
        };
        const unsigned jobs = std::max(1u, std::thread::hardware_concurrency());
        std::vector<std::thread> pool;
        for (unsigned j = 0; j < jobs; ++j)
            pool.emplace_back(worker);
        for (auto &t : pool)
            t.join();
        if (compileFailed)
            return fail("Compiling the game failed.");

        // 4. Link. Runtime symbols resolve against the app executable at load time.
        progress.setPhase("Linking");
        const fs::path tmpLib = work / kLibName;
        std::vector<std::string> link{compiler->string(), "-isysroot", sysroot, "-dynamiclib", "-undefined", "dynamic_lookup",
                                      "-install_name", "@rpath/" + std::string(kLibName), "-o", tmpLib.string()};
        for (auto &f : jsonStringArray(sdkJson, "flags"))
            if (f.rfind("-arch", 0) == 0 || f.rfind("-mmacosx-version-min", 0) == 0 || f == "arm64" || f == "x86_64")
                link.push_back(f);
        for (auto &o : objects)
            link.push_back(o.string());
        if (run(link, buildLog()) != 0)
            return fail("Linking the game failed.");

        fs::rename(tmpLib, libPath(), ec);
        if (ec)
            return fail("Cannot install the game library: " + ec.message());
        std::ofstream(stampPath(), std::ios::trunc) << buildId << "\n";
        const char *keep = std::getenv("RT_KEEP_GAME_WORK");
        if (!(keep && *keep == '1'))
            fs::remove_all(work, ec); // generated code + objects are no longer needed
        return true;
    }

    bool load(std::string &error)
    {
        void *handle = dlopen(libPath().c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!handle)
        {
            error = dlerror();
            return false;
        }
        using RegisterFn = int (*)(PS2Runtime::RecompiledFunction *, uint32_t, uint32_t);
        using BuildIdFn = const char *(*)();
        auto reg = reinterpret_cast<RegisterFn>(dlsym(handle, "rt_game_register"));
        auto id = reinterpret_cast<BuildIdFn>(dlsym(handle, "rt_game_build_id"));
        if (!reg || !id)
        {
            error = "game library is missing its registration entry point";
            return false;
        }
        if (bundleBuildId() != id())
        {
            error = "game library was built by a different app version";
            return false;
        }
        const int n = reg(g_ps2RecompiledFunctionTable, g_ps2RecompiledFunctionTableBase,
                          g_ps2RecompiledFunctionTableSlotCount);
        if (n <= 0)
        {
            error = "game library registered no functions";
            return false;
        }
        std::cout << "[roadtrip] loaded " << libPath() << " (" << n << " function entries)\n";

        g_vu1Entry = dlsym(handle, "rt_vu1_native_execute");
        if (auto hash = reinterpret_cast<uint64_t (*)()>(dlsym(handle, "rt_vu1_native_image_hash")))
            g_vu1ImageHash = hash();
        if (g_vu1Entry)
            std::cout << "[roadtrip] native VU1 microcode available\n";
        return true;
    }
}
