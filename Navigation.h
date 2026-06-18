#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <GLFW/glfw3.h>
#include "Maths.h"
#include <string>

struct ObjectInfo {
    std::string intitule;
    std::string titre;    
    float cameraDistance; 
};
extern std::vector<ObjectInfo> mesObjets = {
    {"Vogue Merry", "Going Merry", 50.0f},
    {"Thousand Sunny", "Thousand Sunny", 60.0f},
    {"Laboon", "Laboon", 75.0f},
    {"Kuzan (Aokiji)", "Kuzan", 40.0f}
};
extern int currentObjectIndex;
extern float radius;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void UpdateNavigationInputs(GLFWwindow* window, size_t totalObjects, const float* objectsCameraDistances);

Mat4 LookAt();
Vec3 GetCameraPosition();

#endif