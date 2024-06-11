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

const int SCREEN_WIDTH = 2460;
const int SCREEN_HEIGHT = 1440;
const int NUM_STEPS_INT = 15;
const float NUM_STEPS = float(NUM_STEPS_INT);
const float G = 0.7f; // Controls how much light will scatter in the forward direction
const float PI = 3.14159265359f;
const float SPECULAR_FACTOR = 16.0f;
const mat4 DITHER_PATTERN = mat4
    (vec4(0.0f, 0.5f, 0.125f, 0.625f),
     vec4(0.75f, 0.22f, 0.875f, 0.375f),
     vec4(0.1875f, 0.6875f, 0.0625f, 0.5625f),
     vec4(0.9375f, 0.4375f, 0.8125f, 0.3125f));
	 
// Henyey-Greenstein phase function
float mieScattering(float cosTheta) {
	float numerator = (1.0f - G*G);
	float denominator =  (4.0f * PI * pow(1.0f + G * G - 2.0f * G * cosTheta, 1.5f));
	return numerator / denominator;
}



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

	vec3 L = normalize(-lightDir);
	vec3 V = updatedPos - camPos;
	float stepSize = length(V) / NUM_STEPS;
	V = normalize(V);
	vec3 step = V * stepSize;
	vec3 position = camPos;
	position += step * DITHER_PATTERN[int(uv.x * SCREEN_WIDTH) % 4][int(uv.y * SCREEN_HEIGHT) % 4];
	vec3 volume = vec3(0.0f);
	for(int i = 0; i < NUM_STEPS; i++) {
		vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
		projCoords = projCoords * 0.5 + 0.5;
		float closestDepth = texture(depthMap, projCoords.xy).r;
		float currentDepth = projCoords.z;
		
		if(closestDepth > currentDepth)
			volume += mieScattering(dot(V, -L)) * vec3(1.0, 1.0, 1.0);
		position += step;
	}
	volume /= NUM_STEPS;


    float shadow = calcShadow();
    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);
    vec3 light = vec3(ambient) + diffuse + specular + volume;
    light *= attenuation;

    FragColor = vec4(light, 1.0);
}