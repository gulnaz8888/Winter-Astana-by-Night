#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 uView;
uniform mat4 uProj;

void main()
{
    vec4 viewPosition = uView * vec4(aPos, 1.0);

    gl_Position = uProj * viewPosition;

    float distanceFromCamera = length(viewPosition.xyz);

gl_PointSize = clamp(
    18.0 / (distanceFromCamera * 0.06),
    4.0,
    14.0
);
}