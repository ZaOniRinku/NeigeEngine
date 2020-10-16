#include "src/Game.h"

int main(void) {
	Window w;
	w.extent.width = 480;
	w.extent.height = 360;
	Game g;
	g.window = &w;
	g.launch();
}