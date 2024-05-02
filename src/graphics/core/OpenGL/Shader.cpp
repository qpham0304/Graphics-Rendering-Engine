#include"Shader.h"

Shader::Shader() = default;

Shader::Shader(const Shader& other)
{
	this->ID = other.ID;
	this->type = other.type;
	this->cache = other.cache;
}

static bool validateFormat(const char* str) {
	size_t len = std::strlen(str);
	if (len >= 5)	// .frag and .vert length
		return std::strcmp(str + len - 5, ".vert") == 0 || std::strcmp(str + len - 5, ".frag") == 0;
	return false;
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	//TODO properly validate format 
	//if false, use a default shader or something to prevent crash maybe?
	if (!(validateFormat(vertexFile) && validateFormat(fragmentFile))) {
		std::cerr << "invalid file format, must be .frag or .vert" << std::endl;
	}

	const char* fileName = std::strrchr(fragmentFile, '/');
	if (fileName != nullptr)
		type = fileName + 1;
	else
		type = fragmentFile;
	type = type.substr(0, type.size() - 5);

	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	compileErrors(vertexShader, "VERTEX");

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	compileErrors(fragmentShader, "FRAGMENT");

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

Shader& Shader::operator=(const Shader& other) {
	if (this != &other) {
		this->ID = other.ID;
	}
	return *this;
}

Shader::Shader(Shader&& other) noexcept : ID(std::exchange(other.ID, 0)), type(std::move(other.type)), cache(std::move(other.cache)) {}

Shader& Shader::operator=(Shader&& other) noexcept {
	if (this != &other) {
		ID = std::exchange(other.ID, 0);
		type = std::move(other.type);
		cache = std::move(other.cache);
	}
	return *this;
}


Shader::~Shader()
{
	glDeleteProgram(ID);
}

// Activates the Shader Program
void Shader::Activate()
{
	glUseProgram(ID);
}

// Deletes the Shader Program
void Shader::Delete()
{
	glDeleteProgram(ID);
}


// Reads a text file and outputs a string with everything in the text file
std::string get_file_contents(const char* filepath)
{
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << filepath << std::endl;
		return "";
	}

	//typedef std::istreambuf_iterator<char> charStreamIterator;
	using charStreamIterator = std::istreambuf_iterator<char>;
	std::string content((charStreamIterator(file)), (charStreamIterator()));

	file.close();
	return content;
}

// Checks if the different Shaders have compiled properly
void Shader::compileErrors(unsigned int shader, const char* type)
{
	// Stores status of compilation
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}


GLuint Shader::getUniformLocation(const std::string& name) const
{
	if (cache.find(name) != cache.end())
		return cache[name];
	GLuint location = glGetUniformLocation(ID, name.c_str());
	cache[name] = location;
	return location;

}

void Shader::setBool(const std::string& name, bool value)
{
	glUniform1i(getUniformLocation(name), (int)value);
}

void Shader::setInt(const std::string& name, int value)
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value)
{
	glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value)
{
	glUniform2fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y)
{
	glUniform2f(getUniformLocation(name), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value)
{
	glUniform3fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z)
{
	glUniform3f(getUniformLocation(name), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value)
{
	glUniform4fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(getUniformLocation(name), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat)
{
	glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat)
{
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}