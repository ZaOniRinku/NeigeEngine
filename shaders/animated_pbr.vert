#version 450

#define MAX_DIR_LIGHTS 10
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10
#define MAX_BONES 256

layout(set = 0, binding = 0) uniform Object {
	mat4 model;
} object;

layout(set = 0, binding = 1) uniform Camera {
	mat4 view;
	mat4 projection;
	vec3 pos;
} camera;

layout(set = 0, binding = 2) uniform Shadow {
	vec3 numLights;
	mat4 dirLightSpaces[MAX_DIR_LIGHTS];
	mat4 spotLightSpaces[MAX_SPOT_LIGHTS];
} shadow;

layout(set = 1, binding = 4) uniform Bones {
	mat4 transformations[MAX_BONES];
	mat4 inverseBindMatrices[MAX_BONES];
} bones;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec4 joints;
layout(location = 6) in vec4 weights;

layout(location = 0) out vec2 outUv;
layout(location = 1) out vec3 outCameraPos;
layout(location = 2) out vec3 outFragmentPos;
layout(location = 3) out vec4 outWeights;
layout(location = 4) out vec4 outDirLightSpaces[MAX_DIR_LIGHTS];
layout(location = MAX_DIR_LIGHTS + 4) out vec4 outSpotLightSpaces[MAX_SPOT_LIGHTS];
layout(location = MAX_DIR_LIGHTS + MAX_SPOT_LIGHTS + 4) out mat3 outTBN;

void main() {
	outUv = uv;
	vec3 bitangent = normalize(cross(normal, tangent));
	vec3 T = normalize(vec3(object.model * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(object.model * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(object.model * vec4(normal, 0.0)));
	outTBN = mat3(T, B, N);
	outCameraPos = camera.pos;
	outWeights = weights;
	
	mat4 skinMat = weights.x * bones.transformations[int(joints.x)]
	+ weights.y * bones.transformations[int(joints.y)]
	+ weights.z * bones.transformations[int(joints.z)]
	+ weights.w * bones.transformations[int(joints.w)];
	
	outFragmentPos = vec3(object.model * skinMat * vec4(position, 1.0));

	int numDirLights = int(shadow.numLights.x);
	int numPointLights = int(shadow.numLights.y);
	int numSpotLights = int(shadow.numLights.z);
	
	for (int i = 0; i < shadow.numLights.x; i++) {
		outDirLightSpaces[i] = shadow.dirLightSpaces[i] * vec4(outFragmentPos, 1.0);
	}
	
	for (int i = 0; i < shadow.numLights.z; i++) {
		outSpotLightSpaces[i] = shadow.spotLightSpaces[i] * vec4(outFragmentPos, 1.0);
	}
	
	gl_Position = camera.projection * camera.view * vec4(outFragmentPos, 1.0);
}