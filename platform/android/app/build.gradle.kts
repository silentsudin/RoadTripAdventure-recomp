// The Android app. Native code is built by CMake outside Gradle (scripts/android/build_apk.sh
// stages libmain.so, libc++_shared.so and the shipped compiler into build/android-jni); Gradle
// only packages. The APK contains no game code or data.
plugins { id("com.android.application") }

android {
    namespace = "io.github.roadtrip"
    compileSdk = 36
    defaultConfig {
        applicationId = "io.github.silentsudin.roadtrip"
        minSdk = 31
        targetSdk = 34
        versionCode = 1
        versionName = "0.1.0"
        ndk { abiFilters += "arm64-v8a" }
    }
    sourceSets["main"].jniLibs.srcDirs(rootDir.resolve("../../build/android-jni"))
    sourceSets["main"].assets.srcDirs(rootDir.resolve("../../build/android-assets"))
    // Native libraries are extracted to nativeLibraryDir, which may execute the shipped compiler.
    packaging { jniLibs { useLegacyPackaging = true; keepDebugSymbols += "**/*.so" } }
    buildTypes {
        getByName("release") { isMinifyEnabled = false; signingConfig = signingConfigs.getByName("debug") }
    }
    compileOptions { sourceCompatibility = JavaVersion.VERSION_17; targetCompatibility = JavaVersion.VERSION_17 }
}
