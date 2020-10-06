#include "src/Renderer.h"

int main(void) {
	Window w;
	w.extent.width = 1280;
	w.extent.height = 720;
	w.init();
	Renderer r;
	r.window = w;
	r.init();
}