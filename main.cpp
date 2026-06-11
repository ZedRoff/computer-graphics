#include <iostream>
#include <vector>
#include <string>
#include <GL/glew.h>  
#include <GLFW/glfw3.h>
#include "tiny_obj_loader.h"
#include <glm/glm.hpp>


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct Mesh {
    std::vector<Vertex> vertices;
    int materialId; 
    
    unsigned int VAO, VBO; 
};




bool LoadOBJ(const std::string& filename, std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials) {
    
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./";
    reader_config.triangulate = true;        

    tinyobj::ObjReader reader;
    
    if (!reader.ParseFromFile(filename, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "Erreur : " << reader.Error() << std::endl;
        }
        return false;
    }

    if (!reader.Warning().empty()) {
        std::cout << "Warning : " << reader.Warning() << std::endl;
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    for (size_t i = 0; i < materials.size(); i++) {
        Material mat;
        mat.ambient  = glm::vec3(materials[i].ambient[0],  materials[i].ambient[1],  materials[i].ambient[2]);
        mat.diffuse  = glm::vec3(materials[i].diffuse[0],  materials[i].diffuse[1],  materials[i].diffuse[2]);
        mat.specular = glm::vec3(materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
        mat.shininess = materials[i].shininess;
        
        outMaterials.push_back(mat);
    }

    for (size_t s = 0; s < shapes.size(); s++) {
        Mesh mesh;
        mesh.materialId = -1;

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
                    vertex.normal = glm::vec3(0.0f);
                }

                if (idx.texcoord_index >= 0) {
                    vertex.texCoords.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    vertex.texCoords.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                } else {
                    vertex.texCoords = glm::vec2(0.0f);
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
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(glm::vec3)));

        glBindVertexArray(0);
    }
}


int main(int argc, char* argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

    GLFWwindow* window = glfwCreateWindow(800, 600, "Projet OpenGL - Justine et Aman - E4FI - 1I", nullptr, nullptr);
    
    glfwMakeContextCurrent(window);
    glewInit();
    glewExperimental = GL_TRUE;

    std::vector<Mesh> meshes;
    std::vector<Material> materials;

    std::cout << "Chargement .obj : " << std::endl;
    if (LoadOBJ("hand.obj", meshes, materials)) {
        std::cout << ".obj chargé" << std::endl;
        SetupMeshBuffers(meshes);
    } else {
        std::cerr << "Erreur" << std::endl;
    }

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}