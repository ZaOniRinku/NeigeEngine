#version 450

layout(set = 0, binding = 0) uniform samplerCube skybox;

layout(location = 0) in vec3 position;

layout(location = 0) out vec4 outColor;

#define M_PI 3.1415926535897932384626433832795

void main() {
	vec3 normal = normalize(position);
	
	vec3 irradiance = vec3(0.0);
	
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up = normalize(cross(normal, right));
	
	float delta = 0.012;
	float samples = 0.0;
	
	for (float phi = 0.0; phi < 2.0 * M_PI; phi += delta) {
		for (float theta = 0.0; theta < 0.5 * M_PI; theta += delta) {
			vec3 tangent = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			vec3 vec = tangent.x * right + tangent.y * up + tangent.z * normal;
		
			irradiance += vec3(texture(skybox, vec)) * cos(theta) * sin(theta);
			samples += 1.0;
		}
	}
	
	irradiance = M_PI * irradiance * (1.0 / samples);
	
	outColor = vec4(irradiance, 1.0);
}