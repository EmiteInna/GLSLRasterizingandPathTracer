#include "BVH.h"

BVH::BVH() {
	BVHTree.clear();
	BVHlength = 0;
}
void BVH::AddModel(Model* model) {
	int cnt = 0;
	for (int i = 0; i < model->meshes.size(); i++) {
		int start = vertices.size();
		for (int j = 0; j < model->meshes[i].vertices.size(); j++) {
			glm::mat4 M = glm::mat4(1);
			M = glm::rotate(M, glm::radians(model->rotation[0]), glm::vec3(1, 0, 0));
			M = glm::rotate(M, glm::radians(model->rotation[1]), glm::vec3(0, 1, 0));
			M = glm::rotate(M, glm::radians(model->rotation[2]), glm::vec3(0, 0, 1));
			M = glm::scale(M, model->scale);
			M = glm::translate(M, model->position);
			model->meshes[i].vertices[j].POSITION = M * model->meshes[i].vertices[j].POSITION;
			cnt++;
			vertices.push_back(model->meshes[i].vertices[j]);
		}
		for (int j = 0; j < model->meshes[i].indices.size(); j += 3) {
			Triangles tri;
			tri.p1 = model->meshes[i].indices[j] + start;
			tri.p2 = model->meshes[i].indices[j+1] + start;
			tri.p3 = model->meshes[i].indices[j+2] + start;
			tri.meshIndex = i;
			tri.center = glm::vec3(
				model->meshes[i].vertices[model->meshes[i].indices[j]].POSITION.x +
				model->meshes[i].vertices[model->meshes[i].indices[j + 1]].POSITION.x +
				model->meshes[i].vertices[model->meshes[i].indices[j + 2]].POSITION.x,

				model->meshes[i].vertices[model->meshes[i].indices[j]].POSITION.y +
				model->meshes[i].vertices[model->meshes[i].indices[j + 1]].POSITION.y +
				model->meshes[i].vertices[model->meshes[i].indices[j + 2]].POSITION.y,

				model->meshes[i].vertices[model->meshes[i].indices[j]].POSITION.z +
				model->meshes[i].vertices[model->meshes[i].indices[j + 1]].POSITION.z +
				model->meshes[i].vertices[model->meshes[i].indices[j + 2]].POSITION.z) / 3.0f;
			triangles.push_back(tri);
		}
	}
	std::cout << "[BVH]成功添加" << cnt << "个节点。" << std::endl;
}
bool cmpx(Triangles t1, Triangles t2) {
	return t1.center.x < t2.center.x;
}
bool cmpy(Triangles t1, Triangles t2) {
	return t1.center.y < t2.center.y;
}
bool cmpz(Triangles t1, Triangles t2) {
	return t1.center.z < t2.center.z;
}
void BVH::BuildBVH() {
	
	int len = triangles.size();
	BVHTree.push_back({-1,-1,-1,glm::vec3(0),glm::vec3(0)});
	BVHTree.push_back({-1,-1,-1,glm::vec3(0),glm::vec3(0)});

	Build(1, 1, len);
}
void BVH::Build(int n, int l, int r) {
	//std::cout << "n=" << n << " l=" << l << " r=" << r << std::endl;
	if (l > r)return;
	Node node = { -1,0,0,glm::vec3(0),glm::vec3(0) };


	node.AA = glm::vec3(1e9, 1e9, 1e9);
	node.BB = glm::vec3(-1e9, -1e9, -1e9);
	for (int i = l; i <= r; i++) {
		float minx = std::min(vertices[triangles[i - 1].p1].POSITION.x, 
					std::min(vertices[triangles[i - 1].p2].POSITION.x,
						vertices[triangles[i - 1].p3].POSITION.x));
		float miny = std::min(vertices[triangles[i - 1].p1].POSITION.y,
			std::min(vertices[triangles[i - 1].p2].POSITION.y,
				vertices[triangles[i - 1].p3].POSITION.y));
		float minz = std::min(vertices[triangles[i - 1].p1].POSITION.z,
			std::min(vertices[triangles[i - 1].p2].POSITION.z,
				vertices[triangles[i - 1].p3].POSITION.z));
		
		float maxx = std::max(vertices[triangles[i - 1].p1].POSITION.x,
			std::max(vertices[triangles[i - 1].p2].POSITION.x,
				vertices[triangles[i - 1].p3].POSITION.x));
		float maxy = std::max(vertices[triangles[i - 1].p1].POSITION.y,
			std::max(vertices[triangles[i - 1].p2].POSITION.y,
				vertices[triangles[i - 1].p3].POSITION.y));
		float maxz = std::max(vertices[triangles[i - 1].p1].POSITION.z,
			std::max(vertices[triangles[i - 1].p2].POSITION.z,
				vertices[triangles[i - 1].p3].POSITION.z));
		node.AA.x = std::min(node.AA.x, minx);
		node.AA.y = std::min(node.AA.y, miny);
		node.AA.z = std::min(node.AA.z, minz);
		node.BB.x = std::max(node.BB.x, maxx);
		node.BB.y = std::max(node.BB.y, maxy);
		node.BB.z = std::max(node.BB.z, maxz);
	}
	if (r == l) {
		if(l%2000==0)std::cout << "[BVH]我好了，节点号为"<<n<<" 位置为"<<l<< std::endl;
		node.triangleIndex = l;//都不减，shader里减
		return;
	}
	int axis = -1;
	int spl = -1;
	float minmax = 1e9;
	std::vector<glm::vec3> preAA, preBB, sufAA, sufBB; 
	int len = r - l + 1;
	for (int a = 0; a < 3; a++) {
		if (a == 0)std::sort(triangles.begin()+l-1, triangles.begin()+r, cmpx);
		if (a == 1)std::sort(triangles.begin()+l-1, triangles.begin()+r, cmpy);
		if (a == 2)std::sort(triangles.begin()+l-1, triangles.begin()+r, cmpz);
		glm::vec3 pAA = glm::vec3(1e9, 1e9, 1e9),pBB=glm::vec3(-1e9,-1e9,-1e9);
		glm::vec3 sAA = glm::vec3(1e9, 1e9, 1e9), sBB = glm::vec3(-1e9, -1e9, -1e9);
		preAA.clear();
		preBB.clear();
		sufAA.clear();
		sufBB.clear();
		for (int i = l; i <= r; i++) {
			float minx = std::min(vertices[triangles[i - 1].p1].POSITION.x,
				std::min(vertices[triangles[i - 1].p2].POSITION.x,
					vertices[triangles[i - 1].p3].POSITION.x));
			float miny = std::min(vertices[triangles[i - 1].p1].POSITION.y,
				std::min(vertices[triangles[i - 1].p2].POSITION.y,
					vertices[triangles[i - 1].p3].POSITION.y));
			float minz = std::min(vertices[triangles[i - 1].p1].POSITION.z,
				std::min(vertices[triangles[i - 1].p2].POSITION.z,
					vertices[triangles[i - 1].p3].POSITION.z));

			float maxx = std::max(vertices[triangles[i - 1].p1].POSITION.x,
				std::max(vertices[triangles[i - 1].p2].POSITION.x,
					vertices[triangles[i - 1].p3].POSITION.x));
			float maxy = std::max(vertices[triangles[i - 1].p1].POSITION.y,
				std::max(vertices[triangles[i - 1].p2].POSITION.y,
					vertices[triangles[i - 1].p3].POSITION.y));
			float maxz = std::max(vertices[triangles[i - 1].p1].POSITION.z,
				std::max(vertices[triangles[i - 1].p2].POSITION.z,
					vertices[triangles[i - 1].p3].POSITION.z));
		//	std::cout << "we have " << minx << " " << miny << " " << minz << " " << maxx << " " << maxy << " " << maxz << std::endl;

			pAA.x = std::min(pAA.x, minx);
			pAA.y = std::min(pAA.y, miny);
			pAA.z = std::min(pAA.z, minz);
			pBB.x = std::max(pBB.x, maxx);
			pBB.y = std::max(pBB.y, maxy);
			pBB.z = std::max(pBB.z, maxz);
			preAA.push_back(pAA);
			preBB.push_back(pBB);
		}
		for (int i = r; i >= l; i--) {
			float minx = std::min(vertices[triangles[i - 1].p1].POSITION.x,
				std::min(vertices[triangles[i - 1].p2].POSITION.x,
					vertices[triangles[i - 1].p3].POSITION.x));
			float miny = std::min(vertices[triangles[i - 1].p1].POSITION.y,
				std::min(vertices[triangles[i - 1].p2].POSITION.y,
					vertices[triangles[i - 1].p3].POSITION.y));
			float minz = std::min(vertices[triangles[i - 1].p1].POSITION.z,
				std::min(vertices[triangles[i - 1].p2].POSITION.z,
					vertices[triangles[i - 1].p3].POSITION.z));

			float maxx = std::max(vertices[triangles[i - 1].p1].POSITION.x,
				std::max(vertices[triangles[i - 1].p2].POSITION.x,
					vertices[triangles[i - 1].p3].POSITION.x));
			float maxy = std::max(vertices[triangles[i - 1].p1].POSITION.y,
				std::max(vertices[triangles[i - 1].p2].POSITION.y,
					vertices[triangles[i - 1].p3].POSITION.y));
			float maxz = std::max(vertices[triangles[i - 1].p1].POSITION.z,
				std::max(vertices[triangles[i - 1].p2].POSITION.z,
					vertices[triangles[i - 1].p3].POSITION.z));

			sAA.x = std::min(sAA.x, minx);
			sAA.y = std::min(sAA.y, miny);
			sAA.z = std::min(sAA.z, minz);
			sBB.x = std::max(sBB.x, maxx);
			sBB.y = std::max(sBB.y, maxy);
			sBB.z = std::max(sBB.z, maxz);
			sufAA.push_back(sAA);
			sufBB.push_back(sBB);
		}
		for (int i = l; i < r; i++) {
			//std::cout << "len=" << len << " i-l=" << i - l << " len-i+l-1=" << len - i+l - 1 << std::endl;
			float lenx = preBB[i-l].x - preAA[i-l].x;
			float leny= preBB[i - l].y - preAA[i-l].y;
			float lenz = preBB[i - l].z - preAA[i-l].z;
			float S1 = lenx * leny + lenx * lenz + leny * lenz;
			float slenx = sufBB[len - i+l - 1].x - sufAA[len - i+l - 1].x;
			float sleny = sufBB[len - i+l - 1].y - sufAA[len - i+l - 1].y;
			float slenz = sufBB[len - i+l - 1].z - sufAA[len - i+l - 1].z;
			float S2 = slenx * sleny + slenx * slenz + sleny * slenz;
			if (std::max(S1, S2) < minmax) {
				minmax = std::max(S1, S2);
				axis = a;
				spl = i;
			}
		}
	}
	if (axis == 0)std::sort(triangles.begin() + l - 1, triangles.begin() + r, cmpx);
	if (axis == 1)std::sort(triangles.begin() + l - 1, triangles.begin() + r, cmpy);
	if (axis == 2)std::sort(triangles.begin() + l - 1, triangles.begin() + r, cmpz);
	int now = BVHTree.size();
	BVHTree.push_back({ -1,-1,-1,glm::vec3(0),glm::vec3(0) });
	BVHTree.push_back({ -1,-1,-1,glm::vec3(0),glm::vec3(0) });
	//std::cout << "l=" << l << " r=" << r << " split=" << spl <<"triangle have:"<<triangles.size() << std::endl;
	Build(now, l, spl);
	Build(now+1, spl + 1, r);
	node.left = now;
	node.right = now + 1;
	BVHTree[n] = node;

}
void BVH::InitializeDebug() {
	//std::cout << "我终结比赛了吗？" << std::endl;
	debugVertices.clear();
	debugIndices.clear();
	DebugLine(1, 1, 8);
	glGenVertexArrays(1, &debugVAO);
	glGenBuffers(1, &debugVBO);
	glBindVertexArray(debugVAO);
	glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
	glBufferData(GL_ARRAY_BUFFER, debugVertices.size()*sizeof(float), &debugVertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &debugEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debugEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, debugIndices.size() * sizeof(unsigned int), &debugIndices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	debugMaterial = new Material("bvhdebug.vs", "bvhdebug.fr", 0);
	std::cout << "BVH初始化完毕，debug顶点数组长度为" << debugVertices.size() << " 索引长度为" << debugIndices.size() << std::endl;
	/*for (int i = 0; i < 20; i++) {
		std::cout << "顶点：" << debugVertices[i] << std::endl;
		std::cout << "索引：" << debugIndices[i] << std::endl;
	}*/
}
void BVH::DebugLine(int n, int floor, int limit) {
	
	if (floor == limit)return;
//	if (floor == 2) {
		int now = debugVertices.size();
		glm::vec3 AAA = BVHTree[n].AA;
		glm::vec3 BBB = BVHTree[n].BB;
		std::cout << "有一个AAA是" << glm::to_string(AAA) << " BBB是" << glm::to_string(BBB) << std::endl;
		glm::vec3 AAB = glm::vec3(AAA.x, AAA.y, BBB.z);
		glm::vec3 ABB = glm::vec3(AAA.x, BBB.y, BBB.z);
		glm::vec3 BAB = glm::vec3(BBB.x, AAA.y, BBB.z);
		glm::vec3 BAA = glm::vec3(BBB.x, AAA.y, AAA.z);
		glm::vec3 BBA = glm::vec3(BBB.x, BBB.y, AAA.z);
		glm::vec3 ABA = glm::vec3(AAA.x, BBB.y, AAA.z);
		debugVertices.push_back(AAA.x);
		debugVertices.push_back(AAA.y);
		debugVertices.push_back(AAA.z);
		debugVertices.push_back(BBB.x);
		debugVertices.push_back(BBB.y);
		debugVertices.push_back(BBB.z);
		debugVertices.push_back(AAB.x);
		debugVertices.push_back(AAB.y);
		debugVertices.push_back(AAB.z);
		debugVertices.push_back(ABB.x);
		debugVertices.push_back(ABB.y);
		debugVertices.push_back(ABB.z);
		debugVertices.push_back(BAB.x);
		debugVertices.push_back(BAB.y);
		debugVertices.push_back(BAB.z);
		debugVertices.push_back(BAA.x);
		debugVertices.push_back(BAA.y);
		debugVertices.push_back(BAA.z);
		debugVertices.push_back(BBA.x);
		debugVertices.push_back(BBA.y);
		debugVertices.push_back(BBA.z);
		debugVertices.push_back(ABA.x);
		debugVertices.push_back(ABA.y);
		debugVertices.push_back(ABA.z);

		debugIndices.push_back(now/3 + 0);	debugIndices.push_back(now/3 + 2);
		debugIndices.push_back(now/3 + 0);	debugIndices.push_back(now/3 + 5);
		debugIndices.push_back(now/3 + 0);	debugIndices.push_back(now/3 + 7);
		debugIndices.push_back(now/3 + 1);	debugIndices.push_back(now/3 + 3);
		debugIndices.push_back(now/3 + 1);	debugIndices.push_back(now/3 + 4);
		debugIndices.push_back(now/3 + 1);	debugIndices.push_back(now/3 + 6);
		debugIndices.push_back(now/3 + 2);	debugIndices.push_back(now/3 + 3);
		debugIndices.push_back(now/3 + 2);	debugIndices.push_back(now/3 + 4);
		debugIndices.push_back(now/3 + 3);	debugIndices.push_back(now/3 + 7);
		debugIndices.push_back(now/3 + 4);	debugIndices.push_back(now/3 + 5);
		debugIndices.push_back(now/3 + 5);	debugIndices.push_back(now/3 + 6);
		debugIndices.push_back(now/3 + 6);	debugIndices.push_back(now/3 + 7);
//	}
	if(BVHTree[n].left!=-1)DebugLine(BVHTree[n].left, floor + 1, limit);
	if(BVHTree[n].right!=-1)DebugLine(BVHTree[n].right, floor + 1, limit);
}
void BVH::DrawDebugLine(Camera* cam) {
	//std::cout << "在画了在画了" << std::endl;
	glBindVertexArray((unsigned int)debugVAO);
	debugMaterial->use();
	glm::mat4 V = glm::lookAt(cam->position, cam->position + cam->target, cam->up);
	debugMaterial->setMatrix("V", glm::value_ptr(V));
	glm::mat4 P = glm::mat4(1);
	P = glm::perspective(glm::radians(cam->FOV), cam->aspect, cam->nearPlane, cam->farPlane);
	debugMaterial->setMatrix("P", glm::value_ptr(P));
	debugMaterial->setFloat3("WorldSpaceCameraPos", cam->position);
	glDrawElements(GL_LINES, debugIndices.size(), GL_UNSIGNED_INT, 0);
}