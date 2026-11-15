#pragma once
#include <array>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

struct SceneGeometry {
    std::vector<float> vertices;
    std::vector<unsigned> indices;

    void triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c,
                  glm::vec2 ua = {}, glm::vec2 ub = {}, glm::vec2 uc = {}) {
        glm::vec3 n=glm::cross(b-a,c-a);
        if (glm::dot(n,n)<1e-10f) return;
        n=glm::normalize(n);
        const std::array<glm::vec3,3> ps={a,b,c};
        const std::array<glm::vec2,3> uvs={ua,ub,uc};
        unsigned first=static_cast<unsigned>(vertices.size()/8);
        for (int i=0;i<3;++i) {
            auto p=ps[i]; auto uv=uvs[i];
            vertices.insert(vertices.end(),{p.x,p.y,p.z,n.x,n.y,n.z,uv.x,uv.y});
        }
        indices.insert(indices.end(),{first,first+1,first+2});
    }

    void quad(glm::vec3 a,glm::vec3 b,glm::vec3 c,glm::vec3 d) {
        triangle(a,b,c,{0,0},{1,0},{1,1});
        triangle(a,c,d,{0,0},{1,1},{0,1});
    }

    void box(glm::vec3 p,glm::vec3 size) {
        const std::array<glm::vec3,6> ns={{{0,0,1},{0,0,-1},{1,0,0},{-1,0,0},{0,1,0},{0,-1,0}}};
        const std::array<glm::vec3,6> us={{{1,0,0},{-1,0,0},{0,0,-1},{0,0,1},{1,0,0},{1,0,0}}};
        const std::array<glm::vec3,6> vs={{{0,1,0},{0,1,0},{0,1,0},{0,1,0},{0,0,-1},{0,0,1}}};
        for (int i=0;i<6;++i) {
            auto c=p+glm::vec3(0,size.y/2,0)+ns[i]*size*0.5f;
            auto u=us[i]*size*0.5f,v=vs[i]*size*0.5f;
            quad(c-u-v,c+u-v,c+u+v,c-u+v);
        }
    }

    void beam(glm::vec3 a,glm::vec3 b,float radius,int sides=6) {
        auto delta=b-a;
        if (glm::dot(delta,delta)<1e-10f) return;
        auto axis=glm::normalize(delta);
        auto reference=std::abs(axis.y)<0.9f ? glm::vec3(0,1,0) : glm::vec3(1,0,0);
        auto u=glm::normalize(glm::cross(axis,reference));
        auto v=glm::cross(axis,u);
        for (int i=0;i<sides;++i) {
            float t=6.28318530718f*i/sides,s=6.28318530718f*(i+1)/sides;
            auto r=radius*(u*std::cos(t)+v*std::sin(t));
            auto q=radius*(u*std::cos(s)+v*std::sin(s));
            quad(a+r,a+q,b+q,b+r);
            triangle(b,b+r,b+q);
            triangle(a,a+q,a+r);
        }
    }

    void ring(glm::vec3 center,float radius,float thickness,int segments=48) {
        for (int i=0;i<segments;++i) {
            float a=6.28318530718f*i/segments,b=6.28318530718f*(i+1)/segments;
            beam(center+glm::vec3(radius*std::cos(a),0,radius*std::sin(a)),
                 center+glm::vec3(radius*std::cos(b),0,radius*std::sin(b)),thickness,5);
        }
    }

    void frustum(glm::vec3 p,float bottom,float top,float height,int segments=24) {
        for (int i=0;i<segments;++i) {
            float a=6.28318530718f*i/segments,b=6.28318530718f*(i+1)/segments;
            glm::vec3 u(std::cos(a),0,std::sin(a)),v(std::cos(b),0,std::sin(b));
            glm::vec3 up(0,height,0);
            quad(p+u*bottom,p+up+u*top,p+up+v*top,p+v*bottom);
            triangle(p+up,p+up+v*top,p+up+u*top);
            triangle(p,p+u*bottom,p+v*bottom);
        }
    }

    void ball(glm::vec3 center,float radius) {
        for (int j=0;j<6;++j) {
            float t0=-1.57079632679f+3.14159265359f*j/6;
            float t1=-1.57079632679f+3.14159265359f*(j+1)/6;
            for (int i=0;i<10;++i) {
                float a=6.28318530718f*i/10,b=6.28318530718f*(i+1)/10;
                auto point=[&](float t,float p) {
                    return center+glm::vec3(radius*std::cos(t)*std::cos(p),radius*std::sin(t),radius*std::cos(t)*std::sin(p));
                };
                quad(point(t0,a),point(t1,a),point(t1,b),point(t0,b));
            }
        }
    }

    void dome(glm::vec3 center,float radius,float height,int rings=10,int segments=32) {
        for (int j=0;j<rings;++j) {
            float t0=1.57079632679f*j/rings,t1=1.57079632679f*(j+1)/rings;
            for (int i=0;i<segments;++i) {
                float a=6.28318530718f*i/segments,b=6.28318530718f*(i+1)/segments;
                auto point=[&](float t,float p) {
                    return center+glm::vec3(radius*std::cos(t)*std::cos(p),height*std::sin(t),radius*std::cos(t)*std::sin(p));
                };
                quad(point(t0,a),point(t1,a),point(t1,b),point(t0,b));
            }
        }
    }
};
