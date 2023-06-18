#ifndef EMITEINNA_BVH
#define EMITEINNA_BVH


#include "core.h"
#include "Model.h"
#include "Camera.h"
#include "PathtracerMaterial.h"

struct Node {
	int triangleIndex;
	int left, right;
	glm::vec3 AA, BB;
};
struct Triangles {
	int p1, p2, p3,meshIndex;
	glm::vec3 center;
};

struct NodeEncoded {
	glm::vec3 triangleIndexleftright;
	glm::vec3 AA, BB;
};
struct TrianglesEncoded {
	glm::vec4 p123meshIndex;
};
struct MaterialEncoded {
	glm::vec3 emissive;
	glm::vec3 baseColor;
	glm::vec3 p1;
	glm::vec3 p2;
	glm::vec3 p3;
	glm::vec3 p4;
};
class BVH {
public:
	std::vector<Node> BVHTree;
	std::vector<Vertex> vertices;
	std::vector<Triangles> triangles;
	std::vector<float> debugVertices;
	std::vector<unsigned int> debugIndices;
	std::vector<MaterialEncoded> materials;
	int BVHlength;
	int totalMeshCount;
	int totalMapCount;
	Material* debugMaterial;
	GLuint debugVAO, debugVBO, debugEBO;
	BVH();
	void AddModel(Model* model,int startID);
	void BuildBVH();
	void Build(int n, int l, int r);
	void InitializeDebug();
	void DebugLine(int n,int floor, int limit);
	void DrawDebugLine(Camera* cam);
	GLuint VAO, VBO;
	Material* material;
	void RenderInitialize();
	void Render(Camera* cam);
	GLuint TBOtri, tritex, TBOvertices, verticestex, TBOBVH, BVHtex,TBOmat,mattex;
	void BindMaterial(int index, PathTracerMaterial mat);
};


#endif