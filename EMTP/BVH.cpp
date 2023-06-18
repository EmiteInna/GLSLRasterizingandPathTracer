#include "BVH.h"

BVH::BVH() {
	std::cout << "创建BVH" << std::endl;
	totalMapCount = 5;
	totalMeshCount = 0;
	VAO = 0;
	VBO = 0;
	BVHTree.clear();
	BVHlength = 0;
}
void BVH::AddModel(Model* model,int idStart) {
	bool useVertexNormal = true;
	int cnt = 0;
	int cnt1 = 0;

	for (int i = 0; i < model->meshes.size(); i++) {
		int start = vertices.size();
		for (int j = 0; j < model->meshes[i].vertices.size(); j++) {
			
			glm::mat4 M = glm::mat4(1);
			glm::mat4 Mr = glm::rotate(glm::mat4(1), glm::radians(model->rotation[0]), glm::vec3(1, 0, 0));
			Mr = glm::rotate(Mr, glm::radians(model->rotation[1]), glm::vec3(0, 1, 0));
			Mr = glm::rotate(Mr, glm::radians(model->rotation[2]), glm::vec3(0, 0, 1));
			glm::mat4 Ms = glm::scale(glm::mat4(1), model->scale);
			glm::mat4 Mt = glm::translate(glm::mat4(1), model->position);
			M = Mt * Mr * Ms;
			glm::mat4 invM = glm::transpose(glm::inverse(M));
			
			model->meshes[i].vertices[j].POSITION = M * model->meshes[i].vertices[j].POSITION;
		/*	if (model->meshes[i].vertices.size() >= 490 && model->meshes[i].vertices.size() <= 800) {
				std::cout << "[DEBUG]vertex为" << glm::to_string(model->meshes[i].vertices[j].POSITION) << std::endl;
			}*/
			if (useVertexNormal) {
				model->meshes[i].vertices[j].NORMAL = invM * model->meshes[i].vertices[j].NORMAL;
				glm::vec3 normNormal = glm::vec3(model->meshes[i].vertices[j].NORMAL);
				normNormal = glm::normalize(normNormal);
				model->meshes[i].vertices[j].NORMAL = glm::vec4(normNormal, 0);
			}
			cnt++;
			vertices.push_back(model->meshes[i].vertices[j]);
		}
		//std::cout << "现在vertices的大小是" << vertices.size() << std::endl;
		for (int j = 0; j < model->meshes[i].indices.size(); j += 3) {
			cnt1++;
			Triangles tri;
			tri.p1 = model->meshes[i].indices[j] + start;
			tri.p2 = model->meshes[i].indices[j+1] + start;
			tri.p3 = model->meshes[i].indices[j+2] + start;
			tri.meshIndex = idStart+i;
			//std::cout << "ID 为" << id << std::endl;
			glm::vec3 p1 = glm::vec3(model->meshes[i].vertices[model->meshes[i].indices[j]].POSITION.x,
				model->meshes[i].vertices[model->meshes[i].indices[j]].POSITION.y,
				model->meshes[i].vertices[model->meshes[i].indices[j]].POSITION.z);
			glm::vec3 p2 = glm::vec3(model->meshes[i].vertices[model->meshes[i].indices[j+1]].POSITION.x,
				model->meshes[i].vertices[model->meshes[i].indices[j+1]].POSITION.y,
				model->meshes[i].vertices[model->meshes[i].indices[j+1]].POSITION.z);
			glm::vec3 p3 = glm::vec3(model->meshes[i].vertices[model->meshes[i].indices[j+2]].POSITION.x,
				model->meshes[i].vertices[model->meshes[i].indices[j+2]].POSITION.y,
				model->meshes[i].vertices[model->meshes[i].indices[j+2]].POSITION.z);
			tri.center = glm::vec3(
				p1.x +p2.x +p3.x,p1.y +p2.y +p3.y,p1.z +p2.z +p3.z)/3.0f;
			if (isnan(tri.center.x) || isnan(tri.center.y) || isnan(tri.center.z))std::cout << "男了" << std::endl;;
			if (!useVertexNormal) {	
				glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));

				vertices[model->meshes[i].indices[j] + start].NORMAL = glm::vec4(normal, 1);
				vertices[model->meshes[i].indices[j + 1] + start].NORMAL = glm::vec4(normal, 1);
				vertices[model->meshes[i].indices[j + 2] + start].NORMAL = glm::vec4(normal, 1);
			}
			triangles.push_back(tri);
		}
		totalMeshCount++;
		materials.push_back({glm::vec3(0),glm::vec3(0) ,glm::vec3(0) ,glm::vec3(0) ,glm::vec3(0),glm::vec3(0) });
	}
	std::cout << "[BVH]成功添加" << cnt << "个节点，"<<cnt1<<" 个三角形" << std::endl;
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
	std::cout << "开始创建BVH" << std::endl;
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
		node.triangleIndex = l-1;
		BVHTree[n] = node;
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
			float cost = S1 * (i - l + 1) + S2 * (r - i);
			if (cost < minmax) {
				minmax = cost;
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
	//std::cout << "l=" << l << " r=" << r << " split=" << spl <<"min max is:"<<minmax << std::endl;
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
		//std::cout << "有一个AAA是" << glm::to_string(AAA) << " BBB是" << glm::to_string(BBB) << std::endl;
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
void BVH::RenderInitialize() {
	std::cout << "开始初始化路径追踪" << std::endl;
	material = new Material("pathtracer.vs", "pathtracer.fr",totalMapCount+4);

	/*glm::vec3 canvas[6] = {
		glm::vec3(-1,-1,0),glm::vec3(-1,1,0),glm::vec3(1,-1,0),
		glm::vec3(1,1,0),glm::vec3(1,-1,0),glm::vec3(-1,1,0)
	};*/
	glm::vec3 canvas[6] = {
		glm::vec3(1,-1,0),glm::vec3(-1,1,0),glm::vec3(-1,-1,0),
		glm::vec3(-1,1,0),glm::vec3(1,-1,0),glm::vec3(1,1,0)
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(canvas), &canvas[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);


	//纹理只能传带有最多4个通道的，我们发现node是三个数，两个vec3
	//triangles是四个数，一个vec3
	//vertex是5个vec4
	//也就是说在这里我们全部使用RGBA来做buffer，一共三个buffer，
	//但是需要做一个对应的映射，并且triangle需要扩充一下，并且还要全部改成float。
	//干脆重新encode一下
	// 对node来说，原来位置为0的之后还是0，而原来的1变成了2，以此类推，下标全部变为2倍
	//triangle的center分量已经没用了
	//vertex的每一维度依然有用，同样，在操作的时候需要将其全部乘5，但是不需要重新编码。

	//编码Node
	std::vector<NodeEncoded> nodes;
	for (int i = 0; i < BVHTree.size(); i++) {
		Node no = BVHTree[i];
		nodes.push_back({ glm::vec3((float)no.triangleIndex,(float)no.left,(float)no.right),no.AA,no.BB });
	//	if (no.triangleIndex > 0)std::cout << "这里有个节点：" << no.triangleIndex << std::endl;
	}

//	glUseProgram(material->shader->ID);

	int bvhsize = BVHTree.size();
	glGenBuffers(1, &TBOBVH);
	glBindBuffer(GL_TEXTURE_BUFFER, TBOBVH);
	glBufferData(GL_TEXTURE_BUFFER, bvhsize * sizeof(NodeEncoded), &nodes[0], GL_STATIC_DRAW);
	glGenTextures(1, &BVHtex);
	glBindTexture(GL_TEXTURE_BUFFER, BVHtex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, TBOBVH);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, BVHtex);
	glUniform1i(glGetUniformLocation(material->shader->ID, "Nodes"), 0);
	std::cout << "Node编码完成" << std::endl;
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	//编码三角形
	std::vector<TrianglesEncoded> tris;
	for (int i = 0; i < triangles.size(); i++) {
		Triangles tri = triangles[i];
		tris.push_back({ glm::vec4(tri.p1,tri.p2,tri.p3,tri.meshIndex) });
	}
	glGenBuffers(1, &TBOtri);
	glBindBuffer(GL_TEXTURE_BUFFER, TBOtri);
	glBufferData(GL_TEXTURE_BUFFER, tris.size() * sizeof(TrianglesEncoded), &tris[0], GL_STATIC_DRAW);
	glGenTextures(1, &tritex);
	glBindTexture(GL_TEXTURE_BUFFER, tritex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, TBOtri);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, tritex);
	glUniform1i(glGetUniformLocation(material->shader->ID, "Triangles"), 1);
	std::cout << "Triangle编码完成" << std::endl;
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	//顶点
	glGenBuffers(1, &TBOvertices);
	glBindBuffer(GL_TEXTURE_BUFFER, TBOvertices);
	glBufferData(GL_TEXTURE_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glGenTextures(1, &verticestex);
	glBindTexture(GL_TEXTURE_BUFFER, verticestex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, TBOvertices);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, verticestex);
	glUniform1i(glGetUniformLocation(material->shader->ID, "Vertices"), 2);
	std::cout << "Vertices编码完成" << std::endl;
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	//编码材质
	glGenBuffers(1, &TBOmat);
	glBindBuffer(GL_TEXTURE_BUFFER, TBOmat);
	glBufferData(GL_TEXTURE_BUFFER, materials.size() * sizeof(MaterialEncoded), &materials[0], GL_STATIC_DRAW);
	glGenTextures(1, &mattex);
	glBindTexture(GL_TEXTURE_BUFFER, mattex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, TBOmat);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_BUFFER, mattex);
	glUniform1i(glGetUniformLocation(material->shader->ID, "Materials"), 3);
	std::cout << "Materials编码完成" << std::endl;
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	//设置步长
	material->setFloat("NodeStride", 3);
	material->setFloat("TriangleStride", 1);
	material->setFloat("VertexStride", 5);
	material->setFloat("MaterialStride", 6);

	
	//std::cout << "[DEBUG]第一个顶点的信息是" << glm::to_string(vertices[1].POSITION)<<" " << glm::to_string(vertices[1].UV)<< " " << glm::to_string(vertices[1].NORMAL)<< " " << glm::to_string(vertices[1].TANGENT)<< " " << glm::to_string(vertices[1].VERTEXCOLOR)<< std::endl;
	//std::cout << "[DEBUG]第一个Node的信息是" << glm::to_string(nodes[1].triangleIndexleftright)<<" "<<glm::to_string(nodes[1].AA)<<" "<<glm::to_string(nodes[1].BB) << std::endl;
	//std::cout << "[DEBUG]第一个Triangle的信息是" << glm::to_string(tris[0].p123meshIndex) << std::endl;
	//std::cout << "[DEBUG]第一个Material的信息是" << glm::to_string(materials[0].emissive) << " " << glm::to_string(materials[1].emissive) << " " << glm::to_string(materials[4].emissive) << std::endl;
	//int siz = vertices.size();
	/*for (int id = 0; id < 24;id++) {
		std::cout << "[DEBUG]顶点的信息是" << glm::to_string(vertices[id].POSITION)<<" " << glm::to_string(vertices[id].UV)<< " " << glm::to_string(vertices[id].NORMAL)<< " " << glm::to_string(vertices[id].TANGENT)<< " " << glm::to_string(vertices[id].VERTEXCOLOR)<< std::endl;

	}
	for (int i = 0; i < 12; i++) {
		std::cout << "[DEBUG]Triangle的信息是" << glm::to_string(tris[i].p123meshIndex) << std::endl;

	}*/
	std::cout << "BVH初始化完成：" << "三角形数组长度为：" << tris.size() << std::endl;

	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindVertexArray(0);

}
void BVH::Render(Camera* cam) {
//	glBindVertexArray(VAO);
	material->updateTextures(material->textureCount);
	material->use();
	material->setFloat3("WorldSpaceCameraPos", cam->position);
	material->setFloat3("WorldSpaceCameraDir", cam->target); 
	material->setFloat("CameraFOV", cam->FOV);
	material->setFloat("CameraAspect", cam->aspect);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, BVHtex);
	glUniform1i(glGetUniformLocation(material->shader->ID, "Nodes"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, tritex);
	glUniform1i(glGetUniformLocation(material->shader->ID, "Triangles"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, verticestex);
	glUniform1i(glGetUniformLocation(material->shader->ID, "Vertices"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_BUFFER, mattex);
	glUniform1i(glGetUniformLocation(material->shader->ID, "Materials"), 3);
	 
	/*glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);*/

}
void BVH::BindMaterial(int index, PathTracerMaterial mat) {
	if (materials.size() <= index-4) {
		std::cout << "材质下标越界" << std::endl;
		return;
	}
	MaterialEncoded m = { mat.emissive,mat.baseColor,
		glm::vec3(mat.subsurface,mat.metallic,mat.specular),
		glm::vec3(mat.specularTint,mat.roughness,mat.anisotropic),
		glm::vec3(mat.sheen,mat.sheenTint,mat.clearcoat),
		glm::vec3(mat.clearcoatGloss,mat.IOR,mat.transmission) };
	materials[index-4] = m;
}