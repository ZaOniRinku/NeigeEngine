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
		if (keyboardInputs.qKey == KeyState::NONE) {
			keyboardInputs.qKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.qKey == KeyState::PRESSED || keyboardInputs.qKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.qKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_W:
		if (keyboardInputs.wKey == KeyState::NONE) {
			keyboardInputs.wKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.wKey == KeyState::PRESSED || keyboardInputs.wKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.wKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_E:
		if (keyboardInputs.eKey == KeyState::NONE) {
			keyboardInputs.eKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.eKey == KeyState::PRESSED || keyboardInputs.eKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.eKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_R:
		if (keyboardInputs.rKey == KeyState::NONE) {
			keyboardInputs.rKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.rKey == KeyState::PRESSED || keyboardInputs.rKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.rKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_T:
		if (keyboardInputs.tKey == KeyState::NONE) {
			keyboardInputs.tKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.tKey == KeyState::PRESSED || keyboardInputs.tKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.tKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_Y:
		if (keyboardInputs.yKey == KeyState::NONE) {
			keyboardInputs.yKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.yKey == KeyState::PRESSED || keyboardInputs.yKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.yKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_U:
		if (keyboardInputs.uKey == KeyState::NONE) {
			keyboardInputs.uKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.uKey == KeyState::PRESSED || keyboardInputs.uKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.uKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_I:
		if (keyboardInputs.iKey == KeyState::NONE) {
			keyboardInputs.iKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.iKey == KeyState::PRESSED || keyboardInputs.iKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.iKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_O:
		if (keyboardInputs.oKey == KeyState::NONE) {
			keyboardInputs.oKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.oKey == KeyState::PRESSED || keyboardInputs.oKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.oKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_P:
		if (keyboardInputs.pKey == KeyState::NONE) {
			keyboardInputs.pKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.pKey == KeyState::PRESSED || keyboardInputs.pKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.pKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_A:
		if (keyboardInputs.aKey == KeyState::NONE) {
			keyboardInputs.aKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.aKey == KeyState::PRESSED || keyboardInputs.aKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.aKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_S:
		if (keyboardInputs.sKey == KeyState::NONE) {
			keyboardInputs.sKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.sKey == KeyState::PRESSED || keyboardInputs.sKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.sKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_D:
		if (keyboardInputs.dKey == KeyState::NONE) {
			keyboardInputs.dKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.dKey == KeyState::PRESSED || keyboardInputs.dKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.dKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_F:
		if (keyboardInputs.fKey == KeyState::NONE) {
			keyboardInputs.fKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.fKey == KeyState::PRESSED || keyboardInputs.fKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.fKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_G:
		if (keyboardInputs.gKey == KeyState::NONE) {
			keyboardInputs.gKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.gKey == KeyState::PRESSED || keyboardInputs.gKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.gKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_H:
		if (keyboardInputs.hKey == KeyState::NONE) {
			keyboardInputs.hKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.hKey == KeyState::PRESSED || keyboardInputs.hKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.hKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_J:
		if (keyboardInputs.jKey == KeyState::NONE) {
			keyboardInputs.jKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.jKey == KeyState::PRESSED || keyboardInputs.jKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.jKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_K:
		if (keyboardInputs.kKey == KeyState::NONE) {
			keyboardInputs.kKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.kKey == KeyState::PRESSED || keyboardInputs.kKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.kKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_L:
		if (keyboardInputs.lKey == KeyState::NONE) {
			keyboardInputs.lKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.lKey == KeyState::PRESSED || keyboardInputs.lKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.lKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_Z:
		if (keyboardInputs.zKey == KeyState::NONE) {
			keyboardInputs.zKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.zKey == KeyState::PRESSED || keyboardInputs.zKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.zKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_X:
		if (keyboardInputs.xKey == KeyState::NONE) {
			keyboardInputs.xKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.xKey == KeyState::PRESSED || keyboardInputs.xKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.xKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_C:
		if (keyboardInputs.cKey == KeyState::NONE) {
			keyboardInputs.cKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.cKey == KeyState::PRESSED || keyboardInputs.cKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.cKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_V:
		if (keyboardInputs.vKey == KeyState::NONE) {
			keyboardInputs.vKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.vKey == KeyState::PRESSED || keyboardInputs.vKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.vKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_B:
		if (keyboardInputs.bKey == KeyState::NONE) {
			keyboardInputs.bKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.bKey == KeyState::PRESSED || keyboardInputs.bKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.bKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_N:
		if (keyboardInputs.nKey == KeyState::NONE) {
			keyboardInputs.nKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.nKey == KeyState::PRESSED || keyboardInputs.nKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.nKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_M:
		if (keyboardInputs.mKey == KeyState::NONE) {
			keyboardInputs.mKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.mKey == KeyState::PRESSED || keyboardInputs.mKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.mKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_LEFT:
		if (keyboardInputs.leftKey == KeyState::NONE) {
			keyboardInputs.leftKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.leftKey == KeyState::PRESSED || keyboardInputs.leftKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.leftKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_RIGHT:
		if (keyboardInputs.rightKey == KeyState::NONE) {
			keyboardInputs.rightKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.rightKey == KeyState::PRESSED || keyboardInputs.rightKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.rightKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_UP:
		if (keyboardInputs.upKey == KeyState::NONE) {
			keyboardInputs.upKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.upKey == KeyState::PRESSED || keyboardInputs.upKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.upKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_DOWN:
		if (keyboardInputs.downKey == KeyState::NONE) {
			keyboardInputs.downKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.downKey == KeyState::PRESSED || keyboardInputs.downKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.downKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_ESCAPE:
		if (keyboardInputs.escapeKey == KeyState::NONE) {
			keyboardInputs.escapeKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.escapeKey == KeyState::PRESSED || keyboardInputs.escapeKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.escapeKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_SPACE:
		if (keyboardInputs.spaceKey == KeyState::NONE) {
			keyboardInputs.spaceKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.spaceKey == KeyState::PRESSED || keyboardInputs.spaceKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.spaceKey = KeyState::RELEASED;
		}
		break;
	case GLFW_KEY_LEFT_SHIFT:
		if (keyboardInputs.shiftKey == KeyState::NONE) {
			keyboardInputs.shiftKey = KeyState::PRESSED;
		}
		else if ((keyboardInputs.shiftKey == KeyState::PRESSED || keyboardInputs.shiftKey == KeyState::HELD) && action == GLFW_RELEASE) {
			keyboardInputs.shiftKey = KeyState::RELEASED;
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
