#version 450
// HUD glyph quad: screen-space position (already NDC, Vulkan y-down), atlas
// UV (half-texel-inset by the host), and a modulate colour.
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;
layout(location = 0) out vec2 vUV;
layout(location = 1) out vec4 vColor;
void main() {
    vUV = inUV;
    vColor = inColor;
    gl_Position = vec4(inPosition, 0.0, 1.0);
}
