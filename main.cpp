#include "src/Game.h"

int main(void) {
	Window w;
	w.extent.width = 1280;
	w.extent.height = 720;
	Renderer r;
	Game g;
	g.window = &w;
	g.renderer = &r;
	g.init();
	g.update();
	g.destroy();
}