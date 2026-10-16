plugins { id("com.android.application") }

android {
    namespace = "io.github.silentsudin.roadtrip.dev"
    compileSdk = 36

    defaultConfig {
        applicationId = "io.github.silentsudin.roadtrip.dev"
        minSdk = 31
        targetSdk = 34
        versionCode = 1
        versionName = "0.1.0-native"
        ndk { abiFilters += "arm64-v8a" }
        externalNativeBuild {
            cmake {
                arguments += listOf("-DANDROID_STL=c++_shared")
            }
        }
    }
    externalNativeBuild {
        cmake { path = file("../../CMakeLists.txt"); version = "3.22.1" }
    }
    sourceSets["main"].java.srcDir(rootProject.file("../../../platform/android/app/src/main/java"))
    packaging { jniLibs { useLegacyPackaging = true; keepDebugSymbols += "**/*.so" } }
    buildTypes { getByName("debug") { isMinifyEnabled = false } }
    compileOptions { sourceCompatibility = JavaVersion.VERSION_17; targetCompatibility = JavaVersion.VERSION_17 }
}
