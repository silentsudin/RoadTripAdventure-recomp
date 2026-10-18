#include "render/race_terrain.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

namespace {
void put32(std::ofstream& out, std::uint32_t value) {
    for (int shift = 24; shift >= 0; shift -= 8) out.put(static_cast<char>(value >> shift));
}
std::uint32_t adler(const std::vector<std::uint8_t>& data) {
    std::uint32_t a = 1, b = 0;
    for (std::uint8_t byte : data) { a = (a + byte) % 65521; b = (b + a) % 65521; }
    return (b << 16) | a;
}
std::uint32_t crc32(const std::uint8_t* bytes, std::size_t size) {
    std::uint32_t crc = ~0u;
    for (std::size_t i = 0; i < size; ++i) { crc ^= bytes[i]; for (int b = 0; b != 8; ++b) crc = (crc >> 1) ^ (0xedb88320u & -(crc & 1)); }
    return ~crc;
}
void chunk(std::ofstream& out, const char name[4], const std::vector<std::uint8_t>& data) {
    put32(out, static_cast<std::uint32_t>(data.size())); out.write(name, 4); out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    std::vector<std::uint8_t> crc(name, name + 4); crc.insert(crc.end(), data.begin(), data.end()); put32(out, crc32(crc.data(), crc.size()));
}
bool writePng(const std::filesystem::path& path, int width, int height, const std::vector<std::uint8_t>& rgb) {
    std::vector<std::uint8_t> scan;
    for (int y = 0; y < height; ++y) { scan.push_back(0); scan.insert(scan.end(), rgb.begin() + static_cast<std::size_t>(y) * width * 3, rgb.begin() + static_cast<std::size_t>(y + 1) * width * 3); }
    std::vector<std::uint8_t> z{0x78, 0x01};
    for (std::size_t at = 0; at < scan.size();) { const std::size_t n = std::min<std::size_t>(65535, scan.size() - at); z.push_back(at + n == scan.size()); z.push_back(n & 255); z.push_back(n >> 8); const std::uint16_t nn = static_cast<std::uint16_t>(~n); z.push_back(nn & 255); z.push_back(nn >> 8); z.insert(z.end(), scan.begin() + at, scan.begin() + at + n); at += n; }
    const auto sum = adler(scan); z.insert(z.end(), {static_cast<std::uint8_t>(sum >> 24), static_cast<std::uint8_t>(sum >> 16), static_cast<std::uint8_t>(sum >> 8), static_cast<std::uint8_t>(sum)});
    std::ofstream out(path, std::ios::binary); if (!out) return false;
    constexpr std::array<std::uint8_t, 8> magic{137, 80, 78, 71, 13, 10, 26, 10}; out.write(reinterpret_cast<const char*>(magic.data()), magic.size());
    std::vector<std::uint8_t> header{static_cast<std::uint8_t>(width >> 24), static_cast<std::uint8_t>(width >> 16), static_cast<std::uint8_t>(width >> 8), static_cast<std::uint8_t>(width), static_cast<std::uint8_t>(height >> 24), static_cast<std::uint8_t>(height >> 16), static_cast<std::uint8_t>(height >> 8), static_cast<std::uint8_t>(height), 8, 2, 0, 0, 0}; chunk(out, "IHDR", header); chunk(out, "IDAT", z); chunk(out, "IEND", {}); return static_cast<bool>(out);
}
}  // namespace

int main(int argc, char** argv) {
    const std::filesystem::path output = argc > 1 ? argv[1] : "race_terrain.png";
    const auto mesh = rt::native::raceTerrain::build(0, {});
    if (!mesh.textured()) { std::fprintf(stderr, "terrain unavailable: %s\n", mesh.diagnostic.c_str()); return 1; }
    constexpr int kSize = 800;
    std::vector<std::uint8_t> rgb(static_cast<std::size_t>(kSize) * kSize * 3, 86);
    float minX = std::numeric_limits<float>::max(), minZ = minX, maxX = -minX, maxZ = -minX;
    for (const auto& v : mesh.vertices) { minX = std::min(minX, v.position[0]); maxX = std::max(maxX, v.position[0]); minZ = std::min(minZ, v.position[2]); maxZ = std::max(maxZ, v.position[2]); }
    const float scale = .92f * kSize / std::max(maxX - minX, maxZ - minZ);
    // Deterministic top-down terrain atlas preview.  It deliberately samples each
    // decoded upload's centre texel: the renderer integration consumes full UVs.
    for (std::size_t t = 0; t < mesh.indices.size() / 3; ++t) {
        const auto textureId = mesh.triangleTexture[t]; if (textureId < 0 || static_cast<std::size_t>(textureId) >= mesh.textures.size()) continue;
        const auto& tex = mesh.textures[static_cast<std::size_t>(textureId)];
        std::array<float, 3> x{}, y{};
        std::array<float, 3> u{}, v{};
        for (int corner = 0; corner < 3; ++corner) { const auto& vertex = mesh.vertices[mesh.indices[t * 3 + corner]]; x[corner] = (vertex.position[0] - minX) * scale + 32; y[corner] = (vertex.position[2] - minZ) * scale + 32; u[corner] = vertex.uv[0]; v[corner] = vertex.uv[1]; }
        const float area = (x[1] - x[0]) * (y[2] - y[0]) - (y[1] - y[0]) * (x[2] - x[0]);
        if (std::fabs(area) < .001f) continue;
        const int left = std::max(0, static_cast<int>(std::floor(std::min({x[0], x[1], x[2]}))));
        const int right = std::min(kSize - 1, static_cast<int>(std::ceil(std::max({x[0], x[1], x[2]}))));
        const int top = std::max(0, static_cast<int>(std::floor(std::min({y[0], y[1], y[2]}))));
        const int bottom = std::min(kSize - 1, static_cast<int>(std::ceil(std::max({y[0], y[1], y[2]}))));
        for (int py = top; py <= bottom; ++py) for (int px = left; px <= right; ++px) {
            const float a = ((x[1] - px) * (y[2] - py) - (y[1] - py) * (x[2] - px)) / area;
            const float b = ((x[2] - px) * (y[0] - py) - (y[2] - py) * (x[0] - px)) / area;
            if (a < 0 || b < 0 || a + b > 1) continue;
            const float cweight = 1.f - a - b;
            const float su = a * u[0] + b * u[1] + cweight * u[2];
            const float sv = a * v[0] + b * v[1] + cweight * v[2];
            const float wrappedU = su - std::floor(su), wrappedV = sv - std::floor(sv);
            const std::size_t sample = (static_cast<std::size_t>(wrappedV * tex.height) % tex.height * tex.width +
                                        static_cast<std::size_t>(wrappedU * tex.width) % tex.width) * 4;
            for (int ch = 0; ch < 3; ++ch) rgb[(static_cast<std::size_t>(py) * kSize + px) * 3 + ch] = tex.rgba[sample + ch];
        }
    }
    if (!output.parent_path().empty()) std::filesystem::create_directories(output.parent_path());
    if (!writePng(output, kSize, kSize, rgb)) return 1;
    std::printf("wrote %s; %s\n", output.c_str(), mesh.diagnostic.c_str());
}
