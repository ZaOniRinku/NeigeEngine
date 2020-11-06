#version 450

#define SAMPLES 1024

layout(location = 0) in vec2 uv;

layout(location = 0) out vec2 outColor;

#define M_PI 3.1415926535897932384626433832795

float geometrySchlickGGX(float NdotV, float roughness) {
	float k = (roughness * roughness) / 2.0;
	float denom = NdotV * (1.0 - k) + k;
	
	return NdotV / denom;
}

float geometrySmith(vec3 n, vec3 v, vec3 l, float roughness) {
	float NdotV = max(dot(n, v), 0.0);
	float NdotL = max(dot(n, l), 0.0);
	float ggxv = geometrySchlickGGX(NdotV, roughness);
	float ggxl = geometrySchlickGGX(NdotL, roughness);
	
	return ggxv * ggxl;
}

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

vec2 integrate(float NdotV, float roughness) {
	vec3 v;
	v.x = sqrt(1.0 - NdotV * NdotV);
	v.y = 0.0;
	v.z = NdotV;
	
	float a = 0.0;
	float b = 0.0;
	
	vec3 n = vec3(0.0, 0.0, 1.0);
	
	for (uint i = 0u; i < SAMPLES; i++) {
		vec2 xi = hammersley(i, SAMPLES);
		vec3 h = importanceSamplingGGX(xi, n, roughness);
		vec3 l = normalize(2.0 * dot(v, h) * h - v);
		
		float NdotL = max(l.z, 0.0);
		float NdotH = max(h.z, 0.0);
		float VdotH = max(dot(v, h), 0.0);
		
		if (NdotL > 0.0) {
			float g = geometrySmith(n, v, l, roughness);
			float gvis = (g * VdotH) / (NdotH * NdotV);
			float fc = pow(1.0 - VdotH, 5.0);
			
			a += (1.0 - fc) * gvis;
			b += fc * gvis;
		}
	}
	
	a /= float(SAMPLES);
	b /= float(SAMPLES);
	
	return vec2(a, b);
}

void main() {
	vec2 integrated = integrate(uv.x, uv.y);
	
	outColor = integrated;
}