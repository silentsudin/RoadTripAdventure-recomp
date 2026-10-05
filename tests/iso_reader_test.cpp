// Checks IsoReader against the user's disc image. Needs RT_ROM=<path to .cue/.bin/.iso>;
// skipped (exit 77) when it is not set, so CI without a ROM still passes.

#include "rom/IsoReader.h"
#include "rom/Sha1.h"

#include <cstdlib>
#include <iostream>
#include <string>

#define CHECK(cond)                                                    \
    do                                                                 \
    {                                                                  \
        if (!(cond))                                                   \
        {                                                              \
            std::cerr << __FILE__ << ":" << __LINE__ << ": " #cond "\n"; \
            return 1;                                                  \
        }                                                              \
    } while (0)

int main()
{
    // Known SHA-1 test vector, so the hasher is covered even without a ROM.
    {
        rt::Sha1 s;
        s.update(reinterpret_cast<const uint8_t *>("abc"), 3);
        CHECK(s.hexdigest() == "a9993e364706816aba3e25717850c26c9cd0d89d");
    }

    const char *rom = std::getenv("RT_ROM");
    if (!rom || !*rom)
    {
        std::cout << "RT_ROM not set; skipping disc checks\n";
        return 77;
    }

    rt::IsoReader iso;
    std::string err;
    CHECK(iso.open(rom, err));
    CHECK(iso.volumeId() == "ROADTRIP");

    auto all = iso.walk();
    size_t files = 0;
    for (auto &e : all)
        files += e.isDir ? 0 : 1;
    std::cout << "files: " << files << "\n";
    CHECK(files == 376);

    rt::IsoEntry e;
    CHECK(iso.find("SOUND/1CH_L.VAG", e));
    CHECK(e.size == 24706368);

    CHECK(iso.find("SLUS_203.98", e));
    CHECK(e.size == 1280576);
    rt::Sha1 sha;
    CHECK(iso.readFile(e, [&](const uint8_t *p, size_t n)
                       { sha.update(p, n); return true; }));
    CHECK(sha.hexdigest() == "2431de1ec3edd0df4be37ba564658d70c4049089");

    std::cout << "ok\n";
    return 0;
}
