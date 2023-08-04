#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;

out vec3 vertexNormal;
out vec3 vertexColor;
out vec3 viewDirection;

uniform mat4 worldMat,viewMat, projMat;
uniform vec3 eyePos;

void main(){
    vertexNormal = normalize(transpose(inverse(mat3(worldMat))) * normal);

    vec3 worldPos = (worldMat * vec4(position, 1.0)).xyz;
    viewDirection = normalize(eyePos - worldPos);

    vertexColor = color;

    gl_Position = projMat * viewMat * vec4(worldPos, 1.0);
}