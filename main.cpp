#include <iostream>
#include <vector>
#include <GL/glew.h>  
#include <GLFW/glfw3.h>

#include "Maths.h"
#include "Structures.h"
#include "Utils.h"

std::vector<Mesh> meshes;
std::vector<Material> materials;
unsigned int shaderProgram;

int main(int argc, char* argv[]) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

    GLFWwindow* window = glfwCreateWindow(800, 600, "Projet OpenGL - Justine et Aman - E4FI - 1I", nullptr, nullptr);
    
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);

    LoadOBJ("./3Dobjects/GoingMerry/GoingMerry.obj", meshes, materials);
    SetupMeshBuffers(meshes);

    Vec3 objCenter(0.0f, 0.0f, 0.0f);
    float objMaxDim = 0.0f;
    ComputeBoundingBox(meshes, objCenter, objMaxDim);

    std::cout << "--- AUTO FRAMING ---" << std::endl;
    std::cout << "Centre de l'objet : (" << objCenter.x << ", " << objCenter.y << ", " << objCenter.z << ")" << std::endl;
    std::cout << "Dimension maximale : " << objMaxDim << std::endl;

    shaderProgram = CreateShaderFromFiles("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    // BOUCLE AUTOMATIQUE DE CHARGEMENT DES PNG
    for (size_t i = 0; i < materials.size(); i++)
    {
        // Si tiny_obj_loader a trouvé un nom de fichier dans "map_kd"
        if (!materials[i].texturePath.empty())
        {
            // On appelle notre fonction pour l'envoyer à la carte graphique
            materials[i].textureID = LoadTexture(materials[i].texturePath);
        }
    }

    float fov = M_PI / 4.0f;
    float aspect = 800.0f / 600.0f;
    float cameraDistance = (objMaxDim * 0.5f) / std::tan(fov * 0.5f) * 1.2f;
    Mat4 modelMatrix = Translate(-objCenter.x, -objCenter.y, -objCenter.z);    
    Mat4 viewMatrix = Translate(0.0f, 0.0f, -cameraDistance);
    float zNear = 0.1f;
    float zFar  = cameraDistance + objMaxDim * 2.0f;
    Mat4 projMatrix  = Perspective(fov, aspect, zNear, zFar);
    
    Mat4 mvpMatrix = Multiply(projMatrix, Multiply(viewMatrix, modelMatrix));

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        unsigned int mvpLoc = glGetUniformLocation(shaderProgram, "u_MVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvpMatrix.m[0]);

        int vertexColorLocation = glGetUniformLocation(shaderProgram, "u_diffuseColor");
        int useTextureLocation = glGetUniformLocation(shaderProgram, "u_Texture");
        glUniform1i(useTextureLocation, 0);

        for (size_t i = 0; i < meshes.size(); i++) {
            int matID = meshes[i].materialId;
            glActiveTexture(GL_TEXTURE0);
            if (matID >= 0 && matID < materials.size()) {
                glUniform3f(vertexColorLocation, materials[matID].diffuse.x, materials[matID].diffuse.y, materials[matID].diffuse.z);
                if (materials[matID].textureID != 0) {
                    glBindTexture(GL_TEXTURE_2D, materials[matID].textureID);
                } else {
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            } else {
                // Couleur grise de secours si la pièce n'a pas de matériau défini
                glUniform3f(vertexColorLocation, 0.6f, 0.6f, 0.6f);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glBindVertexArray(meshes[i].VAO);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(meshes[i].vertices.size()));
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}