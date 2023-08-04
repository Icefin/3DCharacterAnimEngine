#version 330 core

in vec3 vertexNormal;
in vec3 vertexColor;
in vec3 viewDirection;

out vec4 fragColor;

uniform vec3 Ka,Ks,Ke;
uniform vec3 Sa,Ss,Sd;
uniform vec3 lightDir;
uniform float sh;

void main(){
    vec3 normal = normalize(vertexNormal);
    vec3 view = normalize(viewDirection);
    vec3 light = normalize(lightDir);

    vec3 diff = max(dot(normal, light), 0.0) * Sd * vertexColor;

    vec3 refl = 2.0 * normal * dot(normal, light) - light;
    vec3 spec = pow(max(dot(refl, view), 0), sh) * Ss * Ks;

    vec3 ambi = Ka * Sa;

    fragColor = vec4(diff + spec + ambi + Ke, 1.0);

}