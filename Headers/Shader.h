#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
private:
	GLuint id;

public:
	// Reference ID of the Shader Program
	GLuint ID;
	Shader();
	Shader& operator=(const Shader& other);

	//Shader& operator=(const Shader& other);
	
	// Constructor that build the Shader Program from 2 different shaders
	Shader(const char* vertexFile, const char* fragmentFile);

	// Activates the Shader Program
	void Activate();
	// Deletes the Shader Program
	void Delete();

	void compileErrors(unsigned int shader, const char* type);

};

#endif