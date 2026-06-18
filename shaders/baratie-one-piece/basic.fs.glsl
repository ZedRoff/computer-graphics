#version 330 core

struct Material {
    vec3 diffuseColor; 
};

uniform Material u_material;

out vec4 FragColor;

void main()
{
    FragColor = vec4(u_material.diffuseColor, 1.0);
}