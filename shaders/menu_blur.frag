#version 330 core
in vec2 vUV;
uniform sampler2D uTexture;
uniform vec3 uTexel;
uniform float uBlur;
out vec4 FragColor;
void main(){vec3 c=vec3(0);float weights=0;for(int i=-8;i<=8;++i){float w=exp(-float(i*i)/22.0);vec2 offset=vec2(float(i)*uTexel.x*uBlur,float(i%3)*uTexel.y*uBlur*0.18);c+=texture(uTexture,vUV+offset).rgb*w;weights+=w;}FragColor=vec4(c/weights,1);}
