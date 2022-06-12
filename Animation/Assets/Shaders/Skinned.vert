#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec4 weights;
layout (location = 4) in ivec4 joints;

layout (location = 0) out vec3 normal;
layout (location = 1) out vec2 uv;
layout (location = 2) out vec3 fragPos;

layout (location = 5) uniform mat4 model;
layout (location = 6) uniform mat4 view;
layout (location = 7) uniform mat4 projection;

layout (location = 10) uniform mat4 animationPose[120];
layout (location = 130) uniform mat4 inverseBindPose[120];

void main()
{
    mat4 skin  = (animationPose[joints.x] * inverseBindPose[joints.x]) * weights.x;
         skin += (animationPose[joints.y] * inverseBindPose[joints.y]) * weights.y;
         skin += (animationPose[joints.z] * inverseBindPose[joints.w]) * weights.z;
         skin += (animationPose[joints.w] * inverseBindPose[joints.z]) * weights.w;

    gl_Position = projection * view * model * skin * vec4(aPosition, 1.0);

    fragPos = vec3(model * skin * vec4(aPosition, 1.0));
    normal = vec3(model * skin * vec4(aNormal, 0.0f));
    uv = aUV;
}