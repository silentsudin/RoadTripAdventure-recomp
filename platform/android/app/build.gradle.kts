// The Android app. Native code is built by CMake outside Gradle (scripts/android/build_apk.sh
// stages libmain.so, libc++_shared.so and the shipped compiler into build/android-jni); Gradle
// only packages. The APK contains no game code or data.
plugins { id("com.android.application") }

// Version: CI passes GitVersion's (-PrtVersionName=1.2.3 -PrtVersionCode=10203); local builds 0.1.0.
val rtVersionName = (findProperty("rtVersionName") as String?) ?: "0.1.0"
val rtVersionCode = (findProperty("rtVersionCode") as String?)?.toInt() ?: 1
// Release signing: a keystore from the environment (CI secrets), else the debug key.
val releaseKeystore = System.getenv("RT_KEYSTORE_FILE")?.let { file(it) }?.takeIf { it.exists() }

android {
    namespace = "io.github.roadtrip"
    compileSdk = 36
    defaultConfig {
        applicationId = "io.github.silentsudin.roadtrip"
        minSdk = 31
        targetSdk = 34
        versionCode = rtVersionCode
        versionName = rtVersionName
        ndk { abiFilters += "arm64-v8a" }
    }
    sourceSets["main"].jniLibs.srcDirs(rootDir.resolve("../../build/android-jni"))
    sourceSets["main"].assets.srcDirs(rootDir.resolve("../../build/android-assets"))
    // Native libraries are extracted to nativeLibraryDir, which may execute the shipped compiler.
    packaging { jniLibs { useLegacyPackaging = true; keepDebugSymbols += "**/*.so" } }
    signingConfigs {
        if (releaseKeystore != null) {
            create("release") {
                storeFile = releaseKeystore
                storePassword = System.getenv("RT_KEYSTORE_PASSWORD")
                keyAlias = System.getenv("RT_KEY_ALIAS")
                keyPassword = System.getenv("RT_KEY_PASSWORD")
            }
        }
    }
    buildTypes {
        getByName("release") {
            isMinifyEnabled = false
            signingConfig = signingConfigs.getByName(if (releaseKeystore != null) "release" else "debug")
        }
    }
    compileOptions { sourceCompatibility = JavaVersion.VERSION_17; targetCompatibility = JavaVersion.VERSION_17 }
}
