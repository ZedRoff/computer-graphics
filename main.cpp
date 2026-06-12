#include <iostream>
#include <vector>
#include <GL/glew.h>  
#include <GLFW/glfw3.h>

#include "Maths.h"
#include "Structures.h"
#include "Utils.h"
#include "Navigation.h"

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

    shaderProgram = CreateShaderFromFiles("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    for (size_t i = 0; i < materials.size(); i++)
    {
        if (!materials[i].texturePath.empty())
        {
            materials[i].textureID = LoadTexture(materials[i].texturePath);
        }
    }

    float fov = M_PI / 4.0f;
    float aspect = 800.0f / 600.0f;
    float zNear = 0.1f;
    float cameraDistance = (objMaxDim * 0.5f) / std::tan(fov * 0.5f) * 1.2f;
    float zFar  = cameraDistance + objMaxDim * 2.0f;

    Camera camera;
    camera.radius = cameraDistance; 
    camera.yaw = 0.0f;
    camera.pitch = 0.0f;
    camera.isMousePressed = false;

    SetupScrollCallback(window, camera);

    Mat4 projMatrix  = Perspective(fov, aspect, zNear, zFar);
    Mat4 modelMatrix = Translate(-objCenter.x, -objCenter.y, -objCenter.z);    

    float dummyX, dummyY, dummyZ;


    Vec3 lightPos(5.0f, 10.0f, 5.0f);
    Vec3 lightColor(1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glfwPollEvents();

        UpdateCameraFromInputs(window, camera, dummyX, dummyY, dummyZ);

        Mat4 viewMatrix = ComputeViewMatrix(camera);
        Mat4 mvpMatrix  = Multiply(projMatrix, Multiply(viewMatrix, modelMatrix));

        unsigned int mvpLoc = glGetUniformLocation(shaderProgram, "u_MVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvpMatrix.m[0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_Model"), 1, GL_FALSE, &modelMatrix.m[0]);

        Vec3 cameraPos;
        cameraPos.x = camera.radius * std::cos(camera.pitch) * std::cos(camera.yaw);
        cameraPos.y = camera.radius * std::sin(camera.pitch);
        cameraPos.z = camera.radius * std::cos(camera.pitch) * std::sin(camera.yaw);

        glUniform3f(glGetUniformLocation(shaderProgram, "u_lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "u_viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "u_lightColor"), lightColor.x, lightColor.y, lightColor.z);
    
        glUniform1i(glGetUniformLocation(shaderProgram, "u_Texture"), 0);
        for (size_t i = 0; i < meshes.size(); i++) {
            int matID = meshes[i].materialId;

            int ambientLoc = glGetUniformLocation(shaderProgram, "u_ambientColor");
            int diffuseLoc = glGetUniformLocation(shaderProgram, "u_diffuseColor");
            int specularLoc = glGetUniformLocation(shaderProgram, "u_specularColor");
            int shininessLoc = glGetUniformLocation(shaderProgram, "u_shininess");
            int hasTexLoc = glGetUniformLocation(shaderProgram, "u_hasTexture");

            glActiveTexture(GL_TEXTURE0);


            if (matID >= 0 && matID < materials.size()) {

                glUniform3f(ambientLoc, materials[matID].ambient.x, materials[matID].ambient.y, materials[matID].ambient.z);
                glUniform3f(diffuseLoc, materials[matID].diffuse.x, materials[matID].diffuse.y, materials[matID].diffuse.z);
                glUniform3f(specularLoc, materials[matID].specular.x, materials[matID].specular.y, materials[matID].specular.z);
                glUniform1f(shininessLoc, materials[matID].shininess);

                if (materials[matID].textureID != 0) {
                    glUniform1i(hasTexLoc, 1);
                    glBindTexture(GL_TEXTURE_2D, materials[matID].textureID);
                } else {
                    glUniform1i(hasTexLoc, 0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            } else {
                glUniform3f(ambientLoc, 0.1f, 0.1f, 0.1f);
                glUniform3f(diffuseLoc, 0.6f, 0.6f, 0.6f);
                glUniform3f(specularLoc, 0.3f, 0.3f, 0.3f);
                glUniform1f(shininessLoc, 32.0f);
                glUniform1i(hasTexLoc, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glBindVertexArray(meshes[i].VAO);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(meshes[i].vertices.size()));
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glfwSwapBuffers(window);
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}