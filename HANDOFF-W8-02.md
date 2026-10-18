# W8-02 handoff — opt-in headless autopilot

## Delivered files and touched ranges

- `src/native/app/Autopilot.h:1-46` — deterministic, header-only P1 policy:
  disabled by default; `EXPLORE` supplies held gas with zero steering; `RESULTS`
  returns one continue action after its configured logic-frame hold.
- `src/native/app/Autopilot.cpp:1` — module translation-unit anchor for the
  standalone unit-test target. The production app uses the header-only policy,
  so no edit to coordinator-owned `src/native/CMakeLists.txt` was required.
- `src/native/app/main.cpp:35,2064-2075,2266-2273` — parses exact
  `RT_APP_AUTOPILOT=1` only when `RT_APP_HEADLESS` is present, parses
  `RT_APP_AUTOPILOT_HOLD` (minimum/default 1), excludes lockstep socket runs,
  supplies the policy input, and calls `session.continueFromResults()` through
  the same method used by the Return-key path. It logs enablement and each
  automatic continue.
- `tests/native_w22/autopilot_test.cpp:1-62` — pure policy tests: off gating,
  synthesized centered throttle, results hold/reset. Follows the required ELF
  lookup/SKIP convention and prints `checks=N failed=M`.
- `tests/native_w22/CMakeLists.txt:1-6` — registration:
  `add_executable(w22_autopilot_test autopilot_test.cpp .../Autopilot.cpp)`,
  include root, C++20, and `${RT_NATIVE_WARN_OPTIONS}`.

`src/native/CMakeLists.txt` was intentionally not edited. Its current parent
test list does not add `tests/native_w22`, so the coordinator must add that
subdirectory when permitted; the supplied registration itself is valid when
included as a subdirectory.

## Evidence

### TDD unit policy gate

RED (before `Autopilot.cpp` existed):

```sh
c++ -std=c++20 -Wall -Wextra -Isrc/native tests/native_w22/autopilot_test.cpp src/native/app/Autopilot.cpp -o build/w-W8-2-autopilot-test
```

Expected failure: `clang++: error: no such file or directory:
'src/native/app/Autopilot.cpp'`.

GREEN (exact CMake registration, through a temporary parent because the source
parent is coordinator-owned):

```sh
cmake -S /private/var/folders/66/v2t7gtq135l3g5jlkbrp72_80000gn/T/opencode/w8-02-autopilot-cmake -B build/w-W8-2-autopilot-cmake
cmake --build build/w-W8-2-autopilot-cmake --target w22_autopilot_test -j4
RT_ELF="$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98" build/w-W8-2-autopilot-cmake/native_w22/w22_autopilot_test
```

Tail:

```text
[100%] Built target w22_autopilot_test
checks=8 failed=0
```

Existing session contract was built unchanged and passed:

```sh
cmake --build build/w-W8-2 --target native_assets_session_test -j4
build/w-W8-2/native_assets_session_test
```

Tail: `session: all checks passed`.

### App and requested integration gates — blocked before launch

I used the required isolated configuration/build command:

```sh
export RT_SKIP_ANDROID_CHECK=1 RT_SKIP_VRC_PLUGIN=1
cmake -S src/native -B build/w-W8-2 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
cmake --build build/w-W8-2 --target rt-app -j4
```

It fails before linking/running the app due to an unrelated concurrent source
mismatch in `src/native/app/main.cpp:1397`:

```text
error: no viable conversion from returned value of type 'TitleUiDrawData'
to function return type 'rt::native::preview::UiScreen'
if (state == AppState::LOAD) return titleUi.emit(width, height);
```

Therefore none of these `rt-app` runtime gates could truthfully be run on this
change: OFF snapshot comparison, ON explore movement, ON race auto-continue,
or ON snapshot determinism. Rebuild `rt-app` after resolving that unrelated
title-UI conflict, then run the requested commands with all `RT_*` values
before `timeout` and the executable.

## Residual risks

- Explore uses constant centered throttle by design; it can drive into walls or
  remain constrained by unmerged town collision behavior.
- The runtime integration is unverified until the unrelated title-UI compile
  error is resolved; no snapshots or race execution claims are made here.
