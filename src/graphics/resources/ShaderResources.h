#pragma once
#include "../../utils/structs/ShaderStructs.h"
#include "../models/Model.h"
#include "../pipelines/Shader.h"
#include "../shadowmapping/Shadow.h"
#include "../envmap/Envmap.h"
#include "../../ecs/ECS.h"
#include "Image.h"
#include <string>
#include <unordered_map>

inline std::unordered_map<std::string, Model> models;
inline std::unordered_map<std::string, Image> textures;
inline std::vector<Material> materials;
inline std::unordered_map<std::string, Shader> shaders;
inline Entity camera;
inline std::vector<Buffer> cameraBuffers;
inline std::set<Entity> lights;
inline std::vector<Buffer> lightingBuffers;
inline Shadow shadow;
inline std::vector<Buffer> timeBuffers;
inline std::unordered_map<Entity, std::vector<Buffer>> entityBuffers;
inline std::unordered_map<Entity, std::vector<DescriptorSet>> entityDescriptorSets;
inline std::unordered_map<Entity, std::vector<DescriptorSet>> entityShadowDescriptorSets;
inline Envmap envmap;