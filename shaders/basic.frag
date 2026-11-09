
#version 330 core

in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vTexCoord;

out vec4 FragColor;

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform PointLight uLights[2];

uniform vec3 uColor;
uniform vec3 uEmissive;
uniform vec3 uViewPos;
uniform float uShininess;

uniform vec3 uCamPos;
uniform vec3 uFogColor;
uniform float uFogDensity;

uniform sampler2D uTexture;
uniform bool uUseTexture;
uniform vec3 uUVScale;

void main() {
    vec3 baseColor = uColor;

    if (uUseTexture) {
        vec2 uv = vTexCoord * uUVScale.xy;
        baseColor *= texture(uTexture, uv).rgb;
    }

    vec3 N = normalize(vNormal);
    vec3 V = normalize(uViewPos - vWorldPos);

    vec3 result = 0.15 * baseColor;

    for (int i = 0; i < 2; i++) {
        vec3 lightVector = uLights[i].position - vWorldPos;
        float distance = length(lightVector);

        vec3 L = normalize(lightVector);
        float diff = max(dot(N, L), 0.0);

        vec3 diffuse = diff * baseColor;

        vec3 H = normalize(L + V);
        float spec = pow(
            max(dot(N, H), 0.0),
            max(uShininess, 1.0)
        );

        vec3 specular = vec3(0.35) * spec;

        // Improved lighting from teammate
        float attenuation =
            1.0 / (1.0 + 0.035 * distance
                         + 0.004 * distance * distance);

        result += (diffuse + specular)
                * uLights[i].color
                * uLights[i].intensity
                * attenuation;
    }

    float dist = length(vWorldPos - uCamPos);

    float fog = clamp(
        1.0 - exp(-dist * uFogDensity),
        0.0, 1.0
    );

    vec3 color = mix(result, uFogColor, fog);

    color += uEmissive * (1.0 - 0.5 * fog);

    FragColor = vec4(color, 1.0);
}
