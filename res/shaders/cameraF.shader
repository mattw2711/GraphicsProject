#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;       // Fragment position in world coordinates
in vec3 Norm;

uniform sampler2D texture_diffuse1;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

void main()
{    
    // Ambient
    vec3 ambient = material.ambient * texture(texture_diffuse1, TexCoords).rgb;

    // Diffuse
    vec3 norm = normalize(Norm);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;

    // Specular
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec;

    // Final result
    vec3 result = ambient + light.ambient + diffuse * light.diffuse + specular * light.specular;
    FragColor = vec4(result, 1.0);
}