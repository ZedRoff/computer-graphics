#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string LoadShaderSource(const std::string& filePath) {
    std::ifstream shaderFile(filePath);
    
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    return shaderStream.str();
}

unsigned int CreateShaderFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCodeStr = LoadShaderSource(vertexPath);
    std::string fragmentCodeStr = LoadShaderSource(fragmentPath);

    const char* vertexSource = vertexCodeStr.c_str();
    const char* fragmentSource = fragmentCodeStr.c_str();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);


    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    
   

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

bool LoadOBJ(const std::string& filename, std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials) {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./3Dobjects/GoingMerry/";
    reader_config.triangulate = true;        

    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(filename, reader_config)) {
        return false;
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    for (size_t i = 0; i < materials.size(); i++) {
        Material mat;
        mat.ambient  = Vec3(materials[i].ambient[0],  materials[i].ambient[1],  materials[i].ambient[2]);
        mat.diffuse  = Vec3(materials[i].diffuse[0],  materials[i].diffuse[1],  materials[i].diffuse[2]);
        mat.specular = Vec3(materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
        mat.shininess = materials[i].shininess;
        if (!materials[i].diffuse_texname.empty()) {
        mat.texturePath = "./3Dobjects/GoingMerry/textures/" + materials[i].diffuse_texname;
        } else {
            mat.texturePath = "";
        }
        mat.textureID = 0;
        outMaterials.push_back(mat);
    }

    for (size_t s = 0; s < shapes.size(); s++) {
        Mesh mesh;
        size_t index_offset = 0;
        
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]); 

            if (!shapes[s].mesh.material_ids.empty()) {
                mesh.materialId = shapes[s].mesh.material_ids[f];
            }

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                Vertex vertex;
                vertex.position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                vertex.position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                vertex.position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                if (idx.normal_index >= 0) {
                    vertex.normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    vertex.normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    vertex.normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
                } else {
                    vertex.normal = Vec3(0.0f, 1.0f, 0.0f); 
                }

                if (idx.texcoord_index >= 0) {
                    vertex.texCoords.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    vertex.texCoords.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                } else {
                    vertex.texCoords = Vec2{0.0f, 0.0f};
                }

                mesh.vertices.push_back(vertex);
            }
            index_offset += fv;
        }
        outMeshes.push_back(mesh);
    }
    return true;
}

void SetupMeshBuffers(std::vector<Mesh>& meshes) {
    for (size_t i = 0; i < meshes.size(); i++) {
        glGenVertexArrays(1, &meshes[i].VAO);
        glGenBuffers(1, &meshes[i].VBO);

        glBindVertexArray(meshes[i].VAO);
        glBindBuffer(GL_ARRAY_BUFFER, meshes[i].VBO);
        
        glBufferData(GL_ARRAY_BUFFER, meshes[i].vertices.size() * sizeof(Vertex), &meshes[i].vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vec3));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vec3)));

        glBindVertexArray(0);
    }
}

//Méthode vue en cours de Maths3D avec Kacper Pluta
void ComputeBoundingBox(const std::vector<Mesh>& meshes, Vec3& outCenter, float& outMaxDim) {
    if (meshes.empty()) return;

    // On initialise les mins au maximum possible et les maxs au minimum possible
    float minX = 1e9f, minY = 1e9f, minZ = 1e9f;
    float maxX = -1e9f, maxY = -1e9f, maxZ = -1e9f;

    // On parcourt TOUS les sommets de TOUS les meshes
    for (const auto& mesh : meshes) {
        for (const auto& vertex : mesh.vertices) {
            if (vertex.position.x < minX) minX = vertex.position.x;
            if (vertex.position.y < minY) minY = vertex.position.y;
            if (vertex.position.z < minZ) minZ = vertex.position.z;

            if (vertex.position.x > maxX) maxX = vertex.position.x;
            if (vertex.position.y > maxY) maxY = vertex.position.y;
            if (vertex.position.z > maxZ) maxZ = vertex.position.z;
        }
    }

    // 1. Calcul du centre exact de l'objet
    outCenter.x = (minX + maxX) * 0.5f;
    outCenter.y = (minY + maxY) * 0.5f;
    outCenter.z = (minZ + maxZ) * 0.5f;

    // 2. Calcul des dimensions sur chaque axe
    float sizeX = maxX - minX;
    float sizeY = maxY - minY;
    float sizeZ = maxZ - minZ;

    // On garde la dimension la plus grande pour adapter la caméra
    outMaxDim = std::max({sizeX, sizeY, sizeZ});
}

//Fonction du TP
unsigned int LoadTexture(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);
    unsigned int texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    int w, h, comp;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &comp, STBI_rgb_alpha);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    return texID;
}