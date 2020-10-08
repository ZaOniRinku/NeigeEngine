#include "Surface.h"
#include "../utils/RendererResources.h"

void Surface::destroy() {
	vkDestroySurfaceKHR(instance.instance, surface, nullptr);
}