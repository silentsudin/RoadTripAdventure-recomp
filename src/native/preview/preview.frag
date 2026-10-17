#version 450
layout(location = 0) in vec3 vView;
layout(location = 1) in vec3 vColor;
layout(location = 0) out vec4 outColor;
layout(push_constant) uniform PC { mat4 mvp; mat4 view; vec4 fog; } pc;
void main() {
    vec3 n = normalize(cross(dFdx(vView), dFdy(vView)));
    float l = max(dot(n, normalize(vec3(0.4, 0.6, 1.0))), 0.0);
    vec3 lit = vec3(0.35 + 0.65 * l) * vColor;
    float fogAmount = smoothstep(0.5 * pc.fog.a, pc.fog.a, length(vView));
    outColor = vec4(mix(lit, pc.fog.rgb, fogAmount), 1.0);
}
