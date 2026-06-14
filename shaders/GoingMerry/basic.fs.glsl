#version 330 core
out vec4 FragColor;

in vec2 v_TexCoords; 

uniform sampler2D u_Texture; 
uniform vec3 u_diffuseColor;

void main() {
    vec4 texColor = texture(u_Texture, v_TexCoords);
    
    if (texColor.a < 0.1) {
        FragColor = vec4(u_diffuseColor, 1.0);
    } else {
        FragColor = texColor;
    }
}