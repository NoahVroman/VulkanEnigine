#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D normalSampler;
layout(binding = 3) uniform sampler2D specularSampler;
layout(binding = 4) uniform sampler2D glossSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 4) in vec3 fragCameraPosition;
layout(location = 5) in vec4 fragVertexPosition;

layout(location = 0) out vec4 outColor;

void main()
{


    vec4 textureColor = texture(texSampler, fragTexCoord);
    outColor = textureColor;
}