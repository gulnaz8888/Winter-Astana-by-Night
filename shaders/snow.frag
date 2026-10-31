#version 330 core

out vec4 FragColor;

void main()
{
    vec2 point = gl_PointCoord - vec2(0.5);

    float distanceFromCenter = length(point);

    if (distanceFromCenter > 0.5)
        discard;

    float alpha = 1.0 - smoothstep(
        0.25,
        0.5,
        distanceFromCenter
    );

    FragColor = vec4(
    1.0,
    1.0,
    1.0,
    alpha
    );
}