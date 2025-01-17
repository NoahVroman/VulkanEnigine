#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;
layout(location = 4) in vec3 tangent;

layout(location = 0) out vec3 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projectionViewMatrix;
	vec3 directionToLight;
} ubo;


layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

const float AMBIENT_INTENSITY = 0.03;


void main()
{
	gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);
	vec3 normal = normalize(mat3(push.normalMatrix) * normal);
	float lightIntensity = AMBIENT_INTENSITY + max(dot(normal, -ubo.directionToLight),0);
	outColor = color * lightIntensity;
}
