#version 450

layout(set = 0, binding = 0) uniform sampler2D depthPrepass;

layout(set = 0, binding = 1) uniform Camera {
	mat4 view;
	mat4 projection;
	vec3 pos;
} camera;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

vec3 depthToPosition(float depth) {
	float x = uv.x * 2.0 - 1.0;
	float y = uv.y * 2.0 - 1.0;
	vec4 clipSpace = vec4(uv * 2.0 - 1.0, depth, 1.0);
	vec4 viewSpace = inverse(camera.projection) * clipSpace;
	
	return (viewSpace.xyz / viewSpace.w);
}

void main() {
	float depth = texture(depthPrepass, uv).x;
	vec3 pos = depthToPosition(depth);
	vec3 n = normalize(cross(dFdx(pos), dFdy(pos)));
	n.y *= -1;
	
	outColor = vec4(n, 1.0);
}