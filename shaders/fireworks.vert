
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aAlpha;

uniform mat4 uView;
uniform mat4 uProj;

out vec4 vColor;

void main()
{
    vec4 viewPos = uView * vec4(aPos, 1.0);

    gl_Position = uProj * viewPos;

    float distanceToCamera = max(
        length(viewPos.xyz),
        1.0
    );

    // Larger bright sparks
    gl_PointSize = clamp(
        2400.0 / distanceToCamera,
        6.0,
        22.0
    );

    vColor = vec4(
        aColor,
        aAlpha
    );
}
