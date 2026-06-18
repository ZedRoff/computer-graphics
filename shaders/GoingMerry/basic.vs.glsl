#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoords;

uniform mat4 u_Model;

layout (std140) uniform ViewProj {
    mat4 view;
    mat4 projection;
};

void main()
{
    v_FragPos = vec3(u_Model * vec4(aPos, 1.0));

    v_Normal = mat3(u_Model) * aNormal;

    v_TexCoords = aTexCoords;
    
    gl_Position = projection * view * vec4(v_FragPos, 1.0);
}
