#version 330 core

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 specular;
    vec3 diffuse;
};

struct Material {
    vec3  ambient;
    vec3  specular;
    vec3  emissive;
    float shininess;
};

in vec3 vertexNormal;
in vec3 vertexColor;
in vec3 viewDirection;

out vec4 fragColor;

uniform Light    phongLight;
uniform Material material;

void main(){
    vec3 normal = normalize(vertexNormal);
    vec3 view = normalize(viewDirection);
    vec3 light = normalize(phongLight.direction);

    vec3 diff = max(dot(normal, light), 0.0) * phongLight.diffuse * vertexColor;

    vec3 refl = 2.0 * normal * dot(normal, light) - light;
    vec3 spec = pow(max(dot(refl, view), 0), material.shininess) * phongLight.specular * material.specular;

    vec3 ambi = material.ambient * phongLight.ambient;

    fragColor = vec4(diff + spec + ambi + material.emissive, 1.0);

}