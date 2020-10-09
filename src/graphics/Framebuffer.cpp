#include "Framebuffer.h"
#include "../utils/RendererResources.h"

void Framebuffer::destroy() {
	vkDestroyFramebuffer(logicalDevice.device, framebuffer, nullptr);
}
