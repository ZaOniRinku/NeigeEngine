#include "Surface.h"

void Surface::destroy(Instance* instance) {
	vkDestroySurfaceKHR(instance->instance, surface, nullptr);
}