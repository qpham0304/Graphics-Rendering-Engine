#version 330 core

out vec4 color;
uniform vec3 lightColor;

void main()
{
	vec3 c = vec3(1.0f, 1.0f, 0.0f);
	color = vec4(lightColor, 1.0f);
}
