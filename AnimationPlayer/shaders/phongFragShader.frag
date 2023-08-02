#version 330 core
uniform vec3 matSpec, matAmbi, matEmit;
uniform float matSh;
uniform vec3 srcDiff, srcSpec, srcAmbi;
uniform vec3 lightDir;

in vec3 vertexNormal, vertexView;
in vec4 vertexColor;

out vec4 fragColor;

void main()
{
    //normalization
    vec3 normal = normalize(vertexNormal);
    vec3 view = normalize(vertexView);
    vec3 light = normalize(lightDir);

    //diffuse term
    vec3 matDiff = vertexColor.xyz;
    vec3 diff = max(dot(normal, light), 0.0) * srcDiff * matDiff;

    //specular term
    vec3 reflect = 2.0 * normal * dot(normal, light) - light;
    vec3 spec = pow(max(dot(reflect, view), 0.0), matSh) * srcSpec * matSpec;

    //ambient term
    vec3 ambi = srcAmbi * matAmbi;

    fragColor = vec4(diff + spec + ambi + matEmit, 1.0);
}