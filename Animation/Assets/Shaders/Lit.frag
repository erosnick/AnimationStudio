#version 460 core

in vec3 normal;
in vec3 fragPos;
in vec2 uv;

uniform vec3 lightDirection;
uniform sampler2D baseTexture;

out vec4 FragColor;

void main()
{
    vec4 albedo = texture(baseTexture, uv);

    vec3 n = normalize(normal);
    vec3 l = normalize(lightDirection);

    float diffuseIntensity = clamp(dot(n, l), 0.0, 1.0);

    FragColor = albedo * diffuseIntensity;
}