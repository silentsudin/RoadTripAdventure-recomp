// Road Trip's progress and live race state (see GameStats.h).

#include "game/GameStats.h"

#include "platform/Paths.h"
#include "ps2_runtime.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iterator>

#include <cstring>

namespace rt::game
{
    namespace
    {
        // [progress] in config/game_state.toml.
        constexpr uint32_t kProgress = 0x0177F760;
        constexpr uint32_t kProgressSize = 13384;
        constexpr uint32_t kPlayerName = 0x62C, kCurrency = 0x63F, kMoney = 0x654, kScene = 0x538, kLicence = 0x651,
                           kMileage = 0x658, kRaceResults = 0xFF0, kLocation = 0x12DC, kSkiJump = 0x12C8, kStamps = 0x518;
        constexpr uint32_t kCoinsMissing = 0x548, kTownsUnvisited = 0x12DA, kMailInbox = 0x5A8, kMailCount = 0x628;
        // [race], [race.live] and [game]
        constexpr uint32_t kPlace = 0x015B0088, kEntrants = 0x015B008C;
        constexpr uint32_t kCars = 0x0177ACE0, kCarLaps = 0x10B, kCarLapFrames = 0x1D0;
        constexpr uint32_t kTotalFrames = 0x015AFC1C, kCountdown = 0x015AFB8C, kLapTimes = 0x015B0044, kLapShown = 0x015B0034;
        constexpr uint32_t kRace = 0x00335910, kCourseNames = 0x002BE398, kScene3D = 0x003356C4, kScene3DHandler = 0x003356D8,
                           kUiContext = 0x003356CC, kDriveFlags = 0x00335928;
        constexpr uint32_t kRaceHandler = 0x0021CE48, kTownHandler = 0x0021CF80, kTitleContext = 0x0029AFA0;
        constexpr uint32_t kLocationNow = 0x00335923, kClock = 0x00335914, kUnvisitedDoors = 0xBD8;
        constexpr uint32_t kQuickRaceTable = 0x002BE158, kTwoPlayerTable = 0x002BE1D8;

        int popcount(const uint8_t *p, size_t n)
        {
            int c = 0;
            for (size_t i = 0; i < n; ++i)
                c += __builtin_popcount(p[i]);
            return c;
        }

        // A NUL-terminated string at an EE address (printable ASCII only).
        std::string guestString(const uint8_t *ram, uint32_t addr, uint32_t max = 64)
        {
            addr &= 0x1FFFFFFu;
            std::string s;
            for (uint32_t i = 0; i < max && addr + i < 32u * 1024u * 1024u && ram[addr + i]; ++i)
                s += (ram[addr + i] >= 0x20 && ram[addr + i] < 0x7F) ? static_cast<char>(ram[addr + i]) : ' ';
            return s;
        }

        template <typename T>
        T at(const uint8_t *p, uint32_t offset)
        {
            T v;
            std::memcpy(&v, p + offset, sizeof(v));
            return v;
        }

        std::string str(const uint8_t *p, uint32_t length)
        {
            std::string s;
            for (uint32_t i = 0; i < length && p[i]; ++i)
                s += (p[i] >= 0x20 && p[i] < 0x7F) ? static_cast<char>(p[i]) : '?';
            return s;
        }
    }

    Stats readStats(PS2Runtime &runtime)
    {
        Stats s;
        const uint8_t *ram = runtime.memory().getRDRAM();
        if (!ram)
            return s;
        const uint8_t *p = ram + kProgress;
        static_assert(kProgress + kProgressSize <= 32u * 1024u * 1024u);
        s.playerName = str(p + kPlayerName, 19);
        s.currency = str(p + kCurrency, 21);
        s.money = at<uint32_t>(p, kMoney);
        s.mileageMetres = at<uint32_t>(p, kMileage);
        s.licence = p[kLicence];
        s.location = p[kLocation];
        s.skiJumpBest = at<uint32_t>(p, kSkiJump);
        const uint64_t lo = at<uint64_t>(p, kStamps), hi = at<uint64_t>(p, kStamps + 8);
        for (int i = 0; i < 100; ++i)
            s.stamps[i] = ((i < 64 ? lo >> i : hi >> (i - 64)) & 1) != 0;
        std::memcpy(s.raceResults, p + kRaceResults, sizeof(s.raceResults));
        s.coins = 100 - popcount(p + kCoinsMissing, 16);
        s.townsVisited = 10 - __builtin_popcount(at<uint16_t>(p, kTownsUnvisited) & 0x3FFu);
        s.mail = static_cast<int>(std::min<uint32_t>(at<uint32_t>(p, kMailCount), 128));
        for (int i = 0; i < s.mail; ++i)
            s.unreadMail += (p[kMailInbox + i] & 0x80) == 0;
        // The block is the Adventure's once a game started (Quick Race writes a default profile
        // with scene 0; at the title it keeps the last Adventure's, under the title's UI context).
        const uint32_t scene = at<uint32_t>(p, kScene);
        s.adventure = scene != 0 && at<uint32_t>(ram, kUiContext) != kTitleContext && !s.playerName.empty() && s.licence <= 3;

        // Driving: the 3D scene running with the town or the race handler.
        const bool scene3d = at<uint32_t>(ram, kScene3D) == 2;
        s.inTown = scene3d && at<uint32_t>(ram, kScene3DHandler) == kTownHandler && s.adventure;
        if (s.inTown)
        {
            s.town = std::min<int>(ram[kLocationNow], 9);
            const uint32_t minutes = at<uint32_t>(ram, kClock) / 150u; // 150 vblanks a game minute
            s.hour = static_cast<int>(minutes / 60 % 24);
            s.minute = static_cast<int>(minutes % 60);
            s.housesLeft = __builtin_popcount(at<uint32_t>(p, kUnvisitedDoors + 4u * static_cast<uint32_t>(s.town)));
        }
        s.racing = scene3d && at<uint32_t>(ram, kScene3DHandler) == kRaceHandler;
        if (!s.racing)
            return s;
        const uint32_t race = at<uint32_t>(ram, kRace) & 0x1FFFFFFu;
        s.mode = race >= kTwoPlayerTable ? Stats::Mode::TwoPlayer : race >= kQuickRaceTable ? Stats::Mode::QuickRace : Stats::Mode::Adventure;
        s.entrants = std::clamp(static_cast<int>(at<uint32_t>(ram, kEntrants)), 1, 24);
        s.place = std::clamp(static_cast<int>(at<uint32_t>(ram, kPlace)) + 1, 1, s.entrants);
        if (race && race < 32u * 1024u * 1024u - 4)
        {
            s.laps = ram[race + 2];
            const uint8_t course = ram[race];
            s.course = guestString(ram, at<uint32_t>(ram, kCourseNames + 4u * course));
        }
        s.lap = std::min<int>(ram[kCars + kCarLaps] + 1, std::max(s.laps, 1));
        s.finished = (ram[kDriveFlags] & 0x08) != 0;
        s.countdown = static_cast<int>(at<uint32_t>(ram, kCountdown));
        s.totalFrames = at<uint32_t>(ram, kTotalFrames);
        s.lapFrames = at<uint32_t>(ram, kCars + kCarLapFrames);
        const int lapsDone = std::min<int>(ram[kCars + kCarLaps], 8);
        for (int j = 0; j < lapsDone; ++j)
        {
            const uint32_t t = at<uint32_t>(ram, kLapTimes + 0x18u * j);
            if (t == 0 || t >= 0x7FFFFFFF)
                continue;
            if (j == lapsDone - 1)
            {
                s.lastLapFrames = t;
                s.bestBeforeLast = s.bestLapFrames;
            }
            if (!s.bestLapFrames || t < s.bestLapFrames)
                s.bestLapFrames = t;
        }
        return s;
    }

    std::string stampCaption(PS2Runtime &runtime, int n)
    {
        // The notebook's captions follow the "mail %d/%d" format string in the ELF's data, stamp
        // 100 first, NUL-terminated and word-aligned, '#' for line breaks ([stamps] in
        // config/game_state.toml). Read once from the loaded ELF in EE RAM.
        constexpr uint32_t kCaptionMarker = 0x0032E240;
        static std::string captions[100];
        static bool loaded = false;
        if (!loaded)
        {
            const uint8_t *ram = runtime.memory().getRDRAM();
            if (!ram || std::memcmp(ram + kCaptionMarker, "mail %d/%d", 10) != 0)
                return {};
            uint32_t k = kCaptionMarker + 10;
            for (int entry = 0; entry <= 100; ++entry)
            {
                while (k < kCaptionMarker + 0x2000 && ram[k] == 0)
                    ++k;
                std::string text;
                for (; k < kCaptionMarker + 0x2000 && ram[k]; ++k)
                    text += ram[k] == '#' ? ' ' : static_cast<char>(ram[k]);
                if (entry == 0)
                    continue; // the rest of the marker string ("mail %d/%d  ")
                size_t a = text.find_first_not_of(' '), b = text.find_last_not_of(' ');
                captions[100 - entry] = a == std::string::npos ? std::string() : text.substr(a, b - a + 1);
            }
            loaded = true;
        }
        return n >= 1 && n <= 100 ? captions[n - 1] : std::string();
    }

    std::vector<uint8_t> stampIcon(int n)
    {
        // SYS/STAMP.GSL: after a UI atlas, one 64x64 PSMT4 upload and one 16x2 CT32 palette upload
        // per stamp ([stamps] icon_* in config/game_state.toml); the notebook draws palette bank 0.
        constexpr size_t kIcon = 0x94F0, kPalette = 0x9D60, kStride = 0x960, kData = 0x60;
        static std::vector<uint8_t> file;
        static bool read = false;
        if (!read)
        {
            read = true;
            std::ifstream in(rt::paths::discDir() / "SYS" / "STAMP.GSL", std::ios::binary);
            file.assign(std::istreambuf_iterator<char>(in), {});
        }
        if (n < 1 || n > 100)
            return {};
        const size_t k = static_cast<size_t>(n - 1);
        const size_t tex = kIcon + kStride * k, pal = kPalette + kStride * k;
        if (file.size() < pal + kData + 128)
            return {};
        // Guard: the texture packet's BITBLTBUF names DBP 0x1FE2 + 9k (another disc: no icons).
        uint64_t bitbltbuf;
        std::memcpy(&bitbltbuf, file.data() + tex + 0x10, sizeof(bitbltbuf));
        uint64_t palbuf;
        std::memcpy(&palbuf, file.data() + pal + 0x10, sizeof(palbuf));
        if (((bitbltbuf >> 32) & 0x3FFF) != 0x1FE2 + 9 * k || ((palbuf >> 32) & 0x3FFF) != 0x1FEA + 9 * k)
            return {};
        uint32_t clut[16];
        for (int e = 0; e < 16; ++e)
            std::memcpy(&clut[e], file.data() + pal + kData + 4 * ((e & 7) + 16 * (e >> 3)), 4);
        std::vector<uint8_t> rgba(64 * 64 * 4);
        const uint8_t *px = file.data() + tex + kData;
        for (int i = 0; i < 64 * 64; ++i)
        {
            const uint32_t c = clut[(px[i >> 1] >> ((i & 1) * 4)) & 15];
            rgba[i * 4 + 0] = c & 0xFF;
            rgba[i * 4 + 1] = (c >> 8) & 0xFF;
            rgba[i * 4 + 2] = (c >> 16) & 0xFF;
            rgba[i * 4 + 3] = static_cast<uint8_t>(std::min(255u, ((c >> 24) & 0xFF) * 255u / 128u));
        }
        return rgba;
    }

    namespace
    {
        // [music]: SNDMOD's radio state (IOP): +8 flag_play (2 = playing), +0x18 play_time
        // (vblanks into the station's hour), +0x1C play_tune (0 PEACH FM, 1 E-RADIO).
        constexpr uint32_t kIopRadio = 0x542C4;

        struct Song
        {
            float start;
            const char *title, *artist;
        };
        // PEACH FM (SOUND/1CH): four songs looping (lyrics, durations and a published listing).
        const Song kPeachFm[] = {
            {0.00f, "Sunday on the West Side", "Push Kings"}, {221.95f, "The Wild Ones", "Push Kings"},
            {431.75f, "The Minute", "Push Kings"},            {647.25f, "Jade", "The Waking Hours"},
            {828.45f, "Sunday on the West Side", "Push Kings"}, {1049.65f, "The Wild Ones", "Push Kings"},
            {1259.50f, "The Minute", "Push Kings"},           {1474.95f, "Jade", "The Waking Hours"},
            {1657.95f, "Sunday on the West Side", "Push Kings"}, {1879.70f, "The Wild Ones", "Push Kings"},
            {2089.50f, "The Minute", "Push Kings"},           {2304.95f, "Jade", "The Waking Hours"},
            {2486.20f, "Sunday on the West Side", "Push Kings"}, {2707.40f, "The Wild Ones", "Push Kings"},
            {2917.20f, "The Minute", "Push Kings"},           {3132.70f, "Jade", "The Waking Hours"},
            {3314.20f, "Sunday on the West Side", "Push Kings"}, {3535.95f, "The Wild Ones", "Push Kings"},
        };
        // E-RADIO (SOUND/3CH): seventeen tracks; named where matched against Michael Walthius's MIDIs.
        const Song kERadio[] = {
            {0.85f, nullptr, nullptr},    {220.00f, nullptr, nullptr},  {356.20f, nullptr, nullptr},
            {583.10f, nullptr, nullptr},  {745.50f, nullptr, nullptr},  {915.00f, nullptr, nullptr},
            {1089.55f, "Song for My Children", "Michael Walthius"},     {1429.25f, "Echoes", "Michael Walthius"},
            {1767.80f, nullptr, nullptr}, {1917.55f, nullptr, nullptr}, {2112.10f, nullptr, nullptr},
            {2306.00f, nullptr, nullptr}, {2580.00f, nullptr, nullptr}, {2773.95f, nullptr, nullptr},
            {2975.50f, nullptr, nullptr}, {3214.70f, "Pacific Coast Highway", "Michael Walthius"},
            {3539.10f, nullptr, nullptr},
        };
    }

    NowPlaying nowPlaying(PS2Runtime &runtime)
    {
        NowPlaying np;
        uint8_t radio[0x20];
        if (!runtime.readIopMemory(kIopRadio, radio, sizeof(radio)))
            return np;
        const uint32_t flag = at<uint32_t>(radio, 8), time = at<uint32_t>(radio, 0x18), tune = at<uint32_t>(radio, 0x1C);
        if (flag != 2 || tune > 1 || time >= 216000u)
            return np;
        np.playing = true;
        np.station = tune == 0 ? "PEACH FM" : "E-RADIO";
        const float t = time / 60.0f;
        const Song *songs = tune == 0 ? kPeachFm : kERadio;
        const size_t n = tune == 0 ? std::size(kPeachFm) : std::size(kERadio);
        size_t i = 0;
        while (i + 1 < n && songs[i + 1].start <= t)
            ++i;
        const float end = i + 1 < n ? songs[i + 1].start : 3600.0f;
        np.elapsed = std::max(0.0f, t - songs[i].start);
        np.length = end - songs[i].start;
        if (songs[i].title)
            np.title = songs[i].title, np.artist = songs[i].artist;
        return np;
    }

    std::string raceTime(uint32_t frames)
    {
        if (frames >= 0x7FFFFFFF)
            return "--'--\"--";
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%u'%02u\"%02u", frames / 3600, frames / 60 % 60, (frames % 60) * 100 / 60);
        return buf;
    }

    const char *townName(int index)
    {
        static const char *names[] = {"My Garage",      "Peach Town",    "Fuji City",  "Sandpolis", "Chestnut Canyon",
                                      "Mushroom Road",  "White Mountain", "Papaya Island", "Cloud Hill", "My City"};
        return index >= 0 && index < 10 ? names[index] : "?";
    }

    const char *licenceName(int licence)
    {
        static const char *names[] = {"None", "B", "A", "Super A"};
        return licence >= 0 && licence < 4 ? names[licence] : "?";
    }
}
