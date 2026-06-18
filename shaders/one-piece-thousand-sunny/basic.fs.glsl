#version 330 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoords;

struct Light {
    vec3 direction;    
    vec3 diffuseColor;  
    vec3 specularColor; 
    vec3 ambientColor;  
};

struct Material {
    vec3 ambientColor;
    vec3 diffuseColor; 
    vec3 specularColor;
    float shininess;   
};

uniform Light u_light;
uniform Material u_material;

uniform sampler2D u_Texture;
uniform int u_hasTexture;
uniform vec3 u_viewPos; 

out vec4 FragColor;

void main()
{
    vec3 N = normalize(v_Normal);
    vec3 L = normalize(u_light.direction);

    vec3 ambient = u_material.ambientColor * u_light.ambientColor;

    float NdotL = max(dot(N, L), 0.0);
    vec3 baseColor = u_material.diffuseColor;
    if (u_hasTexture == 1) {
        baseColor = texture(u_Texture, v_TexCoords).rgb * u_material.diffuseColor;
    }
    vec3 diffuse = NdotL * u_light.diffuseColor * baseColor;

    vec3 specular = vec3(0.0);
    if (NdotL > 0.0) {
        vec3 V = normalize(u_viewPos - v_FragPos);
        
        vec3 R = reflect(-L, N);
        
        float specFactor = pow(max(dot(R, V), 0.0), u_material.shininess);
        specular = specFactor * u_light.specularColor * u_material.specularColor;
    }

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}