#version 330 core
out vec4 FragColor;

in vec2 v_TexCoords; // Reçu du Vertex Shader

uniform sampler2D u_Texture; // Représente image PNG
uniform vec3 u_diffuseColor;

void main() {
    // Échantillonnage de la couleur du PNG aux coordonnées UV
    vec4 texColor = texture(u_Texture, v_TexCoords);
    
    // SÉCURITÉ : Si le mesh n'a pas de texture (couleur blanche transparente), 
    // ou si l'image est vide, on applique la couleur diffuse du mtl.
    if (texColor.a < 0.1) {
        FragColor = vec4(u_diffuseColor, 1.0);
    } else {
        FragColor = texColor;
    }
}