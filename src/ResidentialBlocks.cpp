#include "Scene.h"
#include <cmath>

void Scene::buildResidentialBlocks() {
    SceneGeometry masonry,trim,roof,snow,frames,glass,glow,rails;
    for(float side:{-1.f,1.f}) {
        float x=side*42.f,z=3.f,w=20.f,d=16.f,h=11.2f;
        masonry.box({x,0,z},{w,h,d});
        masonry.box({x,0,z+d/2+0.45f},{5.0f,h+0.65f,0.9f});
        masonry.box({x-side*(w/2+0.45f),0,z},{0.9f,h+0.65f,4.3f});
        for(float y:{0.35f,3.65f,6.65f,9.65f,11.2f}) {
            trim.box({x,y,z},{w+0.45f,0.23f,d+0.45f});
            trim.box({x,y,z+d/2+0.52f},{5.35f,0.25f,1.12f});
            trim.box({x-side*(w/2+0.52f),y,z},{1.12f,0.25f,4.65f});
        }
        const glm::vec3 bottom[4]={{x-w/2-0.4f,h+0.35f,z+d/2+0.4f},{x+w/2+0.4f,h+0.35f,z+d/2+0.4f},
                                  {x+w/2+0.4f,h+0.35f,z-d/2-0.4f},{x-w/2-0.4f,h+0.35f,z-d/2-0.4f}};
        const glm::vec3 top[4]={{x-w/2+1.6f,h+2.15f,z+d/2-1.5f},{x+w/2-1.6f,h+2.15f,z+d/2-1.5f},
                               {x+w/2-1.6f,h+2.15f,z-d/2+1.5f},{x-w/2+1.6f,h+2.15f,z-d/2+1.5f}};
        for(int i=0;i<4;++i) {
            int j=(i+1)%4;
            roof.quad(bottom[i],bottom[j],top[j],top[i]);
            snow.beam(bottom[i]+glm::vec3(0,0.05f,0),bottom[j]+glm::vec3(0,0.05f,0),0.09f);
        }
        snow.quad(top[0],top[1],top[2],top[3]);
        for(float offset:{-6.6f,6.6f}) {
            trim.box({x+offset,h+0.6f,z+d/2-0.15f},{2.2f,1.2f,0.7f});
            glass.box({x+offset,h+0.78f,z+d/2+0.23f},{1.15f,0.85f,0.08f});
            roof.triangle({x+offset-1.3f,h+1.8f,z+d/2+0.25f},{x+offset+1.3f,h+1.8f,z+d/2+0.25f},{x+offset,h+2.65f,z+d/2+0.25f});
        }
        auto facade=[&](glm::vec3 origin,glm::vec3 u,glm::vec3 n,int columns,float span) {
            auto box=[&](SceneGeometry& g,float along,float y,float offset,float width,float height,float depth) {
                g.box(origin+u*along+n*offset+glm::vec3(0,y,0),glm::abs(u)*width+glm::vec3(0,height,0)+glm::abs(n)*depth);
            };
            for(int floor=0;floor<3;++floor) {
                float y=1.0f+floor*3.f;
                for(int col=0;col<columns;++col) {
                    float along=-span/2+span*(col+0.5f)/columns;
                    bool center=std::abs(along)<2.3f;
                    float front=center ? 1.03f : 0.07f;
                    float ww=center ? 1.65f : 1.35f;
                    box(trim,along,y-0.12f,front,ww+0.42f,2.42f,0.28f);
                    box(glass,along,y,front+0.17f,ww,2.12f,0.045f);
                    if((floor+col+(side>0?1:0))%4!=0) box(glow,along,y+0.12f,front+0.20f,ww-0.20f,1.82f,0.02f);
                    box(frames,along,y,front+0.23f,0.055f,2.12f,0.045f);
                    box(frames,along,y+1.30f,front+0.24f,ww,0.055f,0.045f);
                    box(trim,along,y-0.22f,front+0.24f,ww+0.65f,0.13f,0.65f);
                    if(floor>0 && (center || col%2==0)) {
                        box(trim,along,y-0.25f,front+0.48f,ww+0.8f,0.13f,1.05f);
                        box(rails,along,y+0.05f,front+1.01f,ww+0.60f,0.055f,0.065f);
                        box(rails,along,y+0.70f,front+1.01f,ww+0.60f,0.055f,0.065f);
                        for(int k=0;k<7;++k) box(rails,along-(ww+0.6f)/2+k*(ww+0.6f)/6,y+0.05f,front+1.01f,0.035f,0.7f,0.04f);
                    }
                }
            }
            for(float sign:{-1.f,1.f}) box(trim,sign*(span/2-0.25f),0.4f,0.15f,0.48f,10.75f,0.30f);
            box(trim,0,h+0.18f,0.95f,5.3f,0.38f,1.2f);
            for(int i=0;i<24;++i) {
                float a=3.14159265f*i/24,b=3.14159265f*(i+1)/24;
                auto point=[&](float t){return origin+u*(2.2f*std::cos(t))+n*1.50f+glm::vec3(0,h+0.58f+1.2f*std::sin(t),0);};
                trim.beam(point(a),point(b),0.12f);
            }
        };
        facade({x,0,z+d/2},{1,0,0},{0,0,1},6,w-1.0f);
        facade({x-side*w/2,0,z},{0,0,1},{-side,0,0},5,d-1.0f);
    }
    attach(root,masonry,{0.75f,0.80f,0.87f},{0.07f,0.09f,0.12f},brickTexture.get());
    attach(root,trim,{0.83f,0.80f,0.73f},{0.13f,0.13f,0.13f});
    attach(root,roof,{0.10f,0.15f,0.22f},{0.02f,0.03f,0.05f});
    attach(root,snow,{0.78f,0.85f,0.96f},{0.16f,0.19f,0.25f});
    attach(root,glass,{0.08f,0.16f,0.24f},{0.015f,0.035f,0.06f},blueGlassTexture.get());
    attach(root,glow,{0.90f,0.78f,0.53f},{0.35f,0.22f,0.08f});
    attach(root,frames,{0.74f,0.77f,0.79f},{0.11f,0.12f,0.14f});
    attach(root,rails,{0.09f,0.11f,0.14f},{0.015f,0.02f,0.03f});
}
