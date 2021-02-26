#version 450

#define SAMPLES 64

layout(push_constant) uniform ImageSize {
	vec2 size;
} imageSize;

layout(set = 0, binding = 0) uniform sampler2D positionSampler;
layout(set = 0, binding = 1) uniform sampler2D normalSampler;
layout(set = 0, binding = 2) uniform sampler2D randomTexture;

layout(set = 0, binding = 3) uniform KernelSample {
	vec3 samples[SAMPLES];
} samples;

layout(set = 0, binding = 4) uniform Camera {
	mat4 view;
	mat4 projection;
	vec3 pos;
} camera;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

void main() {
	const vec2 randomScale = vec2(imageSize.size.x / 4.0, imageSize.size.y / 4.0);
	const float radius = 0.25;
	const float bias = 0.025;
	
	vec3 position = texture(positionSampler, uv).xyz;
	vec3 normal = texture(normalSampler, uv).xyz;
	vec3 random = texture(randomTexture, uv * randomScale).xyz;
	
	vec3 tangent = normalize(random - normal * dot(random, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
	
	float occlusion = 0.0;
	for (int i = 0; i < SAMPLES; i++) {
		vec3 samplePos = TBN * samples.samples[i];
		samplePos = position + samplePos * radius;
		
		vec4 offset = vec4(samplePos, 1.0);
		offset = camera.projection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;
		float sampleDepth = texture(positionSampler, offset.xy).z;
		
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / SAMPLES);

	outColor = vec4(vec3(occlusion), 1.0);
}