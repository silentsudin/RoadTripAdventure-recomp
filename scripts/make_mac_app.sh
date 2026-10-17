#!/bin/sh
# make_mac_app.sh — wrap build/native/rt-app in a minimal .app bundle so
# LaunchServices launches it INSIDE the user's GUI login session (the agent
# sandbox can lose WindowServer access; `open` cannot run bare Mach-O).
# Usage: scripts/make_mac_app.sh KEY=VAL [KEY=VAL ...]   (env baked in)
set -e
ROOT=$(cd "$(dirname "$0")/.." && pwd)
BIN="$ROOT/build/native/rt-app"
APP="$ROOT/build/scratch/RoadTripDev.app"
[ -x "$BIN" ] || { echo "missing $BIN"; exit 1; }
rm -rf "$APP"
mkdir -p "$APP/Contents/MacOS"
cat > "$APP/Contents/Info.plist" << 'PLIST'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0"><dict>
  <key>CFBundleExecutable</key><string>launcher</string>
  <key>CFBundleIdentifier</key><string>io.github.silentsudin.roadtrip.mactest</string>
  <key>CFBundleName</key><string>Road Trip DEV (test)</string>
  <key>CFBundlePackageType</key><string>APPL</string>
  <key>CFBundleShortVersionString</key><string>0-dev</string>
  <key>NSHighResolutionCapable</key><true/>
</dict></plist>
PLIST
# LaunchServices rejects shell-script bundle executables (-10827) -> compile
# a tiny Mach-O launcher that bakes the env and execs the real binary.
C_SRC="$ROOT/build/scratch/launcher.c"
{
  echo '#include <unistd.h>'
  echo '#include <stdlib.h>'
  echo '#include <string.h>'
  echo 'int main(void){'
  echo '  if(chdir("'"$ROOT"'")){}'
  for kv in "$@"; do
    echo "  setenv(\"${kv%%=*}\", \"${kv#*=}\", 1);"
  done
  echo '  execl("'"$BIN"'", "rt-app", (char*)0);'
  echo '  return 127;'
  echo '}'
} > "$C_SRC"
cc -o "$APP/Contents/MacOS/launcher" "$C_SRC"
chmod +x "$APP/Contents/MacOS/launcher"
codesign --force -s - "$APP" 2>/dev/null || echo "codesign skipped"
echo "built $APP"
