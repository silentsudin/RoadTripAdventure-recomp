#pragma once

// Player-car render asset and scene submission.  CAR0/Q00.BIN has six model
// starts plus one self-contained PSMT8/CT32 livery chain (loadCarModel,
// SLUS-203.98 0x0020C7B8; docs/native-reference/car-data-status.md:61-132).

#include "assets/native_model.h"
#include "gameplay/game_loop.h"
#include "gameplay/world.h"
#include "preview/preview.h"

#include <string>
#include <vector>

namespace rt::native::preview {

struct CarRenderAsset {
    assets::NativeModel model;  // local-space, +Z forward for previewRenderScene
    PreviewTexture livery;      // decoded Q-file 128x128 PSMT8 livery
};

// Loads the actual near-body mesh and its own decoded livery. targetLength is
// in render units. CAR source X/Z are horizontal and source Y is up; when Q00
// is X-long, rotate it into the preview's +Z-forward convention.
bool loadCarRenderAsset(const std::string& carPath, CarRenderAsset& out,
                        std::string& error, float targetLength = 4.0f);

// Submits the car at snapshot.carPosition/heading through the existing
// textured scene path. FrameSnapshot positions are game units and the preview
// renderer performs the established .1 game-unit -> render-unit conversion.
bool renderPlayerCarChase(Renderer* renderer, const CarRenderAsset& car,
                          const gameplay::FrameSnapshot& snapshot,
                          const gameplay::World& world,
                          std::vector<std::uint8_t>& rgbaOut, std::string& error,
                          const TerrainMesh* terrain = nullptr,
                          const std::vector<RouteLine>* routes = nullptr);

}  // namespace rt::native::preview
