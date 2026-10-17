#version 450
// Textured terrain fragment stage: lighting and fog are identical to
// preview.frag; the page texture modulates the vertex colour (GS
// CLUT/texellate modulation, alphaAdd semantics deferred).  Fully
// transparent texels (AEM-empty palette entries) are discarded like the
// hardware's alpha test against a zero reference.
layout(location = 0) in vec3 vView;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 vUV;
layout(location = 3) in float vW;
layout(location = 0) out vec4 outColor;
layout(push_constant) uniform PC { mat4 mvp; mat4 view; vec4 fog; } pc;
layout(set = 0, binding = 0) uniform sampler2D page;
void main() {
    vec2 uv = vUV / max(vW, 1e-6);
    vec4 tex = texture(page, uv);
    if (tex.a < 0.02) discard;
    vec3 n = normalize(cross(dFdx(vView), dFdy(vView)));
    float l = max(dot(n, normalize(vec3(0.4, 0.6, 1.0))), 0.0);
    // GS-style modulation: the PS2 Modulate texture function multiplies vertex
    // colour x page texture, and the shipped game's ground reads at roughly
    // that x2 level (oracle town roads ~110/255; vtx mean 0.42 x page mean
    // 0.5 x 2 = 0.42). The old 0.35 procedural floor crushed the ground to
    // ~1/5 of oracle brightness (measured (7,19,8) mid-frame 2026-10-17).
    // Time-of-day comes from the vertex colours, not this term; l is only a
    // subtle surface-orientation cue now.
    vec3 modulated = min(vColor * tex.rgb * 2.0, vec3(1.0));
    vec3 lit = vec3(0.85 + 0.15 * l) * modulated;
    float fogAmount = smoothstep(0.5 * pc.fog.a, pc.fog.a, length(vView));
    outColor = vec4(mix(lit, pc.fog.rgb, fogAmount), 1.0);
}
