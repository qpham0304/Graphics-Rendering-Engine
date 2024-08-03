#pragma once
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <vector>
#include <string>
#include <iostream>

class Skybox
{
public:
	Skybox(std::vector<std::string> faces);
	~Skybox();
	void updateTexture(const unsigned int& id);
	unsigned int textureID();

private:
	unsigned int ID;
	unsigned int loadCubeMap(std::vector<std::string> faces);
	void Delete();
};

