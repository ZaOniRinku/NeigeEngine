#version 450

layout(set = 0, binding = 0) uniform sampler2D ssaoSampler;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

void main() {
	vec2 texelSize = 1.0 / vec2(textureSize(ssaoSampler, 0));
	
	float result = 0.0;
	for (float x = -2.0; x < 2.0; x++) {
		for (float y = -2.0; y < 2.0; y++) {
			vec2 offset = vec2(x, y) * texelSize;
			result += texture(ssaoSampler, uv + offset).r;
		}
	}

	outColor = vec4(vec3(result) / (4.0 * 4.0), 1.0);
}