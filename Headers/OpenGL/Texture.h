#pragma once

#include<glad/glad.h>
#include<stb/stb_image.h>
#include"Shader.h"

class Texture
{
private:
	void loadTexture(const char* path, bool flip);
public:
	GLuint ID;
	const char* type;
	GLuint unit = 0;
	std::string path;

	Texture() = default;
	Texture(const char* path);
	Texture(const char* path, const char* texType, const std::string& directory);
	~Texture() = default;

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};