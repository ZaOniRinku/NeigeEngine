#pragma once
#include "../ECS.h"
#include <algorithm>

struct CameraControls : public System {
	void update(double deltaTime);

	float speed = 5.0f;
	float sensitivity = 1.5f;
	float yaw = 0.0f;
	float pitch = 0.0f;
};
