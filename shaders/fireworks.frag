
#version 330 core

in vec4 vColor;

out vec4 FragColor;

void main() {
    vec2 center = gl_PointCoord - vec2(0.5);
    float distanceFromCenter = length(center);

    if (distanceFromCenter > 0.5) {
        discard;
    }

    // Bright center with a soft outer glow.
    float glow = 1.0 - smoothstep(
        0.08,
        0.5,
        distanceFromCenter
    );

    float alpha = glow * vColor.a;

    FragColor = vec4(
        vColor.rgb,
        alpha
    );
}
