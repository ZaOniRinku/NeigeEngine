#version 450

layout(set = 0, binding = 0) uniform sampler2D equirectangularMap;

layout(location = 0) in vec3 position;

layout(location = 0) out vec4 outColor;

const vec2 invATan = vec2(0.1591, 0.3183);

vec2 sampleSphericalMap(vec3 v) {
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invATan;
	uv += 0.5;
	
	return uv;
}

void main() {
	vec2 uv = sampleSphericalMap(normalize(position));
	vec4 colorSample = texture(equirectangularMap, uv);
	
	outColor = vec4(vec3(colorSample), 1.0);
}