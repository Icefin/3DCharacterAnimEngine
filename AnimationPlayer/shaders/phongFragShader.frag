#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shiness;
}

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}

uniform Material material;
uniform Light light;

in vec3 vertexNormal, vertexView;
in vec4 vertexColor;

out vec4 fragColor;

void main()
{
    //diffuse term
    vec3 normal = normalize(vertexNormal);
    vec3 light = normalize(light.direction);
    vec3 diffuse = max(dot(normal, light), 0.0) * light.diffuse * material.diffuse;

    //specular term
    vec3 view = normalize(vertexView);
    vec3 reflect = 2.0 * normal * dot(normal, light) - light;
    vec3 specular = pow(max(dot(reflect, view), 0.0), material.shiness) * light.specular * material.specular;

    //ambient term
    vec3 ambient = light.ambient * material.ambient;

    fragColor = vec4(diffuse + specular + ambient, 1.0);
}