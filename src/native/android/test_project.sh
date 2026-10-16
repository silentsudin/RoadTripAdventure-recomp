#!/usr/bin/env bash
# Fast, host-only contract check for the Android-native packaging identity.
set -euo pipefail

root="$(cd "$(dirname "$0")/../../.." && pwd)"
android="$root/src/native/android"

grep -Fq 'applicationId = "io.github.silentsudin.roadtrip.dev"' "$android/app/build.gradle.kts"
grep -Fq '<string name="app_name">Road Trip DEV</string>' "$android/app/src/main/res/values/strings.xml"
grep -Fq 'android:extractNativeLibs="true"' "$android/app/src/main/AndroidManifest.xml"
grep -Fq 'externalNativeBuild' "$android/app/build.gradle.kts"

echo 'native Android project contract: PASS'
