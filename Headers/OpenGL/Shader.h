#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<unordered_map>
#include <cstring>

std::string get_file_contents(const char* filename);

class Shader
{
private:
	GLuint id;
	GLuint getUniformLocation(const std::string& name) const;

public:
	// Reference ID of the Shader Program
	GLuint ID;
	std::string type;
	mutable std::unordered_map<std::string, GLuint> cache;
	
	Shader();
	Shader(const char* vertexFile, const char* fragmentFile);
	Shader& operator=(const Shader& other);
	~Shader();

	void Activate();
	void Delete();
	void compileErrors(unsigned int shader, const char* type);

	// utility uniform functions
	void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, const glm::vec2& value);
    void setVec2(const std::string& name, float x, float y);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setMat2(const std::string& name, const glm::mat2& mat);
    void setMat3(const std::string& name, const glm::mat3& mat);
    void setMat4(const std::string& name, const glm::mat4& mat);

};

#endif