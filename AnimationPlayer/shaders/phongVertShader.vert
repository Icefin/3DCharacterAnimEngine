#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;

uniform mat4 worldMat, viewMat, projMat;
uniform vec3 eyePos;

out vec3 vertexNormal, vertexView;
out vec4 vertexColor;

void main()
{
    vertexNormal = normalize(transpose(inverse(mat3(worldMat))) * normal);
    vec3 worldPos = (modelMat * vec4(pos, 1.0)).xyz;
    
    vertexView = normalize(eyePos - worldPos);
    gl_Position = projMat * viewMat * worldMat * vec4(pos, 1.0f);
    vertexColor = vec4(color, 1.0);
} 