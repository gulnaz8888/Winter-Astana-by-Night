#version 330 core
in vec2 vUV;
uniform sampler2D uTexture;
out vec4 FragColor;
void main(){vec4 c=texture(uTexture,vUV);if(c.a<0.02)discard;FragColor=c;}
