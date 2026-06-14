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

struct CameraMatricesUBO {
    Mat4 projection;
    Mat4 view;
};

struct ModelDataUBO {
    Mat4 model;
};

#endif