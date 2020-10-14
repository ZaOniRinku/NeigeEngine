#include "src/Game.h"
#include "src/graphics/Shader.h"

int main(void) {
	Shader s;
	s.init("../shaders/dummy_shader.vert");
	Window w;
	w.extent.width = 480;
	w.extent.height = 360;
	Game g;
	g.window = &w;
	g.launch();
}