// W16.3: GL 3.3-core / GLES 3.0 implementation of the flat (previewRender2D, flat-lit previewRender) and
// UI (FONTE/backdrop glyph quads) draw classes. Runs on the presenter's context (main thread), renders to
// an RGBA8 + D32F renderbuffer FBO and reads back top-down RGBA exactly like the Vulkan copy.
#include "preview/gl_api.h"
#include "preview/gl_route.h"
#include "preview/preview_backend.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <unordered_map>

namespace rt::native::preview {
namespace {

using namespace glapi;

// GLSL twins of preview_ui / preview_hud / preview (flat) shaders. No y flip: see gl_route.h.
constexpr const char* kUiVert =
    "layout(location = 0) in vec2 inPosition;\nlayout(location = 1) in vec4 inColor;\nout vec4 vColor;\n"
    "void main() { vColor = inColor; gl_Position = vec4(inPosition, 0.0, 1.0); }\n";
constexpr const char* kUiFrag =
    "in vec4 vColor;\nout vec4 outColor;\nvoid main() { outColor = vColor; }\n";
constexpr const char* kHudVert =
    "layout(location = 0) in vec2 inPosition;\nlayout(location = 1) in vec2 inUV;\nlayout(location = 2) in vec4 inColor;\n"
    "out vec2 vUV;\nout vec4 vColor;\n"
    "void main() { vUV = inUV; vColor = inColor; gl_Position = vec4(inPosition, 0.0, 1.0); }\n";
constexpr const char* kHudFrag =
    "in vec2 vUV;\nin vec4 vColor;\nout vec4 outColor;\nuniform sampler2D atlas;\n"
    "void main() {\n  vec4 tex = texture(atlas, vUV);\n  if (tex.a < 0.02) discard;\n"
    "  outColor = vec4(tex.rgb * vColor.rgb, vColor.a * tex.a);\n}\n";
// Matrix products are written as column sums in the order MoltenVK evaluates them; `M * v` makes the GL compiler
// add column 3 last, a 1-ulp clip difference that flips edge coverage (GL-DIFF.md).
// Vulkan clip z is 0..w, GL's -w..w: z' = 2z - w makes window depth equal Vulkan's z/w.
constexpr const char* kLitVert =
    "layout(location = 0) in vec3 inPos;\nlayout(location = 1) in vec3 inColor;\n"
    "uniform mat4 uMvp;\nuniform mat4 uView;\nout vec3 vView;\nout vec3 vColor;\n"
    "void main() {\n  vColor = inColor;\n  vView = (uView[0] * inPos.x + uView[1] * inPos.y + uView[2] * inPos.z + uView[3]).xyz;\n"
    "  vec4 c = uMvp[0] * inPos.x + uMvp[1] * inPos.y + uMvp[2] * inPos.z + uMvp[3];\n  c.z = 2.0 * c.z - c.w;\n  gl_Position = c;\n}\n";
constexpr const char* kLitFrag =
    "in vec3 vView;\nin vec3 vColor;\nout vec4 outColor;\nuniform vec4 uFog;\n"
    "void main() {\n  vec3 n = normalize(cross(dFdx(vView), dFdy(vView)));\n"
    "  float l = max(dot(n, normalize(vec3(0.4, 0.6, 1.0))), 0.0);\n"
    "  vec3 lit = vec3(0.85 + 0.15 * l) * vColor;\n"
    "  float fogAmount = smoothstep(0.5 * uFog.a, uFog.a, length(vView));\n"
    "  outColor = vec4(mix(lit, uFog.rgb, fogAmount), 1.0);\n}\n";

// Textured terrain twin of preview_textured.vert/.frag (vUV carries uv*clip.w, divided back per fragment).
constexpr const char* kTexVert =
    "layout(location = 0) in vec3 inPos;\nlayout(location = 1) in vec3 inColor;\nlayout(location = 2) in vec2 inUV;\n"
    "uniform mat4 uMvp;\nuniform mat4 uView;\nout vec3 vView;\nout vec3 vColor;\nout vec2 vUV;\nout float vW;\n"
    "void main() {\n  vColor = inColor;\n  vec4 c = uMvp[0] * inPos.x + uMvp[1] * inPos.y + uMvp[2] * inPos.z + uMvp[3];\n"
    "  vView = (uView[0] * inPos.x + uView[1] * inPos.y + uView[2] * inPos.z + uView[3]).xyz;\n  vUV = inUV * c.w;\n  vW = c.w;\n"
    "  c.z = 2.0 * c.z - c.w;\n  gl_Position = c;\n}\n";
constexpr const char* kTexFrag =
    "in vec3 vView;\nin vec3 vColor;\nin vec2 vUV;\nin float vW;\nout vec4 outColor;\nuniform vec4 uFog;\n"
    "uniform sampler2D page;\n"
    "void main() {\n  vec2 uv = vUV / max(vW, 1e-6);\n  vec4 tex = texture(page, uv);\n  if (tex.a < 0.02) discard;\n"
    "  vec3 n = normalize(cross(dFdx(vView), dFdy(vView)));\n"
    "  float l = max(dot(n, normalize(vec3(0.4, 0.6, 1.0))), 0.0);\n"
    "  vec3 modulated = min(vColor * tex.rgb * 2.0, vec3(1.0));\n"
    "  vec3 lit = vec3(0.85 + 0.15 * l) * modulated;\n"
    "  float fogAmount = smoothstep(0.5 * uFog.a, uFog.a, length(vView));\n"
    "  outColor = vec4(mix(lit, uFog.rgb, fogAmount), 1.0);\n}\n";

struct Program { GLuint id = 0; GLint loc[4] = {-1, -1, -1, -1}; };

}  // namespace

struct GlRenderer {
    Gl gl{};
    std::uint32_t W = 0, H = 0;
    GLuint fbo = 0, colRb = 0, depRb = 0;
    std::uint64_t readPasses = 0; double readMs = 0;  // synchronous readback ledger (W16.5), logged at destroy
    Program ui, hud, lit, tex;
    GLuint vao[4] = {}, vbo[4] = {}, ibo = 0, tibo = 0;  // 0 ui, 1 hud, 2 lit, 3 textured
    std::unordered_map<std::uint64_t, GLuint> textures;  // content hash -> RGBA8 NEAREST/REPEAT
    GLuint atlas = 0;
    const std::uint8_t* atlasPtr = nullptr;
    std::uint32_t atlasW = 0, atlasH = 0;
    bool atlasReady = false;
};

namespace {

GLuint compileStage(const Gl& g, GLenum type, const char* body, std::string& error) {
    const char* src[2] = {kPrologue, body};
    const GLuint s = g.CreateShader(type);
    g.ShaderSource(s, 2, src, nullptr); g.CompileShader(s);
    GLint ok = 0; g.GetShaderiv(s, kCompileStatus, &ok);
    if (!ok) {
        char log[1024] = {}; g.GetShaderInfoLog(s, sizeof log, nullptr, log);
        error = std::string("GL shader: ") + log; g.DeleteShader(s); return 0;
    }
    return s;
}

bool makeProgram(const Gl& g, const char* vs, const char* fs, Program& out, std::string& error) {
    const GLuint v = compileStage(g, kVertexShader, vs, error);
    const GLuint f = v ? compileStage(g, kFragmentShader, fs, error) : 0;
    if (!v || !f) { if (v) g.DeleteShader(v); return false; }
    out.id = g.CreateProgram(); g.AttachShader(out.id, v); g.AttachShader(out.id, f); g.LinkProgram(out.id);
    g.DeleteShader(v); g.DeleteShader(f);
    GLint linked = 0; g.GetProgramiv(out.id, kLinkStatus, &linked);
    if (!linked) {
        char log[1024] = {}; g.GetProgramInfoLog(out.id, sizeof log, nullptr, log);
        error = std::string("GL link: ") + log; return false;
    }
    return true;
}

void destroyGl(GlRenderer& R) {
    const Gl& g = R.gl;
    if (g.DeleteTextures) for (auto& e : R.textures) g.DeleteTextures(1, &e.second);
    if (g.DeleteProgram) { for (Program* p : {&R.ui, &R.hud, &R.lit, &R.tex}) if (p->id) g.DeleteProgram(p->id); }
    if (g.DeleteBuffers) { g.DeleteBuffers(4, R.vbo); if (R.ibo) g.DeleteBuffers(1, &R.ibo); if (R.tibo) g.DeleteBuffers(1, &R.tibo); }
    if (g.DeleteVertexArrays) g.DeleteVertexArrays(4, R.vao);
    if (g.DeleteFramebuffers && R.fbo) g.DeleteFramebuffers(1, &R.fbo);
    if (g.DeleteRenderbuffers) { if (R.colRb) g.DeleteRenderbuffers(1, &R.colRb); if (R.depRb) g.DeleteRenderbuffers(1, &R.depRb); }
}

void* glCreate(std::uint32_t W, std::uint32_t H, std::string& error) {
    if (!SDL_GL_GetCurrentContext()) { error = "no current GL context"; return nullptr; }
    auto* R = new GlRenderer();
    R->W = W; R->H = H;
    const auto fail = [&]() -> void* { destroyGl(*R); delete R; return nullptr; };
    if (!loadGl(R->gl)) { error = "GL entry points missing"; return fail(); }
    const Gl& g = R->gl;
    g.GenFramebuffers(1, &R->fbo); g.GenRenderbuffers(1, &R->colRb); g.GenRenderbuffers(1, &R->depRb);
    g.BindRenderbuffer(kRenderbuffer, R->colRb);
    g.RenderbufferStorage(kRenderbuffer, kRgba8, static_cast<GLsizei>(W), static_cast<GLsizei>(H));
    g.BindRenderbuffer(kRenderbuffer, R->depRb);
    g.RenderbufferStorage(kRenderbuffer, kDepthComponent32F, static_cast<GLsizei>(W), static_cast<GLsizei>(H));
    g.BindFramebuffer(kFramebuffer, R->fbo);
    g.FramebufferRenderbuffer(kFramebuffer, kColorAttachment0, kRenderbuffer, R->colRb);
    g.FramebufferRenderbuffer(kFramebuffer, kDepthAttachment, kRenderbuffer, R->depRb);
    const GLenum st = g.CheckFramebufferStatus(kFramebuffer);
    g.BindFramebuffer(kFramebuffer, 0);
    if (st != kFramebufferComplete) { error = "GL framebuffer incomplete"; return fail(); }
    if (!makeProgram(g, kUiVert, kUiFrag, R->ui, error) || !makeProgram(g, kHudVert, kHudFrag, R->hud, error) ||
        !makeProgram(g, kLitVert, kLitFrag, R->lit, error) || !makeProgram(g, kTexVert, kTexFrag, R->tex, error))
        return fail();
    R->tex.loc[0] = g.GetUniformLocation(R->tex.id, "uMvp");
    R->tex.loc[1] = g.GetUniformLocation(R->tex.id, "uView");
    R->tex.loc[2] = g.GetUniformLocation(R->tex.id, "uFog");
    R->tex.loc[3] = g.GetUniformLocation(R->tex.id, "page");
    R->hud.loc[0] = g.GetUniformLocation(R->hud.id, "atlas");
    R->lit.loc[0] = g.GetUniformLocation(R->lit.id, "uMvp");
    R->lit.loc[1] = g.GetUniformLocation(R->lit.id, "uView");
    R->lit.loc[2] = g.GetUniformLocation(R->lit.id, "uFog");
    g.GenVertexArrays(4, R->vao); g.GenBuffers(4, R->vbo); g.GenBuffers(1, &R->ibo); g.GenBuffers(1, &R->tibo);
    const auto attrib = [&](int i, GLuint idx, GLint n, GLsizei stride, std::size_t off) {
        g.EnableVertexAttribArray(idx);
        g.VertexAttribPointer(idx, n, kFloat, 0, stride, reinterpret_cast<const void*>(off));
        (void)i;
    };
    g.BindVertexArray(R->vao[0]); g.BindBuffer(kArrayBuffer, R->vbo[0]);
    attrib(0, 0, 2, sizeof(FlatVertex), 0); attrib(0, 1, 4, sizeof(FlatVertex), 8);
    g.BindVertexArray(R->vao[1]); g.BindBuffer(kArrayBuffer, R->vbo[1]);
    attrib(1, 0, 2, sizeof(GlyphVertex), 0); attrib(1, 1, 2, sizeof(GlyphVertex), 8); attrib(1, 2, 4, sizeof(GlyphVertex), 16);
    g.BindVertexArray(R->vao[2]); g.BindBuffer(kArrayBuffer, R->vbo[2]); g.BindBuffer(kElementArrayBuffer, R->ibo);
    attrib(2, 0, 3, 24, 0); attrib(2, 1, 3, 24, 12);
    g.BindVertexArray(R->vao[3]); g.BindBuffer(kArrayBuffer, R->vbo[3]); g.BindBuffer(kElementArrayBuffer, R->tibo);
    attrib(3, 0, 3, 32, 0); attrib(3, 1, 3, 32, 12); attrib(3, 2, 2, 32, 24);
    g.BindVertexArray(0); g.BindBuffer(kArrayBuffer, 0);
    return R;
}

void glDestroyOp(void* p) {
    auto* R = static_cast<GlRenderer*>(p);
    if (!R) return;
    std::fprintf(stderr, "[preview-gl] readback: %llu passes, %.1f ms total (%.3f ms/pass, includes GPU wait)\n",
                 static_cast<unsigned long long>(R->readPasses), R->readMs, R->readPasses ? R->readMs / R->readPasses : 0.0);
    destroyGl(*R);
    delete R;
}

// Fixed state every pass sets (the presenter shares the context and may have changed any of it).
void beginPass(GlRenderer& R, const float clear[4], bool depth) {
    const Gl& g = R.gl;
    g.BindFramebuffer(kFramebuffer, R.fbo);
    g.Viewport(0, 0, static_cast<GLsizei>(R.W), static_cast<GLsizei>(R.H));
    g.Disable(kBlend); g.Disable(kCullFace); g.Disable(kScissorTest); g.Disable(kDither);
    g.ColorMask(1, 1, 1, 1); g.DepthMask(1);
    if (depth) { g.Enable(kDepthTest); g.DepthFunc(kLess); } else g.Disable(kDepthTest);
    g.ClearColor(clear[0], clear[1], clear[2], clear[3]);
    g.Clear(kColorBufferBit | kDepthBufferBit);  // depth clears to the default 1.0, like the Vulkan pass
}

void endPass(GlRenderer& R, std::vector<std::uint8_t>& rgbaOut) {
    const Gl& g = R.gl;
    rgbaOut.resize(static_cast<std::size_t>(R.W) * R.H * 4);
    g.PixelStorei(kPackAlignment, 1);
    const auto t0 = std::chrono::steady_clock::now();
    g.ReadPixels(0, 0, static_cast<GLsizei>(R.W), static_cast<GLsizei>(R.H), kRgba, kUnsignedByte, rgbaOut.data());
    R.readMs += std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
    ++R.readPasses;
    g.Disable(kDepthTest);
    g.BindFramebuffer(kFramebuffer, 0);
    g.BindVertexArray(0);
}

// One stderr line the first time each GL draw class runs, so a run proves what actually went through GL.
void noteFirst(int slot, const char* what) {
    static bool seen[6] = {};
    if (!seen[slot]) { seen[slot] = true; std::fprintf(stderr, "[preview-gl] first %s draw on GL\n", what); }
}

template <class V>
void upload(GlRenderer& R, int slot, const std::vector<V>& v) {
    R.gl.BindBuffer(kArrayBuffer, R.vbo[slot]);
    R.gl.BufferData(kArrayBuffer, static_cast<GLsizeiptr>(v.size() * sizeof(V)), v.data(), kStreamDraw);
}

bool glRender2D(void* p, const std::vector<ScreenQuad>& quads, const std::array<float, 4>& clear,
                std::vector<std::uint8_t>& rgbaOut, std::string& error) {
    auto& R = *static_cast<GlRenderer*>(p);
    std::vector<FlatVertex> v;
    v.reserve(quads.size() * 6);
    for (const ScreenQuad& q : quads) {
        if (q.width <= 0 || q.height <= 0) continue;
        const float x0 = ndcX(q.x, R.W), x1 = ndcX(q.x + q.width, R.W), y0 = ndcY(q.y, R.H), y1 = ndcY(q.y + q.height, R.H);
        const FlatVertex a{x0, y0, q.color[0], q.color[1], q.color[2], q.color[3]};
        const FlatVertex b{x1, y0, q.color[0], q.color[1], q.color[2], q.color[3]};
        const FlatVertex c{x1, y1, q.color[0], q.color[1], q.color[2], q.color[3]};
        const FlatVertex d{x0, y1, q.color[0], q.color[1], q.color[2], q.color[3]};
        pushQuad(v, a, b, c, d);
    }
    if (v.empty()) { error = "no non-empty screen quads"; return false; }
    const Gl& g = R.gl;
    noteFirst(0, "flat 2D quad");
    beginPass(R, clear.data(), false);
    g.UseProgram(R.ui.id); g.BindVertexArray(R.vao[0]);
    upload(R, 0, v);
    g.DrawArrays(kTriangles, 0, static_cast<GLsizei>(v.size()));
    endPass(R, rgbaOut);
    return true;
}

std::uint64_t hashTexture(const PreviewTexture& tex) {  // same FNV-1a as preview_vk.cpp
    std::uint64_t h = 1469598103934665603ull;
    const auto mix = [&h](const std::uint8_t* b, std::size_t n) { for (std::size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; } };
    const std::uint32_t d[2] = {tex.width, tex.height};
    std::uint8_t dims[8];
    for (int i = 0; i < 2; ++i) for (int k = 0; k < 4; ++k) dims[i * 4 + k] = static_cast<std::uint8_t>(d[i] >> (8 * k));
    mix(dims, 8);
    mix(tex.rgba.data(), tex.rgba.size());
    return h;
}

// One RGBA8 texture object per distinct content (FNV-1a of w,h,rgba), sampler state from samplerFor().
GLuint textureFor(GlRenderer& R, const PreviewTexture& tex, SamplerClass cls, std::string& error) {
    if (!tex.width || !tex.height || tex.rgba.size() < static_cast<std::size_t>(tex.width) * tex.height * 4) {
        error = "malformed preview texture"; return 0;
    }
    const Gl& g = R.gl;
    const std::uint64_t key = hashTexture(tex);
    auto it = R.textures.find(key);
    if (it == R.textures.end()) {
        const SamplerDesc sd = samplerFor(cls);
        GLuint t = 0; g.GenTextures(1, &t); g.BindTexture(kTexture2D, t);
        const GLint filter = static_cast<GLint>(sd.nearest ? kNearest : kLinear);
        const GLint wrap = static_cast<GLint>(sd.repeat ? kRepeat : kClampToEdge);
        g.TexParameteri(kTexture2D, kMinFilter, filter); g.TexParameteri(kTexture2D, kMagFilter, filter);
        g.TexParameteri(kTexture2D, kWrapS, wrap); g.TexParameteri(kTexture2D, kWrapT, wrap);
        g.PixelStorei(kUnpackAlignment, 1);
        g.TexImage2D(kTexture2D, 0, static_cast<GLint>(kRgba8), static_cast<GLsizei>(tex.width), static_cast<GLsizei>(tex.height), 0,
                     kRgba, kUnsignedByte, tex.rgba.data());
        it = R.textures.emplace(key, t).first;
    }
    return it->second;
}

bool glUploadAtlas(void* p, const PreviewTexture& atlas, std::string& error) {
    auto& R = *static_cast<GlRenderer*>(p);
    if (R.atlasReady && R.atlasPtr == atlas.rgba.data() && R.atlasW == atlas.width && R.atlasH == atlas.height) return true;
    const GLuint t = textureFor(R, atlas, SamplerClass::FontAtlas, error);
    if (!t) return false;
    R.atlas = t; R.atlasPtr = atlas.rgba.data(); R.atlasW = atlas.width; R.atlasH = atlas.height; R.atlasReady = true;
    return true;
}

bool glRender2DTextured(void* p, const std::vector<HudQuad>& quads, const std::array<float, 4>& clear,
                        std::vector<std::uint8_t>& rgbaOut, std::string& error) {
    auto& R = *static_cast<GlRenderer*>(p);
    if (!R.atlasReady) { error = "font atlas not uploaded"; return false; }
    std::vector<GlyphVertex> v;
    v.reserve(quads.size() * 6);
    for (const HudQuad& q : quads) {
        if (q.width <= 0 || q.height <= 0) continue;
        const float x0 = ndcX(q.x, R.W), x1 = ndcX(q.x + q.width, R.W), y0 = ndcY(q.y, R.H), y1 = ndcY(q.y + q.height, R.H);
        const GlyphVertex a{x0, y0, q.u0, q.v0, q.color[0], q.color[1], q.color[2], q.color[3]};
        const GlyphVertex b{x1, y0, q.u1, q.v0, q.color[0], q.color[1], q.color[2], q.color[3]};
        const GlyphVertex c{x1, y1, q.u1, q.v1, q.color[0], q.color[1], q.color[2], q.color[3]};
        const GlyphVertex d{x0, y1, q.u0, q.v1, q.color[0], q.color[1], q.color[2], q.color[3]};
        pushQuad(v, a, b, c, d);
    }
    if (v.empty()) { error = "no non-empty hud quads"; return false; }
    const Gl& g = R.gl;
    noteFirst(1, "textured UI/HUD glyph");
    beginPass(R, clear.data(), false);
    g.UseProgram(R.hud.id); g.BindVertexArray(R.vao[1]);
    g.ActiveTexture(kTexture0); g.BindTexture(kTexture2D, R.atlas);
    g.Uniform1i(R.hud.loc[0], 0);
    upload(R, 1, v);
    g.DrawArrays(kTriangles, 0, static_cast<GLsizei>(v.size()));
    endPass(R, rgbaOut);
    return true;
}

bool glRenderLit(void* p, const float* verts, std::size_t nVerts, const std::uint32_t* indices, std::size_t nIndices,
                 const float* pc, const float clear[4], std::vector<std::uint8_t>& rgbaOut, std::string& error) {
    auto& R = *static_cast<GlRenderer*>(p);
    if (!nVerts || !nIndices) { error = "empty geometry"; return false; }
    const Gl& g = R.gl;
    noteFirst(2, "flat-lit model");
    beginPass(R, clear, true);
    g.UseProgram(R.lit.id); g.BindVertexArray(R.vao[2]);
    g.BindBuffer(kArrayBuffer, R.vbo[2]);
    g.BufferData(kArrayBuffer, static_cast<GLsizeiptr>(nVerts * 24), verts, kStreamDraw);
    g.BindBuffer(kElementArrayBuffer, R.ibo);
    g.BufferData(kElementArrayBuffer, static_cast<GLsizeiptr>(nIndices * 4), indices, kStreamDraw);
    g.UniformMatrix4fv(R.lit.loc[0], 1, 0, pc);
    g.UniformMatrix4fv(R.lit.loc[1], 1, 0, pc + 16);
    g.Uniform4fv(R.lit.loc[2], 1, pc + 32);
    g.DrawElements(kTriangles, static_cast<GLsizei>(nIndices), kUnsignedInt, nullptr);
    endPass(R, rgbaOut);
    return true;
}

template <class T>
void uploadElements(GlRenderer& R, GLuint buf, const T* data, std::size_t n) {
    R.gl.BindBuffer(kElementArrayBuffer, buf);
    R.gl.BufferData(kElementArrayBuffer, static_cast<GLsizeiptr>(n * sizeof(T)), data, kStreamDraw);
}

void setSceneUniforms(const GlRenderer& R, const Program& p, const float* pc) {
    const Gl& g = R.gl;
    g.UniformMatrix4fv(p.loc[0], 1, 0, pc);
    g.UniformMatrix4fv(p.loc[1], 1, 0, pc + 16);
    g.Uniform4fv(p.loc[2], 1, pc + 32);
}

// Same order as renderGeometryTextured: sky (no depth), flat pipe, page-batched textured pipe, then the
// depth-off overlay. The ui/lit/tex vertex layouts are the Vulkan pipelines' (24 / 24 / 32 B).
bool glRenderScene(void* p, const GlSceneDraw& d, std::vector<std::uint8_t>& rgbaOut, std::string& error) {
    auto& R = *static_cast<GlRenderer*>(p);
    const Gl& g = R.gl;
    if (!d.pc36 || !d.nFlatV || !d.nFlatI) { error = "empty flat geometry"; return false; }
    // Resolve page textures first so a malformed page fails before anything is drawn.
    std::vector<GLuint> rangeTex(d.nRanges, 0);
    for (std::size_t i = 0; i < d.nRanges; ++i) {
        if (!d.ranges[i].count || !d.ranges[i].tex) continue;
        rangeTex[i] = textureFor(R, *d.ranges[i].tex, SamplerClass::PageTexture, error);
        if (!rangeTex[i]) return false;
    }
    noteFirst(3, "scene (sky/terrain/car/overlay)");
    if (d.nRanges && d.nTexI) noteFirst(4, "textured terrain page");
    beginPass(R, d.clear, true);
    if (d.nSkyVerts) {
        g.Disable(kDepthTest);
        g.UseProgram(R.ui.id); g.BindVertexArray(R.vao[0]);
        g.BindBuffer(kArrayBuffer, R.vbo[0]);
        g.BufferData(kArrayBuffer, static_cast<GLsizeiptr>(d.nSkyVerts * 24), d.skyVerts, kStreamDraw);
        g.DrawArrays(kTriangles, 0, static_cast<GLsizei>(d.nSkyVerts));
        g.Enable(kDepthTest);
    }
    g.UseProgram(R.lit.id); g.BindVertexArray(R.vao[2]);
    g.BindBuffer(kArrayBuffer, R.vbo[2]);
    g.BufferData(kArrayBuffer, static_cast<GLsizeiptr>(d.nFlatV * 24), d.flatV, kStreamDraw);
    uploadElements(R, R.ibo, d.flatI, d.nFlatI);
    setSceneUniforms(R, R.lit, d.pc36);
    g.DrawElements(kTriangles, static_cast<GLsizei>(d.nFlatI), kUnsignedInt, nullptr);
    if (d.nRanges && d.nTexV) {
        g.UseProgram(R.tex.id); g.BindVertexArray(R.vao[3]);
        g.BindBuffer(kArrayBuffer, R.vbo[3]);
        g.BufferData(kArrayBuffer, static_cast<GLsizeiptr>(d.nTexV * 32), d.texV, kStreamDraw);
        uploadElements(R, R.tibo, d.texI, d.nTexI);
        setSceneUniforms(R, R.tex, d.pc36);
        g.ActiveTexture(kTexture0); g.Uniform1i(R.tex.loc[3], 0);
        for (std::size_t i = 0; i < d.nRanges; ++i) {
            if (!rangeTex[i]) continue;
            g.BindTexture(kTexture2D, rangeTex[i]);
            g.DrawElements(kTriangles, static_cast<GLsizei>(d.ranges[i].count), kUnsignedInt,
                           reinterpret_cast<const void*>(static_cast<std::size_t>(d.ranges[i].first) * 4));
        }
    }
    if (d.nOverlayV && d.nOverlayI) {
        noteFirst(5, "overlay");
        g.Disable(kDepthTest);
        g.UseProgram(R.lit.id); g.BindVertexArray(R.vao[2]);
        g.BindBuffer(kArrayBuffer, R.vbo[2]);
        g.BufferData(kArrayBuffer, static_cast<GLsizeiptr>(d.nOverlayV * 24), d.overlayV, kStreamDraw);
        uploadElements(R, R.ibo, d.overlayI, d.nOverlayI);
        setSceneUniforms(R, R.lit, d.pc36);
        g.DrawElements(kTriangles, static_cast<GLsizei>(d.nOverlayI), kUnsignedInt, nullptr);
    }
    endPass(R, rgbaOut);
    return true;
}

const GlBackendOps kOps = {glCreate, glDestroyOp, glRender2D, glUploadAtlas, glRender2DTextured, glRenderLit, glRenderScene};

}  // namespace

const GlBackendOps* glBackendOps() { return &kOps; }

} // namespace rt::native::preview
