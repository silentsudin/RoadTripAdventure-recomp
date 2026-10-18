#include "preview/car_render.h"

#include "gameplay/game_loop.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 3) { std::cerr << "usage: rt-w25-car-preview <CAR file> <out.png>\n"; return 2; }
    rt::native::preview::CarRenderAsset car;
    std::string error;
    if (!rt::native::preview::loadCarRenderAsset(argv[1], car, error)) {
        std::cerr << error << '\n'; return 1;
    }
    rt::native::gameplay::GameLoop loop;
    loop.setCarPose(800.f, 800.f, 0.f);
    const auto snapshot = loop.tick(1.f / 60.f, {});
    constexpr std::uint32_t width = 1280, height = 447;
    auto* renderer = rt::native::preview::previewInit(width, height, error, nullptr);
    if (!renderer) { std::cerr << error << '\n'; return 1; }
    std::vector<std::uint8_t> rgba;
    const bool rendered = rt::native::preview::renderPlayerCarChase(
        renderer, car, snapshot, loop.world(), rgba, error);
    rt::native::preview::previewDestroy(renderer);
    if (!rendered) { std::cerr << error << '\n'; return 1; }
    const auto png = rt::native::preview::encodePng(width, height, rgba);
    std::filesystem::create_directories(std::filesystem::path(argv[2]).parent_path());
    std::ofstream out(argv[2], std::ios::binary);
    out.write(reinterpret_cast<const char*>(png.data()), static_cast<std::streamsize>(png.size()));
    return out ? 0 : 1;
}
