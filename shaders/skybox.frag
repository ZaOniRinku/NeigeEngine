#version 450

layout(set = 0, binding = 1) uniform samplerCube skybox;

layout(location = 0) in vec3 uv;

layout(location = 0) out vec4 outColor;

void main() {
	vec3 skyboxSample = vec3(texture(skybox, uv));
	skyboxSample = skyboxSample / (skyboxSample + vec3(1.0));
	skyboxSample = pow(skyboxSample, vec3(1.0 / 2.4));
	
	outColor = vec4(skyboxSample, 1.0);
}