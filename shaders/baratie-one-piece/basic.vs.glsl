#version 330 core
layout (location = 0) in vec3 aPos; 

uniform mat4 u_Model;

layout (std140) uniform ViewProj {
    mat4 view;
    mat4 projection;
};

void main()
{
    gl_Position = projection * view * u_Model * vec4(aPos, 1.0);
}