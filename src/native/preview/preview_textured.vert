#version 450
// Textured terrain vertex stage: same 144-byte push-constant block as
// preview.vert (mvp/view/fog, only the first 128 bytes are declared), plus
// the GS page-fraction UV.  vUV carries uv * clip.w so the fragment stage's
// interpolation is perspective-correct after dividing by vW.
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;
layout(push_constant) uniform PC { mat4 mvp; mat4 view; vec4 fog; } pc;
layout(location = 0) out vec3 vView;
layout(location = 1) out vec3 vColor;
layout(location = 2) out vec2 vUV;
layout(location = 3) out float vW;
void main() {
    vColor = inColor;
    vec4 clip = pc.mvp * vec4(inPos, 1.0);
    vView = (pc.view * vec4(inPos, 1.0)).xyz;
    vUV = inUV * clip.w;
    vW = clip.w;
    gl_Position = clip;
}
