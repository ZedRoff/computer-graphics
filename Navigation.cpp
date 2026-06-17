#include "Navigation.h"
#include <cmath>
#include "imgui.h" 
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

float radius = 50.0f;
int currentObjectIndex = 0; 

static float phi = 0.0f;   
static float theta = 0.0f; 
static double lastMouseX = 0.0;
static double lastMouseY = 0.0;
static bool firstMouse = true;

static bool rightArrowPressedLastFrame = false;
static bool leftArrowPressedLastFrame = false;

struct ObjectInfo {
    std::string intitule;
    std::string titre;    
    float cameraDistance; 
};

std::vector<ObjectInfo> mesObjets = {
    {"Vogue Merry", "Vogue Merry", 50.0f}
};

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (ImGui::GetIO().WantCaptureMouse) {
        firstMouse = true;
        return;
    }
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

void UpdateNavigationInputs(GLFWwindow* window, size_t totalObjects, const std::vector<ObjectInfo>& objectsList) {
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (!rightArrowPressedLastFrame) {
            currentObjectIndex = (currentObjectIndex + 1) % totalObjects;
            radius = objectsList[currentObjectIndex].cameraDistance; 
            rightArrowPressedLastFrame = true;
        }
    } else {
        rightArrowPressedLastFrame = false;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if (!leftArrowPressedLastFrame) {
            currentObjectIndex = (currentObjectIndex - 1 + totalObjects) % totalObjects;
            radius = objectsList[currentObjectIndex].cameraDistance; 
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


void RenderNavigationUI(size_t totalObjects, const std::vector<ObjectInfo>& objectsList) {
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, 20.0f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.6f);

    ImGui::Begin("Navigation Objet", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_AlwaysAutoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoSavedSettings);

    const ObjectInfo& currentObj = objectsList[currentObjectIndex];

    ImGui::TextDisabled("%s", currentObj.intitule.c_str());
    ImGui::Separator();
    
    ImGui::SetWindowFontScale(1.3f);
    ImGui::Text("%s", currentObj.titre.c_str());
    ImGui::SetWindowFontScale(1.0f);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

  
    if (ImGui::Button("<##Prev", ImVec2(40, 40))) {
        currentObjectIndex = (currentObjectIndex - 1 + totalObjects) % totalObjects;
        radius = objectsList[currentObjectIndex].cameraDistance;
    }
    
    ImGui::SameLine();
    
    ImGui::AlignTextToFramePadding();
    ImGui::Text("  %d / %zu  ", currentObjectIndex + 1, totalObjects);
    
    ImGui::SameLine();
    
    if (ImGui::Button(">##Next", ImVec2(40, 40))) {
        currentObjectIndex = (currentObjectIndex + 1) % totalObjects;
        radius = objectsList[currentObjectIndex].cameraDistance;
    }

    ImGui::End();
}