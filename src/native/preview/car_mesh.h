#pragma once

// src/native/preview/car_mesh.h
//
// CAR container (Qnn.BIN) -> render-ready NativeModel for the player-car
// slot. Merges model 0 (main chain + count-gated part chains), supplies
// flat per-face normals, neutral body color (UNIMPLEMENTED/UNDOCUMENTED in
// the original vertex streams), uv=0, and uniform scale+center to a caller
// target length so the model sits with y=0 at bbox bottom and x/z centered
// at origin. CAR source Y is the recovered up lane; X/Z are horizontal. The
// longest horizontal axis (x or z) is used for the scale basis and reported
// for logging.
//
// This module is self-contained for the mesh math; later wiring (not here)
// plugs the result into previewRenderScene etc.

#include "assets/container.h"
#include "assets/model_converter.h"
#include "assets/native_model.h"

#include <array>
#include <cstdint>
#include <span>
#include <ostream>
#include <string>

namespace rt::native::preview {

struct CarMeshResult {
    assets::CarModelError err = assets::CarModelError::Ok;
    assets::NativeModel model;
    float appliedScale = 1.0f;
    std::array<float, 3> bboxMin{};
    std::array<float, 3> bboxMax{};
    std::string chosenAxis;  // "x" or "z" (longest horizontal)
    std::string sourceFile;
};

struct CarMeshBounds {
    std::size_t vertexCount = 0;
    std::array<float, 3> min{};
    std::array<float, 3> max{};
};

struct CarMeshModelDiagnostic {
    CarMeshBounds raw;
    CarMeshBounds main;
    CarMeshBounds acceptedParts;
    CarMeshBounds rejectedParts;
};

struct CarMeshDiagnostics {
    assets::CarModelError err = assets::CarModelError::Ok;
    std::array<CarMeshModelDiagnostic, 6> models{};
};

// Decodes every model without normalization, reports its main-chain bounds and
// the bounds of count-gated parts accepted/rejected by the body-height gate.
// This is diagnostic-only: all coordinates remain in the CAR/VIF source axes.
CarMeshDiagnostics inspectCarMeshDiagnostics(std::span<const std::uint8_t> bytes,
                                             const std::string& sourceFile);

// Emits one deterministic row per CAR model. Used by the native asset test and
// by loadCarMeshFromFile(verbose=true) for manual asset investigation.
void dumpCarMeshDiagnostics(const CarMeshDiagnostics& diagnostics,
                            std::ostream& out);

// Converts the bytes of a CAR file (via convertCarModels) into a single
// merged NativeModel for model 0 only. See .cpp for merge rules, normal
// fallback, and alignment. targetLength is the desired longest-horizontal
// size in render units (default matches the scale chosen in preview.cpp
// usage sites for cars).
CarMeshResult buildCarMeshFromCarFile(std::span<const std::uint8_t> bytes,
                                      const std::string& sourceFile,
                                      float targetLength = 4.0f);

// Convenience: reads the file at path, computes a *real* (non-zero) SHA-256
// for provenance (zeros are rejected by convertCarModels), then delegates
// to buildCarMeshFromCarFile. Returns true on Ok.
bool loadCarMeshFromFile(const std::string& path, CarMeshResult& out,
                          float targetLength = 4.0f,
                          bool verbose = false);

// ---------------------------------------------------------------------------
// Test helpers (enable hand-built MeshRecord fixtures for merge/normal/scale
// unit tests without constructing full CAR byte blobs, which is impractical
// for synthetic coverage of the math).
// ---------------------------------------------------------------------------

struct CarMeshTransformInfo {
    float scale = 1.0f;
    std::string axis;
    std::array<float, 3> bboxMin{};
    std::array<float, 3> bboxMax{};
};

// Merges the given (non-empty) MeshRecords into one NativeModel, applies
// flat-face normal fallback where normals are zero, sets uv=0 and a neutral
// body color (light blue-grey), then uniformly scales+centers using the
// longest horizontal axis to the target length. Populates info with the
// results. Used by buildCarMeshFromCarFile and by tests.
assets::NativeModel mergeAndNormalizeCarMeshes(
    std::span<const assets::MeshRecord> records,
    float targetLength,
    CarMeshTransformInfo& info);

} // namespace rt::native::preview
