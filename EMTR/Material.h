#ifndef  EMITEINNA_MATERIAL
#define EMITEINNA_MATERIAL

#include "core.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

class Material {
public:
	glm::vec3 emissive;
	glm::vec3 baseColor;
	float subsurface;
	float metallic;
	float specular;
	float specularTint;
	float roughness;
	float anisotropic;
	float sheen;
	float sheenTint;
	float clearcoat;
	float clearcoatGloss;
	float IOR;
	float transmission;
	Shader* shader;
	int textureCount;
	Texture* textures;
	Material(const char* vertex, const char* fragment, int mapcount);
	void use();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setFloat3(const std::string& name, glm::vec3 value)const;
	void setMatrix(const std::string& name, GLfloat* value)const;
	void setTexture(unsigned int target, const std::string& texname, const std::string& path)const;
	void setTexture(unsigned int target, const std::string& texname, Texture tex)const;
	void updateTextures(int n);
};

#endif // ! EMITEINNA_MATERIAL
