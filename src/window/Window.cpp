#include "Window.h"
#include "../graphics/resources/RendererResources.h"

void Window::init() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(extent.width, extent.height, "", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
}

void Window::destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::updateExtent() {
	int width;
	int height;
	glfwGetFramebufferSize(window, &width, &height);
	extent = {
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height)
	};
}

std::vector<const char*> Window::instanceExtensions() {
	uint32_t extensionCount;
	const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
	std::vector<const char*> instanceExtensions (extensions, extensions + extensionCount);
	if (NEIGE_DEBUG) {
		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return instanceExtensions;
}

void Window::createSurface() {
	NEIGE_VK_CHECK(glfwCreateWindowSurface(instance.instance, window, nullptr, &surface.surface));
}

void Window::windowGotResized() {
	gotResized = true;
	updateExtent();
}

void Window::keyAction(int key, int action) {
	switch (key) {
	case GLFW_KEY_Q:
		if (inputs.qKey == NONE) {
			inputs.qKey = PRESSED;
		}
		else if ((inputs.qKey == PRESSED || inputs.qKey == HELD) && action == GLFW_RELEASE) {
			inputs.qKey = RELEASED;
		}
		break;
	case GLFW_KEY_W:
		if (inputs.wKey == NONE) {
			inputs.wKey = PRESSED;
		}
		else if ((inputs.wKey == PRESSED || inputs.wKey == HELD) && action == GLFW_RELEASE) {
			inputs.wKey = RELEASED;
		}
		break;
	case GLFW_KEY_E:
		if (inputs.eKey == NONE) {
			inputs.eKey = PRESSED;
		}
		else if ((inputs.eKey == PRESSED || inputs.eKey == HELD) && action == GLFW_RELEASE) {
			inputs.eKey = RELEASED;
		}
		break;
	case GLFW_KEY_R:
		if (inputs.rKey == NONE) {
			inputs.rKey = PRESSED;
		}
		else if ((inputs.rKey == PRESSED || inputs.rKey == HELD) && action == GLFW_RELEASE) {
			inputs.rKey = RELEASED;
		}
		break;
	case GLFW_KEY_T:
		if (inputs.tKey == NONE) {
			inputs.tKey = PRESSED;
		}
		else if ((inputs.tKey == PRESSED || inputs.tKey == HELD) && action == GLFW_RELEASE) {
			inputs.tKey = RELEASED;
		}
		break;
	case GLFW_KEY_Y:
		if (inputs.yKey == NONE) {
			inputs.yKey = PRESSED;
		}
		else if ((inputs.yKey == PRESSED || inputs.yKey == HELD) && action == GLFW_RELEASE) {
			inputs.yKey = RELEASED;
		}
		break;
	case GLFW_KEY_U:
		if (inputs.uKey == NONE) {
			inputs.uKey = PRESSED;
		}
		else if ((inputs.uKey == PRESSED || inputs.uKey == HELD) && action == GLFW_RELEASE) {
			inputs.uKey = RELEASED;
		}
		break;
	case GLFW_KEY_I:
		if (inputs.iKey == NONE) {
			inputs.iKey = PRESSED;
		}
		else if ((inputs.iKey == PRESSED || inputs.iKey == HELD) && action == GLFW_RELEASE) {
			inputs.iKey = RELEASED;
		}
		break;
	case GLFW_KEY_O:
		if (inputs.oKey == NONE) {
			inputs.oKey = PRESSED;
		}
		else if ((inputs.oKey == PRESSED || inputs.oKey == HELD) && action == GLFW_RELEASE) {
			inputs.oKey = RELEASED;
		}
		break;
	case GLFW_KEY_P:
		if (inputs.pKey == NONE) {
			inputs.pKey = PRESSED;
		}
		else if ((inputs.pKey == PRESSED || inputs.pKey == HELD) && action == GLFW_RELEASE) {
			inputs.pKey = RELEASED;
		}
		break;
	case GLFW_KEY_A:
		if (inputs.aKey == NONE) {
			inputs.aKey = PRESSED;
		}
		else if ((inputs.aKey == PRESSED || inputs.aKey == HELD) && action == GLFW_RELEASE) {
			inputs.aKey = RELEASED;
		}
		break;
	case GLFW_KEY_S:
		if (inputs.sKey == NONE) {
			inputs.sKey = PRESSED;
		}
		else if ((inputs.sKey == PRESSED || inputs.sKey == HELD) && action == GLFW_RELEASE) {
			inputs.sKey = RELEASED;
		}
		break;
	case GLFW_KEY_D:
		if (inputs.dKey == NONE) {
			inputs.dKey = PRESSED;
		}
		else if ((inputs.dKey == PRESSED || inputs.dKey == HELD) && action == GLFW_RELEASE) {
			inputs.dKey = RELEASED;
		}
		break;
	case GLFW_KEY_F:
		if (inputs.fKey == NONE) {
			inputs.fKey = PRESSED;
		}
		else if ((inputs.fKey == PRESSED || inputs.fKey == HELD) && action == GLFW_RELEASE) {
			inputs.fKey = RELEASED;
		}
		break;
	case GLFW_KEY_G:
		if (inputs.gKey == NONE) {
			inputs.gKey = PRESSED;
		}
		else if ((inputs.gKey == PRESSED || inputs.gKey == HELD) && action == GLFW_RELEASE) {
			inputs.gKey = RELEASED;
		}
		break;
	case GLFW_KEY_H:
		if (inputs.hKey == NONE) {
			inputs.hKey = PRESSED;
		}
		else if ((inputs.hKey == PRESSED || inputs.hKey == HELD) && action == GLFW_RELEASE) {
			inputs.hKey = RELEASED;
		}
		break;
	case GLFW_KEY_J:
		if (inputs.jKey == NONE) {
			inputs.jKey = PRESSED;
		}
		else if ((inputs.jKey == PRESSED || inputs.jKey == HELD) && action == GLFW_RELEASE) {
			inputs.jKey = RELEASED;
		}
		break;
	case GLFW_KEY_K:
		if (inputs.kKey == NONE) {
			inputs.kKey = PRESSED;
		}
		else if ((inputs.kKey == PRESSED || inputs.kKey == HELD) && action == GLFW_RELEASE) {
			inputs.kKey = RELEASED;
		}
		break;
	case GLFW_KEY_L:
		if (inputs.lKey == NONE) {
			inputs.lKey = PRESSED;
		}
		else if ((inputs.lKey == PRESSED || inputs.lKey == HELD) && action == GLFW_RELEASE) {
			inputs.lKey = RELEASED;
		}
		break;
	case GLFW_KEY_Z:
		if (inputs.zKey == NONE) {
			inputs.zKey = PRESSED;
		}
		else if ((inputs.zKey == PRESSED || inputs.zKey == HELD) && action == GLFW_RELEASE) {
			inputs.zKey = RELEASED;
		}
		break;
	case GLFW_KEY_X:
		if (inputs.xKey == NONE) {
			inputs.xKey = PRESSED;
		}
		else if ((inputs.xKey == PRESSED || inputs.xKey == HELD) && action == GLFW_RELEASE) {
			inputs.xKey = RELEASED;
		}
		break;
	case GLFW_KEY_C:
		if (inputs.cKey == NONE) {
			inputs.cKey = PRESSED;
		}
		else if ((inputs.cKey == PRESSED || inputs.cKey == HELD) && action == GLFW_RELEASE) {
			inputs.cKey = RELEASED;
		}
		break;
	case GLFW_KEY_V:
		if (inputs.vKey == NONE) {
			inputs.vKey = PRESSED;
		}
		else if ((inputs.vKey == PRESSED || inputs.vKey == HELD) && action == GLFW_RELEASE) {
			inputs.vKey = RELEASED;
		}
		break;
	case GLFW_KEY_B:
		if (inputs.bKey == NONE) {
			inputs.bKey = PRESSED;
		}
		else if ((inputs.bKey == PRESSED || inputs.bKey == HELD) && action == GLFW_RELEASE) {
			inputs.bKey = RELEASED;
		}
		break;
	case GLFW_KEY_N:
		if (inputs.nKey == NONE) {
			inputs.nKey = PRESSED;
		}
		else if ((inputs.nKey == PRESSED || inputs.nKey == HELD) && action == GLFW_RELEASE) {
			inputs.nKey = RELEASED;
		}
		break;
	case GLFW_KEY_M:
		if (inputs.mKey == NONE) {
			inputs.mKey = PRESSED;
		}
		else if ((inputs.mKey == PRESSED || inputs.mKey == HELD) && action == GLFW_RELEASE) {
			inputs.mKey = RELEASED;
		}
		break;
	case GLFW_KEY_ESCAPE:
		if (inputs.escapeKey == NONE) {
			inputs.escapeKey = PRESSED;
		}
		else if ((inputs.escapeKey == PRESSED || inputs.escapeKey == HELD) && action == GLFW_RELEASE) {
			inputs.escapeKey = RELEASED;
		}
		break;
	case GLFW_KEY_SPACE:
		if (inputs.spaceKey == NONE) {
			inputs.spaceKey = PRESSED;
		}
		else if ((inputs.spaceKey == PRESSED || inputs.spaceKey == HELD) && action == GLFW_RELEASE) {
			inputs.spaceKey = RELEASED;
		}
		break;
	case GLFW_KEY_LEFT_SHIFT:
		if (inputs.shiftKey == NONE) {
			inputs.shiftKey = PRESSED;
		}
		else if ((inputs.shiftKey == PRESSED || inputs.shiftKey == HELD) && action == GLFW_RELEASE) {
			inputs.shiftKey = RELEASED;
		}
		break;
	default:
		break;
	}
}

bool Window::windowGotClosed() {
	return glfwWindowShouldClose(window);
}

void Window::pollEvents() {
	inputs.update();
	glfwPollEvents();
}

void Window::waitEvents() {
	glfwWaitEvents();
}
