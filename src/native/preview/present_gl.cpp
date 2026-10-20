// W16.2: GL presenter. Own GL typedefs + SDL_GL_GetProcAddress loading: no system GL headers or
// link dependency (macOS gl.h is deprecated; Android/Linux link libGL/EGL only through SDL).
#include "preview/present_gl.h"
#include "preview/gl_api.h"

#include <SDL3/SDL.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

namespace rt::native::preview {
namespace {

using namespace glapi;

// Fullscreen triangle; y flip lives here (row 0 of the CPU buffer is the top) so no glClipControl.
constexpr const char* kVert =
    "out vec2 vUV;\n"
    "void main() {\n"
    "  vec2 p = vec2(gl_VertexID == 1 ? 3.0 : -1.0, gl_VertexID == 2 ? 3.0 : -1.0);\n"
    "  vUV = vec2(p.x * 0.5 + 0.5, 0.5 - p.y * 0.5);\n"
    "  gl_Position = vec4(p, 0.0, 1.0);\n"
    "}\n";
constexpr const char* kFrag =
    "in vec2 vUV;\nuniform sampler2D uTex;\nout vec4 oColor;\n"
    "void main() { oColor = vec4(texture(uTex, vUV).rgb, 1.0); }\n";

}  // namespace

struct GlPresenter {
    SDL_Window* window = nullptr;
    SDL_GLContext ctx = nullptr;
    Gl gl{};
    GLuint program = 0, vao = 0, tex = 0;
    std::uint32_t w = 0, h = 0;
    std::string dumpPath;
    std::vector<std::uint8_t> readback;
};

void glPresenterSetWindowAttributes() {
#ifdef __ANDROID__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);  // required on macOS
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

static GLuint compile(const Gl& g, GLenum type, const char* body, std::string& error) {
    const char* src[2] = {kPrologue, body};
    const GLuint s = g.CreateShader(type);
    g.ShaderSource(s, 2, src, nullptr); g.CompileShader(s);
    GLint ok = 0; g.GetShaderiv(s, kCompileStatus, &ok);
    if (!ok) { char log[1024] = {}; g.GetShaderInfoLog(s, sizeof log, nullptr, log); error = std::string("GL shader: ") + log; g.DeleteShader(s); return 0; }
    return s;
}

static void allocTexture(GlPresenter& p) {
    p.gl.BindTexture(kTexture2D, p.tex);
    p.gl.PixelStorei(kUnpackAlignment, 1);
    p.gl.TexImage2D(kTexture2D, 0, static_cast<GLint>(kRgba8), static_cast<GLsizei>(p.w), static_cast<GLsizei>(p.h), 0,
                    kRgba, kUnsignedByte, nullptr);
}

void glPresenterResize(GlPresenter* p, std::uint32_t& w, std::uint32_t& h) {
    int pw = 0, ph = 0; SDL_GetWindowSizeInPixels(p->window, &pw, &ph);
    if (pw > 0 && ph > 0 && (static_cast<std::uint32_t>(pw) != p->w || static_cast<std::uint32_t>(ph) != p->h)) {
        p->w = static_cast<std::uint32_t>(pw); p->h = static_cast<std::uint32_t>(ph);
        allocTexture(*p);
    }
    w = p->w; h = p->h;
}

GlPresenter* glPresenterCreate(SDL_Window* window, std::string& error) {
    auto* p = new GlPresenter();
    p->window = window;
    p->ctx = SDL_GL_CreateContext(window);
    if (!p->ctx) { error = std::string("SDL_GL_CreateContext: ") + SDL_GetError(); delete p; return nullptr; }
    if (!loadGl(p->gl)) { error = "GL entry points missing"; glPresenterDestroy(p); return nullptr; }
    SDL_GL_SetSwapInterval(1);
    const Gl& g = p->gl;
    const GLuint vs = compile(g, kVertexShader, kVert, error);
    const GLuint fs = vs ? compile(g, kFragmentShader, kFrag, error) : 0;
    if (!vs || !fs) { if (vs) g.DeleteShader(vs); glPresenterDestroy(p); return nullptr; }
    p->program = g.CreateProgram(); g.AttachShader(p->program, vs); g.AttachShader(p->program, fs); g.LinkProgram(p->program);
    g.DeleteShader(vs); g.DeleteShader(fs);
    GLint linked = 0; g.GetProgramiv(p->program, kLinkStatus, &linked);
    if (!linked) { char log[1024] = {}; g.GetProgramInfoLog(p->program, sizeof log, nullptr, log); error = std::string("GL link: ") + log; glPresenterDestroy(p); return nullptr; }
    g.GenVertexArrays(1, &p->vao);  // core profile needs a VAO even with no attributes
    g.GenTextures(1, &p->tex);
    g.BindTexture(kTexture2D, p->tex);
    g.TexParameteri(kTexture2D, kMinFilter, static_cast<GLint>(kNearest)); g.TexParameteri(kTexture2D, kMagFilter, static_cast<GLint>(kNearest));
    g.TexParameteri(kTexture2D, kWrapS, static_cast<GLint>(kClampToEdge)); g.TexParameteri(kTexture2D, kWrapT, static_cast<GLint>(kClampToEdge));
    std::uint32_t w = 0, h = 0; glPresenterResize(p, w, h);
    if (const char* d = std::getenv("RT_APP_PRESENT_DUMP"); d && *d) p->dumpPath = d;
    std::fprintf(stderr, "[present-gl] %s | %s | %ux%u\n", g.GetString(kVersion) ? reinterpret_cast<const char*>(g.GetString(kVersion)) : "?",
                 g.GetString(kRenderer) ? reinterpret_cast<const char*>(g.GetString(kRenderer)) : "?", p->w, p->h);
    return p;
}

bool glPresent(GlPresenter* p, const std::vector<std::uint8_t>& rgba, std::string& error) {
    (void)error;
    if (!p->w || !p->h || rgba.size() != static_cast<std::size_t>(p->w) * p->h * 4) return true;
    const Gl& g = p->gl;
    g.Viewport(0, 0, static_cast<GLsizei>(p->w), static_cast<GLsizei>(p->h));
    g.UseProgram(p->program);
    g.ActiveTexture(kTexture0); g.BindTexture(kTexture2D, p->tex);
    g.PixelStorei(kUnpackAlignment, 1);
    g.TexSubImage2D(kTexture2D, 0, 0, 0, static_cast<GLsizei>(p->w), static_cast<GLsizei>(p->h), kRgba, kUnsignedByte, rgba.data());
    g.Uniform1i(g.GetUniformLocation(p->program, "uTex"), 0);
    g.BindVertexArray(p->vao);
    g.DrawArrays(kTriangles, 0, 3);
    if (!p->dumpPath.empty()) {  // debug: last presented frame as P6, read back from the back buffer
        p->readback.resize(rgba.size());
        g.PixelStorei(kPackAlignment, 1);
        g.ReadPixels(0, 0, static_cast<GLsizei>(p->w), static_cast<GLsizei>(p->h), kRgba, kUnsignedByte, p->readback.data());
        std::ofstream out(p->dumpPath, std::ios::binary);
        out << "P6\n" << p->w << ' ' << p->h << "\n255\n";
        const std::size_t row = static_cast<std::size_t>(p->w) * 4;
        for (std::uint32_t y = 0; y < p->h; ++y) {
            const std::uint8_t* s = p->readback.data() + static_cast<std::size_t>(p->h - 1 - y) * row;  // GL rows are bottom-up
            for (std::uint32_t x = 0; x < p->w; ++x) out.write(reinterpret_cast<const char*>(s + x * 4), 3);
        }
    }
    SDL_GL_SwapWindow(p->window);
    return true;
}

void glPresenterDestroy(GlPresenter* p) {
    if (!p) return;
    if (p->ctx) {
        if (p->tex) p->gl.DeleteTextures(1, &p->tex);
        if (p->vao) p->gl.DeleteVertexArrays(1, &p->vao);
        if (p->program) p->gl.DeleteProgram(p->program);
        SDL_GL_DestroyContext(p->ctx);
    }
    delete p;
}

} // namespace rt::native::preview
