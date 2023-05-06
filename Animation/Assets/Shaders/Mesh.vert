#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

layout (location = 0) out vec3 normal;
layout (location = 1) out vec2 uv;
layout (location = 2) out vec3 fragPos;

layout (location = 3) uniform mat4 model;
layout (location = 4) uniform mat4 view;
layout (location = 5) uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0);

    fragPos = vec3(model * vec4(aPosition, 1.0));
    normal = vec3(model * vec4(aNormal, 0.0));
    uv = aUV;
}