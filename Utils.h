#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "Structures.h"

std::string LoadShaderSource(const std::string& filePath);
unsigned int CreateShaderFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
bool LoadOBJ(const std::string& filename, std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials);
void SetupMeshBuffers(std::vector<Mesh>& meshes);

#endif