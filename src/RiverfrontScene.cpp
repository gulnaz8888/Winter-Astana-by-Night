#include "RiverfrontScene.h"
#include "Primitives.h"
#include <array>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace {
constexpr float PI = 3.14159265359f;

struct Batch {
    std::vector<float> vertices;
    std::vector<unsigned> indices;

    void triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c,
                  glm::vec2 ua = {}, glm::vec2 ub = {}, glm::vec2 uc = {}) {
        const glm::vec3 cross = glm::cross(b - a, c - a);
        if (glm::dot(cross, cross) < 1e-10f) return;
        const glm::vec3 n = glm::normalize(cross);
        unsigned first = static_cast<unsigned>(vertices.size() / 8);
        const std::array<glm::vec3,3> ps = {a,b,c};
        const std::array<glm::vec2,3> uvs = {ua,ub,uc};
        for (int i=0; i<3; ++i) {
            const glm::vec3 p=ps[i];
            vertices.insert(vertices.end(), {p.x,p.y,p.z,n.x,n.y,n.z,uvs[i].x,uvs[i].y});
        }
        indices.insert(indices.end(), {first, first + 1, first + 2});
    }

    void quad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d) {
        triangle(a, b, c);
        triangle(a, c, d);
    }

    void texturedQuad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
                      glm::vec2 ua, glm::vec2 ub, glm::vec2 uc, glm::vec2 ud) {
        triangle(a,b,c,ua,ub,uc);
        triangle(a,c,d,ua,uc,ud);
    }

    void box(glm::vec3 p, glm::vec3 size, float yaw = 0.f) {
        const glm::mat3 r(glm::rotate(glm::mat4(1.f), yaw, {0.f, 1.f, 0.f}));
        const glm::vec3 center = p + glm::vec3(0.f, size.y * 0.5f, 0.f);
        const std::array<glm::vec3, 6> ns = {{{0,0,1}, {0,0,-1}, {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}}};
        const std::array<glm::vec3, 6> us = {{{1,0,0}, {-1,0,0}, {0,0,-1}, {0,0,1}, {1,0,0}, {1,0,0}}};
        const std::array<glm::vec3, 6> vs = {{{0,1,0}, {0,1,0}, {0,1,0}, {0,1,0}, {0,0,-1}, {0,0,1}}};
        for (int i = 0; i < 6; ++i) {
            const glm::vec3 c = ns[i] * size * 0.5f;
            const glm::vec3 u = us[i] * size * 0.5f;
            const glm::vec3 v = vs[i] * size * 0.5f;
            quad(center + r * (c-u-v), center + r * (c+u-v),
                 center + r * (c+u+v), center + r * (c-u+v));
        }
    }

    void beam(glm::vec3 a, glm::vec3 b, float radius, int sides = 6) {
        const glm::vec3 axis = glm::normalize(b-a);
        const glm::vec3 reference = std::abs(axis.y) < 0.9f ? glm::vec3(0,1,0) : glm::vec3(1,0,0);
        const glm::vec3 u = glm::normalize(glm::cross(axis, reference));
        const glm::vec3 v = glm::cross(axis, u);
        for (int i = 0; i < sides; ++i) {
            float t = 2.f * PI * i / sides, s = 2.f * PI * (i+1) / sides;
            glm::vec3 r = radius * (u * std::cos(t) + v * std::sin(t));
            glm::vec3 q = radius * (u * std::cos(s) + v * std::sin(s));
            quad(a+r, a+q, b+q, b+r);
            triangle(b, b+r, b+q);
            triangle(a, a+q, a+r);
        }
    }

    void ball(glm::vec3 p, float radius) {
        const glm::vec3 top=p+glm::vec3(0,radius,0), bottom=p-glm::vec3(0,radius,0);
        for (int i=0; i<8; ++i) {
            float a=i*PI/4, b=(i+1)*PI/4;
            glm::vec3 x=p+glm::vec3(std::cos(a)*radius,0,std::sin(a)*radius);
            glm::vec3 y=p+glm::vec3(std::cos(b)*radius,0,std::sin(b)*radius);
            triangle(x,top,y);
            triangle(x,y,bottom);
        }
    }

    void turret(glm::vec3 p, float radius, float height) {
        for (int i = 0; i < 8; ++i) {
            float a = 2.f * PI * i / 8, b = 2.f * PI * (i+1) / 8;
            glm::vec3 x = p + glm::vec3(radius * std::cos(a), 0, radius * std::sin(a));
            glm::vec3 y = p + glm::vec3(radius * std::cos(b), 0, radius * std::sin(b));
            triangle(x, p + glm::vec3(0,height,0), y);
        }
    }
};

struct Material {
    glm::vec3 color;
    glm::vec3 emission;
    Batch geometry;
    const Texture* texture = nullptr;
};

bool lit(int building, int floor, int column) {
    return (building * 17 + floor * 11 + column * 7) % 9 > 2;
}
}

RiverfrontScene::RiverfrontScene() {
    towerPhoto=std::make_unique<Texture>("assets/textures/northern_lights_reference.png");
    iceTexture=std::make_unique<Texture>("assets/textures/ice.png");
    Material photo{{1.35f,1.65f,1.85f},{0.008f,0.02f,0.035f},{},towerPhoto.get()};



    Material ledWhite{
        {0.85f, 0.94f, 1.0f},
        {0.85f, 1.00f, 1.15f},
        {}
    };

    Material ledBlue{
        {0.45f, 0.75f, 1.0f},
        {0.32f, 0.55f, 0.85f},
        {}
    };

    Material ledGold{
        {1.0f, 0.72f, 0.20f},
        {1.10f, 0.65f, 0.12f},
        {}
    };

    Material snow{{0.67f,0.77f,0.90f}, {0.12f,0.16f,0.23f}, {}};
    Material ice{{0.17f,0.36f,0.52f}, {0.035f,0.075f,0.12f}, {}};


    Material iceSurface{
        {0.86f,0.94f,1.0f},
        {0.025f,0.045f,0.065f},
        {},
        iceTexture.get()
    };
    Material iceLines{{0.33f,0.58f,0.72f}, {0.065f,0.12f,0.17f}, {}};
    Material stone{{0.36f,0.40f,0.48f}, {0.045f,0.055f,0.08f}, {}};
    Material rail{{0.075f,0.10f,0.15f}, {0.018f,0.025f,0.04f}, {}};
    Material ochre{{0.64f,0.37f,0.20f}, {0.14f,0.073f,0.035f}, {}};
    Material terracotta{{0.48f,0.22f,0.15f}, {0.11f,0.043f,0.025f}, {}};
    Material cream{{0.78f,0.67f,0.48f}, {0.17f,0.13f,0.075f}, {}};
    Material roof{{0.12f,0.17f,0.23f}, {0.025f,0.035f,0.055f}, {}};
    Material warm{{1.f,0.69f,0.30f}, {0.95f,0.48f,0.13f}, {}};
    Material cool{{0.38f,0.70f,0.89f}, {0.15f,0.34f,0.48f}, {}};
    Material darkWindows{{0.025f,0.07f,0.11f}, {0.008f,0.02f,0.035f}, {}};

    Material evergreen{{0.08f,0.25f,0.20f}, {0.022f,0.075f,0.055f}, {}};
    Material crimson{{0.55f,0.10f,0.12f}, {0.14f,0.025f,0.03f}, {}};
    Material skin{{0.75f,0.53f,0.37f}, {0.13f,0.085f,0.055f}, {}};
    Material gold{{0.78f,0.65f,0.37f}, {0.28f,0.20f,0.075f}, {}};
    Material fairyLights{{1.f,0.89f,0.61f},{1.f,0.72f,0.32f},{}};
    snow.geometry.box({0,-2,-80}, {640,1,410});
    ice.geometry.box({0,-0.8f,-46}, {540,0.35f,123});




    iceSurface.geometry.texturedQuad(
        {-270.f,-0.448f,-107.5f},
        {-270.f,-0.448f,15.5f},
        {270.f,-0.448f,15.5f},
        {270.f,-0.448f,-107.5f},
        {0.f,0.f},
        {0.f,4.f},
        {12.f,4.f},
        {12.f,0.f}
    );
    stone.geometry.box({0,-1,27}, {450,3.5f,31});
    snow.geometry.box({0,2.5f,27}, {450,0.28f,31});
    stone.geometry.box({0,-1,-115}, {490,2.8f,18});
    snow.geometry.box({0,1.8f,-118}, {490,0.5f,24});
    snow.geometry.box({0,-0.4f,11}, {480,0.8f,5});
    snow.geometry.box({0,-0.4f,-104}, {480,0.8f,7});
    for (int i = 0; i < 28; ++i) {
        float z = -96.f + i * 3.9f;
        float x = -210.f + (i % 5) * 22.f;
        iceLines.geometry.box({x,-0.435f,z}, {80.f + (i % 7) * 21.f,0.008f,0.035f}, (i % 3 - 1) * 0.08f);
    }

    for (int i = 0; i < 11; ++i) {
        float y = -0.35f + i * 0.28f, z = 1.5f + i * 0.95f;
        stone.geometry.box({49,y,z}, {15,0.28f,1.05f});
        snow.geometry.box({49,y+0.28f,z}, {15,0.045f,0.92f});
    }

    for (int i = -23; i <= 23; ++i) {
        float x = i * 5.f;
        if (x > 40 && x < 60) continue;
        stone.geometry.box({x,2.78f,12}, {0.75f,2.35f,0.85f});
        snow.geometry.box({x,5.13f,12}, {1.05f,0.15f,1.05f});
        if (i == 23 || (x >= 40 && x < 60)) continue;
        rail.geometry.box({x+2.5f,3.1f,12}, {4.35f,0.14f,0.20f});
        rail.geometry.box({x+2.5f,4.78f,12}, {4.35f,0.16f,0.24f});
        snow.geometry.box({x+2.5f,4.94f,12}, {4.35f,0.055f,0.25f});
        for (int k = 1; k <= 4; ++k) {
            float xx = x + k;
            rail.geometry.box({xx,3.23f,12}, {0.075f,1.52f,0.10f});
        }
        const glm::vec3 c(x+2.5f,3.98f,12);
        rail.geometry.beam(c+glm::vec3(-0.8f,0,0), c+glm::vec3(0,0.59f,0),0.045f);
        rail.geometry.beam(c+glm::vec3(0,0.59f,0), c+glm::vec3(0.8f,0,0),0.045f);
        rail.geometry.beam(c+glm::vec3(0.8f,0,0), c+glm::vec3(0,-0.59f,0),0.045f);
        rail.geometry.beam(c+glm::vec3(0,-0.59f,0), c+glm::vec3(-0.8f,0,0),0.045f);
    }

    const float towers[][4] = {
        {-148,-146,18,40}, {-116,-155,21,86}, {-83,-151,21,65}
    };
    const glm::vec2 photoCorners[3][4] = {
        {{103,193},{138,193},{136,111},{101,109}},
        {{200,190},{249,189},{226,34},{194,31}},
        {{289,192},{327,190},{322,77},{283,85}}
    };
    for (int i=0; i<3; ++i) {
        float x=towers[i][0], z=towers[i][1], w=towers[i][2], h=towers[i][3];
        int floors=static_cast<int>(h/2.6f);
        auto center=[&](float t) {
            return x+2.3f*std::sin(t*PI*1.65f+i*0.45f)-1.1f*std::sin(i*0.45f);
        };
        auto width=[&](float t) { return w*(1.f-0.10f*t)+0.65f*std::sin(t*PI); };
        auto uv=[&](float horizontal,float vertical) {
            glm::vec2 bottom=glm::mix(photoCorners[i][0],photoCorners[i][1],horizontal);
            glm::vec2 top=glm::mix(photoCorners[i][3],photoCorners[i][2],horizontal);
            glm::vec2 pixel=glm::mix(bottom,top,vertical);
            return glm::vec2(pixel.x/400.f,1.f-pixel.y/300.f);
        };
        for (int floor=0; floor<floors; ++floor) {
            float t0=static_cast<float>(floor)/floors, t1=static_cast<float>(floor+1)/floors;
            float y0=2+h*t0, y1=2+h*t1;
            float x0=center(t0), x1=center(t1), w0=width(t0), w1=width(t1);
            photo.geometry.texturedQuad({x0-w0/2,y0,z+8},{x0+w0/2,y0,z+8},
                {x1+w1/2,y1,z+8},{x1-w1/2,y1,z+8},uv(0,t0),uv(1,t0),uv(1,t1),uv(0,t1));
            photo.geometry.texturedQuad({x0+w0/2,y0,z-8},{x0-w0/2,y0,z-8},
                {x1-w1/2,y1,z-8},{x1+w1/2,y1,z-8},uv(0,t0),uv(1,t0),uv(1,t1),uv(0,t1));
            photo.geometry.texturedQuad({x0+w0/2,y0,z+8},{x0+w0/2,y0,z-8},
                {x1+w1/2,y1,z-8},{x1+w1/2,y1,z+8},uv(0.65f,t0),uv(0.95f,t0),uv(0.95f,t1),uv(0.65f,t1));
            photo.geometry.texturedQuad({x0-w0/2,y0,z-8},{x0-w0/2,y0,z+8},
                {x1-w1/2,y1,z+8},{x1-w1/2,y1,z-8},uv(0.05f,t0),uv(0.35f,t0),uv(0.35f,t1),uv(0.05f,t1));

            for (int col=0; col<7; ++col) {
                if ((floor*13+col*7+i)%29 != 0) continue;
                float tm=(t0+t1)*0.5f, f=(col+0.5f)/7.f-0.5f;
                warm.geometry.box({center(tm)+width(tm)*f,y0+0.35f,z+8.13f},
                                  {0.55f,(y1-y0)*0.5f,0.04f});
            }
        }



        constexpr float offset = 0.25f;
        constexpr float halfStrip = 0.14f;
        constexpr int columns = 10;

        auto facePoint = [&](int face, float t, float s) -> glm::vec3 {
            const float cx = center(t);
            const float hw = width(t) * 0.5f;
            const float y = 2.0f + h * t;

            if (face == 0) return {cx + hw * s, y, z + 8.0f + offset};
            if (face == 1) return {cx - hw * s, y, z - 8.0f - offset};
            if (face == 2) return {cx + hw + offset, y, z - 8.0f * s};
            return {cx - hw - offset, y, z + 8.0f * s};
        };

        for (int face = 0; face < 4; ++face) {

            for (int col = 0; col <= columns; ++col) {
                const float s = -0.90f + col * (1.80f / columns);

                const float delta = (face < 2) ? (halfStrip * 2.0f / w)
                                               : (halfStrip / 8.0f);
                for (int floor = 0; floor < floors; ++floor) {
                    const float t0 = float(floor) / floors;
                    const float t1 = float(floor + 1) / floors;
                    const auto a = facePoint(face, t0, s - delta);
                    const auto b = facePoint(face, t0, s + delta);
                    const auto c = facePoint(face, t1, s + delta);
                    const auto d = facePoint(face, t1, s - delta);

                    Material& led = col % 4 == 0 ? ledBlue : ledWhite;
                    led.geometry.quad(a, b, c, d);
                }
            }

            for (int floor = 0; floor <= floors; floor += 2) {
                const float t = float(floor) / floors;
                const float dy = 0.065f / h;
                ledBlue.geometry.quad(
                    facePoint(face, t - dy, -0.94f),
                    facePoint(face, t - dy,  0.94f),
                    facePoint(face, t + dy,  0.94f),
                    facePoint(face, t + dy, -0.94f)
                );
            }




            if (face == 0) {
                const float goldPositions[] = { -0.12f, 0.12f };
                const float goldHalfWidth = 0.035f;

                for (float s : goldPositions) {
                    for (int segment = 0; segment < 35; ++segment) {
                        float t0 = 0.10f + 0.70f * segment / 35.0f;
                        float t1 = 0.10f + 0.70f * (segment + 1) / 35.0f;

                        glm::vec3 a = facePoint(face, t0, s - goldHalfWidth);
                        glm::vec3 b = facePoint(face, t0, s + goldHalfWidth);
                        glm::vec3 c = facePoint(face, t1, s + goldHalfWidth);
                        glm::vec3 d = facePoint(face, t1, s - goldHalfWidth);

                        a.z += 0.04f;
                        b.z += 0.04f;
                        c.z += 0.04f;
                        d.z += 0.04f;

                        ledGold.geometry.quad(a, b, c, d);
                    }
                }
            }


        }

        snow.geometry.box({center(1),h+2,z},{width(1)+0.25f,0.25f,16.25f});
    }

    for (int i=0; i<7; ++i) {
        float x=39.f+i*23.f, z=-146.f-(i%2)*5.f, w=22.f;
        float h=26.f+(i%3)*7.f;
        Material& facade = i%2 ? terracotta : ochre;
        facade.geometry.box({x,2,z},{w,h,20});
        cream.geometry.box({x,2,z+0.3f},{w+1,2,21});
        for (float y : {8.f,h+1.f}) {
            cream.geometry.box({x,y,z},{w+1.4f,0.65f,21.4f});
            snow.geometry.box({x,y+0.65f,z},{w+1.5f,0.12f,21.5f});
        }
        roof.geometry.box({x,h+2,z},{w+1,3.5f,21});
        roof.geometry.turret({x,h+5.5f,z},w*0.55f,4.f);
        for (float side : {-1.f,1.f}) {
            cream.geometry.box({x+side*(w*0.5f-0.6f),4,z+10.1f},{0.8f,h-2,0.6f});
        }
        for (int floor=0; floor<static_cast<int>((h-4)/3.4f); ++floor) {
            float y=5.f+floor*3.4f;
            for (int col=0; col<6; ++col) {
                float xx=x-w*0.5f+2.1f+col*3.55f;
                cream.geometry.box({xx,y-0.18f,z+10.18f},{1.8f,2.55f,0.30f});
                Material& m=lit(i+10,floor,col) ? warm : darkWindows;
                m.geometry.box({xx,y,z+10.36f},{1.23f,2.1f,0.08f});
                cream.geometry.box({xx,y+0.98f,z+10.43f},{1.25f,0.09f,0.06f});
            }
        }
        if (i%2 == 0) {
            for (float side : {-1.f,1.f}) {
                float xx=x+side*(w*0.5f-1.7f), zz=z+7;
                cream.geometry.box({xx,h+2,zz},{4.6f,4.2f,4.6f});
                warm.geometry.box({xx,h+3,zz+2.34f},{1.25f,1.8f,0.08f});
                roof.geometry.turret({xx,h+6.2f,zz},3.65f,5.5f);
                rail.geometry.beam({xx,h+11.7f,zz},{xx,h+14.f,zz},0.10f);
                snow.geometry.box({xx,h+6.05f,zz},{4.8f,0.18f,4.8f});
            }
        }
    }

    for (int i=0; i<9; ++i) {
        float x=-58.f+i*10.f, z=-170.f-(i%3)*7.f, h=6.f+(i%4)*2.2f;
        stone.geometry.box({x,2,z},{9,h,11});
        snow.geometry.box({x,h+2,z},{9.3f,0.28f,11.3f});
        for (int j=0; j<4; ++j) {
            warm.geometry.box({x-3+j*2.f,5,z+5.55f},{0.8f,1.4f,0.08f});
        }
    }

    for (int bank=0; bank<2; ++bank) {
        for (int i=-10; i<=10; ++i) {
            float x=i*22.f, z=bank ? -112.f : 18.f;
            float y=bank ? 2.3f : 2.78f;
            if (!bank && x>40 && x<60) continue;
            rail.geometry.box({x,y,z},{0.8f,0.45f,0.8f});
            rail.geometry.beam({x,y,z},{x,y+7,z},0.13f);
            rail.geometry.box({x,y+6.7f,z},{0.9f,0.18f,0.9f});
            warm.geometry.box({x,y+6.88f,z},{0.58f,1.05f,0.58f});
            roof.geometry.turret({x,y+7.93f,z},0.75f,0.65f);
            snow.geometry.box({x,y+8.5f,z},{0.24f,0.10f,0.24f});
        }
    }

    for (int bank=0; bank<2; ++bank) {
        float z=bank ? -112.f : 18.f;
        float anchorY=(bank ? 2.3f : 2.78f)+6.7f;
        for (int span=-10; span<10; ++span) {
            float x=span*22.f;
            if (!bank && ((x>40 && x<60) || (x+22>40 && x+22<60))) continue;
            auto point=[&](float t) {
                return glm::vec3(x+22.f*t,anchorY-1.35f*std::sin(PI*t),z);
            };
            for (int k=0; k<16; ++k) {
                rail.geometry.beam(point(k/16.f),point((k+1)/16.f),0.027f,4);
            }
            for (int k=1; k<12; ++k) {
                glm::vec3 wire=point(k/12.f);
                rail.geometry.beam(wire,wire-glm::vec3(0,0.20f,0),0.025f,4);
                fairyLights.geometry.ball(wire-glm::vec3(0,0.30f,0),0.15f);
            }
        }
    }

    for (int i=0; i<15; ++i) {
        float x=-200.f+i*29.f, z=-128.f;
        float h=5.5f+(i%3);
        rail.geometry.beam({x,2.3f,z},{x,h+2.3f,z},0.20f);
        for (int k=0; k<4; ++k) {
            float a=k*PI*0.5f+i, y=4.f+(k%2)*1.5f;
            glm::vec3 a0(x,y,z), b(x+std::cos(a)*2.8f,y+2.6f,z+std::sin(a)*2.8f);
            snow.geometry.beam(a0,b,0.10f);
        }
    }

    for (int i=0; i<5; ++i) {
        float x=-37.f+i*17.f, z=-115.f, y=2.3f;
        Material& paint=i%2 ? evergreen : crimson;
        paint.geometry.box({x,y,z},{8,3.4f,5.5f});
        darkWindows.geometry.box({x,y+1.1f,z+2.80f},{6.4f,1.85f,0.08f});
        cream.geometry.box({x,y+1.05f,z+3.0f},{7.1f,0.22f,1.0f});
        for (float side : {-1.f,1.f}) {
            cream.geometry.box({x+side*3.5f,y,z+2.85f},{0.22f,3.8f,0.25f});
        }
        float eave=y+3.65f, peak=y+6.0f;
        snow.geometry.quad({x-4.7f,eave,z+3.4f},{x,peak,z+3.4f},
                           {x,peak,z-3.4f},{x-4.7f,eave,z-3.4f});
        snow.geometry.quad({x,peak,z+3.4f},{x+4.7f,eave,z+3.4f},
                           {x+4.7f,eave,z-3.4f},{x,peak,z-3.4f});
        paint.geometry.triangle({x-4,eave,z+2.8f},{x+4,eave,z+2.8f},{x,peak,z+2.8f});
        snow.geometry.beam({x-4.8f,eave+0.08f,z+3.4f},{x+4.8f,eave+0.08f,z+3.4f},0.14f);
        snow.geometry.beam({x,peak+0.08f,z-3.4f},{x,peak+0.08f,z+3.4f},0.16f);
        for (int k=0; k<9; ++k) {
            float xx=x-3.8f+k*0.95f;
            warm.geometry.ball({xx,eave-0.08f-0.3f*std::sin(k*PI/8),z+3.5f},0.13f);
        }
        for (int k=0; k<4; ++k) {
            Material& goods=k%2 ? gold : evergreen;
            goods.geometry.box({x-2.2f+k*1.45f,y+1.27f,z+2.9f},{0.45f,0.45f,0.45f});
        }
    }

    auto person=[&](glm::vec3 p, float h, int index, bool skating) {
        Material& coat=index%3 == 0 ? crimson : (index%3 == 1 ? evergreen : ochre);
        float stride=skating ? h*0.17f : h*0.085f;
        for (float side : {-1.f,1.f}) {
            rail.geometry.beam(p+glm::vec3(side*stride,0.10f,side*stride*0.4f),
                               p+glm::vec3(side*h*0.07f,h*0.43f,0),h*0.048f);
            rail.geometry.box(p+glm::vec3(side*stride,0,side*stride*0.4f+0.05f),
                              {h*0.12f,h*0.08f,h*0.21f});
            glm::vec3 shoulder=p+glm::vec3(side*h*0.16f,h*0.71f,0);
            glm::vec3 hand=p+glm::vec3(side*h*(skating ? 0.34f : 0.21f),h*(skating ? 0.55f : 0.43f),0.05f);
            coat.geometry.beam(shoulder,hand,h*0.055f);
        }
        coat.geometry.box(p+glm::vec3(0,h*0.34f,0),{h*0.34f,h*0.42f,h*0.23f});
        skin.geometry.ball(p+glm::vec3(0,h*0.86f,0),h*0.10f);
        cream.geometry.box(p+glm::vec3(0,h*0.74f,0),{h*0.29f,h*0.07f,h*0.26f});
        coat.geometry.ball(p+glm::vec3(0,h*0.94f,-0.015f),h*0.115f);
        snow.geometry.ball(p+glm::vec3(0,h*1.04f,0),h*0.045f);
    };
    for (int i=0; i<12; ++i) {
        person({-43.f+i*8.f,2.3f,-108.2f+(i%3)*0.45f},i%4 == 0 ? 2.6f : 3.6f,i,false);
    }
    const glm::vec3 skaters[] = {
        {-45,-0.44f,-48},{-41,-0.44f,-51},{-14,-0.44f,-29},
        {-10,-0.44f,-31},{20,-0.44f,-62},{24,-0.44f,-60},
        {40,-0.44f,-35},{43,-0.44f,-37},{-69,-0.44f,-72}
    };
    for (int i=0; i<9; ++i) person(skaters[i],i%3 == 0 ? 2.1f : 3.0f,i,true);

    for (Material* m : { &snow,&ice,&iceSurface,&iceLines,&stone,&rail,&photo,
                    &ledWhite,&ledBlue,&ledGold,
                   &ochre,&terracotta,&cream,&roof,&warm,&cool,&darkWindows,
                   &evergreen,&crimson,&skin,&gold,&fairyLights }) {
        if (m->geometry.indices.empty()) continue;
        meshes.push_back(std::make_unique<Mesh>(m->geometry.vertices,m->geometry.indices));
        auto node=std::make_unique<Node>();
        node->mesh=meshes.back().get();
        node->texture=m->texture;
        node->color=m->color;
        node->emissive=m->emission;
        node->isArchitecturalLED =
            (m == &ledWhite || m == &ledBlue || m == &ledGold);
        root.addChild(std::move(node));
    }

    meshes.push_back(std::make_unique<Mesh>(Primitives::sphere(12,20)));
    auto moon=std::make_unique<Node>();
    moon->mesh=meshes.back().get();
    moon->position={-24,93,-210};
    moon->scale={9,9,9};
    moon->color={0.72f,0.82f,1.f};
    moon->emissive={0.65f,0.77f,0.98f};
    root.addChild(std::move(moon));
}

void RiverfrontScene::draw(const Shader& shader) const {
    root.draw(shader,glm::mat4(1.f));
}
