#include "Scene.h"
#include "Primitives.h"
#include <cmath>

namespace {
constexpr float PI=3.14159265359f;
const glm::vec3 snowColor(0.78f,0.85f,0.96f);
const glm::vec3 snowGlow(0.16f,0.19f,0.25f);
const glm::vec3 white(0.84f,0.89f,0.96f);
const glm::vec3 whiteGlow(0.15f,0.18f,0.23f);
const glm::vec3 metal(0.075f,0.10f,0.15f);
const glm::vec3 warm(1.f,0.85f,0.52f);
const glm::vec3 warmGlow(1.f,0.73f,0.33f);
}

Scene::Scene():sphere(Primitives::sphere(32,48)) {
    snowTexture=std::make_unique<Texture>("assets/textures/snow.png");
    buildGround();
    buildAvenue();
    buildLamps();
    buildSkyline();
    buildMosque();
    buildKhanShatyr();
    buildBaiterek();
}

Node* Scene::attach(Node& parent,const SceneGeometry& geometry,glm::vec3 color,
                    glm::vec3 emission,const Texture* texture) {
    if (geometry.indices.empty()) return nullptr;
    meshes.push_back(std::make_unique<Mesh>(geometry.vertices,geometry.indices));
    auto node=std::make_unique<Node>();
    node->mesh=meshes.back().get();
    node->color=color;
    node->emissive=emission;
    node->texture=texture;
    return parent.addChild(std::move(node));
}

void Scene::buildGround() {
    SceneGeometry ground,path,borders,stone;
    ground.box({0,-0.38f,-30},{240,0.3f,310});
    path.box({0,-0.02f,-5},{16,0.06f,110});
    for (float side : {-1.f,1.f}) {
        borders.box({side*8.7f,-0.02f,-5},{1.1f,0.28f,110});
        stone.box({side*9.f,-0.08f,-5},{1.8f,0.2f,110});
        for (int i=0;i<6;++i) {
            float z=29.f-i*14.f;
            stone.box({side*9.f,0.15f,z},{0.35f,0.45f,3.6f});
        }
    }
    for (int i=0;i<4;++i) {
        stone.box({0,0.12f+i*0.17f,-49.f-i*1.2f},{21.f-i*1.2f,0.17f,2.f});
    }
    ground.frustum({0,0.25f,-60},10.5f,10.5f,0.4f,48);
    attach(root,ground,snowColor,snowGlow,snowTexture.get());
    attach(root,path,{0.70f,0.77f,0.88f},{0.16f,0.19f,0.24f},snowTexture.get());
    attach(root,borders,snowColor,snowGlow);
    attach(root,stone,{0.29f,0.34f,0.43f},{0.045f,0.06f,0.085f});
}

void Scene::buildAvenue() {
    SceneGeometry green,snow,trunks,wood,iron;
    for (int i=0;i<7;++i) {
        float z=34.f-i*13.f;
        for (float side : {-1.f,1.f}) {
            float x=side*(13.8f+(i%2)*1.3f),h=7.7f+(i%3)*0.9f;
            trunks.frustum({x,0,z},0.28f,0.18f,2.6f,8);
            for (int tier=0;tier<4;++tier) {
                float base=1.0f+tier*h*0.17f;
                float r=3.2f*(1.f-tier*0.19f);
                float th=h*0.47f;
                green.frustum({x,base,z},r,0,th,12);
                snow.frustum({x,base+th*0.27f,z},r*0.76f,0,th*0.78f,12);
            }
            snow.dome({x,0.08f,z},3.5f,0.65f,3,16);
        }
    }
    for (int i=0;i<5;++i) {
        float z=26.f-i*15.f;
        for (float side : {-1.f,1.f}) {
            float x=side*10.7f;
            wood.box({x,0.75f,z},{1.3f,0.2f,3.4f});
            wood.box({x+side*0.65f,1.0f,z},{0.14f,0.8f,3.4f});
            snow.box({x,0.95f,z},{1.25f,0.06f,3.4f});
            for (float dz : {-1.2f,1.2f}) {
                iron.box({x,0,z+dz},{1.1f,0.75f,0.15f});
            }
        }
    }
    attach(root,green,{0.045f,0.16f,0.13f},{0.012f,0.038f,0.034f});
    attach(root,snow,snowColor,snowGlow);
    attach(root,trunks,{0.21f,0.14f,0.10f},{0.025f,0.02f,0.02f});
    attach(root,wood,{0.33f,0.20f,0.12f},{0.05f,0.027f,0.015f});
    attach(root,iron,metal,{0.012f,0.02f,0.03f});
}

void Scene::buildLamps() {
    SceneGeometry posts,hoods,bulbs,cables,snow;
    for (int i=0;i<7;++i) {
        float z=35.f-i*13.f;
        for (float side : {-1.f,1.f}) {
            float x=side*8.5f;
            posts.frustum({x,0,z},0.20f,0.11f,7.4f,10);
            posts.frustum({x,0,z},0.43f,0.25f,0.45f,10);
            hoods.dome({x,7.45f,z},0.67f,0.23f,4,16);
            bulbs.frustum({x,7.3f,z},0.49f,0.49f,0.15f,16);
            snow.dome({x,7.63f,z},0.56f,0.15f,3,12);
        }
        for (int k=0;k<32;++k) {
            float t=k/32.f,u=(k+1)/32.f;
            auto point=[&](float f) {
                return glm::vec3(-8.5f+17.f*f,7.1f-1.35f*std::sin(PI*f),z);
            };
            cables.beam(point(t),point(u),0.025f,4);
        }
        for (int k=1;k<20;++k) {
            float t=k/20.f;
            glm::vec3 p(-8.5f+17*t,7.1f-1.35f*std::sin(PI*t),z);
            cables.beam(p,p-glm::vec3(0,0.17f,0),0.018f,4);
            bulbs.ball(p-glm::vec3(0,0.25f,0),0.12f);
        }
    }
    attach(root,posts,metal,{0.018f,0.025f,0.04f});
    attach(root,hoods,{0.25f,0.29f,0.36f},{0.045f,0.055f,0.075f});
    attach(root,bulbs,warm,warmGlow);
    attach(root,cables,metal,{0.01f,0.015f,0.025f});
    attach(root,snow,snowColor,snowGlow);
}

void Scene::buildSkyline() {
    SceneGeometry gold,frames,glass,light,snow,buildings;
    for (float side : {-1.f,1.f}) {
        float x=side*18.5f,z=-103.f;
        gold.frustum({x,0,z},5.2f,3.45f,31.f,32);
        frames.frustum({x,31,z},3.5f,3.2f,1.f,32);
        frames.frustum({x,32,z},2.7f,0,6.f,16);
        for (int i=0;i<16;++i) {
            float a=2*PI*i/16;
            frames.beam({x+5.23f*std::sin(a),0,z+5.23f*std::cos(a)},
                        {x+3.48f*std::sin(a),31,z+3.48f*std::cos(a)},0.045f,4);
        }
        for (int j=1;j<16;++j) {
            float y=j*1.9f,r=5.2f-(y/31.f)*1.75f;
            frames.ring({x,y,z},r+0.035f,0.025f,32);
            float rr=r+0.065f;
            glass.box({x,y,z+rr},{1.7f,1.5f,0.07f});
        }
        light.beam({x,37.5f,z},{x,39,z},0.06f);
    }
    for (float side : {-1.f,1.f}) {
        for (int i=0;i<1;++i) {
            float x=side*(42.f+i*12.f),z=3.f-i*26.f;
            float w=20.f+i*2.f,h=11.f+(i%3)*3.2f,d=16.f;
            buildings.box({x,0,z},{w,h,d});
            buildings.box({x,h,z},{w+0.6f,0.45f,d+0.6f});
            snow.box({x,h+0.45f,z},{w+0.9f,0.25f,d+0.9f});
            for (float y=2.5f;y<h-1;y+=2.9f) {
                for (int k=0;k<7;++k) {
                    float xx=x-w*0.42f+k*w*0.14f;
                    if ((i+k+static_cast<int>(y))%4 == 0) continue;
                    light.box({xx,y,z+d*0.5f+0.06f},{1.1f,1.6f,0.08f});
                }
                for (int k=0;k<5;++k) {
                    float zz=z-d*0.4f+k*d*0.2f;
                    light.box({x-side*(w*0.5f+0.06f),y,zz},{0.08f,1.6f,1.0f});
                }
            }
            for (float offset : {-0.45f,0.f,0.45f}) {
                frames.box({x+w*offset,0,z+d*0.5f+0.12f},{0.22f,h,0.25f});
            }
        }
    }
    attach(root,gold,{0.94f,0.68f,0.18f},{0.24f,0.14f,0.025f});
    attach(root,frames,{0.49f,0.45f,0.30f},{0.08f,0.07f,0.045f});
    attach(root,glass,{0.07f,0.19f,0.24f},{0.018f,0.045f,0.07f});
    attach(root,light,warm,{0.62f,0.37f,0.12f});
    attach(root,snow,snowColor,snowGlow);
    attach(root,buildings,{0.50f,0.59f,0.69f},{0.07f,0.09f,0.12f});
}

void Scene::buildMosque() {
    auto group=std::make_unique<Node>();
    group->position={80,0,-105};
    Node* mosque=root.addChild(std::move(group));
    SceneGeometry walls,domes,details,windows;
    walls.box({0,0,0},{28,8,21});
    walls.box({0,8,0},{17,2.5f,15});
    walls.frustum({0,10.5f,0},6.5f,6.5f,2.f,32);
    domes.dome({0,12.5f,0},7.3f,7.f,12,40);
    details.beam({0,19.5f,0},{0,22,0},0.10f);
    details.ring({0,12.7f,0},7.35f,0.10f,48);
    for (float x : {-9.f,9.f}) {
        for (float z : {-6.f,6.f}) {
            domes.dome({x,8,z},3.4f,3.3f,8,24);
        }
    }
    for (float sx : {-1.f,1.f}) {
        for (float sz : {-1.f,1.f}) {
            glm::vec3 p(sx*16.f,0,sz*11.f);
            walls.frustum(p,1.1f,0.68f,26.f,12);
            walls.frustum(p+glm::vec3(0,20,0),1.5f,1.5f,0.6f,16);
            walls.frustum(p+glm::vec3(0,25,0),1.2f,0.9f,1.0f,16);
            domes.frustum(p+glm::vec3(0,26,0),1.05f,0,4.f,16);
            details.beam(p+glm::vec3(0,30,0),p+glm::vec3(0,32,0),0.07f);
        }
    }
    for (int i=-5;i<=5;++i) {
        if (i==0) continue;
        windows.box({i*2.2f,2.1f,10.57f},{1.1f,3.3f,0.08f});
        domes.dome({i*2.2f,5.4f,10.60f},0.55f,0.6f,4,12);
    }
    windows.box({0,0.5f,10.65f},{3,5,0.1f});
    attach(*mosque,walls,white,{0.23f,0.24f,0.27f});
    attach(*mosque,domes,{0.75f,0.82f,0.88f},{0.15f,0.19f,0.23f});
    attach(*mosque,details,{0.85f,0.68f,0.28f},{0.20f,0.14f,0.04f});
    attach(*mosque,windows,warm,{0.60f,0.35f,0.12f});
    for (auto& child : mosque->children) child->scale.x=1.18f;
}

void Scene::buildKhanShatyr() {
    auto group=std::make_unique<Node>();
    group->position={-68,0,-104};
    Node* tent=root.addChild(std::move(group));
    SceneGeometry fabric,seams,base,windows;
    glm::vec3 peak(3.5f,31,-2);
    for (int i=0;i<48;++i) {
        float a=2*PI*i/48,b=2*PI*(i+1)/48;
        glm::vec3 p(19*std::cos(a),2,14*std::sin(a));
        glm::vec3 q(19*std::cos(b),2,14*std::sin(b));
        fabric.triangle(p,peak,q);
        if (i%2==0) seams.beam(p,peak,0.045f,4);
    }
    base.frustum({0,0,0},18,18,2.2f,48);
    seams.beam(peak,peak+glm::vec3(0.6f,6,0),0.15f);
    for (int i=0;i<12;++i) {
        float a=PI*i/12;
        windows.box({16*std::cos(a),0.6f,13*std::sin(a)+0.2f},{1.2f,1.1f,0.3f});
    }
    attach(*tent,fabric,{0.73f,0.80f,0.90f},{0.14f,0.18f,0.24f});
    attach(*tent,seams,{0.93f,0.95f,1.f},{0.23f,0.26f,0.32f});
    attach(*tent,base,{0.40f,0.45f,0.55f},{0.065f,0.08f,0.115f});
    attach(*tent,windows,warm,{0.70f,0.40f,0.13f});
    for (auto& child : tent->children) child->scale.x=1.15f;
}

void Scene::buildBaiterek() {
    auto group=std::make_unique<Node>();
    group->position={0,0.65f,-60};
    Node* monument=root.addChild(std::move(group));
    SceneGeometry ribs,lattice,shaft,base,collar,panes;
    const float heights[]={0,10,20,26,31,36,41,47};
    const float radii[]={3.35f,2.65f,2.15f,2.5f,3.6f,5.25f,7.25f,9.15f};
    constexpr int count=24;
    auto point=[&](int level,int rib) {
        float a=2*PI*rib/count;
        return glm::vec3(radii[level]*std::sin(a),heights[level],radii[level]*std::cos(a));
    };
    for (int i=0;i<count;++i) {
        for (int j=0;j<7;++j) {
            ribs.beam(point(j,i),point(j+1,i),j<3 ? 0.105f : 0.085f);
            if (j>=2) {
                lattice.beam(point(j,i),point(j+1,i+1),0.038f,4);
                lattice.beam(point(j,i+1),point(j+1,i),0.038f,4);
            }
        }
    }
    for (int j=1;j<7;++j) lattice.ring({0,heights[j],0},radii[j],0.07f,72);
    shaft.frustum({0,0,0},1.35f,1.35f,40.f,32);
    for (int i=0;i<8;++i) {
        float a=2*PI*i/8;
        ribs.beam({1.40f*std::sin(a),0,1.40f*std::cos(a)},
                  {1.40f*std::sin(a),39,1.40f*std::cos(a)},0.045f,4);
    }
    base.frustum({0,0,0},5.7f,5.7f,0.5f,48);
    base.frustum({0,0.5f,0},4.7f,4.7f,0.3f,48);
    collar.frustum({0,39.3f,0},4.8f,5.6f,0.9f,48);
    ribs.ring({0,40.2f,0},5.7f,0.12f,72);
    for (int i=0;i<36;++i) {
        float a=2*PI*i/36;
        ribs.beam({5.7f*std::sin(a),40.1f,5.7f*std::cos(a)},
                  {5.7f*std::sin(a),41.1f,5.7f*std::cos(a)},0.028f,4);
    }
    ribs.ring({0,41.1f,0},5.7f,0.045f,72);
    attach(*monument,ribs,white,{0.29f,0.32f,0.37f});
    attach(*monument,lattice,{0.61f,0.69f,0.78f},{0.15f,0.19f,0.24f});
    attach(*monument,shaft,{0.055f,0.17f,0.21f},{0.018f,0.055f,0.07f});
    attach(*monument,base,{0.55f,0.59f,0.65f},{0.10f,0.12f,0.16f});
    attach(*monument,collar,{0.24f,0.26f,0.28f},{0.065f,0.07f,0.08f});
    auto orb=std::make_unique<Node>();
    orb->mesh=&sphere;
    orb->position={0,45.7f,0};
    orb->scale={12.4f,12.4f,12.4f};
    orb->color={1.f,0.73f,0.20f};
    orb->emissive={0.46f,0.28f,0.045f};
    monument->addChild(std::move(orb));
    float radius=6.24f;
    for (int j=-6;j<=6;++j) {
        float theta=j*PI/16;
        panes.ring({0,45.7f+radius*std::sin(theta),0},radius*std::cos(theta),0.022f,64);
    }
    for (int i=0;i<24;++i) {
        float a=2*PI*i/24;
        for (int j=0;j<24;++j) {
            float t0=-PI*0.5f+PI*j/24,t1=-PI*0.5f+PI*(j+1)/24;
            auto p=[&](float t) {
                return glm::vec3(radius*std::cos(t)*std::sin(a),45.7f+radius*std::sin(t),radius*std::cos(t)*std::cos(a));
            };
            panes.beam(p(t0),p(t1),0.022f,4);
        }
    }
    panes.beam({0,51.9f,0},{0,54.2f,0},0.055f);
    attach(*monument,panes,{0.31f,0.27f,0.14f},{0.045f,0.038f,0.018f});
    auto pivot=std::make_unique<Node>();
    pivot->position={0,40.6f,0};
    beaconPivot=monument->addChild(std::move(pivot));
    SceneGeometry beacon;
    beacon.ball({5.85f,0,0},0.12f);
    attach(*beaconPivot,beacon,warm,warmGlow);
}

void Scene::update(float time) {
    if (beaconPivot) beaconPivot->rotation.y=time*0.35f;
}

void Scene::draw(const Shader& shader) const {
    root.draw(shader,glm::mat4(1.f));
}
