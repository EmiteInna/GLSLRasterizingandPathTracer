#ifndef EMITEINNA_BVH
#define EMITEINNA_BVH


#include "core.h"
#include "Model.h"
#include "Camera.h"

struct Node {
	int triangleIndex;
	int left, right;
	glm::vec3 AA, BB;
};
struct Triangles {
	int p1, p2, p3,meshIndex;
	glm::vec3 center;
};
class BVH {
public:
	std::vector<Node> BVHTree;
	std::vector<Vertex> vertices;
	std::vector<Triangles> triangles;
	std::vector<float> debugVertices;
	std::vector<unsigned int> debugIndices;
	int BVHlength;
	Material* debugMaterial;
	GLuint debugVAO, debugVBO, debugEBO;
	BVH();
	void AddModel(Model* model);
	void BuildBVH();
	void Build(int n, int l, int r);
	void InitializeDebug();
	void DebugLine(int n,int floor, int limit);
	void DrawDebugLine(Camera* cam);
};


#endif