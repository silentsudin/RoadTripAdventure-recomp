#ifdef __ANDROID__
#include "app/second_display.h"

#include <SDL3/SDL_system.h>
#include <jni.h>

#include <atomic>
#include <deque>
#include <mutex>
#include <utility>

namespace rt::native::app::seconddisplay {
namespace {
std::atomic<int> g_width{0}, g_height{0};
std::mutex g_touchMutex;
std::deque<std::pair<float, float>> g_touches;
bool g_supported = false;
bool g_shown = false;
double g_checked = -10, g_asked = -10;

// Calls a boolean/void method of the activity from the SDL main thread; false if the JVM is unreachable.
template <class Call>
bool onActivity(const char* name, const char* signature, Call call) {
    auto* activity = static_cast<jobject>(SDL_GetAndroidActivity());
    auto* env = static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
    if (!activity || !env) return false;
    jclass cls = env->GetObjectClass(activity);
    jmethodID method = cls ? env->GetMethodID(cls, name, signature) : nullptr;
    if (method) call(env, activity, method);
    const bool failed = env->ExceptionCheck();
    if (failed) env->ExceptionClear();
    if (cls) env->DeleteLocalRef(cls);
    return method && !failed;
}
}  // namespace

bool supported() { return g_supported; }

void service(bool wanted, double now) {
    if (now - g_checked >= 1.0) {
        g_checked = now;
        bool present = false;
        onActivity("hasSecondDisplay", "()Z", [&](JNIEnv* env, jobject a, jmethodID m) {
            present = env->CallBooleanMethod(a, m) == JNI_TRUE;
        });
        g_supported = present;
    }
    const bool want = wanted && g_supported;
    // Ask on a change, and once a second while it should be up but has no surface (show() can fail early).
    const bool retry = want && g_width.load() == 0 && now - g_asked >= 1.0;
    if (want == g_shown && !retry) return;
    g_asked = now;
    g_shown = want;
    onActivity("setSecondScreen", "(Z)V", [&](JNIEnv* env, jobject a, jmethodID m) {
        env->CallVoidMethod(a, m, static_cast<jboolean>(want));
    });
}

bool surface(int& width, int& height) {
    width = g_width.load();
    height = g_height.load();
    return width > 0 && height > 0;
}

bool takeTouch(float& x, float& y) {
    std::lock_guard<std::mutex> lock(g_touchMutex);
    if (g_touches.empty()) return false;
    x = g_touches.front().first;
    y = g_touches.front().second;
    g_touches.pop_front();
    return true;
}

void push(const std::uint8_t* rgba, int width, int height) {
    onActivity("drawSecondFrame", "(Ljava/nio/ByteBuffer;II)V", [&](JNIEnv* env, jobject a, jmethodID m) {
        jobject buffer = env->NewDirectByteBuffer(const_cast<std::uint8_t*>(rgba),
                                                  static_cast<jlong>(width) * height * 4);
        if (!buffer) return;
        env->CallVoidMethod(a, m, buffer, width, height);
        env->DeleteLocalRef(buffer);
    });
}

}  // namespace rt::native::app::seconddisplay

extern "C" {
JNIEXPORT void JNICALL Java_io_github_silentsudin_roadtrip_dev_RoadTripNativeActivity_nativeSecondSurface(
    JNIEnv*, jclass, jint width, jint height) {
    rt::native::app::seconddisplay::g_height = height;
    rt::native::app::seconddisplay::g_width = width;
}
JNIEXPORT void JNICALL Java_io_github_silentsudin_roadtrip_dev_RoadTripNativeActivity_nativeSecondTouch(
    JNIEnv*, jclass, jfloat x, jfloat y) {
    std::lock_guard<std::mutex> lock(rt::native::app::seconddisplay::g_touchMutex);
    auto& q = rt::native::app::seconddisplay::g_touches;
    if (q.size() < 16) q.emplace_back(x, y);
}
}
#endif
