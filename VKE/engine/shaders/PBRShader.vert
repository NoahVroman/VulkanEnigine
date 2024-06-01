#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;
layout(location = 4) in vec3 inTangent;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragUV;
layout(location = 3) out vec3 fragTangent;

layout(location = 4) out vec3 fragCameraPosition;
layout(location = 5) out vec4 fragVertexPosition;

layout(location = 6) out int fragRenderMode;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    vec3 directionToLight;
    vec3 cameraPosition;
    int  rendermode;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main()
{
    gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(inPosition, 1.0);

    vec4 tNormal = push.normalMatrix * vec4(inNormal, 0.0);
    fragNormal = normalize(tNormal.xyz);

    fragColor = inColor;
    fragUV = inUV;

    vec4 tTangent = push.normalMatrix * vec4(inTangent, 0.0);
    fragTangent = normalize(tTangent.xyz);

    fragCameraPosition = ubo.cameraPosition;
    fragVertexPosition = push.modelMatrix * vec4(inPosition, 1.0);
    fragRenderMode = ubo.rendermode;
}
