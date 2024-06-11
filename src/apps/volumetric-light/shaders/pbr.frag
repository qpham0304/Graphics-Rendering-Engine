#version 460 core

out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 updatedPos;
in vec4 fragPosLight;

uniform vec3 lightPos;
uniform vec3 camPos;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D depthMap;

float calcShadow() {
	vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(depthMap, projCoords.xy).r;   
	float currentDepth = projCoords.z;  
	
	float bias = max(0.05 * (1.0 - dot(normal, normalize(lightPos - updatedPos))), 0.005); 
	
	float shadow = 0.0;
	if(currentDepth > 1.0f)
		return shadow;
	//shadow = (currentDepth > closestDepth + bias ) ? 1.0 : 0.0; 
	
	// sample area for soft shadow
	vec2 pixelSize = 1.0 / textureSize(depthMap, 0);
	for(int y = -2; y <= 2; y++)
	{
		for(int x = -2; x <= 2; x++)
		{
		    float closestDepth = texture(depthMap, projCoords.xy + vec2(x, y) * pixelSize).r;
			if (currentDepth > closestDepth + bias)
				shadow += 1.0f;     
		}    
	}
	// Get average shadow
	shadow /= pow((2 * 2 + 1), 2);

	return shadow;
}

void main() {
    vec4  ambient = texture(diffuseMap, uv) * 0.5;

    vec3 lightDir = normalize(lightPos - updatedPos);
    
    // vec3 directionLight = vec3(-1.0f, -1.0f, -1.0f);
    // lightDir = normalize(-directionLight);    // for directional light
    vec3 viewDir = normalize(camPos - updatedPos);
    float distance = length(lightPos - updatedPos);

    float lightConstant = 1.0f;
    float lightLinear = 0.09f;
    float lightQuadratic = 0.032f;
    float attenuation = 1.0 / (lightConstant + lightLinear * distance + lightQuadratic * (distance * distance));

    float NdotL = max(0.0, dot(normal, lightDir));
    vec3 diffuse = NdotL * texture(diffuseMap, uv).rgb;

    vec3 halfwayDir = normalize(viewDir + lightDir);
    float NdotR = max(0.0, dot(normal, halfwayDir));
    vec3 specular = pow(NdotR, 32) * texture(specularMap, uv).rgb;


    float shadow = calcShadow();
    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);
    vec3 light = vec3(ambient) + diffuse + specular;
    light *= attenuation;

    FragColor = vec4(light, 1.0);
}