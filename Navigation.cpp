#include "Navigation.h"
#include <cmath>

static Camera* g_cameraPtr = nullptr;

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (g_cameraPtr) {
        g_cameraPtr->radius -= yoffset * 0.5f;
        if (g_cameraPtr->radius < 1.0f) g_cameraPtr->radius = 1.0f;
    }
}

void SetupScrollCallback(GLFWwindow* window, Camera& camera) {
    g_cameraPtr = &camera;
    glfwSetScrollCallback(window, scroll_callback);
}

void UpdateCameraFromInputs(GLFWwindow* window, Camera& camera, float& outCamX, float& outCamY, float& outCamZ) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!camera.isMousePressed) {
            camera.lastX = mouseX;
            camera.lastY = mouseY;
            camera.isMousePressed = true;
        }

        double deltaX = mouseX - camera.lastX;
        double deltaY = mouseY - camera.lastY;
        
        camera.lastX = mouseX;
        camera.lastY = mouseY;

        float sensitivity = 0.005f;
        camera.yaw   += deltaX * sensitivity;
        camera.pitch -= deltaY * sensitivity;

        if (camera.pitch > 1.5f)  camera.pitch = 1.5f;
        if (camera.pitch < -1.5f) camera.pitch = -1.5f;
    } else {
        camera.isMousePressed = false;
    }

    outCamX = camera.radius * std::cos(camera.pitch) * std::cos(camera.yaw);
    outCamY = camera.radius * std::sin(camera.pitch);
    outCamZ = camera.radius * std::cos(camera.pitch) * std::sin(camera.yaw);
}

Mat4 ComputeViewMatrix(const Camera& camera) {
    return Multiply(Translate(0.0f, 0.0f, -camera.radius), 
           Multiply(RotateX(camera.pitch), RotateY(-camera.yaw - M_PI / 2.0f)));
}