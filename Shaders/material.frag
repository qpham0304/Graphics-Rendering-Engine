#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 uv;
in vec3 normal;
in vec3 curFragPos;

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

uniform sampler2D diffuse0;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform vec3 lightColor;


void main()
{ 
    // float ambient = 0.20f;
    // float specularIntensity = 0.5f;
    
    // vec3 lightDirection = normalize(light.position - curFragPos);
    // float dif = max(dot(normal, lightDirection), 0.0f);
    // vec3 diffuse = dif * vec3(lightColor);

    // vec3 viewDir = normalize(viewPos - curFragPos);
    // vec3 reflectDir = reflect(-lightDirection, normal);
    // float specAmount = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    // vec3 specular = specularIntensity * specAmount * lightColor;
    
    // vec3 result = (ambient + diffuse + specular) * color;
    // FragColor = texture(diffuse0, uv) * vec4(result, 1.0);

    // ambient
    vec3 ambient = light.ambient * material.ambient;
  	
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - curFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - curFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
        
    vec3 result = ambient + diffuse + specular;
    FragColor = texture(diffuse0, uv) * vec4(result, 1.0);
}