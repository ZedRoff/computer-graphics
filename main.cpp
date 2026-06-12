#include <iostream>
#include <vector>
#include <GL/glew.h>  
#include <GLFW/glfw3.h>

#include "Maths.h"
#include "Structures.h"
#include "Utils.h"

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

    std::vector<Mesh> meshes;
    std::vector<Material> materials;

    LoadOBJ("hand.obj", meshes, materials);
    SetupMeshBuffers(meshes);
   

    unsigned int shaderProgram = CreateShaderFromFiles("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    Mat4 modelMatrix; 
    Mat4 viewMatrix  = Translate(7.0f, 18.0f, -25.0f);
    Mat4 projMatrix  = Perspective(M_PI / 4, 800.0f / 600.0f, 0.1f, 500.0f);
    
    Mat4 mvpMatrix = Multiply(projMatrix, Multiply(viewMatrix, modelMatrix));

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        unsigned int mvpLoc = glGetUniformLocation(shaderProgram, "u_MVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvpMatrix.m[0]);

        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, 0.7f, 0.7f, 1.0f);

        for (size_t i = 0; i < meshes.size(); i++) {
            glBindVertexArray(meshes[i].VAO);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(meshes[i].vertices.size()));
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}