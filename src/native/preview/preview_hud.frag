#version 450
// HUD glyph fragment: FONTE atlas texel modulated by the quad colour.
// Empty cells (alpha 0 after the ratified TEXA/AEM decode) are discarded,
// mirroring the hardware's alpha test against a zero reference; surviving
// texels write vColor-modulated RGB with vColor.a * tex.a so the host
// composite (src-over) sees clean glyph coverage.
layout(location = 0) in vec2 vUV;
layout(location = 1) in vec4 vColor;
layout(location = 0) out vec4 outColor;
layout(set = 0, binding = 0) uniform sampler2D atlas;
void main() {
    vec4 tex = texture(atlas, vUV);
    if (tex.a < 0.02) discard;
    outColor = vec4(tex.rgb * vColor.rgb, vColor.a * tex.a);
}
