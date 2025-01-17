#version 450


layout(location = 0) in vec3 inColor;
layout(location = 0) out vec4 outColor;


layout(binding = 1) uniform sampler2D texSampler;

layout(push_constant) uniform Push{
	mat4 transform; // projection * view * model
	mat4 normalMatrix;
} push;


void main()
{
	outColor = vec4(inColor, 1.0);
}
