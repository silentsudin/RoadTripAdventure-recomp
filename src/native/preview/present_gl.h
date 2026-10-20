#pragma once
// W16.2: OpenGL 3.3-core / GLES 3.0 presenter. Same contract as the Vulkan present():
// a top-down RGBA8 CPU buffer of exactly the window's pixel size is shown 1:1 (nearest).
#include <cstdint>
#include <string>
#include <vector>

struct SDL_Window;

namespace rt::native::preview {

struct GlPresenter;

// Call before SDL_CreateWindow of an SDL_WINDOW_OPENGL window.
void glPresenterSetWindowAttributes();
// Creates the context on `window` (made with SDL_WINDOW_OPENGL) and the blit pipeline.
GlPresenter* glPresenterCreate(SDL_Window* window, std::string& error);
// Re-reads the drawable size (call after a window resize); returns it.
void glPresenterResize(GlPresenter* p, std::uint32_t& w, std::uint32_t& h);
// Uploads rgba (w*h*4 of the current size) and swaps. Size mismatch is a no-op, like the Vulkan path.
bool glPresent(GlPresenter* p, const std::vector<std::uint8_t>& rgba, std::string& error);
void glPresenterDestroy(GlPresenter* p);

} // namespace rt::native::preview
