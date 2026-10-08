#pragma once

// Android: the build kit (headers, sysroot, compiler resources: no game code) ships in the APK as
// assets/rt.tar and is extracted to files/res (paths::bundleResources()) when its build id
// (assets/rt.id) differs from the extracted one.

#include <string>

namespace rt::android
{
    bool ensureBuildKit(std::string &error);
}
