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

layout (location = 10) uniform mat2x4 animationPose[120];
layout (location = 130) uniform mat2x4 inverseBindPose[120];

vec4 multiplyQuaternion(vec4 q1, vec4 q2)
{
    return vec4(
      q2.x * q1.w + q2.y * q1.z - q2.z * q1.y + q2.w * q1.x,
     -q2.x * q1.z + q2.y * q1.w + q2.z * q1.x + q2.w * q1.y,
      q2.x * q1.y - q2.y * q1.x + q2.z * q1.w + q2.w * q1.z,
     -q2.x * q1.x - q2.y * q1.y - q2.z * q1.z + q2.w * q1.w);
}

mat2x4 normalizeDualQuaternion(mat2x4 dualQuaternion)
{
    float inverseMagnitude = 1.0 / length(dualQuaternion[0]);

    dualQuaternion[0] *= inverseMagnitude;
    dualQuaternion[1] *= inverseMagnitude;

    return dualQuaternion;
}

mat2x4 combineDualQuaternion(mat2x4 left, mat2x4 right)
{
    left = normalizeDualQuaternion(left);
    right = normalizeDualQuaternion(right);

    vec4 real = multiplyQuaternion(left[0], right[0]);
    vec4 dual = multiplyQuaternion(left[0], right[1]) + multiplyQuaternion(left[1], right[0]);

    return mat2x4(real, dual);
}

vec4 transformVector(mat2x4 dualQuaternion, vec3 vector)
{
    vec4 real = dualQuaternion[0];

    vec3 rotationVector = real.xyz;
    float scalar = real.w;

    vec3 rotatedVector = rotationVector * 2.0 * dot(rotationVector, vector) + 
    vector * (scalar * scalar - dot(rotationVector, rotationVector)) + 
    cross(rotationVector, vector) * 2.0 * scalar;

    return vec4(rotatedVector, 0.0);
}

vec4 transformPoint(mat2x4 dualQuaternion, vec3 vector)
{
    vec4 real = dualQuaternion[0];
    vec4 dual = dualQuaternion[1];

    vec3 rotatedVector = transformVector(dualQuaternion, vector).xyz;
    vec4 conjugate = vec4(-real.xyz, real.w);
    vec3 translation = multiplyQuaternion(conjugate, dual * 2.0).xyz;

    return vec4(rotatedVector + translation, 1.0);
}

void main()
{
    vec4 weightsCopy = weights;
    // Neighborhood all of the quaternions correctly
    if (dot(animationPose[joints.x][0], animationPose[joints.y][0]) < 0.0)
    {
        weightsCopy.y *= -1.0;
    }

    if (dot(animationPose[joints.x][0], animationPose[joints.z][0]) < 0.0)
    {
        weightsCopy.z *= -1.0;
    }

    if (dot(animationPose[joints.x][0], animationPose[joints.w][0]) < 0.0)
    {
        weightsCopy.w *= -1.0;
    }

    // Combine
    mat2x4 dualQuaternion0 = combineDualQuaternion(inverseBindPose[joints.x], animationPose[joints.x]);
    mat2x4 dualQuaternion1 = combineDualQuaternion(inverseBindPose[joints.y], animationPose[joints.y]);
    mat2x4 dualQuaternion2 = combineDualQuaternion(inverseBindPose[joints.z], animationPose[joints.z]);
    mat2x4 dualQuaternion3 = combineDualQuaternion(inverseBindPose[joints.w], animationPose[joints.w]);

    mat2x4 skinDualQuaternion = weightsCopy.x * dualQuaternion0 + 
                                weightsCopy.y * dualQuaternion1 + 
                                weightsCopy.z * dualQuaternion2 + 
                                weightsCopy.w * dualQuaternion3;

    skinDualQuaternion = normalizeDualQuaternion(skinDualQuaternion);

    vec4 position = transformPoint(skinDualQuaternion, aPosition);
    gl_Position = projection * view * model * position;
    fragPos = vec3(model * position);
    normal = transformVector(skinDualQuaternion, aNormal).xyz;
    normal = vec3(model * vec4(normal, 0.0));
    uv = aUV;
}