#!/bin/bash
# Rebuilds paraLLEl-GS's precompiled shaders (gs/shaders/slangmosh.hpp) after editing them.
# Builds the slangmosh compiler once (needs Granite's glslang/shaderc/spirv-* submodules:
#   git -C <parallel-gs>/Granite submodule update --init --depth 1 third_party/glslang
#   third_party/shaderc third_party/spirv-cross third_party/spirv-tools third_party/spirv-headers
#   third_party/rapidjson).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PGS="$ROOT/third_party/PS2Recomp/ps2xRuntime/third_party/parallel-gs"
TOOL="$ROOT/build/tools/slangmosh"
if [ ! -x "$TOOL/granite/slangmosh/slangmosh" ]; then
    cmake -S "$ROOT/tools/slangmosh" -B "$TOOL" -G Ninja -DCMAKE_BUILD_TYPE=Release
    cmake --build "$TOOL" --target slangmosh
fi
cd "$PGS/gs"
"$TOOL/granite/slangmosh/slangmosh" --namespace ParallelGS shaders/slangmosh.json \
    --output shaders/slangmosh.hpp --output-interface shaders/slangmosh_iface.hpp -O --strip
echo "regenerated $PGS/gs/shaders/slangmosh.hpp"
