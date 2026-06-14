#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <vector>
#include "Maths.h"

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 texCoords;
};

struct Material {
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
    float shininess = 0.0f;

    std::string texturePath;
    unsigned int textureID = 0;
};

struct Mesh {
    std::vector<Vertex> vertices;
    int materialId = -1; 
    unsigned int VAO = 0, VBO = 0; 
};

struct ViewProj {
    Mat4 viewMatrix;
    Mat4 projectionMatrix;
    uint32_t UBO{ 0 }; 
};

const uint32_t cameraBindingPoint = 0;

#endif