#pragma once
#include "../ECS.h"

struct CameraControls : public System {
	void update(double deltaTime);

	float speed = 5.0f;
};
