#ifndef  EMITEINNA_SHADER
#define EMITEINNA_SHADER
#include "core.h"
class Shader {
public:
	unsigned int ID;
	Shader(const std::string& vertex, const std::string& fragment);
	void use();
	void setBool(const std::string& name, bool value)const;
	void setInt(const std::string& name, int value)const;
	void setFloat(const std::string& name, float value)const;
	void setFloat3(const std::string& name, glm::vec3 value)const;
	void setMatrix(const std::string& name, GLfloat* value)const;
};

#endif // ! EMITEINNA_SHADER
