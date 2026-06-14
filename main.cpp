#include <iostream>
#include <vector>
#include <GL/glew.h>  
#include <GLFW/glfw3.h>
#include <cmath>

#include "Maths.h"
#include "Structures.h"
#include "Utils.h"
#include "common/GLShader.h"
#include "Navigation.h"

ViewProj g_Camera;

struct ObjectData {
    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    Mat4 modelMatrix; 
    float cameraDistance; 
    float zFar; 
    GLShader shader; 
};
std::vector<ObjectData> sceneObjects;
int currentObjectIndex = 0; 

bool rightArrowPressedLastFrame = false;
bool leftArrowPressedLastFrame = false;

void AddObjectToScene(const std::string& objPath, 
                      const std::string& mtlDir, 
                      const std::string& vertPath, 
                      const std::string& fragPath, 
                      float fov) 
{
    ObjectData obj;

    LoadOBJ(objPath, obj.meshes, obj.materials, mtlDir);
    SetupMeshBuffers(obj.meshes);

    obj.shader.LoadVertexShader(vertPath.c_str());
    obj.shader.LoadFragmentShader(fragPath.c_str());
    obj.shader.Create(); 

    uint32_t programID = obj.shader.GetProgram();
    auto blockIndex = glGetUniformBlockIndex(programID, "ViewProj"); 
    if (blockIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(programID, blockIndex, cameraBindingPoint); 
    }
    
    Vec3 center(0.0f, 0.0f, 0.0f);
    float maxDim = 0.0f;
    ComputeBoundingBox(obj.meshes, center, maxDim);
    
    for (size_t i = 0; i < obj.materials.size(); i++) {
        if (!obj.materials[i].texturePath.empty()) {
            obj.materials[i].textureID = LoadTexture(obj.materials[i].texturePath);
        }
    }

    obj.cameraDistance = (maxDim * 0.5f) / std::tan(fov * 0.5f) * 1.2f;
    obj.zFar = obj.cameraDistance + maxDim * 2.0f; 
    obj.modelMatrix = Translate(-center.x, -center.y, -center.z);

    sceneObjects.push_back(obj);
}

int main(int argc, char* argv[]) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

    GLFWwindow* window = glfwCreateWindow(800, 600, "Projet OpenGL - Justine et Aman - E4FI - 1I", nullptr, nullptr);
    
    glfwMakeContextCurrent(window);
    
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);

    float fov = M_PI / 4.0f;
    float aspect = 800.0f / 600.0f;
    float zNear = 0.1f;

    glGenBuffers(1, &g_Camera.UBO); 
    glBindBuffer(GL_UNIFORM_BUFFER, g_Camera.UBO); 
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Mat4) * 2, nullptr, GL_STREAM_DRAW); 
    glBindBufferBase(GL_UNIFORM_BUFFER, cameraBindingPoint, g_Camera.UBO); 
    glBindBuffer(GL_UNIFORM_BUFFER, 0); 

    AddObjectToScene("./3Dobjects/GoingMerry/GoingMerry.obj", 
                     "./3Dobjects/GoingMerry/", 
                     "shaders/GoingMerry/basic.vs.glsl", 
                     "shaders/GoingMerry/basic.fs.glsl", 
                     fov);
    AddObjectToScene("./3Dobjects/laboon-one-piece/Laboon_One_Piece.obj", 
                     "./3Dobjects/laboon-one-piece/", 
                     "shaders/laboon-one-piece/basic.vs.glsl", 
                     "shaders/laboon-one-piece/basic.fs.glsl", 
                     fov);

     AddObjectToScene("./3Dobjects/one-piece-kuzan/Aokiji.obj", 
                     "./3Dobjects/one-piece-kuzan/", 
                     "shaders/one-piece-kuzan/basic.vs.glsl", 
                     "shaders/one-piece-kuzan/basic.fs.glsl", 
                     fov);

    Vec3 lightPos(5.0f, 10.0f, 5.0f);
    Vec3 lightColor(1.0f, 1.0f, 1.0f);

    int lastObjectIndex = -1; 
    radius = sceneObjects[0].cameraDistance;

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            if (!rightArrowPressedLastFrame) {
                currentObjectIndex = (currentObjectIndex + 1) % sceneObjects.size();
                radius = sceneObjects[currentObjectIndex].cameraDistance; 
                rightArrowPressedLastFrame = true;
            }
        } else {
            rightArrowPressedLastFrame = false;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            if (!leftArrowPressedLastFrame) {
                currentObjectIndex = (currentObjectIndex - 1 + sceneObjects.size()) % sceneObjects.size();
                radius = sceneObjects[currentObjectIndex].cameraDistance; 
                leftArrowPressedLastFrame = true;
            }
        } else {
            leftArrowPressedLastFrame = false;
        }

        ObjectData& currentObj = sceneObjects[currentObjectIndex];

        if (currentObjectIndex != lastObjectIndex) {
            g_Camera.projectionMatrix = Perspective(fov, aspect, zNear, currentObj.zFar);
            lastObjectIndex = currentObjectIndex;
        }
        
        g_Camera.viewMatrix = LookAt();

        glBindBuffer(GL_UNIFORM_BUFFER, g_Camera.UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Mat4), &g_Camera.viewMatrix); 
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Mat4), sizeof(Mat4), &g_Camera.projectionMatrix);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        uint32_t activeProgramID = currentObj.shader.GetProgram();
        glUseProgram(activeProgramID);

        glUniformMatrix4fv(glGetUniformLocation(activeProgramID, "u_Model"), 1, GL_FALSE, &currentObj.modelMatrix.m[0]); 

        Vec3 camPos = GetCameraPosition();
        glUniform3f(glGetUniformLocation(activeProgramID, "u_lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(activeProgramID, "u_viewPos"), camPos.x, camPos.y, camPos.z);
        glUniform3f(glGetUniformLocation(activeProgramID, "u_lightColor"), lightColor.x, lightColor.y, lightColor.z);

        glUniform1i(glGetUniformLocation(activeProgramID, "u_Texture"), 0);

        for (size_t i = 0; i < currentObj.meshes.size(); i++) {
            int matID = currentObj.meshes[i].materialId;

            int ambientLoc  = glGetUniformLocation(activeProgramID, "u_ambientColor");
            int diffuseLoc  = glGetUniformLocation(activeProgramID, "u_diffuseColor");
            int specularLoc = glGetUniformLocation(activeProgramID, "u_specularColor");
            int shininessLoc = glGetUniformLocation(activeProgramID, "u_shininess");
            int hasTexLoc    = glGetUniformLocation(activeProgramID, "u_hasTexture");

            glActiveTexture(GL_TEXTURE0);

            if (matID >= 0 && matID < currentObj.materials.size()) {
                if (currentObj.materials[matID].textureID != 0) {
                    glUniform1i(hasTexLoc, 1);
                    glBindTexture(GL_TEXTURE_2D, currentObj.materials[matID].textureID);
                } else {
                    glUniform1i(hasTexLoc, 0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
                glUniform3f(ambientLoc, currentObj.materials[matID].ambient.x, currentObj.materials[matID].ambient.y, currentObj.materials[matID].ambient.z);
                glUniform3f(diffuseLoc, currentObj.materials[matID].diffuse.x, currentObj.materials[matID].diffuse.y, currentObj.materials[matID].diffuse.z);
                glUniform3f(specularLoc, currentObj.materials[matID].specular.x, currentObj.materials[matID].specular.y, currentObj.materials[matID].specular.z);
                glUniform1f(shininessLoc, currentObj.materials[matID].shininess);
            } else {
                glUniform3f(ambientLoc, 0.1f, 0.1f, 0.1f);
                glUniform3f(diffuseLoc, 0.6f, 0.6f, 0.6f);
                glUniform3f(specularLoc, 0.3f, 0.3f, 0.3f);
                glUniform1f(shininessLoc, 32.0f);
                glUniform1i(hasTexLoc, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glBindVertexArray(currentObj.meshes[i].VAO);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(currentObj.meshes[i].vertices.size()));
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glfwSwapBuffers(window);
    }

    for (size_t i = 0; i < sceneObjects.size(); i++) {
        sceneObjects[i].shader.Destroy();
    }
    glDeleteBuffers(1, &g_Camera.UBO);

    glfwTerminate();
    return 0;
}