#version 450

layout(set = 0, binding = 0) uniform Object {
	mat4 model;
} object;

layout(set = 0, binding = 1) uniform Camera {
	mat4 viewProj;
	vec3 pos;
} camera;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;
layout(location = 4) in vec3 tangent;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUv;
layout(location = 3) out vec3 outColor;
layout(location = 4) out vec3 outTangent;
layout(location = 5) out vec3 outCameraPos;
layout(location = 6) out vec3 outFragmentPos;
layout(location = 7) out mat3 outTBN;

void main() {
	mat3 normalMatrix = transpose(inverse(mat3(object.model)));
	outPosition = position;
	outNormal = normalize(normalMatrix * normal);
	outUv = uv;
	outColor = color;
	outTangent = tangent;
	vec3 bitangent = normalize(cross(normal, tangent));
	vec3 T = normalize(vec3(object.model * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(object.model * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(object.model * vec4(normal, 0.0)));
	outTBN = mat3(T, B, N);
	outCameraPos = camera.pos;
	outFragmentPos = vec3(object.model * vec4(position, 1.0));

	gl_Position = camera.viewProj * vec4(outFragmentPos, 1.0);
}