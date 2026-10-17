#pragma once

// Runtime decoder for the shipped SYS/TITLE.GSL title backdrop.  The pack's
// first PSMT8 upload is the complete 640x448 title scene and its following
// PSMCT32 upload is the palette (sys-gsl-packs.md §3).

#include "preview/preview.h"

#include <filesystem>
#include <optional>
#include <span>
#include <vector>

namespace rt::native::assets {

// Ordered runtime locations.  Keeping this public makes the precedence and
// absent-disc behavior testable without shipping any disc data.
std::vector<std::filesystem::path> titleBackdropPaths(const std::filesystem::path& discRoot);

// Decodes the first 640x448 PSMT8 + immediately-following PSMCT32 pair in a
// TITLE.GSL pack.  Malformed or differently shaped input returns false and
// leaves out default constructed.
bool decodeTitleBackdrop(std::span<const std::uint8_t> gsl,
                         preview::PreviewTexture& out);

// Loads the shipped backdrop at runtime only.  Missing/invalid assets are an
// ordinary nullopt so callers retain their byte-identical placeholder path.
std::optional<preview::PreviewTexture> loadTitleBackdrop(const std::filesystem::path& discRoot);

} // namespace rt::native::assets
