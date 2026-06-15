#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <GLFW/glfw3.h>
#include "Maths.h"

extern int currentObjectIndex;
extern float radius;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void UpdateNavigationInputs(GLFWwindow* window, size_t totalObjects, const float* objectsCameraDistances);

Mat4 LookAt();
Vec3 GetCameraPosition();

#endif