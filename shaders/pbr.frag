#version 450

layout(set = 1, binding = 0) uniform sampler2D colorMap;
layout(set = 1, binding = 1) uniform sampler2D normalMap;
layout(set = 1, binding = 2) uniform sampler2D metallicRoughnessMap;
layout(set = 1, binding = 3) uniform sampler2D occlusionMap;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 cameraPos;
layout(location = 6) in vec3 fragmentPos;
layout(location = 7) in mat3 TBN;

layout(location = 0) out vec4 outColor;

#define M_PI 3.1415926535897932384626433832795

float distribution(float NdotH, float roughness) {
	float a = roughness * roughness;
	float asquare = a * a;
	float NdotHsquare = NdotH * NdotH;
	float denom = NdotHsquare * (asquare - 1.0) + 1.0;

	return asquare / (M_PI * denom * denom);
}

vec3 fresnel(float costheta, vec3 f0) {
	return f0 + (1.0 - f0) * pow(1.0 - costheta, 5.0);
}

float g(float NdotV, float roughness) {
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;
	float denom = NdotV * (1.0 - k) + k;

	return NdotV / denom;
}

float smith(float LdotN, float VdotN, float roughness) {
	float gv = g(VdotN, roughness);
	float gl = g(LdotN, roughness);

	return gv * gl;
}

vec3 diffuseFresnelCorrection(vec3 ior) {
	vec3 iorsquare = ior * ior;
	bvec3 TIR = lessThan(ior, vec3(1.0));
	vec3 invdenum = mix(vec3(1.0), vec3(1.0) / (iorsquare * iorsquare * (vec3(554.33) - 380.7 * ior)), TIR);
	vec3 num = ior * mix(vec3(0.1921156102251088), ior * 298.25 - 261.38 * iorsquare + 138.43, TIR);
	num += mix(vec3(0.8078843897748912), vec3(-1.67), TIR);

	return num * invdenum;
}

vec3 brdf(float LdotH, float NdotH, float VdotH, float LdotN, float VdotN, vec3 diffuse, float metallic, float roughness) {
	float d = distribution(NdotH, roughness);
	vec3 f = fresnel(LdotH, mix(vec3(0.04), diffuse, metallic));
	vec3 fT = fresnel(LdotN, mix(vec3(0.04), diffuse, metallic));
	vec3 fTIR = fresnel(VdotN, mix(vec3(0.04), diffuse, metallic));
	float g = smith(LdotN, VdotN, roughness);
	vec3 dfc = diffuseFresnelCorrection(vec3(1.05));

	vec3 lambertian = diffuse / M_PI;

	return (d * f * g) / max(4.0 * LdotN * VdotN, 0.001) + ((vec3(1.0) - fT) * (vec3(1.0) - fTIR) * lambertian) * dfc;
}

vec3 shade(vec3 n, vec3 v, vec3 l, vec3 lc, vec3 diffuse, float metallic, float roughness) {
	vec3 h = normalize(v + l);
	float LdotH = max(dot(l, h), 0.0);
	float NdotH = max(dot(n, h), 0.0);
	float VdotH = max(dot(v, h), 0.0);
	float LdotN = max(dot(l, n), 0.0);
	float VdotN = max(dot(v, n), 0.0);
	vec3 brdf = brdf(LdotH, NdotH, VdotH, LdotN, VdotN, diffuse, metallic, roughness);
	vec3 ret = lc * brdf * LdotN;

	return ret;
}

void main() {
	vec4 colorSample = texture(colorMap, uv);
	vec3 normalSample = texture(normalMap, uv).xyz;
	float metallicSample = texture(metallicRoughnessMap, uv).g;
	float roughnessSample = texture(metallicRoughnessMap, uv).b;
	float occlusionSample = texture(occlusionMap, uv).r;

	vec3 d = vec3(colorSample);
	vec3 n = normalSample * 2.0 - 1.0;
	n = normalize(TBN * n);
	vec3 v = normalize(cameraPos - fragmentPos);

	vec3 l = vec3(1.0, -1.0, 0.0);
	l = normalize(-l);
	
	vec3 tmpColor = shade(n, v, l, vec3(1.0), d, metallicSample, roughnessSample);
	vec3 ambient = vec3(0.03) * d * occlusionSample;
	tmpColor += ambient;

	tmpColor = tmpColor / (tmpColor + vec3(1.0));
	tmpColor = pow(tmpColor, vec3(1.0 / 2.4));

	outColor = vec4(tmpColor, 1.0);
}