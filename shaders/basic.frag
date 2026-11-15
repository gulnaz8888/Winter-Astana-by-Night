
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

// Lighting controls
uniform bool uLightingEnabled;
uniform int uSceneIndex;
uniform float uTime;
uniform bool uIsBaiterek;
uniform bool uIsArchitecturalLED;

// Blinn-Phong point light calculation
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
    float distance = length(lightVector);
    vec3 L = normalize(lightVector);

    float diffuseFactor = max(dot(N, L), 0.0);
    vec3 diffuse = diffuseFactor * baseColor;

    vec3 H = normalize(L + V);
    float specularFactor = pow(
        max(dot(N, H), 0.0),
        max(uShininess, 1.0)
    );

    vec3 specular = vec3(0.20) * specularFactor;

    float attenuation = 1.0 /
        (1.0 + linearTerm * distance
             + quadraticTerm * distance * distance);

    return (diffuse + specular)
        * lightColor
        * intensity
        * attenuation;
}

void main() {

    // Hide architectural LED lighting when L is OFF
    if (uIsArchitecturalLED && !uLightingEnabled) {
        discard;
    }

    vec3 baseColor = uColor;

    if (uUseTexture) {
        vec2 uv = vTexCoord * uUVScale.xy;
        baseColor *= texture(uTexture, uv).rgb;
    }

    vec3 N = normalize(vNormal);
    vec3 V = normalize(uViewPos - vWorldPos);

    float ambientStrength =
        uLightingEnabled ? 0.15 : 0.05;

    vec3 result = ambientStrength * baseColor;

    if (uLightingEnabled) {

        // Two main Blinn-Phong point lights
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

        // SCENE 2: EXPO - stronger colored tree lighting
if (uSceneIndex == 1) {

    // Decorative colored light near the Christmas tree
    float redPulse   = 0.70 + 0.30 * sin(uTime * 1.6);
    float greenPulse = 0.70 + 0.30 * sin(uTime * 1.6 + 2.1);
    float bluePulse  = 0.70 + 0.30 * sin(uTime * 1.6 + 4.2);

    vec3 treeColor = vec3(
        0.95 * redPulse,
        0.95 * greenPulse,
        0.95 * bluePulse
    );

    // Extra local light coming from the tree itself
    vec3 treeLightPos = vec3(-15.0, 12.0, 1.5);

    result += calculateLight(
        treeLightPos,
        treeColor,
        4.8,
        baseColor,
        N,
        V,
        0.05,
        0.006
    );

    // Colored light pools on the snow/ground around the tree
    vec2 redSpot   = vec2(-18.0,  1.0);
    vec2 greenSpot = vec2(-12.0, -2.0);
    vec2 blueSpot  = vec2(-15.0,  6.0);

    float groundMask = smoothstep(0.45, 0.92, N.y);

    float redFalloff =
        1.0 - smoothstep(
            0.5, 18.0,
            distance(vWorldPos.xz, redSpot)
        );

    float greenFalloff =
        1.0 - smoothstep(
            0.5, 18.0,
            distance(vWorldPos.xz, greenSpot)
        );

    float blueFalloff =
        1.0 - smoothstep(
            0.5, 18.0,
            distance(vWorldPos.xz, blueSpot)
        );

    vec3 coloredGroundLight =
        vec3(1.00, 0.10, 0.12) * redPulse   * redFalloff +
        vec3(0.10, 1.00, 0.18) * greenPulse * greenFalloff +
        vec3(0.12, 0.30, 1.00) * bluePulse  * blueFalloff;

    // stronger visible color on snow and ground
    result += coloredGroundLight * groundMask * 2.35;
}

// SCENE 1: Winter City - decorative city lighting
if (uSceneIndex == 0) {

    // -------------------------------------------------
    // 1) KHAN SHATYR - stronger purple / pink lighting
    // -------------------------------------------------
    vec3 khanLightPos = vec3(-68.0, 18.0, -104.0);
    vec3 khanLightColor = vec3(0.95, 0.25, 0.88);

    result += calculateLight(
        khanLightPos,
        khanLightColor,
        8.0,
        baseColor,
        N,
        V,
        0.02,
        0.002
    );

    float khanArea =
        1.0 - smoothstep(
            8.0, 40.0,
            distance(vWorldPos.xz, vec2(-68.0, -104.0))
        );

    float groundMaskKhan = smoothstep(0.35, 0.92, N.y);

    result += vec3(0.75, 0.20, 0.72)
        * khanArea
        * groundMaskKhan
        * 1.4;


    // -------------------------------------------------
    // 2) TWO GOLDEN TOWERS behind Baiterek
    // -------------------------------------------------
    vec3 towerLeftPos  = vec3(-18.0, 22.0, -73.0);
    vec3 towerRightPos = vec3( 18.0, 22.0, -73.0);
    vec3 towerColor    = vec3(1.00, 0.58, 0.15);

    result += calculateLight(
        towerLeftPos,
        towerColor,
        4.8,
        baseColor,
        N,
        V,
        0.03,
        0.003
    );

    result += calculateLight(
        towerRightPos,
        towerColor,
        4.8,
        baseColor,
        N,
        V,
        0.03,
        0.003
    );

    float towerLeftArea =
        1.0 - smoothstep(
            6.0, 22.0,
            distance(vWorldPos.xz, vec2(-18.0, -73.0))
        );

    float towerRightArea =
        1.0 - smoothstep(
            6.0, 22.0,
            distance(vWorldPos.xz, vec2(18.0, -73.0))
        );

    float groundMaskTower = smoothstep(0.35, 0.92, N.y);

    result += vec3(0.95, 0.55, 0.16)
        * (towerLeftArea + towerRightArea)
        * groundMaskTower
        * 0.70;


    // -------------------------------------------------
    // 3) MOSQUE - warm white illumination
    // -------------------------------------------------
    vec3 mosqueMainPos = vec3(66.0, 10.0, -76.0);
    vec3 mosqueLeftMinaretPos  = vec3(54.0, 18.0, -77.0);
    vec3 mosqueRightMinaretPos = vec3(78.0, 18.0, -77.0);

    vec3 mosqueColor = vec3(1.00, 0.95, 0.86);

    result += calculateLight(
        mosqueMainPos,
        mosqueColor,
        2.0,
        baseColor,
        N,
        V,
        0.035,
        0.0035
    );

    result += calculateLight(
        mosqueLeftMinaretPos,
        mosqueColor,
        1.3,
        baseColor,
        N,
        V,
        0.04,
        0.004
    );

    result += calculateLight(
        mosqueRightMinaretPos,
        mosqueColor,
        1.3,
        baseColor,
        N,
        V,
        0.04,
        0.004
    );

    float mosqueArea =
        1.0 - smoothstep(
            10.0, 32.0,
            distance(vWorldPos.xz, vec2(66.0, -76.0))
        );

    float groundMaskMosque = smoothstep(0.35, 0.92, N.y);

    result += vec3(0.90, 0.86, 0.76)
        * mosqueArea
        * groundMaskMosque
        * 0.22;
}

        // SCENE 3: Riverfront - repeated streetlights

        if (uSceneIndex == 2) {

            // Find nearest lamp in each riverbank row
            float nearestX = clamp(
                floor(vWorldPos.x / 22.0 + 0.5)
                    * 22.0,
                -220.0,
                220.0
            );



            vec3 nearBankLamp =
                vec3(nearestX, 9.7, 18.0);

            vec3 farBankLamp =
                vec3(nearestX, 9.2, -112.0);

            vec3 warmLampColor =
                vec3(1.0, 0.72, 0.35);

            result += calculateLight(
                nearBankLamp,
                warmLampColor,
                5.5,
                baseColor,
                N,
                V,
                0.12,
                0.08
            );

            result += calculateLight(
                farBankLamp,
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

    // Fog
    float dist = length(vWorldPos - uCamPos);

    float fog = clamp(
        1.0 - exp(-dist * uFogDensity),
        0.0,
        1.0
    );

    vec3 color = mix(result, uFogColor, fog);

    // Normal emissive materials remain visible
    vec3 emission = uEmissive;

    // SCENE 1: Special golden Baiterek lighting
if (uIsBaiterek) {

    if (uLightingEnabled) {
        // Stronger at the bottom of the tower
        float shaftGlow =
            1.0 - smoothstep(
                12.0, 43.0, vWorldPos.y
            );

        vec3 goldenIllumination =
            vec3(1.0, 0.37, 0.035)
            * 1.45 * shaftGlow;

        // Softer golden light near the sphere
        goldenIllumination +=
            vec3(0.65, 0.35, 0.08) * 0.35;

        emission += goldenIllumination;
    } else {
        // Keep Baiterek faintly visible when lighting is OFF
        float shaftGlow =
            1.0 - smoothstep(
                12.0, 43.0, vWorldPos.y
            );

        vec3 softResidualGlow =
            vec3(0.10, 0.06, 0.02) * (0.35 + 0.65 * shaftGlow);

        emission += softResidualGlow;
    }
}

    color += emission * (1.0 - 0.5 * fog);

    FragColor = vec4(color, 1.0);
}
