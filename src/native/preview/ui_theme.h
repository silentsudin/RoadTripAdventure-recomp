#pragma once

// Recomp pause/setup UI palette, identical RGBA per docs/native-reference/ui-parity.md:105-111.
// Source: src/ui/Theme.h:17-50 (RRGGBBAA; 6-digit values are opaque).

#include <array>
#include <cstdint>

namespace rt::native::preview::theme {

inline constexpr std::uint32_t kOlive = 0x485800F2u;
inline constexpr std::uint32_t kFrameOuter = 0xC05B1EFFu;
inline constexpr std::uint32_t kFrame = 0xF86018FFu;
inline constexpr std::uint32_t kFrameLight = 0xF8BF84FFu;
inline constexpr std::uint32_t kListBody = 0x0098C8FFu;
inline constexpr std::uint32_t kListBevelLight = 0x8ED4F0FFu;
inline constexpr std::uint32_t kListBevelDark = 0x0F5E92FFu;
inline constexpr std::uint32_t kListText = 0xF6E68FFFu;
inline constexpr std::uint32_t kListSelected = 0xECECECFFu;
inline constexpr std::uint32_t kSelectionTop = 0x2C6CE8FFu;
inline constexpr std::uint32_t kSelectionBottom = 0x0838B0FFu;
inline constexpr std::uint32_t kCapGold = 0xF8C838FFu;
inline constexpr std::uint32_t kHorn = 0xF2C230FFu;
inline constexpr std::uint32_t kPrompt = 0x304090F2u;
inline constexpr std::uint32_t kPromptRimOuter = 0x91D2E8FFu;
inline constexpr std::uint32_t kPromptRimInner = 0x1CA1E6FFu;
inline constexpr std::uint32_t kOutline = 0x1A1A10FFu;
inline constexpr std::uint32_t kOutlineBlue = 0x000820FFu;
inline constexpr std::uint32_t kScrim = 0x000A2899u;

constexpr std::array<float, 4> rgba(std::uint32_t c) {
    return {static_cast<float>((c >> 24) & 0xFF) / 255.f, static_cast<float>((c >> 16) & 0xFF) / 255.f,
            static_cast<float>((c >> 8) & 0xFF) / 255.f, static_cast<float>(c & 0xFF) / 255.f};
}

}  // namespace rt::native::preview::theme
