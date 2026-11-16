
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
    float distanceToLight = length(lightVector);
    vec3 L = normalize(lightVector);

    float diffuseFactor = max(dot(N, L), 0.0);
    vec3 diffuse = diffuseFactor * baseColor;

    vec3 H = normalize(L + V);

    float specularFactor = pow(
        max(dot(N, H), 0.0),
        max(uShininess, 1.0)
    );

    vec3 specular = vec3(0.20) * specularFactor;

    float attenuation = 1.0 / (
        1.0
        + linearTerm * distanceToLight
        + quadraticTerm * distanceToLight * distanceToLight
    );

    return (diffuse + specular)
        * lightColor
        * intensity
        * attenuation;
}





float snowNoise(vec3 p) {
    float a = sin(p.x * 0.43 + p.z * 0.37);
    float b = sin(p.z * 0.89 - p.x * 0.21);
    float c = sin(p.x * 1.63 + p.z * 1.27);

    return a * 0.5 + b * 0.3 + c * 0.2;
}

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

    float emissionMask = 1.0 - smoothstep(
        0.18,
        0.48,
        emissionStrength
    );

    float noiseValue = snowNoise(vWorldPos);

    float facingUp = clamp(N.y, 0.0, 1.0);

    float slopeCoverage = smoothstep(
        0.38,
        0.82,
        facingUp + noiseValue * 0.075
    );





    if (uSceneIndex == 0) {

        float heightCoverage = smoothstep(
            1.5,
            4.0,
            vWorldPos.y
        );

        return clamp(
            slopeCoverage *
            heightCoverage *
            emissionMask,
            0.0,
            1.0
        );
    }





    if (uSceneIndex == 1) {








        float distanceToTree = length(
            vWorldPos.xz - vec2(-15.0, -2.0)
        );

        float treeProtection = 1.0 - smoothstep(
            8.0,
            11.0,
            distanceToTree
        );

        if (
            treeProtection > 0.99 &&
            vWorldPos.y < 30.0
        ) {
            return 0.0;
        }





        vec3 sphereCenter = vec3(
            6.0,
            38.0,
            -42.0
        );

        float distanceFromCenter = length(
            vWorldPos - sphereCenter
        );

        float shellMask = 1.0 - smoothstep(
            0.8,
            2.0,
            abs(distanceFromCenter - 30.0)
        );



        float darkShellMask = 1.0 - smoothstep(
            0.12,
            0.28,
            max(
                max(uColor.r, uColor.g),
                uColor.b
            )
        );


        float capHeight =
            51.0 + noiseValue * 1.6;

        float topHeightMask = smoothstep(
            capHeight - 2.0,
            capHeight + 4.0,
            vWorldPos.y
        );

        float sphereSnow =
            shellMask *
            darkShellMask *
            topHeightMask *
            slopeCoverage;









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

        float podiumArea = insideX * insideZ;


        float podiumHeight =
            smoothstep(
                8.8,
                10.0,
                vWorldPos.y
            ) *
            (
                1.0 - smoothstep(
                    12.0,
                    15.0,
                    vWorldPos.y
                )
            );


        float podiumSnow =
            podiumArea *
            podiumHeight *
            smoothstep(
                0.55,
                0.90,
                facingUp
            );


        podiumSnow *= 1.0 - treeProtection;


        podiumSnow = clamp(
            podiumSnow * 1.25,
            0.0,
            1.0
        );

        float coverage = max(
            sphereSnow,
            podiumSnow
        );

        return clamp(
            coverage * emissionMask,
            0.0,
            1.0
        );
    }





    if (uSceneIndex == 2) {

        float heightCoverage = smoothstep(
            2.5,
            5.0,
            vWorldPos.y
        );

        return clamp(
            slopeCoverage *
            heightCoverage *
            emissionMask,
            0.0,
            1.0
        );
    }

    return 0.0;
}





vec3 calculateMatteSnow(float fog) {

    vec3 snowColor = vec3(
        0.96,
        0.98,
        1.0
    );

    float variation = snowNoise(
        vWorldPos * 1.3
    );

    snowColor *= 0.97 + 0.03 * variation;


    return mix(
        snowColor,
        uFogColor,
        fog * 0.32
    );
}





void main() {

    if (uIsArchitecturalLED && !uLightingEnabled) {
        discard;
    }

    vec3 baseColor = uColor;

    if (uUseTexture) {

        vec2 uv = vTexCoord * uUVScale.xy;

        baseColor *= texture(
            uTexture,
            uv
        ).rgb;
    }

    vec3 N = normalize(vNormal);
    vec3 V = normalize(uViewPos - vWorldPos);

    float ambientStrength =
        uLightingEnabled ? 0.15 : 0.05;

    vec3 result = ambientStrength * baseColor;

    if (uSceneIndex == 2 && uUseTexture) {
        result += baseColor * 0.20;
    }

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





        if (uSceneIndex == 1) {

            float redPulse =
                0.70 + 0.30 * sin(uTime * 1.6);

            float greenPulse =
                0.70 + 0.30 * sin(uTime * 1.6 + 2.1);

            float bluePulse =
                0.70 + 0.30 * sin(uTime * 1.6 + 4.2);

            vec3 treeColor = vec3(
                0.95 * redPulse,
                0.95 * greenPulse,
                0.95 * bluePulse
            );

            vec3 treeLightPos = vec3(
                -15.0,
                12.0,
                1.5
            );

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

            vec2 redSpot = vec2(-18.0, 1.0);
            vec2 greenSpot = vec2(-12.0, -2.0);
            vec2 blueSpot = vec2(-15.0, 6.0);

            float groundMask = smoothstep(
                0.45,
                0.92,
                N.y
            );

            float redFalloff =
                1.0 - smoothstep(
                    0.5,
                    18.0,
                    distance(vWorldPos.xz, redSpot)
                );

            float greenFalloff =
                1.0 - smoothstep(
                    0.5,
                    18.0,
                    distance(vWorldPos.xz, greenSpot)
                );

            float blueFalloff =
                1.0 - smoothstep(
                    0.5,
                    18.0,
                    distance(vWorldPos.xz, blueSpot)
                );

            vec3 coloredGroundLight =
                vec3(1.00, 0.10, 0.12)
                    * redPulse * redFalloff
                +
                vec3(0.10, 1.00, 0.18)
                    * greenPulse * greenFalloff
                +
                vec3(0.12, 0.30, 1.00)
                    * bluePulse * blueFalloff;

            result +=
                coloredGroundLight *
                groundMask *
                2.35;
        }





        if (uSceneIndex == 0) {

            vec3 khanLightPos = vec3(
                -68.0,
                18.0,
                -104.0
            );

            vec3 khanLightColor = vec3(
                0.95,
                0.25,
                0.88
            );

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
                    8.0,
                    40.0,
                    distance(
                        vWorldPos.xz,
                        vec2(-68.0, -104.0)
                    )
                );

            float groundMaskKhan = smoothstep(
                0.35,
                0.92,
                N.y
            );

            result +=
                vec3(0.75, 0.20, 0.72) *
                khanArea *
                groundMaskKhan *
                1.4;


            vec3 towerLeftPos = vec3(
                -18.0,
                22.0,
                -73.0
            );

            vec3 towerRightPos = vec3(
                18.0,
                22.0,
                -73.0
            );

            vec3 towerColor = vec3(
                1.00,
                0.58,
                0.15
            );

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

            float towerLeftArea = 1.0 - smoothstep(
                6.0,
                22.0,
                distance(
                    vWorldPos.xz,
                    vec2(-18.0, -73.0)
                )
            );

            float towerRightArea = 1.0 - smoothstep(
                6.0,
                22.0,
                distance(
                    vWorldPos.xz,
                    vec2(18.0, -73.0)
                )
            );

            float groundMaskTower = smoothstep(
                0.35,
                0.92,
                N.y
            );

            result +=
                vec3(0.95, 0.55, 0.16) *
                (towerLeftArea + towerRightArea) *
                groundMaskTower *
                0.70;


            vec3 mosqueMainPos = vec3(
                66.0,
                10.0,
                -76.0
            );

            vec3 mosqueLeftMinaretPos = vec3(
                54.0,
                18.0,
                -77.0
            );

            vec3 mosqueRightMinaretPos = vec3(
                78.0,
                18.0,
                -77.0
            );

            vec3 mosqueColor = vec3(
                1.00,
                0.95,
                0.86
            );

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

            float mosqueArea = 1.0 - smoothstep(
                10.0,
                32.0,
                distance(
                    vWorldPos.xz,
                    vec2(66.0, -76.0)
                )
            );

            float groundMaskMosque = smoothstep(
                0.35,
                0.92,
                N.y
            );

            result +=
                vec3(0.90, 0.86, 0.76) *
                mosqueArea *
                groundMaskMosque *
                0.22;
        }





        if (uSceneIndex == 2) {

            float nearestX = clamp(
                floor(vWorldPos.x / 22.0 + 0.5)
                    * 22.0,
                -220.0,
                220.0
            );

            vec3 nearBankLamp = vec3(
                nearestX,
                9.7,
                18.0
            );

            vec3 farBankLamp = vec3(
                nearestX,
                9.2,
                -112.0
            );

            vec3 warmLampColor = vec3(
                1.0,
                0.72,
                0.35
            );

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





    float dist = length(vWorldPos - uCamPos);

    float fog = clamp(
        1.0 - exp(-dist * uFogDensity),
        0.0,
        1.0
    );

    vec3 color = mix(
        result,
        uFogColor,
        fog
    );





    vec3 emission = uEmissive;

    if (uIsBaiterek) {

        if (uLightingEnabled) {

            float shaftGlow = 1.0 - smoothstep(
                12.0,
                43.0,
                vWorldPos.y
            );

            vec3 goldenIllumination =
                vec3(1.0, 0.37, 0.035)
                * 1.45 * shaftGlow;

            goldenIllumination +=
                vec3(0.65, 0.35, 0.08)
                * 0.35;

            emission += goldenIllumination;

        } else {

            float shaftGlow = 1.0 - smoothstep(
                12.0,
                43.0,
                vWorldPos.y
            );

            vec3 softResidualGlow =
                vec3(0.10, 0.06, 0.02)
                * (0.35 + 0.65 * shaftGlow);

            emission += softResidualGlow;
        }
    }

    color += emission * (1.0 - 0.5 * fog);





    float snowCoverage = calculateSnowCoverage(N);

    if (snowCoverage > 0.0) {

        vec3 matteSnow = calculateMatteSnow(fog);

        float opacity = clamp(
            snowCoverage * 0.98,
            0.0,
            0.98
        );

        color = mix(
            color,
            matteSnow,
            opacity
        );
    }

    FragColor = vec4(color, 1.0);
}
