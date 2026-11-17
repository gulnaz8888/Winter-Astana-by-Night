
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

uniform bool uLightingEnabled;
uniform int uSceneIndex;
uniform float uTime;

uniform bool uIsBaiterek;
uniform bool uIsArchitecturalLED;
uniform bool uAutoSnowEnabled;

uniform int uWowBuildingId;
uniform bool uWowWindow;
uniform bool uWowActive;
uniform float uWowProgress;

const float PI = 3.14159265358979323846;
const vec3 EXPO_CENTER = vec3(6.0, 38.0, -42.0);
const float EXPO_RADIUS = 30.0;

// =====================================================
// LIGHT
// =====================================================

vec3 calculateLight(
    vec3 lightPos,
    vec3 lightColor,
    float intensity,
    vec3 baseColor,
    vec3 N,
    vec3 V,
    float linearTerm,
    float quadraticTerm
) {
    vec3 lightVector = lightPos - vWorldPos;
    float d = length(lightVector);

    vec3 L = lightVector / max(d, 0.0001);
    vec3 H = normalize(L + V);

    float diffuse = max(dot(N, L), 0.0);

    float specular = pow(
        max(dot(N, H), 0.0),
        max(uShininess, 1.0)
    );

    float attenuation = 1.0 / (
        1.0 +
        linearTerm * d +
        quadraticTerm * d * d
    );

    return (
        baseColor * diffuse +
        vec3(0.20) * specular
    ) * lightColor * intensity * attenuation;
}

// =====================================================
// NOISE
// =====================================================

float hash21(vec2 p) {
    return fract(
        sin(dot(p, vec2(127.1, 311.7))) *
        43758.5453
    );
}

float snowNoise(vec3 p) {
    return
        sin(p.x * 0.43 + p.z * 0.37) * 0.5 +
        sin(p.z * 0.89 - p.x * 0.21) * 0.3 +
        sin(p.x * 1.63 + p.z * 1.27) * 0.2;
}

// =====================================================
// EXPO SPHERE DETECTION
// =====================================================

bool isNurAlemGlass(vec3 N) {
    if (uSceneIndex != 1 || !uUseTexture) {
        return false;
    }

    vec3 offset = vWorldPos - EXPO_CENTER;
    float radius = length(offset);

    if (abs(radius - EXPO_RADIUS) > 0.30) {
        return false;
    }

    return dot(
        N,
        normalize(offset)
    ) > 0.90;
}

// =====================================================
// NUR ALEM TEXTURED GLASS
// =====================================================

vec3 calculateNurAlemGlass(
    vec3 N,
    vec3 V,
    float fog
) {
    // Real blue_glass.png texture from ExpoScene.cpp.
    vec2 uv = vTexCoord * uUVScale.xy;

    vec3 glass = texture(
        uTexture,
        uv
    ).rgb;

    // Preserve the original texture details.
    float fresnel = pow(
        1.0 - max(dot(N, V), 0.0),
        3.0
    );

    glass *= vec3(
        0.94,
        1.03,
        1.12
    );

    // Subtle glass reflection.
    glass += vec3(
        0.025,
        0.060,
        0.085
    ) * fresnel;

    if (uLightingEnabled) {
        // Blue illumination without hiding the texture.
        glass += vec3(
            0.025,
            0.065,
            0.11
        );
    }

    return mix(
        glass,
        uFogColor,
        fog
    );
}

// =====================================================
// EXPO PAVILION LIGHT
// =====================================================

vec3 calculateExpoPodiumLight(
    vec3 baseColor,
    vec3 N
) {
    if (
        uSceneIndex != 1 ||
        !uLightingEnabled
    ) {
        return vec3(0.0);
    }

    float insideX = 1.0 - smoothstep(
        55.0,
        64.0,
        abs(vWorldPos.x - 6.0)
    );

    float insideZ = 1.0 - smoothstep(
        13.0,
        21.0,
        abs(vWorldPos.z + 42.0)
    );

    float lowHeight =
        1.0 - smoothstep(
            8.0,
            12.0,
            vWorldPos.y
        );

    float pavilionMask =
        insideX *
        insideZ *
        lowHeight;

    float frontMask = smoothstep(
        -38.0,
        -25.0,
        vWorldPos.z
    );

    // Blue ambient lighting.
    vec3 blueFill = vec3(
        0.08,
        0.19,
        0.30
    ) * pavilionMask;

    // Warm entrance lighting.
    vec3 warmFill = vec3(
        0.30,
        0.22,
        0.13
    ) * pavilionMask * frontMask;

    float vertical = 1.0 - abs(N.y);

    return (
        blueFill +
        warmFill * vertical
    ) * 0.85 +
        baseColor * pavilionMask * 0.30;
}

// =====================================================
// EXPO BUILDING LIGHT
// =====================================================

vec3 calculateExpoBuildingLight(
    vec3 baseColor,
    vec3 N
) {
    if (
        uSceneIndex != 1 ||
        !uLightingEnabled
    ) {
        return vec3(0.0);
    }

    float sideBuildings = smoothstep(
        85.0,
        115.0,
        abs(vWorldPos.x)
    );

    float rearBuildings =
        1.0 - smoothstep(
            -125.0,
            -85.0,
            vWorldPos.z
        );

    float buildingHeight = smoothstep(
        12.0,
        25.0,
        vWorldPos.y
    );

    float mask = clamp(
        max(sideBuildings, rearBuildings) *
        buildingHeight,
        0.0,
        1.0
    );

    float vertical = 1.0 - abs(N.y);

    vec3 fill = baseColor *
        vec3(0.48, 0.63, 0.82) *
        mask * 1.6;

    fill += vec3(
        0.065,
        0.11,
        0.17
    ) * mask * vertical;

    return fill;
}

// =====================================================
// CHRISTMAS TREE
// =====================================================

vec3 calculateTreeFill(vec3 baseColor) {
    if (
        uSceneIndex != 1 ||
        !uLightingEnabled
    ) {
        return vec3(0.0);
    }

    float distanceToTree = length(
        vWorldPos.xz - vec2(-15.0, -2.0)
    );

    float treeMask =
        1.0 - smoothstep(
            7.0,
            11.0,
            distanceToTree
        );

    float heightMask =
        smoothstep(1.0, 4.0, vWorldPos.y) *
        (
            1.0 - smoothstep(
                27.0,
                33.0,
                vWorldPos.y
            )
        );

    float pulse =
        0.85 + 0.15 * sin(uTime * 1.7);

    return (
        baseColor * vec3(
            0.38,
            0.46,
            0.27
        ) * pulse +
        vec3(0.025, 0.035, 0.018)
    ) * treeMask * heightMask;
}


 // =====================================================
 // SNOW
 // =====================================================

float calculateSnowCoverage(vec3 N) {
    if (!uAutoSnowEnabled) {
        return 0.0;
    }

    if (uIsBaiterek || uIsArchitecturalLED) {
        return 0.0;
    }

    float emissionStrength = max(
        max(uEmissive.r, uEmissive.g),
        uEmissive.b
    );

    float emissionMask =
        1.0 - smoothstep(
            0.18,
            0.48,
            emissionStrength
        );

    float facingUp = clamp(N.y, 0.0, 1.0);

    float slopeCoverage = smoothstep(
        0.38,
        0.82,
        facingUp +
        snowNoise(vWorldPos) * 0.075
    );

    // =============================================
    // SCENE 1: WINTER CITY
    // =============================================

    if (uSceneIndex == 0) {
        return clamp(
            slopeCoverage *
            smoothstep(1.5, 4.0, vWorldPos.y) *
            emissionMask,
            0.0,
            1.0
        );
    }

    // =============================================
    // SCENE 2: EXPO
    // =============================================

    if (uSceneIndex == 1) {
        float radius = length(
            vWorldPos - EXPO_CENTER
        );

        // Snow near the top of Nur Alem.
        float shellMask =
            1.0 - smoothstep(
                0.8,
                2.0,
                abs(radius - EXPO_RADIUS)
            );

        float sphereSnow =
            shellMask *
            smoothstep(49.0, 55.0, vWorldPos.y) *
            slopeCoverage * 0.65;

        // Snow on the podium roof.
        float insideX =
            1.0 - smoothstep(
                58.0,
                64.0,
                abs(vWorldPos.x - 6.0)
            );

        float insideZ =
            1.0 - smoothstep(
                13.0,
                19.0,
                abs(vWorldPos.z + 42.0)
            );

        float podiumSnow =
            insideX *
            insideZ *
            smoothstep(8.8, 10.0, vWorldPos.y) *
            (
                1.0 - smoothstep(
                    12.0,
                    15.0,
                    vWorldPos.y
                )
            ) *
            smoothstep(0.55, 0.90, facingUp);

        // Keep snow away from the Christmas tree.
        float treeProtection =
            1.0 - smoothstep(
                8.0,
                11.0,
                length(
                    vWorldPos.xz -
                    vec2(-15.0, -2.0)
                )
            );

        podiumSnow *= 1.0 - treeProtection;

        return clamp(
            max(sphereSnow, podiumSnow) *
            emissionMask,
            0.0,
            1.0
        );
    }

    // =============================================
    // SCENE 3: RIVERFRONT
    // =============================================

    if (uSceneIndex == 2) {
        return clamp(
            slopeCoverage *
            smoothstep(2.5, 5.0, vWorldPos.y) *
            emissionMask,
            0.0,
            1.0
        );
    }

    return 0.0;
}

// =====================================================
// MATTE SNOW MATERIAL
// =====================================================

vec3 calculateMatteSnow(float fog) {
    vec3 snowColor = vec3(
        0.96,
        0.98,
        1.0
    );

    snowColor *=
        0.97 + 0.03 * snowNoise(vWorldPos * 1.3);

    return mix(
        snowColor,
        uFogColor,
        fog * 0.32
    );
}

// =====================================================
// WOW MOMENT
// =====================================================

float calculateBuildingLightReveal() {
    if (!uWowActive || uSceneIndex != 0) {
        return 1.0;
    }

    if (uWowBuildingId < 0) {
        return 1.0;
    }

    int index = uWowBuildingId % 6;
    float centerX;

    if (index < 3) {
        centerX = -38.0 - 26.0 * float(index);
    } else {
        centerX = 38.0 + 26.0 * float(index - 3);
    }

    float order = clamp(
        (centerX + 110.0) / 220.0,
        0.0,
        1.0
    );

    float activationTime = mix(
        0.25,
        0.625,
        order
    );

    return smoothstep(
        activationTime - 0.025,
        activationTime + 0.025,
        uWowProgress
    );
}

void main() {
    if (
        uIsArchitecturalLED &&
        !uLightingEnabled
    ) {
        discard;
    }

    vec3 N = normalize(vNormal);
    vec3 V = normalize(uViewPos - vWorldPos);

    float dist = length(vWorldPos - uCamPos);

    float fog = clamp(
        1.0 - exp(-dist * uFogDensity),
        0.0,
        1.0
    );

    // =============================================
    // NUR ALEM SPHERE
    // =============================================

    if (isNurAlemGlass(N)) {
        vec3 glassColor =
            calculateNurAlemGlass(N, V, fog);

        float snowCoverage =
            calculateSnowCoverage(N);

        if (snowCoverage > 0.0) {
            glassColor = mix(
                glassColor,
                calculateMatteSnow(fog),
                snowCoverage * 0.75
            );
        }

        FragColor = vec4(glassColor, 1.0);
        return;
    }

    // =============================================
    // MATERIAL
    // =============================================

    vec3 baseColor = uColor;

    if (uUseTexture) {
        vec2 uv = vTexCoord * uUVScale.xy;
        baseColor = texture(uTexture, uv).rgb;
    }

    bool isExpoTreeFoliage =
        uSceneIndex == 1 &&
        uUseTexture &&
        vWorldPos.y < 35.0 &&
        (
            length(
                vWorldPos.xz - vec2(-15.0, -2.0)
            ) < 12.0 ||
            abs(vWorldPos.x) > 60.0
        );

    if (isExpoTreeFoliage) {
        baseColor *= vec3(0.78, 0.86, 0.78);
    }

    float ambientStrength =
    uLightingEnabled ? 0.15 : 0.05;

    // More ambient illumination for EXPO snow and ground.
    if (uSceneIndex == 1 && N.y > 0.75) {
    ambientStrength = 0.65;
    }

    vec3 result = ambientStrength * baseColor;

    if (uSceneIndex == 2 && uUseTexture) {
        result += baseColor * 0.20;
    }

    // =============================================
    // LIGHTING
    // =============================================

    if (uLightingEnabled) {
        for (int i = 0; i < 2; ++i) {
            result += calculateLight(
                uLights[i].position,
                uLights[i].color,
                uLights[i].intensity,
                baseColor,
                N,
                V,
                0.035,
                0.004
            );
        }

        // EXPO
        if (uSceneIndex == 1) {
            result += calculateExpoBuildingLight(
                baseColor,
                N
            );

            result += calculateExpoPodiumLight(
                baseColor,
                N
            );

            result += calculateTreeFill(baseColor);

            // Gentle blue light around the sphere.
            float sphereDistance = max(
                length(vWorldPos - EXPO_CENTER) -
                EXPO_RADIUS,
                0.0
            );

            float sphereFalloff = 1.0 / (
                1.0 +
                0.07 * sphereDistance +
                0.006 * sphereDistance *
                sphereDistance
            );

            result += baseColor *
                vec3(0.10, 0.25, 0.40) *
                sphereFalloff * 0.22;

            // Pavilion entrance.
            vec3 entranceLight = vec3(
                clamp(vWorldPos.x, -51.0, 65.0),
                3.8,
                -22.0
            );

            result += calculateLight(
                entranceLight,
                vec3(1.0, 0.79, 0.54),
                1.15,
                baseColor,
                N,
                V,
                0.06,
                0.018
            );

            // Christmas tree.
            float redPulse =
                0.70 + 0.30 * sin(uTime * 1.6);

            float greenPulse =
                0.70 + 0.30 *
                sin(uTime * 1.6 + 2.1);

            float bluePulse =
                0.70 + 0.30 *
                sin(uTime * 1.6 + 4.2);

            vec3 treeColor = vec3(
                0.95 * redPulse,
                0.95 * greenPulse,
                0.95 * bluePulse
            );

            float treeIntensity =
                isExpoTreeFoliage ? 0.20 : 0.42;

            result += calculateLight(
                vec3(-15.0, 12.0, 1.5),
                treeColor,
                treeIntensity,
                baseColor,
                N,
                V,
                0.05,
                0.006
            );

            // Colored light on snow.
            float groundMask = smoothstep(
                0.45,
                0.92,
                N.y
            );

            float redFalloff =
                1.0 - smoothstep(
                    0.5,
                    18.0,
                    distance(
                        vWorldPos.xz,
                        vec2(-18.0, 1.0)
                    )
                );

            float greenFalloff =
                1.0 - smoothstep(
                    0.5,
                    18.0,
                    distance(
                        vWorldPos.xz,
                        vec2(-12.0, -2.0)
                    )
                );

            float blueFalloff =
                1.0 - smoothstep(
                    0.5,
                    18.0,
                    distance(
                        vWorldPos.xz,
                        vec2(-15.0, 6.0)
                    )
                );

            vec3 coloredGroundLight =
                vec3(1.0, 0.10, 0.12) *
                    redPulse * redFalloff
                +
                vec3(0.10, 1.0, 0.18) *
                    greenPulse * greenFalloff
                +
                vec3(0.12, 0.30, 1.0) *
                    bluePulse * blueFalloff;

            if (!isExpoTreeFoliage) {
                result +=
                    coloredGroundLight *
                    groundMask * 0.45;
            }

            if (isExpoTreeFoliage) {
                result = min(
                    result,
                    baseColor * 1.15 +
                    vec3(0.065)
                );
            }
        }

        // WINTER CITY
        if (uSceneIndex == 0) {
            result += calculateLight(
                vec3(-68.0, 18.0, -104.0),
                vec3(0.95, 0.25, 0.88),
                8.0,
                baseColor,
                N,
                V,
                0.02,
                0.002
            );

            vec3 towerColor = vec3(
                1.0, 0.58, 0.15
            );

            result += calculateLight(
                vec3(-18.0, 22.0, -73.0),
                towerColor,
                4.8,
                baseColor,
                N,
                V,
                0.03,
                0.003
            );

            result += calculateLight(
                vec3(18.0, 22.0, -73.0),
                towerColor,
                4.8,
                baseColor,
                N,
                V,
                0.03,
                0.003
            );

            vec3 mosqueColor = vec3(
                1.0, 0.95, 0.86
            );

            result += calculateLight(
                vec3(66.0, 10.0, -76.0),
                mosqueColor,
                2.0,
                baseColor,
                N,
                V,
                0.035,
                0.0035
            );

            result += calculateLight(
                vec3(54.0, 18.0, -77.0),
                mosqueColor,
                1.3,
                baseColor,
                N,
                V,
                0.04,
                0.004
            );

            result += calculateLight(
                vec3(78.0, 18.0, -77.0),
                mosqueColor,
                1.3,
                baseColor,
                N,
                V,
                0.04,
                0.004
            );
        }

        // RIVERFRONT
        if (uSceneIndex == 2) {
            float nearestX = clamp(
                floor(vWorldPos.x / 22.0 + 0.5)
                    * 22.0,
                -220.0,
                220.0
            );

            vec3 warmLampColor = vec3(
                1.0, 0.72, 0.35
            );

            result += calculateLight(
                vec3(nearestX, 9.7, 18.0),
                warmLampColor,
                5.5,
                baseColor,
                N,
                V,
                0.12,
                0.08
            );

            result += calculateLight(
                vec3(nearestX, 9.2, -112.0),
                warmLampColor,
                5.5,
                baseColor,
                N,
                V,
                0.12,
                0.08
            );
        }
    }

    // =============================================
    // FOG
    // =============================================

    vec3 color = mix(
        result,
        uFogColor,
        fog
    );

    // =============================================
    // WOW WINDOW REVEAL
    // =============================================

    float wowLightReveal =
        calculateBuildingLightReveal();

    if (uWowWindow) {
        color *= mix(
            0.08,
            1.0,
            wowLightReveal
        );
    }

    // =============================================
    // EMISSIVE MATERIALS
    // =============================================

    vec3 emission = uEmissive;

    if (uWowWindow) {
        emission *= wowLightReveal;
    }

    if (uIsBaiterek) {
        float shaftGlow =
            1.0 - smoothstep(
                12.0,
                43.0,
                vWorldPos.y
            );

        if (uLightingEnabled) {
            emission +=
                vec3(1.0, 0.37, 0.035) *
                1.45 * shaftGlow;

            emission +=
                vec3(0.65, 0.35, 0.08) *
                0.35;
        } else {
            emission +=
                vec3(0.10, 0.06, 0.02) *
                (0.35 + 0.65 * shaftGlow);
        }
    }

    // Existing EXPO windows.
    if (uSceneIndex == 1) {
        if (uLightingEnabled) {
            float windowStrength = max(
                max(uEmissive.r, uEmissive.g),
                uEmissive.b
            );

            float windowMask = smoothstep(
                0.025,
                0.10,
                windowStrength
            );

            float buildingArea = max(
                smoothstep(
                    85.0,
                    115.0,
                    abs(vWorldPos.x)
                ),
                1.0 - smoothstep(
                    -125.0,
                    -85.0,
                    vWorldPos.z
                )
            );

            buildingArea *= smoothstep(
                8.0,
                18.0,
                vWorldPos.y
            );

            emission +=
                uEmissive *
                windowMask *
                buildingArea *
                2.8;

            float treeDistance = length(
                vWorldPos.xz -
                vec2(-15.0, -2.0)
            );

            float treeMask =
                1.0 - smoothstep(
                    8.0,
                    11.0,
                    treeDistance
                );

            float bulbStrength = smoothstep(
                0.18,
                0.45,
                windowStrength
            );

            emission +=
                uEmissive *
                treeMask *
                bulbStrength *
                0.65;
        } else {
            emission *= 0.18;
        }
    }

    color += emission * (1.0 - 0.5 * fog);

    // =============================================
    // SNOW
    // =============================================

    float snowCoverage =
        calculateSnowCoverage(N);

    if (snowCoverage > 0.0) {
        vec3 matteSnow =
            calculateMatteSnow(fog);

        if (
            uSceneIndex == 1 &&
            uLightingEnabled
        ) {
            vec2 closest = vec2(
                clamp(vWorldPos.x, -51.0, 65.0),
                -22.0
            );

            float warmth = exp(
                -distance(
                    vWorldPos.xz,
                    closest
                ) * 0.22
            );

            matteSnow +=
                vec3(0.12, 0.072, 0.026) *
                warmth;
        }

        color = mix(
            color,
            matteSnow,
            clamp(
                snowCoverage * 0.98,
                0.0,
                0.98
            )
        );
    }

    // =============================================
    // CITY-WIDE WOW MOMENT
    // =============================================

    if (
        uWowActive &&
        uSceneIndex == 0 &&
        !uWowWindow
    ) {
        float blockX = floor(
            (vWorldPos.x + 120.0) / 25.0
        ) * 25.0 - 107.5;

        float order = clamp(
            (blockX + 110.0) / 220.0,
            0.0,
            1.0
        );

        float activationTime = mix(
            0.25,
            0.625,
            order
        );

        float reveal = smoothstep(
            activationTime - 0.025,
            activationTime + 0.025,
            uWowProgress
        );

        color *= mix(
            0.16,
            1.0,
            reveal
        );
    }

    FragColor = vec4(color, 1.0);
}
