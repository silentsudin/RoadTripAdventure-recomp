#include "StateSlots.h"

#include "game/GameStats.h"
#include "platform/Host.h"
#include "platform/Paths.h"
#include "settings/Capabilities.h"

#include "ps2_runtime.h"
#include "raylib.h"
#include "runtime/ps2_host_presenter.h"
#include "runtime/ps2_save_state.h"
#include "runtime/ps2_test_harness.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

namespace fs = std::filesystem;
namespace ss = ps2_save_state;

namespace rt::states
{
    namespace
    {
        constexpr const char *kThumbnailKey = "thumbnail";
        constexpr int kThumbHeight = 180;            // the picture in a state's header
        constexpr double kPictureTimeoutSeconds = 1.5; // no picture by then: saved without one
        constexpr uint32_t kLiveMaxVblanks = 600;    // not paused (tests, a load at startup)

        std::mutex g_mutex;
        Status g_status;
        // The operation in flight.
        uint64_t g_resultSeq = 0, g_writeSeq = 0;
        uint64_t g_lastStepVblank = UINT64_MAX, g_startVblank = 0;
        uint32_t g_settle = 0;
        std::chrono::steady_clock::time_point g_pictureDeadline;
        float g_pictureAspect = 4.0f / 3.0f;
        std::string g_thumbnailPng; // for the next save's header
        std::string g_checkError;   // the load check's verdict (worker)
        int g_checkState = 0;       // 0 running, 1 accepted, 2 refused
        // Slot headers, re-read when a file changes.
        std::map<int, Slot> g_cache;

        double secondsSince(std::chrono::steady_clock::time_point t)
        {
            return std::chrono::duration<double>(std::chrono::steady_clock::now() - t).count();
        }

        fs::path statesDir() { return rt::paths::dataRoot() / "states"; }

        // The picture of the moment as a PNG `kThumbHeight` high at the shape it is shown at.
        std::string encodeThumbnail(std::vector<uint8_t> rgba, uint32_t w, uint32_t h, float aspect)
        {
            if (rgba.empty() || w == 0 || h == 0 || rgba.size() < size_t(w) * h * 4)
                return {};
            for (size_t i = 3; i < rgba.size(); i += 4)
                rgba[i] = 0xFF; // the GS's alpha is not the picture's
            Image src{rgba.data(), static_cast<int>(w), static_cast<int>(h), 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
            Image img = ImageCopy(src);
            const int th = kThumbHeight;
            const int tw = std::clamp(static_cast<int>(std::lround(th * aspect)), th, th * 4);
            ImageResize(&img, tw, th);
            int size = 0;
            unsigned char *png = ExportImageToMemory(img, ".png", &size);
            UnloadImage(img);
            std::string out;
            if (png && size > 0)
                out.assign(reinterpret_cast<const char *>(png), static_cast<size_t>(size));
            if (png)
                MemFree(png);
            return out;
        }

        bool decodeThumbnail(const std::string &png, Slot &slot)
        {
            if (png.empty())
                return false;
            Image img = LoadImageFromMemory(".png", reinterpret_cast<const unsigned char *>(png.data()), static_cast<int>(png.size()));
            if (!img.data)
                return false;
            ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
            slot.thumbWidth = img.width;
            slot.thumbHeight = img.height;
            const auto *p = static_cast<const uint8_t *>(img.data);
            slot.thumbnail.assign(p, p + size_t(img.width) * img.height * 4);
            UnloadImage(img);
            return true;
        }

        uint64_t fileStamp(const fs::path &p)
        {
            std::error_code ec;
            const auto size = fs::file_size(p, ec);
            if (ec)
                return 0;
            const auto t = fs::last_write_time(p, ec).time_since_epoch().count();
            return static_cast<uint64_t>(t) * 1000003u ^ size;
        }

        Slot readSlot(int index)
        {
            Slot s;
            s.index = index;
            const fs::path path = slotPath(index);
            std::error_code ec;
            if (!fs::exists(path, ec))
                return s;
            s.empty = false;
            s.stamp = fileStamp(path);
            ss::FileInfo info;
            std::string error;
            if (!ss::readStateFileInfo(path.string(), info, error))
            {
                s.problem = friendlyLoadError(error);
                return s;
            }
            s.readable = true;
            s.savedUnixTime = info.savedUnixTime;
            s.place = info.meta("place");
            s.mode = info.meta("mode");
            s.raceMode = info.meta("race_mode");
            s.player = info.meta("player");
            decodeThumbnail(info.meta(kThumbnailKey), s);
            return s;
        }

        std::string busyMessage(const std::string &blockers)
        {
            if (blockers.find("mpeg") != std::string::npos)
                return "A movie is playing. Try again once it ends.";
            if (blockers.find("memory_card") != std::string::npos)
                return "The game is using the memory card. Try again in a moment.";
            return "The game is busy right now. Try again in a moment.";
        }

        // Locked. Ends the operation.
        void finishLocked(bool ok, std::string message, std::string error = {})
        {
            g_status.phase = ok ? Phase::Done : Phase::Failed;
            g_status.message = std::move(message);
            g_status.error = std::move(error);
            ++g_status.sequence;
            g_cache.erase(g_status.slot); // re-read
            std::cout << "[states] " << (g_status.saving ? "save to" : "load from") << " slot " << g_status.slot << ": "
                      << (ok ? "ok" : "failed") << (g_status.error.empty() ? "" : " (" + g_status.error + ")") << " after "
                      << g_status.vblanks << " vblanks\n";
        }

        void startWaitingLocked()
        {
            g_status.phase = Phase::Waiting;
            g_lastStepVblank = UINT64_MAX;
            g_startVblank = ps2_test::currentVblank();
            g_status.vblanks = 0;
        }

        // Locked. While the menu holds the game, let it through one vblank at a time. Returns
        // false once the limit is reached.
        bool stepLocked(uint32_t limitPaused)
        {
            const uint64_t now = ps2_test::currentVblank();
            if (ps2_test::paused())
            {
                if (g_lastStepVblank == UINT64_MAX || now != g_lastStepVblank)
                {
                    if (g_status.vblanks >= limitPaused)
                        return false;
                    g_lastStepVblank = now;
                    ++g_status.vblanks;
                    ps2_test::stepPaused(1);
                }
                return true;
            }
            g_status.vblanks = static_cast<uint32_t>(now - std::min(now, g_startVblank));
            return g_status.vblanks <= kLiveMaxVblanks;
        }
    }

    std::string Slot::title() const
    {
        if (empty)
            return "Empty";
        if (!readable)
            return "Can't be read";
        if (!place.empty())
            return place;
        if (mode == "race")
            return "A race";
        return player.empty() ? std::string("Road Trip") : player + "'s Adventure";
    }

    std::string Slot::detail() const
    {
        if (empty)
            return "Nothing saved here yet";
        if (!readable)
            return problem;
        if (mode == "race")
            return raceMode == "quick" ? "Quick Race" : raceMode == "2p" ? "2 Player race" : "Adventure race";
        if (mode == "town")
            return "Adventure, driving";
        return "Adventure";
    }

    std::string Slot::when() const
    {
        // The day and the time of day it was saved, never a bare "00:47", which reads like the
        // race timers next to it.
        if (empty || !readable || savedUnixTime <= 0)
            return {};
        const std::time_t t = static_cast<std::time_t>(savedUnixTime), now = std::time(nullptr);
        std::tm at{}, today{};
        localtime_r(&t, &at);
        localtime_r(&now, &today);
        char clock[16];
        std::strftime(clock, sizeof(clock), "%H:%M", &at);
        std::tm yesterday = today;
        yesterday.tm_mday -= 1;
        std::mktime(&yesterday);
        if (at.tm_year == today.tm_year && at.tm_yday == today.tm_yday)
            return std::string("Today at ") + clock;
        if (at.tm_year == yesterday.tm_year && at.tm_yday == yesterday.tm_yday)
            return std::string("Yesterday at ") + clock;
        char day[32];
        std::strftime(day, sizeof(day), at.tm_year == today.tm_year ? "%e %b" : "%e %b %Y", &at);
        std::string d = day;
        if (!d.empty() && d[0] == ' ')
            d.erase(0, 1);
        return d + " at " + clock;
    }

    std::string slotPath(int index) { return (statesDir() / ("slot" + std::to_string(index) + ".rtstate")).string(); }

    std::vector<Slot> slots()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::vector<Slot> out;
        for (int i = 1; i <= kSlots; ++i)
        {
            auto it = g_cache.find(i);
            const uint64_t stamp = fileStamp(slotPath(i));
            if (it == g_cache.end() || it->second.stamp != stamp)
                it = g_cache.insert_or_assign(i, readSlot(i)).first;
            out.push_back(it->second);
        }
        return out;
    }

    bool available(PS2Runtime &runtime, std::string *why)
    {
        const auto no = [why](const char *reason)
        {
            if (why)
                *why = reason;
            return false;
        };
        if (!rt::settings::capabilities().saveStates)
            return no("this GS can't save states yet");
        if (ss::gameIdentity() == 0)
            return no("unknown game");
        if (ss::moviePlaying())
            return no("a movie is playing");
        const rt::game::Stats st = rt::game::readStats(runtime);
        if (st.demo || !(st.adventure || st.racing))
            return no("not in a game");
        return true;
    }

    bool beginSave(int index)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (index < 1 || index > kSlots || (g_status.phase != Phase::Idle && g_status.phase != Phase::Done && g_status.phase != Phase::Failed))
            return false;
        g_status = Status{Phase::Picture, true, index, false, {}, {}, 0, g_status.sequence};
        g_thumbnailPng.clear();
        g_pictureDeadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(static_cast<int>(kPictureTimeoutSeconds * 1000));
        // The picture's shape as shown (widescreen, or 4:3 for 2D screens).
        g_pictureAspect = 4.0f / 3.0f;
        if (PS2Runtime *rt = rt::host::runtime())
            if (ps2x::HostPresenter *p = rt->presenter())
                g_pictureAspect = ps2x::pictureAspect(*rt, *p);
        ps2_test::requestAppFrameCapture();
        return true;
    }

    bool beginLoad(int index, bool checkNow)
    {
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            if (index < 1 || index > kSlots ||
                (g_status.phase != Phase::Idle && g_status.phase != Phase::Done && g_status.phase != Phase::Failed))
                return false;
            g_status = Status{Phase::Checking, false, index, false, {}, {}, 0, g_status.sequence};
            g_checkState = 0;
            g_resultSeq = ss::lastResult().sequence; // before the request: it may be served at once
        }
        // Reading and checking a state (tens of MB, decompressed) takes a moment: off this thread.
        auto check = [index]
        {
            std::string error;
            const bool ok = ss::requestLoadFile(slotPath(index), error);
            std::lock_guard<std::mutex> lock(g_mutex);
            g_checkError = error;
            g_checkState = ok ? 1 : 2;
        };
        if (checkNow)
            check();
        else
            std::thread(check).detach();
        update();
        return true;
    }

    void update()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        switch (g_status.phase)
        {
        case Phase::Picture:
        {
            std::vector<uint8_t> rgba;
            uint32_t w = 0, h = 0;
            const bool have = ps2_test::takeAppFrameCapture(rgba, w, h);
            if (!have && std::chrono::steady_clock::now() < g_pictureDeadline)
                return;
            g_thumbnailPng = have ? encodeThumbnail(std::move(rgba), w, h, g_pictureAspect) : std::string();
            if (g_thumbnailPng.empty())
                std::cout << "[states] no picture of the moment (" << (have ? "empty" : "none in time") << ", " << w << "x" << h
                          << "): saving without one\n";
            std::error_code ec;
            fs::create_directories(statesDir(), ec);
            g_resultSeq = ss::lastResult().sequence;
            g_writeSeq = ss::lastWrite().sequence;
            ss::requestSaveFile(slotPath(g_status.slot));
            startWaitingLocked();
            [[fallthrough]];
        }
        case Phase::Waiting:
        {
            const ss::Result r = ss::lastResult();
            if (r.sequence != g_resultSeq)
            {
                if (g_status.saving)
                {
                    if (!r.ok)
                    {
                        // Something the loop-top check doesn't see was half-way (a VU1
                        // microprogram): ask again at a later point, within the same limit.
                        if (stepLocked(kMaxVblanks))
                        {
                            std::cout << "[states] save refused here (" << r.error << "): trying a later point\n";
                            g_resultSeq = r.sequence;
                            ss::requestSaveFile(slotPath(g_status.slot));
                            return;
                        }
                        g_thumbnailPng.clear();
                        return finishLocked(false, "Couldn't save here. Try again in a moment.", r.error);
                    }
                    g_status.phase = Phase::Writing;
                    return;
                }
                if (!r.ok)
                    return finishLocked(false,
                                        r.rolledBack ? "This state couldn't be loaded. The game carries on as it was."
                                                     : "This state couldn't be loaded.",
                                        r.error);
                // The game re-reads the memory cards (they may hold other saves by now).
                ss::markMemoryCardsChanged();
                g_status.phase = Phase::Settling;
                g_settle = 0;
                g_lastStepVblank = UINT64_MAX;
                return;
            }
            if (!stepLocked(kMaxVblanks))
            {
                ss::cancel();
                g_thumbnailPng.clear();
                return finishLocked(false, busyMessage(r.blockers), "no savable point (" + r.blockers + ")");
            }
            return;
        }
        case Phase::Checking:
            if (g_checkState == 0)
                return;
            if (g_checkState == 2)
            {
                g_status.refused = true;
                return finishLocked(false, friendlyLoadError(g_checkError), g_checkError);
            }
            // requestLoadFile has asked already; a result may even be in by now.
            g_status.phase = Phase::Waiting;
            g_lastStepVblank = UINT64_MAX;
            g_startVblank = ps2_test::currentVblank();
            g_status.vblanks = 0;
            if (ss::lastResult().sequence == g_resultSeq)
                stepLocked(kMaxVblanks);
            return;
        case Phase::Writing:
        {
            const ss::WriteResult w = ss::lastWrite();
            if (w.sequence == g_writeSeq)
                return;
            if (!w.ok)
                return finishLocked(false, "The state couldn't be written to disk.", w.error);
            return finishLocked(true, "Saved in slot " + std::to_string(g_status.slot) + ".");
        }
        case Phase::Settling:
            // A couple of frames, so the picture behind the menu is the loaded moment.
            if (ps2_test::paused())
            {
                const uint64_t now = ps2_test::currentVblank();
                if (g_lastStepVblank == UINT64_MAX || now != g_lastStepVblank)
                {
                    if (g_settle >= 2)
                        return finishLocked(true, "Loaded slot " + std::to_string(g_status.slot) + ".");
                    ++g_settle;
                    g_lastStepVblank = now;
                    ps2_test::stepPaused(1);
                }
                return;
            }
            return finishLocked(true, "Loaded slot " + std::to_string(g_status.slot) + ".");
        default:
            return;
        }
    }

    Status status()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        return g_status;
    }

    bool busy()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        return g_status.phase != Phase::Idle && g_status.phase != Phase::Done && g_status.phase != Phase::Failed;
    }

    void acknowledge()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_status.phase == Phase::Done || g_status.phase == Phase::Failed)
            g_status.phase = Phase::Idle;
    }

    std::vector<std::pair<std::string, std::string>> takeSaveMetadata()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::vector<std::pair<std::string, std::string>> out;
        if (!g_thumbnailPng.empty())
            out.emplace_back(kThumbnailKey, std::move(g_thumbnailPng));
        g_thumbnailPng.clear();
        return out;
    }

    std::string friendlyLoadError(const std::string &error)
    {
        const auto has = [&](const char *s) { return error.find(s) != std::string::npos; };
        if (has("older version"))
            return "Made by an older version of the app, which this one can't load. Memory card saves are not affected.";
        if (has("newer version"))
            return "Made by a newer version of the app. Update the app to load it.";
        if (has("another game"))
            return "This state belongs to another game.";
        if (has("damaged") || has("not a save state") || has("compression"))
            return "The file is damaged and can't be loaded.";
        if (has("can't resume"))
            return "This version of the app can't continue the game from there.";
        if (has("can't open") || has("can't read") || has("out of memory"))
            return "The file couldn't be read.";
        return "This state can't be loaded (" + error + ").";
    }
}
