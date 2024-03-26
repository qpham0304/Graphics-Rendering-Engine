#pragma once
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <vector>
#include <string>
#include <iostream>

class Skybox
{
public:
	GLuint ID;
	char* path;


	Skybox(std::vector<std::string> faces);

private:
	GLuint loadCubeMap(std::vector<std::string> faces);
};

