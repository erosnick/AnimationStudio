#version 460 core

layout (location = 0) in vec3 normal;
layout (location = 1) in vec3 fragPos;
layout (location = 2) in vec3 uv;

uniform vec3 lightDirection;
uniform sampler2D baseTexture;

layout (location = 0) out vec4 FragColor;

void main()
{
    vec4 albedo = vec4(1.0, 1.0, 1.0, 1.0);

    vec3 n = normalize(normal);
    vec3 l = normalize(lightDirection);

    float diffuseIntensity = clamp(dot(n, l), 0.0, 1.0);

    FragColor = albedo * diffuseIntensity;
}