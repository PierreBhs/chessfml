#version 130

uniform sampler2D texture;
uniform vec4 glowColor;

void main() {
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    float intensity = pixel.a * 0.5;
    
    // Radial gradient
    vec2 center = vec2(0.5, 0.5);
    float distance = length(gl_TexCoord[0].xy - center);
    float falloff = 1.0 - smoothstep(0.3, 0.7, distance);
    
    gl_FragColor = mix(pixel, glowColor, falloff * intensity);
}
