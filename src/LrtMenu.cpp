#include "LrtMenu.h"
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <string>
#include <stdexcept>

namespace {
glm::mat4 view(float time=0) { return glm::lookAt(glm::vec3(0.07f*std::sin(time*1.2f),4.5f+0.045f*std::sin(time*1.9f),12),glm::vec3(0,4.5f,0),glm::vec3(0,1,0)); }
glm::mat4 projection(float aspect) { return glm::perspective(glm::radians(55.f),aspect,0.1f,100.f); }
void cushion(SceneGeometry& g,glm::vec3 p,glm::vec3 size,float radius) {
    const std::array<glm::vec3,6> ns={{{0,0,1},{0,0,-1},{1,0,0},{-1,0,0},{0,1,0},{0,-1,0}}};
    const std::array<glm::vec3,6> us={{{1,0,0},{-1,0,0},{0,0,-1},{0,0,1},{1,0,0},{1,0,0}}};
    const std::array<glm::vec3,6> vs={{{0,1,0},{0,1,0},{0,1,0},{0,1,0},{0,0,-1},{0,0,1}}};
    auto center=p+glm::vec3(0,size.y*0.5f,0),half=size*0.5f;
    for(int face=0;face<6;++face) for(int i=0;i<8;++i) for(int j=0;j<8;++j) {
        auto point=[&](int x,int y) {
            auto q=ns[face]*half+us[face]*half*(x/4.f-1.f)+vs[face]*half*(y/4.f-1.f);
            auto core=glm::clamp(q,-half+glm::vec3(radius),half-glm::vec3(radius));
            return center+core+glm::normalize(q-core)*radius;
        };
        g.quad(point(i,j),point(i+1,j),point(i+1,j+1),point(i,j+1));
    }
}

}
Node* LrtMenu::add(const SceneGeometry& g,glm::vec3 color,glm::vec3 glow,bool outside) {
    meshes.push_back(std::make_unique<Mesh>(g.vertices,g.indices));
    auto n=std::make_unique<Node>();n->mesh=meshes.back().get();n->color=color;n->emissive=glow;
    return (outside ? exterior : root).addChild(std::move(n));
}
LrtMenu::LrtMenu():textShader("shaders/menu_text.vert","shaders/menu_text.frag"),glassShader("shaders/menu_text.vert","shaders/menu_glass.frag"),outsideShader("shaders/menu_outside.vert","shaders/menu_outside.frag"),blurShader("shaders/menu_blur.vert","shaders/menu_blur.frag") {
    SceneGeometry walls,trim,seats,seams,steel,black,lights,city,windows,gold,white,water,snow,stars,garland,expoLines,plinths,plinthEdges;
    walls.box({-10,0,0},{2,10,0.6f});walls.box({10,0,0},{2,10,0.6f});
    walls.box({0,0,0},{18,2.8f,0.6f});walls.box({0,8,0},{18,2,0.6f});
    walls.box({0,-0.3f,5},{24,0.3f,14});
    cushion(trim,{0,2.68f,0.18f},{18.4f,0.26f,0.35f},0.10f);
    cushion(trim,{0,7.86f,0.18f},{18.4f,0.26f,0.35f},0.10f);
    for(float x:{-9.f,9.f}) cushion(trim,{x,2.7f,0.18f},{0.28f,5.4f,0.35f},0.10f);
    for(int i=0;i<6;++i) {
        float x=-6.75f+i*2.7f;
        cushion(seats,{x,0.42f,1.8f},{2.35f,1.75f,0.80f},0.24f);
        cushion(seats,{x,0.22f,2.70f},{2.35f,0.45f,1.75f},0.18f);
        cushion(seats,{x,1.72f,2.01f},{1.87f,0.43f,0.60f},0.18f);
        seams.box({x,0.95f,2.22f},{1.65f,0.015f,0.025f});
        steel.beam({x-1.2f,0.4f,2.7f},{x-1.2f,1.0f,2.7f},0.055f);
        cushion(trim,{x-1.2f,0.94f,2.7f},{0.18f,0.13f,0.9f},0.06f);
    }
    for(float side:{-1.f,1.f}) steel.beam({side*4.7f,8.15f,1.4f},{side*9.f,8.15f,1.4f},0.055f);
    for(float x:{-7.8f,-5.1f,5.1f,7.8f}) {
        SceneGeometry strap,grip;
        strap.beam({0,0,0},{0,-0.50f,0},0.022f,6);
        grip.beam({0,-0.5f,0},{-0.22f,-0.88f,0},0.035f,8);
        grip.beam({-0.22f,-0.88f,0},{0.22f,-0.88f,0},0.035f,8);
        grip.beam({0.22f,-0.88f,0},{0,-0.5f,0},0.035f,8);
        auto pivot=std::make_unique<Node>();pivot->position={x,8.15f,1.4f};
        Node* parent=root.addChild(std::move(pivot));handles.push_back(parent);
        auto mount=[&](const SceneGeometry& g,glm::vec3 color) {
            meshes.push_back(std::make_unique<Mesh>(g.vertices,g.indices));
            auto node=std::make_unique<Node>();node->mesh=meshes.back().get();node->color=color;node->emissive=color*0.10f;
            parent->addChild(std::move(node));
        };
        mount(strap,{0.39f,0.42f,0.43f});mount(grip,{0.74f,0.75f,0.72f});
    }
    cushion(black,{0,8.24f,0.35f},{7.8f,1.12f,0.18f},0.08f);
    auto label=[&](const std::string& name,float x,float y,float z,float w,float h) {
        labels.push_back(std::make_unique<Texture>("assets/menu/"+name+".png"));
        SceneGeometry quad;quad.quad({x-w/2,y,z},{x+w/2,y,z},{x+w/2,y+h,z},{x-w/2,y+h,z});
        meshes.push_back(std::make_unique<Mesh>(quad.vertices,quad.indices));
        auto node=std::make_unique<Node>();node->mesh=meshes.back().get();node->texture=labels.back().get();
        textRoot.addChild(std::move(node));
    };
    label("title",0,8.52f,0.58f,6.4f,0.84f);
    label("subtitle",0,8.29f,0.58f,6.0f,0.48f);
    label("hint",0,2.28f,0.64f,8.0f,0.53f);
    for(float x:{-7.f,7.f}) lights.box({x,8.85f,0.5f},{2.1f,0.045f,0.06f});
    for(int i=0;i<80;++i) {
        float t=i/80.f,u=(i+1)/80.f;
        auto p=[](float f){return glm::vec3(-8.8f+17.6f*f,7.91f-0.38f*std::sin(f*9.424778f)*std::sin(f*9.424778f),0.70f);};
        garland.beam(p(t),p(u),0.014f,4);
        if(i%3==0) {auto q=p(t);garland.beam(q,q-glm::vec3(0,0.09f,0),0.01f,4);lights.ball(q-glm::vec3(0,0.12f,0),0.045f);}
    }
    for(int layer=0;layer<3;++layer) {
        SceneGeometry silhouette,distantWindows;
        for(int i=0;i<25;++i) {
            float x=-25.f+i*2.1f,h=0.9f+((i*13+layer*7)%11)*0.24f,z=-16.f+layer*3.f;
            silhouette.box({x,3.0f,z},{1.6f,h,1.0f});
            if(i%3==0) silhouette.box({x,h+3.0f,z},{1.15f,0.35f,0.75f});
            if(i%5==0) silhouette.frustum({x,h+3.0f,z},0.5f,0.08f,0.65f,8);
            for(int k=0;k<3;++k) silhouette.box({x-0.55f+k*0.55f,3.0f,z+0.53f},{0.035f,h,0.035f});
            if(i%4==0) silhouette.box({x,h+3,z},{1.0f,0.25f,0.7f});
            for(int row=0;row<static_cast<int>((h-0.2f)/0.32f);++row) {
                for(int col=0;col<4;++col) {
                    if((i*7+row*3+col*5+layer)%7>1) continue;
                    distantWindows.box({x-0.55f+col*0.36f,3.18f+row*0.32f,z+0.56f},{0.10f,0.14f,0.025f});
                }
            }
        }
        float v=0.025f+layer*0.012f;
        Node* block=add(silhouette,{v,v*1.3f,v*1.8f},{v*0.35f,v*0.5f,v*0.8f},true);
        skyline.push_back(block);
        meshes.push_back(std::make_unique<Mesh>(distantWindows.vertices,distantWindows.indices));
        auto panes=std::make_unique<Node>();panes->mesh=meshes.back().get();
        panes->color={0.68f,0.59f,0.43f};panes->emissive={0.27f,0.18f,0.08f};
        block->addChild(std::move(panes));
    }
    for(int i=0;i<90;++i) {
        float x=-28.f+(i*137%997)/997.f*56.f,y=7.f+(i*73%503)/503.f*9.f;
        stars.ball({x,y,-24.f},i%7==0 ? 0.045f : 0.025f);
    }
    snow.box({0,2.88f,-15.5f},{55,0.14f,29});
    for(int i=0;i<3;++i) {
        float x=(i-1)*5.3f;
        SceneGeometry panel,outline;
        panel.quad({x-2.25f,3.06f,0.54f},{x+2.25f,3.06f,0.54f},{x+2.25f,3.71f,0.54f},{x-2.25f,3.71f,0.54f});
        plinths.box({x,3.01f,-2.3f},{4.2f,0.12f,2.5f});
        plinthEdges.box({x,3.13f,-1.03f},{4.2f,0.025f,0.035f});
        meshes.push_back(std::make_unique<Mesh>(panel.vertices,panel.indices));
        auto pane=std::make_unique<Node>();pane->mesh=meshes.back().get();pane->color={0.08f,0.18f,0.23f};glassRoot.addChild(std::move(pane));
        outline.box({x,3.055f,0.56f},{4.50f,0.018f,0.025f});
        outline.box({x,3.70f,0.56f},{4.50f,0.012f,0.025f});
        for(float side:{-1.f,1.f}) outline.box({x+side*2.24f,3.06f,0.56f},{0.012f,0.65f,0.025f});
        markers[i]=add(outline,{0.52f,0.76f,0.86f},{0.24f,0.48f,0.61f});
        label(i==0 ? "baiterek" : (i==1 ? "expo" : "riverfront"),x,3.10f,0.59f,3.55f,0.57f);
    }
    const float ys[]={3.1f,3.8f,4.6f,5.2f,5.7f,6.2f};
    const float rs[]={0.22f,0.16f,0.17f,0.27f,0.48f,0.72f};
    for(int i=0;i<18;++i) {
        float a=6.2831853f*i/18,b=6.2831853f*(i+1)/18;
        for(int j=0;j<5;++j) {
            auto point=[&](int k,float t){return glm::vec3(-5.3f+rs[k]*std::sin(t),ys[k],-2+rs[k]*std::cos(t));};
            white.beam(point(j,a),point(j+1,a),0.016f,4);
            if(j>1) white.beam(point(j,a),point(j+1,b),0.008f,4);
        }
    }
    white.frustum({-5.3f,3.0f,-2},0.62f,0.62f,0.12f);
    gold.ball({-5.3f,6.16f,-2},0.46f);
    SceneGeometry globe,podium;
    podium.box({0,3.12f,-2},{3.5f,0.45f,1.7f});
    for(float side:{-1.f,1.f}) {
        podium.box({side*1.35f,3.12f,-2.15f},{1.1f,0.55f,1.0f});
        snow.box({side*1.35f,3.67f,-2.15f},{1.15f,0.055f,1.05f});
    }
    for(int i=0;i<17;++i) {
        float x=-1.6f+i*0.20f;
        expoLines.box({x,3.18f,-1.12f},{0.018f,0.31f,0.025f});
        windows.box({x+0.08f,3.22f,-1.13f},{0.06f,0.15f,0.025f});
    }
    snow.box({0,3.57f,-2},{3.55f,0.06f,1.75f});
    add(podium,{0.17f,0.23f,0.30f},{0.035f,0.055f,0.075f},true);
    globe.ball({0,4.72f,-2},1.07f);add(globe,{0.055f,0.17f,0.25f},{0.018f,0.075f,0.12f},true);
    for(int i=0;i<7;++i) for(int j=0;j<36;++j) {
        float a=6.2831853f*i/7,t=6.2831853f*j/36,u=6.2831853f*(j+1)/36;
        auto p=[&](float b){return glm::vec3(1.085f*std::cos(b)*std::cos(a),4.72f+1.085f*std::sin(b),-2+1.085f*std::cos(b)*std::sin(a));};
        expoLines.beam(p(t),p(u),0.0045f,4);
    }
    for(int j=-2;j<=2;++j) {
        float y=j*0.35f;expoLines.ring({0,4.72f+y,-2},std::sqrt(1.085f*1.085f-y*y),0.0045f,48);
    }
    water.box({5.3f,3.04f,-1.3f},{4.1f,0.025f,2.2f});
    for(int i=0;i<3;++i) {
        float x=4.45f+i*0.68f,h=i==1 ? 2.1f : (i==0 ? 1.2f : 1.65f);
        city.box({x,3.03f,-2.7f},{0.45f,h,0.5f});
        for(int j=0;j<5;++j) windows.box({x,3.15f+j*0.22f,-2.43f},{0.09f,0.10f,0.02f});
    }
    for(int i=0;i<6;++i) {
        float x=6.15f+i*0.29f;city.box({x,3.03f,-2.7f},{0.25f,0.7f+(i%2)*0.2f,0.45f});
    }
    add(walls,{0.96f,0.955f,0.93f},{0.23f,0.23f,0.22f});
    add(trim,{0.96f,0.955f,0.93f},{0.23f,0.23f,0.22f});
    add(seats,{0.055f,0.105f,0.23f},{0.012f,0.023f,0.055f});
    add(seams,{0.035f,0.06f,0.13f},{0.008f,0.012f,0.026f});
    add(steel,{0.55f,0.51f,0.44f},{0.065f,0.058f,0.048f});
    add(black,{0.035f,0.043f,0.05f},{0.006f,0.009f,0.013f});
    add(lights,{1,0.86f,0.66f},{0.60f,0.43f,0.25f});
    add(garland,{0.11f,0.13f,0.10f},{0.015f,0.02f,0.013f});
    add(city,{0.13f,0.20f,0.26f},{0.025f,0.048f,0.065f},true);
    add(windows,{0.85f,0.68f,0.37f},{0.26f,0.17f,0.07f},true);
    add(gold,{0.94f,0.69f,0.24f},{0.33f,0.19f,0.035f},true);
    add(white,{0.80f,0.83f,0.87f},{0.22f,0.23f,0.25f},true);
    add(water,{0.05f,0.16f,0.25f},{0.012f,0.046f,0.08f},true);
    add(snow,{0.23f,0.29f,0.37f},{0.025f,0.039f,0.065f},true);
    add(expoLines,{0.24f,0.37f,0.48f},{0.035f,0.065f,0.09f},true);
    add(plinths,{0.14f,0.18f,0.23f},{0.025f,0.04f,0.06f},true);
    add(plinthEdges,{0.48f,0.57f,0.64f},{0.10f,0.16f,0.20f},true);
    add(stars,{0.7f,0.79f,0.9f},{0.4f,0.48f,0.6f},true);
    SceneGeometry streaks;
    for(int i=0;i<70;++i) {
        float x=-18+(i*67%997)/997.f*36,y=3.4f+(i*37%503)/503.f*5.5f,z=-3.f-(i%4)*0.6f;
        streaks.beam({x,y,z},{x+0.25f,y-0.055f,z},0.007f,4);
    }
    wind=add(streaks,{0.1f,0.15f,0.20f},{0.025f,0.04f,0.06f},true);
    wind->isArchitecturalLED=true;
    std::vector<float> vertices={-1,-1,0,0,0,1,0,0, 1,-1,0,0,0,1,1,0, 1,1,0,0,0,1,1,1, -1,1,0,0,0,1,0,1};
    screen=std::make_unique<Mesh>(vertices,std::vector<unsigned>{0,1,2,0,2,3});

}
LrtMenu::~LrtMenu() {
    if(depthBuffer) glDeleteRenderbuffers(1,&depthBuffer);
    if(colorBuffer) glDeleteTextures(1,&colorBuffer);
    if(framebuffer) glDeleteFramebuffers(1,&framebuffer);
}
void LrtMenu::update(float time) {
    menuTime=time;
    if(introStart<0) introStart=time;
    introTime=time-introStart;
    float stop=glm::smoothstep(0.f,3.6f,introTime);
    exterior.position.x=-5.f*(1.f-stop);
    if(wind) wind->position.x=std::fmod(time*2.8f,3.f)-1.5f;
    for(size_t i=0;i<handles.size();++i) handles[i]->rotation.z=(0.055f+0.07f*(1.f-stop))*std::sin(time*1.7f+i*0.20f);
    for(size_t i=0;i<skyline.size();++i) skyline[i]->position.x=0.85f*std::sin(time*0.065f)*(0.5f+i*0.3f);
}
void LrtMenu::draw(const Shader& shader,float aspect,int hovered) const {
    glEnable(GL_DEPTH_TEST);
    shader.use();shader.setMat4("uView",view(menuTime));shader.setMat4("uProj",projection(aspect));
    shader.setVec3("uCamPos",{0,4.5f,12});shader.setVec3("uViewPos",{0,4.5f,12});
    shader.setFloat("uFogDensity",0);shader.setVec3("uFogColor",{0.005f,0.012f,0.03f});
    shader.setInt("uAutoSnowEnabled",0);shader.setInt("uLightingEnabled",1);shader.setInt("uSceneIndex",-1);
    shader.setFloat("uShininess",6);
    for (int i=0;i<2;++i) {
        std::string name="uLights["+std::to_string(i)+"]";
        shader.setVec3(name+".position",{i==0 ? -4.f : 4.f,7,6});
        shader.setVec3(name+".color",{1.f,0.95f,0.88f});shader.setFloat(name+".intensity",0.65f);
    }
    for (int i=0;i<3;++i) markers[i]->emissive=introTime<6.2f ? glm::vec3(0) : i==hovered ? glm::vec3(0.90f,0.62f,0.23f) : glm::vec3(0.24f,0.48f,0.61f);
    GLint viewport[4];glGetIntegerv(GL_VIEWPORT,viewport);
    int width=viewport[2],height=viewport[3];
    if(width!=bufferWidth || height!=bufferHeight) {
        if(!framebuffer) glGenFramebuffers(1,&framebuffer);
        if(!colorBuffer) glGenTextures(1,&colorBuffer);
        if(!depthBuffer) glGenRenderbuffers(1,&depthBuffer);
        glBindTexture(GL_TEXTURE_2D,colorBuffer);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glBindRenderbuffer(GL_RENDERBUFFER,depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,width,height);
        glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,colorBuffer,0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthBuffer);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) throw std::runtime_error("Menu framebuffer incomplete");
        bufferWidth=width;bufferHeight=height;
    }
    glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
    glClearColor(0.005f,0.012f,0.026f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    outsideShader.use();outsideShader.setMat4("uView",view(menuTime));outsideShader.setMat4("uProj",projection(aspect));
    outsideShader.setFloat("uIntroTime",introTime);
    exterior.draw(outsideShader,glm::mat4(1));
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glDisable(GL_DEPTH_TEST);
    blurShader.use();blurShader.setInt("uTexture",0);
    blurShader.setVec3("uTexel",{1.f/width,1.f/height,0});
    blurShader.setFloat("uBlur",10.f*(1.f-glm::smoothstep(0.5f,3.8f,introTime)));
    glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,colorBuffer);screen->draw();
    glEnable(GL_DEPTH_TEST);shader.use();
    root.draw(shader,glm::mat4(1),false);
    glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glassShader.use();glassShader.setMat4("uView",view(menuTime));glassShader.setMat4("uProj",projection(aspect));
    if(introTime>6.2f) glassRoot.draw(glassShader,glm::mat4(1),false);
    textShader.use();textShader.setMat4("uView",view(menuTime));textShader.setMat4("uProj",projection(aspect));
    if(introTime>6.2f) textRoot.draw(textShader,glm::mat4(1),false);
    glDepthMask(GL_TRUE);glDisable(GL_BLEND);
}
int LrtMenu::pick(double x,double y,int width,int height) const {
    if (introTime<6.2f || width<=0 || height<=0) return -1;
    auto inverse=glm::inverse(projection(static_cast<float>(width)/height)*view(menuTime));
    glm::vec4 p=inverse*glm::vec4(2*x/width-1,1-2*y/height,1,1);p/=p.w;
    glm::vec3 origin(0.025f*std::sin(menuTime*0.8f),4.5f+0.018f*std::sin(menuTime*1.6f),12),direction=glm::vec3(p)-origin;
    if (std::abs(direction.z)<0.0001f) return -1;
    auto hit=origin+direction*(-origin.z/direction.z);
    if (hit.y<3.0f || hit.y>7.6f) return -1;
    for (int i=0;i<3;++i) if (std::abs(hit.x-(i-1)*5.3f)<2.4f) return i;
    return -1;
}
