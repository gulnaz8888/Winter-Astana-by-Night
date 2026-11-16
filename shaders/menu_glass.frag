#version 330 core
in vec2 vUV;
uniform vec3 uColor;
out vec4 FragColor;
void main(){float edge=min(min(vUV.x,1.0-vUV.x),min(vUV.y,1.0-vUV.y));float rim=1.0-smoothstep(0.0,0.045,edge);float sheen=pow(max(0.0,1.0-abs(vUV.y-0.78)*5.0),8.0)*0.08;FragColor=vec4(uColor+vec3(0.23,0.38,0.47)*rim+vec3(sheen),0.20+0.36*rim);}
