#version 450

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D normalSampler;
layout(binding = 3) uniform sampler2D specularSampler;
layout(binding = 4) uniform sampler2D glossSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;
layout(location = 3) in vec3 fragTangent;
layout(location = 4) in vec3 fragCameraPosition;
layout(location = 5) in vec4 fragVertexPosition;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;

vec4 CalculateLambert(float kd, vec3 cd) {
    return vec4(cd * kd / PI, 1.0);
}

void main() 
{
    const vec3 lightDirection = normalize(vec3(0.5f, 0.8f, 0.5f));
    const float lightIntensity = 7.0f;
    const vec3 ambientLighting = vec3(0.03f, 0.03f, 0.03f);

    vec3 sampledNormal = texture(normalSampler, fragUV).rgb;
    sampledNormal = normalize(sampledNormal * 2.0 - 1.0);
    vec3 binormal = cross(fragNormal, fragTangent);
    mat3 tangentSpaceAxis = mat3(fragTangent, binormal, fragNormal);
    vec3 finalNormal = normalize(tangentSpaceAxis * sampledNormal);

    vec3 albedo = texture(texSampler, fragUV).rgb;
    float glossinessMap = texture(glossSampler, fragUV).r;
    vec3 specularMap = texture(specularSampler, fragUV).rgb;

    vec3 viewDir = normalize(fragCameraPosition - fragVertexPosition.xyz);

    const float observedArea = max(dot(finalNormal, -lightDirection), 0.0);

    vec4 lambert = CalculateLambert(1.0, albedo);

    float shininess = 100.0f; 
    float specularExp = shininess * glossinessMap;
    float specular = pow(max(dot(viewDir, reflect(normalize(lightDirection), finalNormal)), 0.0), specularExp) * specularMap.r;

    vec3 finalColor = (lightIntensity * lambert.rgb + specular) * observedArea;
    outColor = vec4(finalColor, 1.0);
}
