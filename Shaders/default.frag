#version 330 core

in vec3 fragmentColor;
in vec2 uv;
in vec3 normal;
in vec3 updatedPos;
in vec4 fragPosLight;

out vec4 FragColor;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;
uniform float ambientIntensity;
uniform int sampleRadius = 5;
uniform bool useTexture = false;

uniform sampler2D diffuse0;
uniform sampler2D diffuse1;
uniform sampler2D shadowMap;
uniform sampler2D specular0;

vec4 toonShader() {
	float intensity;
	vec4 color;
	vec3 lightDir = normalize(lightPos - updatedPos);
	intensity = dot(lightDir,normal);

	if (intensity > 0.95)
		color = vec4(0.5,0.5,0.5,1.0);
	else if (intensity > 0.5)
		color = vec4(0.3,0.3,0.3,1.0);
	else if (intensity > 0.25)
		color = vec4(0.2,0.2,0.2,1.0);
	else
		color = vec4(0.1,0.1,0.1,1.0);
	//#FFE5D8FF skin color
	return color;
}


float calcShadow() {
	vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;   
	float currentDepth = projCoords.z;  
	float bias = max(0.05 * (1.0 - dot(normal, normalize(lightPos - updatedPos))), 0.005); 
	
	float shadow = 0.0;
	//shadow = (currentDepth > closestDepth + bias ) ? 1.0 : 0.0; 
	
	// Smoothens out the shadows
	vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
	for(int y = -sampleRadius; y <= sampleRadius; y++)
	{
		for(int x = -sampleRadius; x <= sampleRadius; x++)
		{
		    float closestDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * pixelSize).r;
			if (currentDepth > closestDepth + bias)
				shadow += 1.0f;     
		}    
	}
	// Get average shadow
	shadow /= pow((sampleRadius * 2 + 1), 2);

	return shadow;
}

vec4 pointLight() {
	vec3 lightVector = lightPos - updatedPos;
	float dist = length(lightVector);
	float a = 1.0; //TODO: send a uniform for both so we can update them
	float b = 0.01;
	float intensity = 1.0f / (a * dist * dist + b * dist + 1.0f);

	float ambient = 0.20f;
	float specularLight = 0.50f;

	vec3 norm = normalize(normal);
	vec3 lightDirection = normalize(lightVector);
	float diffuse = max(dot(norm, lightDirection), 0.0f);

	vec3 viewDirection = normalize(camPos - updatedPos);
	vec3 reflectionDirection = reflect(-lightDirection, norm);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(diffuse0, uv) * (diffuse * intensity + ambient) + texture(diffuse1, uv).r * specular * intensity) * lightColor;
}

vec4 spotLight() {
	float outerCone = 0.90f;
	float innerCone = 0.95f;
	float ambient = 0.20f;
	float specularLight = 0.50f;

	vec3 norm = normalize(normal);
	vec3 lightDirection = normalize(lightPos - updatedPos);
	float diffuse = max(dot(norm, lightDirection), 0.0f);

	vec3 viewDirection = normalize(camPos - updatedPos);
	vec3 reflectionDirection = reflect(-lightDirection, norm);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float intensity = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(diffuse0, uv) * (diffuse * intensity + ambient) + texture(diffuse1, uv).r * specular * intensity) * lightColor;
}


vec4 directionalLight() {
/*
	vec3 direction = vec3(-1.0f, -1.0f, 1.0f);
	float ambient = 0.45f;
	float specularLight = 0.50f;

	vec3 norm = normalize(normal);
	vec3 lightDirection = normalize(direction);
	float diffuse = max(dot(norm, lightDirection), 0.0f);

	vec3 viewDirection = normalize(camPos - updatedPos);
	vec3 reflectionDirection = reflect(-lightDirection, norm);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	float shadow = 0.0f;
	// Sets lightCoords to cull space
	vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
	if(lightCoords.z <= 1.0f)
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float currentDepth = lightCoords.z;
		// Prevents shadow acne
		float bias = max(0.025f * (1.0f - dot(normal, lightDirection)), 0.0005f);

		// Smoothens out the shadows
		int sampleRadius = 20;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;     
		    }    
		}
		// Get average shadow
		shadow /= pow((sampleRadius * 2 + 1), 2);
	}
	shadow = calcShadow(fragPosLight);

	return (texture(diffuse0, uv) * (diffuse * (1.0f - shadow) + ambient) + texture(diffuse1, uv).r * specular * (1.0f - shadow)) * lightColor;
*/

	// ambient
    vec3 ambient = ambientIntensity * texture(diffuse0, uv).rgb;
  	
    // diffuse 
    vec3 norm = normalize(normal);
    // vec3 lightDir = normalize(light.position - FragPos);
    vec3 lightDir = normalize(-(lightPos - updatedPos));  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(0.5f, 0.5f, 0.5f) * diff * texture(diffuse0, uv).rgb;  
    
    // specular
    vec3 viewDir = normalize(camPos - updatedPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    vec3 specular = vec3(1.0, 1.0, 1.0) * spec * texture(diffuse1, uv).rgb;  

	float shadow = calcShadow();
    vec3 result = (ambient + diffuse + specular) * (1.0 - shadow);

	return vec4(result, 1.0);


}

void main()
{
    vec3 color = useTexture ? texture(diffuse0, uv).rgb : vec3(0.5f, 0.5f, 0.5f);
    vec3 normal = normalize(normal);
    // ambient
    vec3 ambient = ambientIntensity * lightColor.xyz;
    // diffuse
    vec3 lightDir = normalize(lightPos - updatedPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor.xyz;
    // specular
    vec3 viewDir = normalize(camPos - updatedPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor.xyz;    
    // calculate shadow
    float shadow = calcShadow();                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
	vec4 toonShading = toonShader();
    FragColor = vec4(lighting, 1.0);

	//FragColor = pointLight();
	//FragColor = directionalLight();
	//FragColor = spotLight();
}
