#version 330 core
in vec3 vNormal;
in vec3 vWorldPos;
out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uEmissive;
uniform vec3 uCamPos;
uniform vec3 uFogColor;
uniform float uFogDensity;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(vec3(-0.4, 0.8, 0.5));
    float diff = max(dot(N, L), 0.0);
    vec3 lit = uColor * (0.62 + 0.38 * diff);
    float dist = length(vWorldPos - uCamPos);
    float fog = 1.0 - exp(-dist * uFogDensity);
    vec3 color = mix(lit, uFogColor, fog);
    color += uEmissive * (1.0 - 0.5 * fog);
    FragColor = vec4(color, 1.0);
}
