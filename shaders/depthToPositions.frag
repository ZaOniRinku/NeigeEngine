#version 450

layout(set = 0, binding = 0) uniform sampler2D depthMap;

layout(set = 0, binding = 1) uniform Camera {
	mat4 view;
	mat4 projection;
	vec3 pos;
} camera;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

void main() {
	float depth = texture(depthMap, uv).x;
	
	float x = uv.x * 2.0 - 1.0;
	float y = uv.y * 2.0 - 1.0;
	vec4 clipSpace = vec4(x, y, depth, 1.0);
	vec4 viewSpace = inverse(camera.projection) * clipSpace;
	viewSpace /= viewSpace.w;
	
	outColor = vec4(vec3(viewSpace), 1.0);
}