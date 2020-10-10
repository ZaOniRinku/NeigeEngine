#include "src/Game.h"

int main(void) {
	Window w;
	w.extent.width = 1920;
	w.extent.height = 1080;
	Game g;
	g.window = &w;
	g.launch();
}