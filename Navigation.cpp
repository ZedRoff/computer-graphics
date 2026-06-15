#include "Navigation.h"
#include <cmath>

float radius = 50.0f;
int currentObjectIndex = 0; 

static float phi = 0.0f;   
static float theta = 0.0f; 
static double lastMouseX = 0.0;
static double lastMouseY = 0.0;
static bool firstMouse = true;

static bool rightArrowPressedLastFrame = false;
static bool leftArrowPressedLastFrame = false;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
        firstMouse = true;
        return;
    }

    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    float xoffset = (float)(xpos - lastMouseX);
    float yoffset = (float)(lastMouseY - ypos); 
    lastMouseX = xpos;
    lastMouseY = ypos;

    float sensitivity = 0.005f; 
    phi += xoffset * sensitivity;
    theta += yoffset * sensitivity;

    if (theta > 1.55f)  theta = 1.55f;  
    if (theta < -1.55f) theta = -1.55f; 
    
    if (phi > M_PI) phi -= 2.0f * M_PI;
    if (phi < -M_PI) phi += 2.0f * M_PI;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    radius -= (float)yoffset * 0.5f; 
}

void UpdateNavigationInputs(GLFWwindow* window, size_t totalObjects, const float* objectsCameraDistances) {
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (!rightArrowPressedLastFrame) {
            currentObjectIndex = (currentObjectIndex + 1) % totalObjects;
            radius = objectsCameraDistances[currentObjectIndex]; 
            rightArrowPressedLastFrame = true;
        }
    } else {
        rightArrowPressedLastFrame = false;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if (!leftArrowPressedLastFrame) {
            currentObjectIndex = (currentObjectIndex - 1 + totalObjects) % totalObjects;
            radius = objectsCameraDistances[currentObjectIndex]; 
            leftArrowPressedLastFrame = true;
        }
    } else {
        leftArrowPressedLastFrame = false;
    }
}

Vec3 GetCameraPosition() {
    float camX = radius * cosf(theta) * cosf(phi);
    float camY = radius * sinf(theta);
    float camZ = radius * cosf(theta) * sinf(phi);
    return { camX, camY, camZ };
}

Mat4 LookAt() {
    Vec3 position = GetCameraPosition();
    Vec3 target = { 0.0f, 0.0f, 0.0f }; 
    Vec3 up = { 0.0f, 1.0f, 0.0f };

    Vec3 forward = Normalize({ position.x - target.x, position.y - target.y, position.z - target.z });
    Vec3 right = Normalize(Cross(up, forward));
    Vec3 upCorr = Cross(forward, right);

    float dotRight = Dot(position, right);
    float dotUp = Dot(position, upCorr);
    float dotForward = Dot(position, forward);

    Mat4 res = Identity();

    res.m[0] = right.x;   
    res.m[1] = upCorr.x;   
    res.m[2] = forward.x;   
    res.m[3] = 0.0f;
    res.m[4] = right.y;   
    res.m[5] = upCorr.y;   
    res.m[6] = forward.y;   
    res.m[7] = 0.0f;
    res.m[8] = right.z;   
    res.m[9] = upCorr.z;   
    res.m[10] = forward.z;  
    res.m[11] = 0.0f;
    res.m[12] = -dotRight; 
    res.m[13] = -dotUp;    
    res.m[14] = -dotForward; 
    res.m[15] = 1.0f;

    return res;
}