#version 330 core
out vec4 FragColor;


uniform vec3 u_diffuseColor;

void main() {
   
        FragColor = vec4(u_diffuseColor, 1.0);
   
}