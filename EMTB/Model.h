#ifndef EMITEINNA_MODEL
#define EMITEINNA_MODEL

#include "core.h"
#include "Material.h"
#include "Texture.h"
#include "Camera.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
struct Vertex {
	glm::vec4 POSITION;
	glm::vec4 UV;
	glm::vec4 NORMAL;//第四通道放材质ID
	glm::vec4 TANGENT;
	glm::vec4 VERTEXCOLOR;
};
struct Mesh {
	//glm::vec3 Position;
	//glm::vec3 Rotation;
	//glm::vec3 Scale;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};
class Model {
public:
	
	std::vector<Mesh> meshes;
	std::vector<Material*> materials;
	Material* baseMaterial;
	Camera* cam;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	std::vector<unsigned int> VBOs;
	std::vector<unsigned int> VAOs;
	std::vector<unsigned int> EBOs;
	Mesh dfsMesh(aiMesh* mesh, const aiScene* scene);
	void dfsNode(aiNode* node, const aiScene* scene);
	void InstantiateWithData();
	void use(int idx);
	void Render();
	void DeleteObject();
	Model(const std::string& path, Material* Material, glm::vec3 Position, glm::vec3 Rotation, glm::vec3 Scale, Camera* camera,bool flipUV);
	Model(const std::string& path, Material* Material, Camera* camera, bool flipUV);
	void show();
};



#endif // !EMITEINNA_MODEL
