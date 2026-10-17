#pragma once

// Runtime decoder for the shipped race/drive HUD art from ITEM/METER*.GSL
// (speedometer dial face) and SYS/SCORE.GSL (race HUD digit/panel atlas).
// Follows the GSL/TEX0 unit discovery + PSMT8/PSMT4 + palette (CT32/CT16)
// decode pattern from title_screen.cpp and glyph_atlas.cpp (CSM1 index
// swizzle, TEXA-style alpha, payload-linear on disc).
//
// Every member is optional: total miss (no disc, wrong layout, malformed)
// yields a default-constructed HudArt (all nullopt, never crash).
// Callers retain their authored placeholders.
//
// Packaged cache resolution mirrors resolveDiscOrPackaged (disc first,
// then RT_DATA_DIR/assets/sys/... or RT_ASSET_DIR/sys/... fallbacks).
// No game data is embedded; all bytes come from user's disc at runtime.

#include "preview/preview.h"

#include <filesystem>
#include <optional>
#include <span>
#include <vector>

namespace rt::native::assets {

// Discovery order for the containers that hold HUD art. Public for tests.
std::vector<std::filesystem::path> meterDialPaths(const std::filesystem::path& discRoot);
std::vector<std::filesystem::path> scoreAtlasPaths(const std::filesystem::path& discRoot);

// Decodes the first matching PSMT8 256x128 (or PSMT4 variant) + immediate
// following 16x16 CLUT from a METER*.GSL. Returns true only on exact match
// and successful decode; otherwise leaves out default and returns false.
bool decodeMeterDial(std::span<const std::uint8_t> gsl, preview::PreviewTexture& out);

// Decodes the first PSMT4 256x256 + following CT16 CLUT from a SCORE.GSL
// (the race-HUD element atlas per hud-layout.md). Malformed yields false.
// clutBaseEntry selects the CLUT base within the 512B CT16 unit: 0 default,
// 56 = CSA 1 (TOTAL TIME banner + yellow timer colours per race-hud-atlas.md
// and the corpus pixel-exact base-entry fit).
bool decodeRaceAtlas(std::span<const std::uint8_t> gsl, preview::PreviewTexture& out, int clutBaseEntry = 0);

// Loads the shipped HUD art pieces at runtime. Missing/invalid = nullopt.
struct HudArt {
    std::optional<preview::PreviewTexture> speedDial;   // 256x128 baked dial ring + glyphs (ITEM/METER0.GSL unit 0)
    std::optional<preview::PreviewTexture> speedDigits; // 256x128 dynamic gauge parts, digit row at v=102 (ITEM/METER10.GSL unit 0)
    std::optional<preview::PreviewTexture> raceAtlas;   // 256x256 elements (SYS/SCORE.GSL unit 0)
    std::optional<preview::PreviewTexture> raceBanner;   // same texels, CLUT bank CSA 1: TOTAL TIME banner + yellow timer (race-hud-atlas.md)
    // needle: rotated procedurally (ElementA type-2 quad); no static texture.
    // minimapMap: live-rendered viewport of world data (no static GSL graphic located).
};

HudArt loadHudArt(const std::filesystem::path& discRoot);

} // namespace rt::native::assets
