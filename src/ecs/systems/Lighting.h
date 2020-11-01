#pragma once
#include "../ECS.h"
#include "../../utils/structs/ShaderStructs.h"

struct Lighting : public System {
	void init();
	void update();
};