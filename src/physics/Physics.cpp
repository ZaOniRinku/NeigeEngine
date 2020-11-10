#include "Physics.h"
#include "../ecs/components/Transform.h"
#include "../ecs/components/Rigidbody.h"

extern ECS ecs;

void Physics::update(double deltaTime) {
	for (const auto& entity : entities) {
		auto& entityRigidbody = ecs.getComponent<Rigidbody>(entity);

		if (entityRigidbody.affectedByGravity) {
			auto& entityTransform = ecs.getComponent<Transform>(entity);

			entityTransform.position += entityRigidbody.velocity * static_cast<float>(deltaTime);
			entityRigidbody.velocity += gravity * static_cast<float>(deltaTime);
		}
	}
}
