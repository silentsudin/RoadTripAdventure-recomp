#pragma once
// W16.2/W16.3: the GL entry points rt-app uses (GL 3.3 core / GLES 3.0 subset), loaded through
// SDL_GL_GetProcAddress so there is no system GL header or link dependency. Shared by the presenter
// (present_gl.cpp) and the scene renderer (preview_gl.cpp), which run on one context.
#include <SDL3/SDL.h>

#include <cstddef>
#include <cstdio>
#include <type_traits>

namespace rt::native::preview::glapi {

#ifdef _WIN32
#define RT_GLAPI __stdcall
#else
#define RT_GLAPI
#endif
using GLuint = unsigned; using GLint = int; using GLenum = unsigned; using GLsizei = int;
using GLchar = char; using GLbitfield = unsigned; using GLfloat = float; using GLboolean = unsigned char;
using GLsizeiptr = std::ptrdiff_t;

constexpr GLenum kVertexShader = 0x8B31, kFragmentShader = 0x8B30, kCompileStatus = 0x8B81,
                 kLinkStatus = 0x8B82, kTexture2D = 0x0DE1, kRgba = 0x1908, kRgba8 = 0x8058,
                 kUnsignedByte = 0x1401, kMinFilter = 0x2801, kMagFilter = 0x2800, kNearest = 0x2600, kLinear = 0x2601, kLinearMipmapLinear = 0x2703,
                 kWrapS = 0x2802, kWrapT = 0x2803, kClampToEdge = 0x812F, kRepeat = 0x2901, kTriangles = 4,
                 kUnpackAlignment = 0x0CF5, kPackAlignment = 0x0D05, kTexture0 = 0x84C0, kVersion = 0x1F02,
                 kRenderer = 0x1F01, kArrayBuffer = 0x8892, kElementArrayBuffer = 0x8893, kStreamDraw = 0x88E0,
                 kFloat = 0x1406, kUnsignedInt = 0x1405, kFramebuffer = 0x8D40, kRenderbuffer = 0x8D41,
                 kColorAttachment0 = 0x8CE0, kDepthAttachment = 0x8D00, kDepthComponent32F = 0x8CAC,
                 kFramebufferComplete = 0x8CD5, kColorBufferBit = 0x4000, kDepthBufferBit = 0x100,
                 kDepthTest = 0x0B71, kBlend = 0x0BE2, kCullFace = 0x0B44, kScissorTest = 0x0C11,
                 kDither = 0x0BD0, kLess = 0x0201;

struct Gl {
    GLuint (RT_GLAPI* CreateShader)(GLenum); void (RT_GLAPI* ShaderSource)(GLuint, GLsizei, const GLchar* const*, const GLint*);
    void (RT_GLAPI* CompileShader)(GLuint); void (RT_GLAPI* GetShaderiv)(GLuint, GLenum, GLint*);
    void (RT_GLAPI* GetShaderInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
    GLuint (RT_GLAPI* CreateProgram)(); void (RT_GLAPI* AttachShader)(GLuint, GLuint); void (RT_GLAPI* LinkProgram)(GLuint);
    void (RT_GLAPI* GetProgramiv)(GLuint, GLenum, GLint*); void (RT_GLAPI* GetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
    void (RT_GLAPI* UseProgram)(GLuint); GLint (RT_GLAPI* GetUniformLocation)(GLuint, const GLchar*); void (RT_GLAPI* Uniform1i)(GLint, GLint);
    void (RT_GLAPI* DeleteShader)(GLuint); void (RT_GLAPI* DeleteProgram)(GLuint);
    void (RT_GLAPI* GenVertexArrays)(GLsizei, GLuint*); void (RT_GLAPI* BindVertexArray)(GLuint); void (RT_GLAPI* DeleteVertexArrays)(GLsizei, const GLuint*);
    void (RT_GLAPI* GenTextures)(GLsizei, GLuint*); void (RT_GLAPI* BindTexture)(GLenum, GLuint); void (RT_GLAPI* DeleteTextures)(GLsizei, const GLuint*);
    void (RT_GLAPI* TexParameteri)(GLenum, GLenum, GLint);
    void (RT_GLAPI* TexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
    void (RT_GLAPI* TexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void*);
    void (RT_GLAPI* ActiveTexture)(GLenum); void (RT_GLAPI* PixelStorei)(GLenum, GLint);
    void (RT_GLAPI* Viewport)(GLint, GLint, GLsizei, GLsizei); void (RT_GLAPI* DrawArrays)(GLenum, GLint, GLsizei);
    void (RT_GLAPI* ReadPixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void*);
    const unsigned char* (RT_GLAPI* GetString)(GLenum);
    // W16.3 additions
    void (RT_GLAPI* GenBuffers)(GLsizei, GLuint*); void (RT_GLAPI* BindBuffer)(GLenum, GLuint); void (RT_GLAPI* DeleteBuffers)(GLsizei, const GLuint*);
    void (RT_GLAPI* BufferData)(GLenum, GLsizeiptr, const void*, GLenum);
    void (RT_GLAPI* EnableVertexAttribArray)(GLuint);
    void (RT_GLAPI* VertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
    void (RT_GLAPI* GenFramebuffers)(GLsizei, GLuint*); void (RT_GLAPI* BindFramebuffer)(GLenum, GLuint); void (RT_GLAPI* DeleteFramebuffers)(GLsizei, const GLuint*);
    void (RT_GLAPI* GenRenderbuffers)(GLsizei, GLuint*); void (RT_GLAPI* BindRenderbuffer)(GLenum, GLuint); void (RT_GLAPI* DeleteRenderbuffers)(GLsizei, const GLuint*);
    void (RT_GLAPI* RenderbufferStorage)(GLenum, GLenum, GLsizei, GLsizei);
    void (RT_GLAPI* FramebufferRenderbuffer)(GLenum, GLenum, GLenum, GLuint);
    GLenum (RT_GLAPI* CheckFramebufferStatus)(GLenum);
    void (RT_GLAPI* ClearColor)(GLfloat, GLfloat, GLfloat, GLfloat); void (RT_GLAPI* Clear)(GLbitfield);
    void (RT_GLAPI* Enable)(GLenum); void (RT_GLAPI* Disable)(GLenum);
    void (RT_GLAPI* DepthFunc)(GLenum); void (RT_GLAPI* DepthMask)(GLboolean);
    void (RT_GLAPI* ColorMask)(GLboolean, GLboolean, GLboolean, GLboolean);
    void (RT_GLAPI* DrawElements)(GLenum, GLsizei, GLenum, const void*);
    void (RT_GLAPI* UniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat*);
    void (RT_GLAPI* Uniform4fv)(GLint, GLsizei, const GLfloat*);
    GLenum (RT_GLAPI* GetError)();
};

inline bool loadGl(Gl& g) {
    bool ok = true;
    auto load = [&](auto& fn, const char* name) {
        fn = reinterpret_cast<std::remove_reference_t<decltype(fn)>>(SDL_GL_GetProcAddress(name));
        if (!fn) { std::fprintf(stderr, "[gl] missing %s\n", name); ok = false; }
    };
#define L(n) load(g.n, "gl" #n)
    L(CreateShader); L(ShaderSource); L(CompileShader); L(GetShaderiv); L(GetShaderInfoLog); L(CreateProgram);
    L(AttachShader); L(LinkProgram); L(GetProgramiv); L(GetProgramInfoLog); L(UseProgram); L(GetUniformLocation);
    L(Uniform1i); L(DeleteShader); L(DeleteProgram); L(GenVertexArrays); L(BindVertexArray); L(DeleteVertexArrays);
    L(GenTextures); L(BindTexture); L(DeleteTextures); L(TexParameteri); L(TexImage2D); L(TexSubImage2D);
    L(ActiveTexture); L(PixelStorei); L(Viewport); L(DrawArrays); L(ReadPixels); L(GetString);
    L(GenBuffers); L(BindBuffer); L(DeleteBuffers); L(BufferData); L(EnableVertexAttribArray); L(VertexAttribPointer);
    L(GenFramebuffers); L(BindFramebuffer); L(DeleteFramebuffers); L(GenRenderbuffers); L(BindRenderbuffer);
    L(DeleteRenderbuffers); L(RenderbufferStorage); L(FramebufferRenderbuffer); L(CheckFramebufferStatus);
    L(ClearColor); L(Clear); L(Enable); L(Disable); L(DepthFunc); L(DepthMask); L(ColorMask); L(DrawElements);
    L(UniformMatrix4fv); L(Uniform4fv); L(GetError);
#undef L
    return ok;
}

// ES 3.0 needs a precision default for samplers; desktop 330 core ignores none of this.
#ifdef __ANDROID__
constexpr const char* kPrologue = "#version 300 es\nprecision highp float;\nprecision highp sampler2D;\n";
#else
constexpr const char* kPrologue = "#version 330 core\n";
#endif

}  // namespace rt::native::preview::glapi
