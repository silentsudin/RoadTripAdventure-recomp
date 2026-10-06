// Road Trip's progress and live race state (see GameStats.h).

#include "game/GameStats.h"

#include "platform/Paths.h"
#include "ps2_runtime.h"

#include <algorithm>
#include <cstdio>
#include <cctype>
#include <cstdlib>
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
        constexpr uint32_t kPhotosTaken = 0x508, kMapMode = 0x0177A215;
        constexpr uint32_t kDoorTable = 0x002C0698, kNpcNames = 0x002A7ED0, kLocations = 0x002BE438, kTile = 0x00335954;
        constexpr int kDoorLocations = 24;
        // New-game unvisited_doors masks ([houses] initial_unvisited): a door is a house to visit
        // when its bit starts set.
        constexpr uint32_t kInitialUnvisited[22] = {0,   0x3DFFF, 0x7FBFF, 0x1FFFFF, 0xFBF, 0xF, 0xFFFFF, 0x3FFFF, 0x17F, 0x7FFFDF, 0xF,
                                                    0xF, 0xF,     0x1F,    7,        0x3F,  3,   1,       1,       0xF,   1,        1};
        constexpr int kLastLocation = 21;
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
            // Where we are: the location whose map tile the car is in (0x335923 only changes at a
            // door), else that byte.
            s.tile = static_cast<int>(at<uint32_t>(ram, kTile) & 0xFFFF);
            s.town = std::min<int>(ram[kLocationNow], kLastLocation);
            for (int l = 1; l <= kLastLocation; ++l)
                if (at<int16_t>(ram, kLocations + 8u * l + 4) == s.tile)
                {
                    s.town = l;
                    break;
                }
            if (s.town >= 1 && s.town <= 9)
                s.townLabel = townName(s.town);
            else
            {
                // Field areas: the game's own name when it has one (Bridge, UFO, Ruins,
                // LightHouse); the others are named by their map tile ("022").
                const std::string n = guestString(ram, at<uint32_t>(ram, kLocations + 8u * s.town));
                s.townLabel = !n.empty() && !std::isdigit(static_cast<unsigned char>(n[0])) && n != "???" ? n : "On the road";
            }
            const uint32_t minutes = at<uint32_t>(ram, kClock) / 150u; // 150 vblanks a game minute
            s.hour = static_cast<int>(minutes / 60 % 24);
            s.minute = static_cast<int>(minutes % 60);
            s.housesLeft = __builtin_popcount(at<uint32_t>(p, kUnvisitedDoors + 4u * static_cast<uint32_t>(s.town)));
        }
        s.photos = popcount(p + kPhotosTaken, 16);
        if (s.inTown)
        {
            float pos[3];
            std::memcpy(pos, ram + kCars, sizeof(pos));
            s.carX = pos[0];
            s.carZ = pos[2];
            s.mapZoom = ram[kMapMode] ? 25 : 50;
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
        // E-RADIO (SOUND/3CH): seventeen tracks, all Michael Walthius songs arranged for the game:
        // named from "Road Trip Adventure - The Stolen Tracks" (youtu.be/kEExb86iAiA, its chapters)
        // and alignment with his own MIDIs (keybdwizrd.com, dongrays.com); the rest unnamed.
        constexpr const char *kWalthius = "Michael Walthius";
        const Song kERadio[] = {
            {0.85f, nullptr, kWalthius},                   {220.00f, nullptr, kWalthius},
            {356.20f, "Jammin' on Sunset", kWalthius},      {583.10f, "Cyberbeat", kWalthius},
            {745.50f, nullptr, kWalthius},                 {915.00f, nullptr, kWalthius},
            {1089.55f, "Song for My Children", kWalthius},  {1429.25f, "Echoes", kWalthius},
            {1767.80f, nullptr, kWalthius},                {1917.55f, nullptr, kWalthius},
            {2112.10f, nullptr, kWalthius},                {2306.00f, "Dreaming in Stereo", kWalthius},
            {2580.00f, "Funkengruven", kWalthius},          {2773.95f, "Nitefunk", kWalthius},
            {2975.50f, "Incident at Dark Shores", kWalthius}, {3214.70f, "Pacific Coast Highway", kWalthius},
            {3539.10f, nullptr, kWalthius},
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
            np.title = songs[i].title;
        if (songs[i].artist)
            np.artist = songs[i].artist;
        return np;
    }

    namespace
    {
        // The FLD tiles' origins (a checkerboard of 1600-unit tiles; 0x218424 / 0x218B10).
        float tileOriginX(int t) { return static_cast<float>(t & 0xF) * 800.0f; }
        float tileOriginZ(int t) { return static_cast<float>(((t >> 4) * 2 + (t & 1)) * 1600); }

        // A location's buildings in its own tile's coordinates: door quads ([doors]) and the
        // people or shops behind them (the NPC table at 0x2A7ED0: per location, an entry per door
        // whose first word names it, "Q's Factory", "Quick-Pic Shop No.5").
        std::vector<Place> placesOf(const uint8_t *ram, int loc)
        {
            std::vector<Place> places;
            uint32_t table[kDoorLocations];
            std::memcpy(table, ram + kDoorTable, sizeof(table));
            const uint32_t start = table[loc] & 0x1FFFFFFu;
            uint32_t end = 0xFFFFFFFFu;
            for (uint32_t t : table)
                if ((t & 0x1FFFFFFu) > start)
                    end = std::min(end, t & 0x1FFFFFFu);
            const int doors = std::min<int>(ram[kLocations + 8u * loc + 6], end == 0xFFFFFFFFu ? 0 : static_cast<int>((end - start) / 32));
            const uint32_t names = at<uint32_t>(ram, kNpcNames + 4u * loc) & 0x1FFFFFFu;
            const uint8_t *p = ram + kProgress;
            const uint32_t unvisited = at<uint32_t>(p, kUnvisitedDoors + 4u * loc);
            for (int j = 0; j < doors && start + 32u * (j + 1) <= 32u * 1024u * 1024u; ++j)
            {
                Place pl;
                float q[8];
                std::memcpy(q, ram + start + 32u * j, sizeof(q));
                pl.x = (q[0] + q[2] + q[4] + q[6]) / 4;
                pl.z = (q[1] + q[3] + q[5] + q[7]) / 4;
                if (pl.x < 0 || pl.z < 0)
                    continue; // not placed
                if (names)
                {
                    const uint32_t entry = at<uint32_t>(ram, names + 4u * j) & 0x1FFFFFFu;
                    if (entry)
                        pl.name = guestString(ram, at<uint32_t>(ram, entry));
                }
                const std::string &n = pl.name;
                auto has = [&](const char *w) { return n.find(w) != std::string::npos; };
                if (has("Quick-Pic"))
                {
                    pl.kind = Place::Kind::PhotoBooth;
                    const size_t no = n.find("No");
                    pl.photo = no == std::string::npos ? 0 : std::atoi(n.c_str() + no + 3); // "No.n" (once "No,n")
                    pl.done = pl.photo >= 1 && pl.photo <= 100 && (p[kPhotosTaken + (pl.photo - 1) / 8] >> ((pl.photo - 1) % 8) & 1);
                }
                else if (has("Factory"))
                    pl.kind = Place::Kind::Factory;
                else if (has("Parts"))
                    pl.kind = Place::Kind::Shop, pl.letter = 'P';
                else if (has("Body"))
                    pl.kind = Place::Kind::Shop, pl.letter = 'B';
                else if (has("Paint"))
                    pl.kind = Place::Kind::Shop, pl.letter = 'C';
                else if (kInitialUnvisited[loc] >> j & 1)
                {
                    pl.kind = Place::Kind::House;
                    pl.done = !(unvisited >> j & 1);
                }
                else
                    pl.kind = Place::Kind::Other;
                places.push_back(std::move(pl));
            }
            return places;
        }
    }

    std::vector<Place> placesAround(PS2Runtime &runtime, const Stats &st)
    {
        std::vector<Place> all;
        const uint8_t *ram = runtime.memory().getRDRAM();
        if (!ram || st.town < 1 || st.town > kLastLocation)
            return all;
        // Cloud Hill is no FLD tile (ACTION/A16): only its own places.
        const bool tiled = st.tile > 0 && st.tile < 0x40;
        for (int loc = 1; loc <= kLastLocation; ++loc)
        {
            const int t = at<int16_t>(ram, kLocations + 8u * loc + 4);
            float dx = 0, dz = 0;
            if (loc != st.town)
            {
                if (!tiled || t <= 0 || t >= 0x40)
                    continue;
                dx = tileOriginX(t) - tileOriginX(st.tile);
                dz = tileOriginZ(t) - tileOriginZ(st.tile);
                if (std::abs(dx) > 1600.0f || std::abs(dz) > 1600.0f)
                    continue; // not a neighbouring tile
            }
            for (Place pl : placesOf(ram, loc))
            {
                pl.x += dx;
                pl.z += dz;
                pl.here = loc == st.town;
                all.push_back(std::move(pl));
            }
        }
        return all;
    }

    void mapPoint(const Stats &st, float x, float z, float &fx, float &fy)
    {
        // [map]: north-up, centred on the car at (102, 183); s / 200 field pixels a unit across,
        // 0.47 of that down (fields are half height).
        fx = 102.0f + (x - st.carX) * st.mapZoom / 200.0f;
        fy = 183.0f - (z - st.carZ) * 0.47f * st.mapZoom / 200.0f;
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
