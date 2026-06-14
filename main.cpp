#include <iostream>
#include <vector>
#include <GL/glew.h>  
#include <GLFW/glfw3.h>

#include "Maths.h"
#include "Structures.h"
#include "Utils.h"
#include "Navigation.h"
#include "common/GLShader.h"

struct ObjectData {
    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    Mat4 modelMatrix; 
    float cameraDistance; 
    GLShader shader; 
};

std::vector<ObjectData> sceneObjects;
int currentObjectIndex = 0; 

bool rightArrowPressedLastFrame = false;
bool leftArrowPressedLastFrame = false;


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


    float fov = M_PI / 4.0f;
    float aspect = 800.0f / 600.0f;
    float zNear = 0.1f;

  
    ObjectData obj0;
    LoadOBJ("./3Dobjects/GoingMerry/GoingMerry.obj", obj0.meshes, obj0.materials, "./3Dobjects/GoingMerry/");
    SetupMeshBuffers(obj0.meshes);

    obj0.shader.LoadVertexShader("shaders/GoingMerry/vertex_shader.glsl");
    obj0.shader.LoadFragmentShader("shaders/GoingMerry/fragment_shader.glsl");
    obj0.shader.Create(); 
    
    Vec3 center0(0.0f, 0.0f, 0.0f);
    float maxDim0 = 0.0f;
    ComputeBoundingBox(obj0.meshes, center0, maxDim0);
    
    for (size_t i = 0; i < obj0.materials.size(); i++) {
        if (!obj0.materials[i].texturePath.empty()) {
            obj0.materials[i].textureID = LoadTexture(obj0.materials[i].texturePath);
        }
    }
    obj0.cameraDistance = (maxDim0 * 0.5f) / std::tan(fov * 0.5f) * 1.2f;
    obj0.modelMatrix = Translate(-center0.x, -center0.y, -center0.z);
    sceneObjects.push_back(obj0);

    ObjectData obj1;
    LoadOBJ("./3Dobjects/Tree/Tree.obj", obj1.meshes, obj1.materials, "./3Dobjects/Tree/");
    SetupMeshBuffers(obj1.meshes);
    
     obj1.shader.LoadVertexShader("shaders/Tree/vertex_shader.glsl");
    obj1.shader.LoadFragmentShader("shaders/tREE/fragment_shader.glsl");
    obj1.shader.Create(); 

    Vec3 center1(0.0f, 0.0f, 0.0f);
    float maxDim1 = 0.0f;
    ComputeBoundingBox(obj1.meshes, center1, maxDim1);
    for (size_t i = 0; i < obj1.materials.size(); i++) {
        if (!obj1.materials[i].texturePath.empty()) {
            obj1.materials[i].textureID = LoadTexture(obj1.materials[i].texturePath);
        }
    }
    obj1.cameraDistance = (maxDim1 * 0.5f) / std::tan(fov * 0.5f) * 1.2f;
    obj1.modelMatrix = Translate(-center1.x, -center1.y, -center1.z); 
    sceneObjects.push_back(obj1);

    float zFar = sceneObjects[0].cameraDistance + maxDim0 * 2.0f;

    Camera camera;
    camera.radius = sceneObjects[0].cameraDistance; 
    camera.yaw = 0.0f;
    camera.pitch = 0.0f;
    camera.isMousePressed = false;

    SetupScrollCallback(window, camera);

    Mat4 projMatrix = Perspective(fov, aspect, zNear, zFar);
    float dummyX, dummyY, dummyZ;

    Vec3 lightPos(5.0f, 10.0f, 5.0f);
    Vec3 lightColor(1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwPollEvents();

   
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            if (!rightArrowPressedLastFrame) {
                currentObjectIndex = (currentObjectIndex + 1) % sceneObjects.size();
                camera.radius = sceneObjects[currentObjectIndex].cameraDistance; 
                rightArrowPressedLastFrame = true;
            }
        } else {
            rightArrowPressedLastFrame = false;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            if (!leftArrowPressedLastFrame) {
                currentObjectIndex = (currentObjectIndex - 1 + sceneObjects.size()) % sceneObjects.size();
                camera.radius = sceneObjects[currentObjectIndex].cameraDistance;
                leftArrowPressedLastFrame = true;
            }
        } else {
            leftArrowPressedLastFrame = false;
        }

        UpdateCameraFromInputs(window, camera, dummyX, dummyY, dummyZ);

        ObjectData& currentObj = sceneObjects[currentObjectIndex];

        uint32_t activeProgramID = currentObj.shader.GetProgram();

        glUseProgram(activeProgramID);

        Mat4 viewMatrix = ComputeViewMatrix(camera);
        Mat4 mvpMatrix  = Multiply(projMatrix, Multiply(viewMatrix, currentObj.modelMatrix));

      unsigned int mvpLoc = glGetUniformLocation(activeProgramID, "u_MVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvpMatrix.m[0]);
        glUniformMatrix4fv(glGetUniformLocation(activeProgramID, "u_Model"), 1, GL_FALSE, &currentObj.modelMatrix.m[0]);


        Vec3 cameraPos;
        cameraPos.x = camera.radius * std::cos(camera.pitch) * std::cos(camera.yaw);
        cameraPos.y = camera.radius * std::sin(camera.pitch);
        cameraPos.z = camera.radius * std::cos(camera.pitch) * std::sin(camera.yaw);

       glUniform3f(glGetUniformLocation(activeProgramID, "u_lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(activeProgramID, "u_viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
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
    glfwTerminate();
    return 0;
}