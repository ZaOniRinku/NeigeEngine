#version 450

layout(push_constant) uniform View {
	mat4 viewProj;
} view;

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 outPos;

void main() {
	outPos = position;
	
	gl_Position = view.viewProj * vec4(position, 1.0);
}