#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <GLFW/glfw3.h>
#include "Maths.h"

struct Camera {
    float pitch = 0.0f;
    float yaw = -M_PI / 2.0f;
    float radius = 10.0f;
    
    bool isMousePressed = false;
    double lastX = 0.0;
    double lastY = 0.0;
};

void UpdateCameraFromInputs(GLFWwindow* window, Camera& camera, float& outCamX, float& outCamY, float& outCamZ);
Mat4 ComputeViewMatrix(const Camera& camera);
void SetupScrollCallback(GLFWwindow* window, Camera& camera);

#endif