#version 450

#define SAMPLES 2048

#define M_PI 3.1415926535897932384626433832795

layout(set = 0, binding = 0) uniform samplerCube skybox;

layout(set = 0, binding = 1) uniform Roughness {
	float roughness;
} roughness;

layout(location = 0) in vec3 position;

layout(location = 0) out vec4 outColor;

vec3 importanceSamplingGGX(vec2 xi, vec3 normal, float roughness) {
	float a = roughness * roughness;
	
	float phi = 2.0 * M_PI * xi.x;
	float costheta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
	float sintheta = sqrt(1.0 - costheta * costheta);
	
	vec3 h;
	h.x = cos(phi) * sintheta;
	h.y = sin(phi) * sintheta;
	h.z = costheta;
	
	vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, normal));
	vec3 bitangent = normalize(cross(normal, tangent));
	
	vec3 sampleVec = tangent * h.x + bitangent * h.y + normal * h.z;
	
	return normalize(sampleVec);
}

float radicalInverseVanDerCorpus(uint bits) {
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	
	return float(bits) * 2.3283064365386963e-10;
}

vec2 hammersley(uint i, uint N) {
	return vec2(float(i) / float(N), radicalInverseVanDerCorpus(i));
}

void main() {
	vec3 normal = normalize(position);
	vec3 r = normal;
	vec3 v = r;
	
	float totalweight = 0.0;
	vec3 prefiltered = vec3(0.0);
	
	for (uint i = 0u; i < SAMPLES; i++) {
		vec2 xi = hammersley(i, SAMPLES);
		vec3 h = importanceSamplingGGX(xi, normal, roughness.roughness);
		vec3 l = normalize(2.0 * dot(v, h) * h - v);
		
		float NdotL = max(dot(normal, l), 0.0);
		if (NdotL > 0.0) {
			prefiltered += vec3(texture(skybox, l)) * NdotL;
			totalweight += NdotL;
		}
	}
	
	prefiltered = prefiltered / totalweight;
	
	outColor = vec4(prefiltered, 1.0);
}