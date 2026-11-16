#pragma once
#include <memory>
#include <vector>
#include "Node.h"
#include "SceneGeometry.h"

class LrtMenu {
public:
    LrtMenu();
    ~LrtMenu();
    void update(float time);
    void draw(const Shader& shader,float aspect,int hovered) const;
    int pick(double x,double y,int width,int height) const;
private:
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::vector<std::unique_ptr<Texture>> labels;
    Shader textShader;
    Shader glassShader;
    Shader outsideShader;
    Shader blurShader;
    std::unique_ptr<Mesh> screen;
    mutable unsigned framebuffer=0, colorBuffer=0, depthBuffer=0;
    mutable int bufferWidth=0,bufferHeight=0;
    float introStart=-1;
    float introTime=0;
    Node exterior;
    Node* wind=nullptr;
    float menuTime=0;
    std::vector<Node*> handles;
    std::vector<Node*> skyline;
    Node root;
    Node textRoot;
    Node glassRoot;
    Node* markers[3]{};
    Node* add(const SceneGeometry& geometry,glm::vec3 color,glm::vec3 glow,bool outside=false);
};
