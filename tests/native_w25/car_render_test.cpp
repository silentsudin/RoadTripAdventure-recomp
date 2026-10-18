#include "preview/car_render.h"

#include "gameplay/game_loop.h"

#include <bit>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace {
int failures = 0;
int checks = 0;
void check(bool ok, const char* message) {
    ++checks;
    if (!ok) { ++failures; std::cerr << "FAIL: " << message << '\n'; }
}

std::uint64_t hashModel(const rt::native::assets::NativeModel& model) {
    std::uint64_t hash = 0xcbf29ce484222325ull;
    for (const auto& vertex : model.vertices) {
        for (float value : {vertex.px, vertex.py, vertex.pz, vertex.u, vertex.v}) {
            hash ^= std::bit_cast<std::uint32_t>(value);
            hash *= 0x100000001b3ull;
        }
    }
    return hash;
}
}  // namespace

int main() {
    const char* root = std::getenv("RT_DISC");
    if (!root) { std::cout << "SKIP: RT_DISC unset\n"; return 0; }
    const auto carPath = std::filesystem::path(root) / "CAR0/Q00.BIN";
    if (!std::filesystem::exists(carPath)) { std::cout << "SKIP: Q00 absent\n"; return 0; }

    rt::native::preview::CarRenderAsset first, second;
    std::string error;
    check(rt::native::preview::loadCarRenderAsset(carPath.string(), first, error), "load real Q00 car+livery");
    check(first.model.vertices.size() > 100 && first.model.indices.size() >= 3, "real Q00 has mesh");
    check(first.livery.width == 128 && first.livery.height == 128 &&
              first.livery.rgba.size() == 128u * 128u * 4u, "real Q00 has 128x128 RGBA livery");
    bool hasAuthoredUv = false;
    for (const auto& vertex : first.model.vertices) {
        if (vertex.u != 0.f || vertex.v != 0.f) { hasAuthoredUv = true; break; }
    }
    check(hasAuthoredUv, "real Q00 preserves authored texture coordinates");
    check(rt::native::preview::loadCarRenderAsset(carPath.string(), second, error), "reload real Q00");
    check(hashModel(first.model) == hashModel(second.model) && first.livery.rgba == second.livery.rgba,
          "CAR render asset is deterministic");

    // Quick-race start camera uses the ROM-backed chase tuple: 7 RU back,
    // +2 RU base, 0x0380 pitch (camera-constants.md:11-14). The centre vertex
    // must project in front of that camera and the 4-RU vehicle remains finite.
    rt::native::gameplay::GameLoop loop;
    loop.setCarPose(800.f, 800.f, 0.f);
    const auto snapshot = loop.tick(1.f / 60.f, {});
    const auto& view = snapshot.view;
    const float x = snapshot.carPosition.x * .1f;
    const float z = snapshot.carPosition.z * .1f;
    const float cameraZ = view[2] * x + view[6] * 0.f + view[10] * z + view[14];
    check(std::isfinite(cameraZ) && cameraZ < -0.1f, "start pose is in front of chase camera");
    check(!first.model.vertices.empty(), "projected-footprint source is non-empty");

    std::cout << "w25_car_render_test: " << checks << " checks, " << failures << " failures\n";
    return failures == 0 ? 0 : 1;
}
