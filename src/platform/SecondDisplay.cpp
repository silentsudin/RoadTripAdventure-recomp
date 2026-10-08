// A second display's surface and touches (see SecondDisplay.h).

#include "platform/SecondDisplay.h"

#include "settings/Capabilities.h"

#include <mutex>

#if defined(__ANDROID__)
#include <SDL3/SDL_system.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>
#endif

namespace rt::seconddisplay
{
    namespace
    {
        std::mutex g_mutex;
        void *g_window = nullptr; // acquired
        bool g_windowChanged = false;
        std::vector<Touch> g_touches;
    }

    bool takeWindow(void *&window)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (!g_windowChanged)
            return false;
        window = g_window;
        g_window = nullptr;
        g_windowChanged = false;
        return true;
    }

    std::vector<Touch> takeTouches()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::vector<Touch> out;
        out.swap(g_touches);
        return out;
    }

    void setEnabled(bool on)
    {
#if defined(__ANDROID__)
        auto *env = static_cast<JNIEnv *>(SDL_GetAndroidJNIEnv());
        auto activity = static_cast<jobject>(SDL_GetAndroidActivity());
        if (!env || !activity)
            return;
        jclass cls = env->GetObjectClass(activity);
        if (jmethodID m = env->GetMethodID(cls, "setSecondScreenEnabled", "(Z)V"))
            env->CallVoidMethod(activity, m, static_cast<jboolean>(on));
        if (env->ExceptionCheck())
            env->ExceptionClear();
        env->DeleteLocalRef(cls);
        env->DeleteLocalRef(activity);
#else
        (void)on;
#endif
    }
}

#if defined(__ANDROID__)
using namespace rt::seconddisplay;

extern "C" JNIEXPORT void JNICALL Java_io_github_roadtrip_RoadTripActivity_nativeSecondSurface(JNIEnv *env, jclass, jobject surface)
{
    ANativeWindow *window = surface ? ANativeWindow_fromSurface(env, surface) : nullptr; // acquired
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_window)
        ANativeWindow_release(static_cast<ANativeWindow *>(g_window)); // never handed out
    g_window = window;
    g_windowChanged = true;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_roadtrip_RoadTripActivity_nativeSecondDisplay(JNIEnv *, jclass, jboolean present)
{
    rt::settings::capabilities().secondDisplay = present;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_roadtrip_RoadTripActivity_nativeSecondTouch(JNIEnv *, jclass, jint action, jint pointer,
                                                                                             jfloat x, jfloat y)
{
    // MotionEvent: 0 DOWN, 1 UP, 2 MOVE, 3 CANCEL, 5 POINTER_DOWN, 6 POINTER_UP.
    Touch::Kind kind;
    switch (action)
    {
    case 0:
    case 5: kind = Touch::Down; break;
    case 2: kind = Touch::Move; break;
    case 1:
    case 3:
    case 6: kind = Touch::Up; break;
    default: return;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_touches.size() < 256)
        g_touches.push_back({kind, pointer, x, y});
}
#endif
