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
	Texture(const char* path, const char* texType);
	Texture(const char* fileName, const char* texType, const std::string& directory);
	~Texture() = default;

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();

	static void BIND_ALBEDO();
	static void BIND_NORMAL();
	static void BIND_METALLIC();
	static void BIND_ROUGHNESS();
	static void BIND_AO();
	static void BIND_IRRADIANCE();
	static void BIND_PREFILTER();
	static void BIND_BRDF_LUT_TEXTURE();
	static void BIND_HEIGHT();
	static void BIND_SHADOW();
};