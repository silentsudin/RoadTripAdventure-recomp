#pragma once
// Android second display (the Thor's lower screen). RoadTripNativeActivity shows a Presentation with
// a SurfaceView; this is the native end of that seam. Every other platform gets inline no-ops, so
// the desktop and headless paths do no extra work.
#include <cstdint>

namespace rt::native::app::seconddisplay {

#ifdef __ANDROID__
bool supported();                                    // a presentation display is attached (polled once a second)
void service(bool wanted, double now);               // show/dismiss the Presentation to match Options -> Second screen
bool surface(int& width, int& height);               // the SurfaceView's surface exists, at this size
bool takeTouch(float& x, float& y);                  // next finger-down, as 0..1 fractions of the surface
void push(const std::uint8_t* rgba, int width, int height);  // draw one RGBA frame (surface size)
#else
constexpr bool supported() { return false; }
inline void service(bool, double) {}
inline bool surface(int&, int&) { return false; }
inline bool takeTouch(float&, float&) { return false; }
inline void push(const std::uint8_t*, int, int) {}
#endif

}  // namespace rt::native::app::seconddisplay
