#include "Surface.h"
#include "../graphics/resources/RendererResources.h"

void Surface::destroy() {
	vkDestroySurfaceKHR(instance.instance, surface, nullptr);
}