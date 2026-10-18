// W2-13 registry emitter: prints the enhancement retention registry as CSV
// or JSON (byte-identical to the committed copies under
// tests/native_w2/data/). Registered automatically by the Step-0 tool glob
// as target rt-w2-enh_registry.
//
// Usage: rt-w2-enh_registry [csv|json]
#include <cstdio>
#include <string>

#include "gameplay/enhancements.h"

int main(int argc, char** argv) {
    const std::string format = argc > 1 ? argv[1] : "csv";
    if (format == "csv") {
        std::fputs(rt::native::gameplay::registryCsv().c_str(), stdout);
    } else if (format == "json") {
        std::fputs(rt::native::gameplay::registryJson().c_str(), stdout);
    } else {
        std::fprintf(stderr, "usage: %s [csv|json]\n", argv[0]);
        return 1;
    }
    return 0;
}
