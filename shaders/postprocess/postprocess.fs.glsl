


#version 330 core
out vec4 FragColor;

in vec2 v_TexCoords;

uniform sampler2D u_ScreenTexture;
uniform int u_EnableEffect; 

void main() {
    vec4 texColor = texture(u_ScreenTexture, v_TexCoords);

    if (u_EnableEffect == 1) {
       vec4 color = texture(u_ScreenTexture, v_TexCoords);

    vec3 sepia = vec3(
        dot(color.rgb, vec3(0.393, 0.769, 0.189)),
        dot(color.rgb, vec3(0.349, 0.686, 0.168)),
        dot(color.rgb, vec3(0.272, 0.534, 0.131))
    );
    FragColor = vec4(sepia, 1.0);
    } else {
        FragColor = texColor;
    }
}