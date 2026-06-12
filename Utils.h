#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "Structures.h"

std::string LoadShaderSource(const std::string& filePath);
unsigned int CreateShaderFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
bool LoadOBJ(const std::string& filename, std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials);
void SetupMeshBuffers(std::vector<Mesh>& meshes);
void ComputeBoundingBox(const std::vector<Mesh>& meshes, Vec3& outCenter, float& outMaxDim);
unsigned int LoadTexture(const std::string& path);

#endif