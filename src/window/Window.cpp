#include "Window.h"
#include "../graphics/resources/RendererResources.h"
#include "../inputs/Inputs.h"

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
		if (keyboardInputs.qKey == NONE) {
			keyboardInputs.qKey = PRESSED;
		}
		else if ((keyboardInputs.qKey == PRESSED || keyboardInputs.qKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.qKey = RELEASED;
		}
		break;
	case GLFW_KEY_W:
		if (keyboardInputs.wKey == NONE) {
			keyboardInputs.wKey = PRESSED;
		}
		else if ((keyboardInputs.wKey == PRESSED || keyboardInputs.wKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.wKey = RELEASED;
		}
		break;
	case GLFW_KEY_E:
		if (keyboardInputs.eKey == NONE) {
			keyboardInputs.eKey = PRESSED;
		}
		else if ((keyboardInputs.eKey == PRESSED || keyboardInputs.eKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.eKey = RELEASED;
		}
		break;
	case GLFW_KEY_R:
		if (keyboardInputs.rKey == NONE) {
			keyboardInputs.rKey = PRESSED;
		}
		else if ((keyboardInputs.rKey == PRESSED || keyboardInputs.rKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.rKey = RELEASED;
		}
		break;
	case GLFW_KEY_T:
		if (keyboardInputs.tKey == NONE) {
			keyboardInputs.tKey = PRESSED;
		}
		else if ((keyboardInputs.tKey == PRESSED || keyboardInputs.tKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.tKey = RELEASED;
		}
		break;
	case GLFW_KEY_Y:
		if (keyboardInputs.yKey == NONE) {
			keyboardInputs.yKey = PRESSED;
		}
		else if ((keyboardInputs.yKey == PRESSED || keyboardInputs.yKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.yKey = RELEASED;
		}
		break;
	case GLFW_KEY_U:
		if (keyboardInputs.uKey == NONE) {
			keyboardInputs.uKey = PRESSED;
		}
		else if ((keyboardInputs.uKey == PRESSED || keyboardInputs.uKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.uKey = RELEASED;
		}
		break;
	case GLFW_KEY_I:
		if (keyboardInputs.iKey == NONE) {
			keyboardInputs.iKey = PRESSED;
		}
		else if ((keyboardInputs.iKey == PRESSED || keyboardInputs.iKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.iKey = RELEASED;
		}
		break;
	case GLFW_KEY_O:
		if (keyboardInputs.oKey == NONE) {
			keyboardInputs.oKey = PRESSED;
		}
		else if ((keyboardInputs.oKey == PRESSED || keyboardInputs.oKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.oKey = RELEASED;
		}
		break;
	case GLFW_KEY_P:
		if (keyboardInputs.pKey == NONE) {
			keyboardInputs.pKey = PRESSED;
		}
		else if ((keyboardInputs.pKey == PRESSED || keyboardInputs.pKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.pKey = RELEASED;
		}
		break;
	case GLFW_KEY_A:
		if (keyboardInputs.aKey == NONE) {
			keyboardInputs.aKey = PRESSED;
		}
		else if ((keyboardInputs.aKey == PRESSED || keyboardInputs.aKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.aKey = RELEASED;
		}
		break;
	case GLFW_KEY_S:
		if (keyboardInputs.sKey == NONE) {
			keyboardInputs.sKey = PRESSED;
		}
		else if ((keyboardInputs.sKey == PRESSED || keyboardInputs.sKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.sKey = RELEASED;
		}
		break;
	case GLFW_KEY_D:
		if (keyboardInputs.dKey == NONE) {
			keyboardInputs.dKey = PRESSED;
		}
		else if ((keyboardInputs.dKey == PRESSED || keyboardInputs.dKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.dKey = RELEASED;
		}
		break;
	case GLFW_KEY_F:
		if (keyboardInputs.fKey == NONE) {
			keyboardInputs.fKey = PRESSED;
		}
		else if ((keyboardInputs.fKey == PRESSED || keyboardInputs.fKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.fKey = RELEASED;
		}
		break;
	case GLFW_KEY_G:
		if (keyboardInputs.gKey == NONE) {
			keyboardInputs.gKey = PRESSED;
		}
		else if ((keyboardInputs.gKey == PRESSED || keyboardInputs.gKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.gKey = RELEASED;
		}
		break;
	case GLFW_KEY_H:
		if (keyboardInputs.hKey == NONE) {
			keyboardInputs.hKey = PRESSED;
		}
		else if ((keyboardInputs.hKey == PRESSED || keyboardInputs.hKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.hKey = RELEASED;
		}
		break;
	case GLFW_KEY_J:
		if (keyboardInputs.jKey == NONE) {
			keyboardInputs.jKey = PRESSED;
		}
		else if ((keyboardInputs.jKey == PRESSED || keyboardInputs.jKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.jKey = RELEASED;
		}
		break;
	case GLFW_KEY_K:
		if (keyboardInputs.kKey == NONE) {
			keyboardInputs.kKey = PRESSED;
		}
		else if ((keyboardInputs.kKey == PRESSED || keyboardInputs.kKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.kKey = RELEASED;
		}
		break;
	case GLFW_KEY_L:
		if (keyboardInputs.lKey == NONE) {
			keyboardInputs.lKey = PRESSED;
		}
		else if ((keyboardInputs.lKey == PRESSED || keyboardInputs.lKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.lKey = RELEASED;
		}
		break;
	case GLFW_KEY_Z:
		if (keyboardInputs.zKey == NONE) {
			keyboardInputs.zKey = PRESSED;
		}
		else if ((keyboardInputs.zKey == PRESSED || keyboardInputs.zKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.zKey = RELEASED;
		}
		break;
	case GLFW_KEY_X:
		if (keyboardInputs.xKey == NONE) {
			keyboardInputs.xKey = PRESSED;
		}
		else if ((keyboardInputs.xKey == PRESSED || keyboardInputs.xKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.xKey = RELEASED;
		}
		break;
	case GLFW_KEY_C:
		if (keyboardInputs.cKey == NONE) {
			keyboardInputs.cKey = PRESSED;
		}
		else if ((keyboardInputs.cKey == PRESSED || keyboardInputs.cKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.cKey = RELEASED;
		}
		break;
	case GLFW_KEY_V:
		if (keyboardInputs.vKey == NONE) {
			keyboardInputs.vKey = PRESSED;
		}
		else if ((keyboardInputs.vKey == PRESSED || keyboardInputs.vKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.vKey = RELEASED;
		}
		break;
	case GLFW_KEY_B:
		if (keyboardInputs.bKey == NONE) {
			keyboardInputs.bKey = PRESSED;
		}
		else if ((keyboardInputs.bKey == PRESSED || keyboardInputs.bKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.bKey = RELEASED;
		}
		break;
	case GLFW_KEY_N:
		if (keyboardInputs.nKey == NONE) {
			keyboardInputs.nKey = PRESSED;
		}
		else if ((keyboardInputs.nKey == PRESSED || keyboardInputs.nKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.nKey = RELEASED;
		}
		break;
	case GLFW_KEY_M:
		if (keyboardInputs.mKey == NONE) {
			keyboardInputs.mKey = PRESSED;
		}
		else if ((keyboardInputs.mKey == PRESSED || keyboardInputs.mKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.mKey = RELEASED;
		}
		break;
	case GLFW_KEY_ESCAPE:
		if (keyboardInputs.escapeKey == NONE) {
			keyboardInputs.escapeKey = PRESSED;
		}
		else if ((keyboardInputs.escapeKey == PRESSED || keyboardInputs.escapeKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.escapeKey = RELEASED;
		}
		break;
	case GLFW_KEY_SPACE:
		if (keyboardInputs.spaceKey == NONE) {
			keyboardInputs.spaceKey = PRESSED;
		}
		else if ((keyboardInputs.spaceKey == PRESSED || keyboardInputs.spaceKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.spaceKey = RELEASED;
		}
		break;
	case GLFW_KEY_LEFT_SHIFT:
		if (keyboardInputs.shiftKey == NONE) {
			keyboardInputs.shiftKey = PRESSED;
		}
		else if ((keyboardInputs.shiftKey == PRESSED || keyboardInputs.shiftKey == HELD) && action == GLFW_RELEASE) {
			keyboardInputs.shiftKey = RELEASED;
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
	keyboardInputs.update();
	glfwPollEvents();
}

void Window::waitEvents() {
	glfwWaitEvents();
}
