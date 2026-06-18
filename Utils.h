#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "Structures.h"

bool LoadOBJ(const std::string& filename, std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials, const std::string& ext);
void SetupMeshBuffers(std::vector<Mesh>& meshes);
void ComputeBoundingBox(const std::vector<Mesh>& meshes, Vec3& outCenter, float& outMaxDim);
unsigned int LoadTexture(const std::string& path);
unsigned int LoadCubemap(const std::vector<std::string>& faces);

#endif