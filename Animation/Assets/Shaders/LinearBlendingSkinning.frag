#version 460 core

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 fragPos;

uniform vec3 lightDirection;
uniform sampler2D baseTexture;

layout (location = 0) out vec4 FragColor;

void main()
{
    vec4 albedo = texture(baseTexture, uv);

    vec3 n = normalize(normal);
    vec3 l = normalize(lightDirection);

    float diffuseIntensity = clamp(dot(n, l) + 0.1, 0.0, 1.0);

    FragColor = albedo * diffuseIntensity;
}