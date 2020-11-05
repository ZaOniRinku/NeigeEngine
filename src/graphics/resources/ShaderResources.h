#pragma once
#include "../../utils/structs/ShaderStructs.h"
#include "../models/Model.h"
#include "../pipelines/Shader.h"
#include "../../ecs/ECS.h"
#include "Image.h"
#include <string>
#include <unordered_map>

inline std::unordered_map<std::string, Model> models;
inline std::unordered_map<std::string, Image> textures;
inline std::vector<Material> materials;
inline std::unordered_map<std::string, Shader> shaders;
inline Entity camera;
inline std::set<Entity> lights;