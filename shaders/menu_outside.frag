#version 330 core
in vec3 vPos;
in vec3 vNormal;
uniform vec3 uColor;
uniform vec3 uEmissive;
uniform float uIntroTime;
uniform bool uIsArchitecturalLED;
out vec4 FragColor;
void main(){
    float group=vPos.x< -2.4?0.0:(vPos.x<2.4?1.0:2.0);
    float wake=smoothstep(3.7+group*0.65,4.25+group*0.65,uIntroTime);
    bool landmark=vPos.z> -6.0&&vPos.y>3.18&&!uIsArchitecturalLED;
    bool city=vPos.z< -6.0&&vPos.z> -21.0&&vPos.y<7.5;
    float cityWake=smoothstep(4.7,6.1,uIntroTime);
    float light=landmark?wake:(city?cityWake:1.0);
    float d=max(dot(normalize(vNormal),normalize(vec3(-0.3,1.0,0.6))),0.0);
    vec3 c=uColor*(0.10+0.20*d)*(landmark?0.16+1.15*light:1.0);
    c+=uEmissive*light*(landmark?2.3:1.0);
    if(landmark)c+=uColor*vec3(0.07,0.10,0.13)*wake;
    if(vPos.y<3.18&&vPos.z> -6.0){
        for(int i=0;i<3;++i){
            vec2 delta=vPos.xz-vec2(float(i-1)*5.3,-2.0);
            float on=smoothstep(3.7+float(i)*0.65,4.25+float(i)*0.65,uIntroTime);
            vec3 tint=i==0?vec3(0.20,0.13,0.035):vec3(0.035,0.12,0.19);
            c+=tint*exp(-dot(delta,delta)*0.55)*on;
        }
    }
    FragColor=vec4(c,1);
}
