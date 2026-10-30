#version 330 core
in vec3 vNormal;
in vec3 vWorldPos;
out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uEmissive;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(vec3(-0.4, 0.8, 0.5));
    float diff = max(dot(N, L), 0.0) * 0.6;
    vec3 light = vec3(0.55, 0.62, 0.9) * (0.22 + diff);
    FragColor = vec4(uColor * light + uEmissive, 1.0);
}
