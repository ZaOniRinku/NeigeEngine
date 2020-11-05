#version 450

layout(set = 0, binding = 0) uniform Camera {
	mat4 view;
	mat4 projection;
	vec3 pos;
} camera;

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 uv;

void main() {
	uv = position;
	uv.y *= -1;
	vec4 pos = camera.projection * vec4(mat3(camera.view) * position, 1.0);
	
	gl_Position = pos.xyww;
}