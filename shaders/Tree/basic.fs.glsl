#version 330 core
out vec4 FragColor;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoords;

uniform vec3 u_ambientColor;
uniform vec3 u_diffuseColor;
uniform vec3 u_specularColor;
uniform float u_shininess;
uniform bool u_hasTexture;

uniform sampler2D u_Texture;

uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform vec3 u_lightColor;

void main() {
    vec4 baseColor = vec4(u_diffuseColor, 1.0);
    if (u_hasTexture) {
        vec4 texColor = texture(u_Texture, v_TexCoords);
        if (texColor.a > 0.1) {
            baseColor = texColor;
        }
    }
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_lightPos - v_FragPos);
    vec3 viewDir = normalize(u_viewPos - v_FragPos);

    vec3 ambient = u_ambientColor * baseColor.rgb;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_diffuseColor * diff * baseColor.rgb * u_lightColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), u_shininess);
    vec3 specular = u_specularColor * spec * u_lightColor;

    vec3 finalColor = ambient + diffuse + specular;

    FragColor = vec4(finalColor, baseColor.a);
}