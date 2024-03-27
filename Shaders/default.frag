#version 330 core

in vec3 fragmentColor;
in vec2 uv;
in vec3 normal;
in vec3 updatedPos;

out vec4 FragColor;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

uniform sampler2D diffuse0;
uniform sampler2D diffuse1;
uniform sampler2D specular0;

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
	vec3 direction = vec3(1.0f, 1.0f, 0.0f);
	float ambient = 0.20f;
	float specularLight = 0.50f;

	vec3 norm = normalize(normal);
	vec3 lightDirection = normalize(direction);
	float diffuse = max(dot(norm, lightDirection), 0.0f);

	vec3 viewDirection = normalize(camPos - updatedPos);
	vec3 reflectionDirection = reflect(-lightDirection, norm);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	//FragColor = 
	return (texture(diffuse0, uv) * (diffuse + ambient) + texture(diffuse1, uv).r * specular) * lightColor;
}

void main()
{
	//TODO: hard coded condition for now, find a better way to do condition
	bool flag = true;
	if(flag) {
		FragColor = texture(diffuse0, uv);
	}

	else if(!flag) {
		float r = 127/255.0;
		float g = 127/255.0;
		float b = 127/255.0;
		float a = 255/255.0;
		FragColor = vec4(r, g, b, a);
	}
	
	else {
		float ambient = 0.20f;
		float specularLight = 0.50f;

		vec3 norm = normalize(normal);
		vec3 lightDirection = normalize(lightPos - updatedPos);
		float diffuse = max(dot(norm, lightDirection), 0.0f);

		vec3 viewDirection = normalize(camPos - updatedPos);
		vec3 reflectionDirection = reflect(-lightDirection, norm);
		float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
		float specular = specAmount * specularLight;

		FragColor = pointLight();
		// FragColor = directionalLight();
		// FragColor = spotLight();
	
	}
}
