#version 330 core
in vec2 vUV;
uniform vec3 uHorizon;
uniform vec3 uTop;
uniform float uStars;
out vec4 FragColor;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec3 horizonColor = uHorizon;
    vec3 topColor = uTop;
    float t = clamp((vUV.y - 0.38) / 0.62, 0.0, 1.0);
    vec3 color = mix(horizonColor, topColor, pow(t, 0.7));

    vec2 scaled = gl_FragCoord.xy / 16.0;
    vec2 cell = floor(scaled);
    vec2 local = fract(scaled);
    vec2 pos = vec2(0.2 + 0.6 * hash(cell + 1.7), 0.2 + 0.6 * hash(cell + 4.3));
    float size = mix(0.05, 0.14, hash(cell + 9.1));
    float d = length(local - pos);
    float star = step(0.82, hash(cell)) * (1.0 - smoothstep(size * 0.4, size, d));
    star *= smoothstep(0.4, 0.75, vUV.y);

    FragColor = vec4(color + vec3(star * uStars), 1.0);
}
