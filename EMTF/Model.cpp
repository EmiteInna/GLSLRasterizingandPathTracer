#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
Model::Model(const std::string& path, Material* _Material, glm::vec3 _Position, glm::vec3 _Rotation, glm::vec3 _Scale, Camera* Cam, bool flipUV) {
	materials.clear();
	baseMaterial = _Material;
	cam = Cam;
	position = _Position;
	rotation = _Rotation;
	scale = _Scale;
	VAOs.clear();
	VBOs.clear();
	EBOs.clear();
	Assimp::Importer importer;
	const aiScene* scene;
	if (flipUV)
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	else
		scene = importer.ReadFile(path, aiProcess_Triangulate);
	if (!scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "读取模型失败！" << std::endl;
	}
	dfsNode(scene->mRootNode, scene);
	InstantiateWithData();
	//show();
}
Model::Model(const std::string& path, Material* _Material, Camera* Cam, bool flipUV) {
	materials.clear();
	baseMaterial = _Material;
	cam = Cam;
	position = glm::vec3(0,0,0);
	rotation = glm::vec3(-90,0,0);
	scale = glm::vec3(1, 1, 1);
	VAOs.clear();
	VBOs.clear();
	EBOs.clear();
	Assimp::Importer importer;
	const aiScene* scene;
	if (flipUV)
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	else
		scene = importer.ReadFile(path, aiProcess_Triangulate);
	if (!scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "读取模型失败！" << std::endl;
	}
	dfsNode(scene->mRootNode, scene);
	InstantiateWithData();
//	show();
}
void Model::dfsNode(aiNode* node, const aiScene* scene) {
	std::cout <<"[模型读取]当前进行到模型，"<<"含有"<<node->mNumMeshes<<"个Mesh" << std::endl;
	for (int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		
		meshes.push_back(dfsMesh(mesh,scene));
	}
	for (int i = 0; i < node->mNumChildren; i++) {
		dfsNode(node->mChildren[i], scene);
	}
	std::cout << "[模型读取]当前完成" << node->mName.C_Str() << std::endl;
}
Mesh Model::dfsMesh(aiMesh* mesh, const aiScene* scene) {
	/*std::cout << "[模型读取]当前进行到的Mesh含有"<<mesh->mNumVertices<<"个节点 " <<mesh->mNumFaces<<"个面" << std::endl;
	if (mesh->mNumVertices == 24) {
		for (int i = 0; i < mesh->mNumVertices; i++) {
			glm::vec3 pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			std::cout << "节点" << i << "在" << glm::to_string(pos) << std::endl;
		}
		for (int i = 0; i < mesh->mNumVertices; i++) {
			glm::vec3 pos = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			std::cout << "法线" << i << "在" << glm::to_string(pos) << std::endl;
		}
		for (int i = 0; i < mesh->mNumFaces; i++) {
			std::cout << "索引" << i << "为" << mesh->mFaces[i].mIndices[0]<<" " << mesh->mFaces[i].mIndices[1] << " " << mesh->mFaces[i].mIndices[2] << std::endl;
		}
	}*/
	Mesh ret;
	ret.vertices.clear();
	ret.indices.clear();
	for (int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vertex.POSITION.x = mesh->mVertices[i].x;
		vertex.POSITION.y = mesh->mVertices[i].y;
		vertex.POSITION.z = mesh->mVertices[i].z;
		vertex.POSITION.w = 1;
		if (mesh->HasNormals()) {
			vertex.NORMAL.x = mesh->mNormals[i].x;
			vertex.NORMAL.y = mesh->mNormals[i].y;
			vertex.NORMAL.z = mesh->mNormals[i].z;
			vertex.NORMAL.w = 0;
		}
		else vertex.NORMAL = glm::vec4(0);
		//vertex.NORMAL.w = (int)meshes.size();
		vertex.NORMAL = glm::normalize(vertex.NORMAL);
		if (mesh->HasTextureCoords(0)) {
			vertex.UV.x = mesh->mTextureCoords[0][i].x;
			vertex.UV.y = mesh->mTextureCoords[0][i].y;
			vertex.UV.z = 0;
			vertex.UV.w = 0;
		}
		else vertex.UV = glm::vec4(0);
		if (mesh->HasTangentsAndBitangents()) {
			vertex.TANGENT.x = mesh->mTangents[i].x;
			vertex.TANGENT.y = mesh->mTangents[i].y;
			vertex.TANGENT.z = mesh->mTangents[i].z;
			vertex.TANGENT.w = 1;
		}
		else vertex.TANGENT = glm::vec4(0);
		if (mesh->HasVertexColors(0)) {
			vertex.VERTEXCOLOR.x = mesh->mColors[0][i].r;
			vertex.VERTEXCOLOR.y = mesh->mColors[0][i].g;
			vertex.VERTEXCOLOR.z = mesh->mColors[0][i].b;
			vertex.VERTEXCOLOR.w = mesh->mColors[0][i].a;
		}
		else vertex.VERTEXCOLOR = glm::vec4(0);
	/*	if (mesh->mNumVertices == 24) {
			std::cout << "[DEBUG]法线为" << glm::to_string(vertex.NORMAL) << std::endl;
		}*/
		ret.vertices.push_back(vertex);
	}
	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			ret.indices.push_back(face.mIndices[j]);
		}
	}
	materials.push_back(baseMaterial);
	return ret;
}
void Model::InstantiateWithData() {
	for (int i = 0; i < meshes.size(); i++) {
		VAOs.push_back(0);
		VBOs.push_back(0);
		EBOs.push_back(0);
		glGenVertexArrays(1, &VAOs[i]);
		glGenBuffers(1, &VBOs[i]);
		glBindVertexArray(VAOs[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, meshes[i].vertices.size() * sizeof(Vertex), &meshes[i].vertices[0], GL_STATIC_DRAW);
		glGenBuffers(1, &EBOs[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshes[i].indices.size() * sizeof(unsigned int), &(meshes[i].indices[0]), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT,GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,POSITION));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,UV));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,NORMAL));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TANGENT));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, VERTEXCOLOR));
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindVertexArray(0);
	}
}
void Model::use(int idx) {
	glBindVertexArray((unsigned int)VAOs[idx]);
}
void Model::Render() {
	for (int i = 0; i < meshes.size(); i++) {
		use(i);
		materials[i]->updateTextures(materials[i]->textureCount);
		materials[i]->use();
		glm::mat4 M = glm::mat4(1);
		glm::mat4 Mr = glm::rotate(glm::mat4(1), glm::radians(rotation[0]), glm::vec3(1, 0, 0));
		Mr = glm::rotate(Mr, glm::radians(rotation[1]), glm::vec3(0, 1, 0));
		Mr = glm::rotate(Mr, glm::radians(rotation[2]), glm::vec3(0, 0, 1));
		glm::mat4 Ms = glm::scale(glm::mat4(1), scale);
		glm::mat4 Mt = glm::translate(glm::mat4(1), position);
		M = Mt * Mr * Ms;
		materials[i]->setMatrix("M", glm::value_ptr(M));
		glm::mat4 V = glm::lookAt(cam->position, cam->position + cam->target, cam->up);
		materials[i]->setMatrix("V", glm::value_ptr(V));
		glm::mat4 P = glm::mat4(1);
		P = glm::perspective(glm::radians(cam->FOV), cam->aspect, cam->nearPlane, cam->farPlane);
		materials[i]->setMatrix("P", glm::value_ptr(P));
		materials[i]->setFloat3("WorldSpaceCameraPos", cam->position);
		glDrawElements(GL_TRIANGLES, meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
	}
}
void Model::DeleteObject() {
	for (int i = 0; i < meshes.size(); i++) {
		glDeleteVertexArrays(1, &VAOs[i]);
		glDeleteBuffers(1, &VBOs[i]);
		glDeleteBuffers(1, &EBOs[i]);
	}
	meshes.clear();
	VAOs.clear();
	VBOs.clear();
	EBOs.clear();
}
void Model::show() {
	for (int i = 0; i < meshes.size(); i++) {
		std::cout << "[DEBUG]现在是第" << i + 1 << "个Mesh" << std::endl;
		for (int j = 0; j < std::min(15, (int)meshes[i].vertices.size()); j++) {
			std::cout << "[DEBUG]Vertices:" << glm::to_string(meshes[i].vertices[j].POSITION) << std::endl;
			std::cout << "[DEBUG]Normals:" << glm::to_string(meshes[i].vertices[j].NORMAL) << std::endl;
			std::cout << "[DEBUG]UVs:" << glm::to_string(meshes[i].vertices[j].UV) << std::endl;
		}
	}
}