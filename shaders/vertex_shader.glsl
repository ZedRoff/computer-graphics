#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 v_FragPos;  
out vec3 v_Normal;   
out vec2 v_TexCoords;

uniform mat4 u_MVP;
uniform mat4 u_Model;

void main() {
    gl_Position = u_MVP * vec4(aPos, 1.0);
    v_FragPos = vec3(u_Model * vec4(aPos, 1.0));
    v_Normal = mat3(u_Model) * aNormal; 
    v_TexCoords = aTexCoords;
}