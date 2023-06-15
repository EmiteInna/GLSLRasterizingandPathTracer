#include "Material.h"
Material::Material(const char* vertex, const char* fragment, int Mapcount) {
	textures = (Texture*)malloc(15 * sizeof(Texture));
	textureCount = Mapcount;
	shader = new Shader(vertex, fragment);
	emissive=glm::vec3(0);
	baseColor=glm::vec3(0);
	subsurface=0;
	metallic=0;
	specular=0;
	specularTint=0;
	roughness=0;
	anisotropic=0;
	sheen=0;
	sheenTint=0;
	clearcoat=0;
	clearcoatGloss=0;
	IOR=0;
	transmission=0;
}
void Material::use() {
	shader->use();
}
void Material::setBool(const std::string& name, bool value) const
{
	shader->use();
	shader->setBool(name, value);
}
void Material::setInt(const std::string& name, int value) const
{
	shader->use();
	shader->setInt(name, value);
}
void Material::setFloat(const std::string& name, float value) const
{
	shader->use();
	shader->setFloat(name, value);
}
void Material::setFloat3(const std::string& name, glm::vec3 value)const
{
	shader->use();
	shader->setFloat3(name, value);
}
void Material::setMatrix(const std::string& name, GLfloat* value)const
{
	shader->use();
	shader->setMatrix(name, value);
}
void Material::setTexture(unsigned int target, const std::string& texname, const std::string& path)const {
	shader->use();
	Texture texture(path);
	textures[target] = texture;
	shader->setInt(texname.c_str(), target);
}
void Material::setTexture(unsigned int target, const std::string& texname, Texture tex)const {
	shader->use();
	textures[target] = tex;
	shader->setInt(texname.c_str(), target);
}
void Material::updateTextures(int n) {
	use();
	for (int i = 0; i < n; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].GetTextureID());
	}
}
