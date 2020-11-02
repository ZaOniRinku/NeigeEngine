#version 450

#define MAX_DIR_LIGHTS 10
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

layout(set = 0, binding = 0) uniform Object {
	mat4 model;
} object;

layout(set = 0, binding = 1) uniform Shadow {
	vec3 numLights;
	mat4 dirLightSpaces[MAX_DIR_LIGHTS];
	mat4 spotLightSpaces[MAX_SPOT_LIGHTS];
} shadow;

layout(push_constant) uniform LightIndex {
	int lightIndex;
} lightIndex;

layout(location = 0) in vec3 position;

void main() {
	int numDirLights = int(shadow.numLights.x);
	
	if (lightIndex.lightIndex < numDirLights) {
		gl_Position = shadow.dirLightSpaces[lightIndex.lightIndex] * object.model * vec4(position, 1.0);
	}
	else if (lightIndex.lightIndex >= numDirLights) {
		gl_Position = shadow.spotLightSpaces[lightIndex.lightIndex - numDirLights] * object.model * vec4(position, 1.0);
	}
	gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
}