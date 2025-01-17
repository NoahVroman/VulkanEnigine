#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;

layout(location = 0) out vec3 outColor;


layout(push_constant) uniform Push{
	mat4 transform; // projection * view * model
	mat4 normalMatrix;
} push;


void main()
{
	gl_Position = vec4(position,0.0, 1.0);
	outColor = color;
}
