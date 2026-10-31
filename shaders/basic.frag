
#version 330 core

in vec3 vNormal;
in vec3 vWorldPos;

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

// Fog uniforms from Gulnaz's scene
uniform vec3 uCamPos;
uniform vec3 uFogColor;
uniform float uFogDensity;

void main()
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uViewPos - vWorldPos);

    // Ambient lighting
    vec3 result = 0.15 * uColor;

    // Two Blinn-Phong point lights
    for (int i = 0; i < 2; i++)
    {
        vec3 lightVector =
            uLights[i].position - vWorldPos;

        float distance = length(lightVector);
        vec3 L = normalize(lightVector);

        // Diffuse
        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = diff * uColor;

        // Blinn-Phong specular
        vec3 H = normalize(L + V);

        float spec = pow(
            max(dot(N, H), 0.0),
            max(uShininess, 1.0)
        );

        vec3 specular = vec3(0.35) * spec;

        // Light attenuation
        float attenuation =
            1.0 / (1.0 + 0.09 * distance
                         + 0.032 * distance * distance);

        result += (diffuse + specular)
            * uLights[i].color
            * uLights[i].intensity
            * attenuation;
    }

    // Fog from Gulnaz's implementation
    float dist = length(vWorldPos - uCamPos);

    float fog = clamp(
        1.0 - exp(-dist * uFogDensity),
        0.0,
        1.0
    );

    vec3 color = mix(result, uFogColor, fog);

    // Emissive windows and lamps
    color += uEmissive * (1.0 - 0.5 * fog);

    FragColor = vec4(color, 1.0);
}
