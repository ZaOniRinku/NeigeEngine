#include "src/Game.h"
#include "src/utils/ImageTools.h"
#include "src/utils/ModelLoader.h"

int main(void) {
	// ModelLoader::load("../gltfmodels/2CylinderEngine.gltf");
	Window w;
	w.extent.width = 480;
	w.extent.height = 360;
	Game g;
	g.window = &w;
	g.launch();
}