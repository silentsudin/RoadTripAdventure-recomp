#version 450
layout(location = 0) in vec3 vView;
layout(location = 1) in vec3 vColor;
layout(location = 0) out vec4 outColor;
layout(push_constant) uniform PC { mat4 mvp; mat4 view; vec4 fog; } pc;
void main() {
    vec3 n = normalize(cross(dFdx(vView), dFdy(vView)));
    float l = max(dot(n, normalize(vec3(0.4, 0.6, 1.0))), 0.0);
    // Ambient floor raised 0.35 -> 0.85 (2026-10-17): flat-pass colours are
    // already display values (car livery / HUD models); the old floor rendered
    // the Q00 body as a dark brown blob under the day sky.
    vec3 lit = vec3(0.85 + 0.15 * l) * vColor;
    float fogAmount = smoothstep(0.5 * pc.fog.a, pc.fog.a, length(vView));
    outColor = vec4(mix(lit, pc.fog.rgb, fogAmount), 1.0);
}
