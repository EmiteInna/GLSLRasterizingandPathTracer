#include "Shader.h"
Shader::Shader(const std::string& vertex, const std::string& fragment) {
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vertexFile;
	std::ifstream fragmentFile;
	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vertexFile.open(vertex);
		fragmentFile.open(fragment);
		std::stringstream vertexStream, fragmentStream;
		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();
		vertexFile.close();
		fragmentFile.close();
		vertexCode = vertexStream.str();
		fragmentCode = fragmentStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ÎÞ·¨´ò¿ª" << vertex << " " << fragment << std::endl;
	}
	const char* vertexS = vertexCode.c_str();
	const char* fragmentS = fragmentCode.c_str();

	unsigned int vs, fr;
	int success;
	char info[512];
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexS, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vs, 512, NULL, info);
		std::cout << "shader±àÒë´íÎó" << info << std::endl;
	}
	fr = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fr, 1, &fragmentS, NULL);
	glCompileShader(fr);
	glGetShaderiv(fr, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fr, 512, NULL, info);
		std::cout << "shader±àÒë´íÎó" << info << std::endl;
	}
	ID = glCreateProgram();
	glAttachShader(ID, vs);
	glAttachShader(ID, fr);
	glLinkProgram(ID);
	glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(ID, 512, NULL, info);
		std::cout << "shaderÁ´½Ó´íÎó" << info << std::endl;
	}
	glDeleteShader(vs);
	glDeleteShader(fr);
}
void Shader::use() {
	glUseProgram(ID);
}
void Shader::setBool(const std::string& name, bool value)const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value)const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setFloat(const std::string& name, float value)const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat3(const std::string& name, glm::vec3 value)const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x,value.y,value.z);
}
void Shader::setMatrix(const std::string& name, GLfloat* value)const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1,GL_FALSE,value);
}