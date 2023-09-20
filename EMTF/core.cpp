#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <bits/stdc++.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include "Camera.h"
#include "Material.h"
#include "Model.h"
#include "BVH.h"
#include "imm.h"
#include "Texture.h"
#include "PathtracerMaterial.h"
#include "RayTracing.h"
#include <Windows.h>
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
typedef struct Mouse {
	bool haveRead;
	float lastx, lasty;
	float pitch, yaw;
}Mouse;
Mouse mouse = { false,0,0,0,0 };
typedef struct Timer {
	float timeSineStart;
	float deltaTime;
	float currentTime;
	int frame;
}Timer;
Timer timer={0,0,0,0};
GLFWwindow* window;
Camera* maincam;
Pass* passpt,* pass1,* pass2;
RayTracing *rt;

bool haveReadMouse = false;
void framebuffer_size_callback(GLFWwindow* window ,int w,int h) {
	glViewport(0, 0, w, h);
}
void mouse_callback(GLFWwindow* window ,double x,double y) {
	if (mouse.haveRead == false) {
		mouse.haveRead = true;
		mouse.lastx = x;
		mouse.lasty = y;
	}
	else {
		float xoff = x - mouse.lastx;
		float yoff =- y + mouse.lasty;
		mouse.lastx = x;
		mouse.lasty = y;
		float s = 0.1f;
		mouse.yaw += xoff * s;
		mouse.pitch += yoff * s;
		mouse.pitch = std::min(mouse.pitch, 85.0f);
		mouse.pitch = std::max(mouse.pitch, -85.0f);
		glm::vec3 front;
		front.x = cos(glm::radians(mouse.yaw)) * cos(glm::radians(mouse.pitch));
		front.y = sin(glm::radians(mouse.pitch));
		front.z = sin(glm::radians(mouse.yaw)) * cos(glm::radians(mouse.pitch));
		maincam->target = glm::normalize(front);
	}
}
void scroll_callback(GLFWwindow* window,double xoff,double yoff) {
	float fov = maincam->FOV - yoff;
	fov = std::max(1.0f, fov);
	fov = std::min(45.0f, fov);
	maincam->FOV = fov;
}

GLFWwindow* InitializeGL() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(512, 512, "EMITEINNA PATH TRACING", NULL, NULL);
	if (window == NULL) {
		std::cout << "���ڴ���ʧ��" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "GLAD��ʼ��ʧ��" << std::endl;
		return NULL;
	}
	HWND hw = glfwGetWin32Window(window);
	HIMC hi = ImmGetContext(hw);
	ImmAssociateContext(hw, NULL);
	ImmReleaseContext(hw, hi);
	//
	glfwSetInputMode(window, GLFW_CURSOR,GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window,mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	return window;
}

Material* kleemat,*spacecraftmat,*cubehousemat,*cubelightmat;
Model* klee,*spacecraft,*console,*cubehouse,*cubelight, * cubelight2, * cubelight3, * cubelight4,*mirror;
PathTracerMaterial *normalMat, *lightMat,*mirrorMat,*wallMat, * lightMat2, * lightMat3, * lightMat4;

BVH* bvh;

void UpdatePerFrame() {
	timer.timeSineStart = glfwGetTime();
	timer.deltaTime = timer.timeSineStart - timer.currentTime;
	timer.currentTime = timer.timeSineStart;


	//
	maincam->UpdateCamera(window,timer.deltaTime);
	//

}
void InitializeObjects() {
	kleemat = new Material("0000.vs", "0000.fr", 3);
	kleemat->setTexture(0, "texture0", "kleehair.png");
	kleemat->setTexture(1, "texture1", "kleeface.png");
	kleemat->setTexture(2, "texture2", "kleecloth.png"); 
	klee = new Model("klee.fbx", kleemat, glm::vec3(0,-1.1, -4), glm::vec3(-90, 0, 180), glm::vec3(0.2, 0.2, 0.2), maincam, false);
	
	spacecraftmat = new Material("0000.vs", "0000.fr", 1);
	cubehousemat = new Material("0000.vs", "0000.fr", 1);
	cubelightmat = new Material("0000.vs", "0000.fr", 1);
	cubehousemat->setTexture(0, "texture0", "cubehouseBasemap.png");
	cubelightmat->setTexture(0, "texture0", "lightBasemap.png");

	/*Texture* spacecrafttex = new Texture("spacecraftBasemap.png");
	spacecraftmat->setTexture(0, "texture0", *spacecrafttex);
	spacecraft = new Model("spacecraft.fbx",spacecraftmat,glm::vec3(3,0,-10),glm::vec3(-90,0,0),glm::vec3(1,1,1), maincam, false);*/


	console = new Model("console.fbx", spacecraftmat, glm::vec3(0, 0, -20), glm::vec3(-90, 0, 0), glm::vec3(1, 1, 1), maincam,false);
	cubehouse = new Model("cubehouse.fbx", cubehousemat, glm::vec3(0, -2, 0), glm::vec3(-90, 0, 45), glm::vec3(1, 1, 1),maincam, false);
	cubelight = new Model("cubelight.fbx", cubelightmat, glm::vec3(0, 3.2, 0), glm::vec3(-90, 0, 45), glm::vec3(2, 2, 0.2), maincam, false);
	cubelight2 = new Model("cubelight.fbx", cubelightmat, glm::vec3(0, 3.2, -9), glm::vec3(-90, 0, 45), glm::vec3(2, 2, 0.2), maincam, false);
	cubelight3 = new Model("cubelight.fbx", cubelightmat, glm::vec3(4.5, 3.2, -4.5), glm::vec3(-90, 0, 45), glm::vec3(2, 2, 0.2), maincam, false);
	cubelight4 = new Model("cubelight.fbx", cubelightmat, glm::vec3(-4.5, 3.2, -4.5), glm::vec3(-90, 0, 45), glm::vec3(2, 2, 0.2), maincam, false);
	mirror = new Model("cubelight.fbx", cubelightmat, glm::vec3(0, 0, -8), glm::vec3(-90, 0, 0), glm::vec3(2, 2, 2), maincam, false);
	//���ʴ�����˳��Ϊ
	//�Է��⡢������ɫ���α���ɢ������������ȡ����淴��ǿ�ȡ����淴����ɫ���ֲڶ�
	//�������Բ�����͸��ȡ�͸����ɫ������ǿ�ȡ�����ֲڶȣ�������ɫ��Ϊԭɫ�Ͱ�ɫ��Ĳ�ֵ
	float light = 2.2;
	normalMat = new PathTracerMaterial(glm::vec3(.0), glm::vec3(1), 0.5, 0.3, 0.3, 0, 0.6, 0, 0, 0, 0, 0, 0, 0);
	lightMat = new PathTracerMaterial(glm::vec3(1,1,1)*light, glm::vec3(1),0,0,0,0,0,0,0,0,0,0,0,0);
	lightMat2 = new PathTracerMaterial(glm::vec3(1, 0.8, 0.8) * light, glm::vec3(1),0,0,0,0,0,0,0,0,0,0,0,0);
	lightMat3 = new PathTracerMaterial(glm::vec3(0.8, 1, 0.8) * light, glm::vec3(1),0,0,0,0,0,0,0,0,0,0,0,0);
	lightMat4 = new PathTracerMaterial(glm::vec3(0.8, 0.8, 1) * light, glm::vec3(1),0,0,0,0,0,0,0,0,0,0,0,0);
	mirrorMat = new PathTracerMaterial(glm::vec3(0), glm::vec3(1),0,1,1,1,0.1,0,0,0,0,0,0,0);
	wallMat = new PathTracerMaterial(glm::vec3(0), glm::vec3(1),0,0.6,0.7,0.6,0.1,0,0,0,0,0,0,0);


	bvh = new BVH();
	bvh->AddModel(klee,0);//�������ģ�ͣ����ģ����������3��ģ�ͣ�������

	bvh->AddModel(cubehouse,3);//���뷿��ģ�ͣ����Ǹ�������ģ��

	bvh->AddModel(cubelight,4); //�����Դ
	bvh->AddModel(cubelight2,5); //�����Դ
	bvh->AddModel(cubelight3,6); //�����Դ
	bvh->AddModel(cubelight4,7); //�����Դ
	bvh->AddModel(mirror,8); //���뾵��
	bvh->totalMapCount = 6;
	bvh->BuildBVH();
	
	bvh->BindMaterial(4, *normalMat);//���ڳ����д�����⣬ÿ����һ��ģ�ͣ�����Ҫ�󶨵�ͬ��ģ��������material
	bvh->BindMaterial(5, *normalMat);
	bvh->BindMaterial(6, *normalMat);
	bvh->BindMaterial(7, *wallMat);
	bvh->BindMaterial(8, *lightMat);
	bvh->BindMaterial(9, *lightMat2);
	bvh->BindMaterial(10, *lightMat3);
	bvh->BindMaterial(11, *lightMat4);
	bvh->BindMaterial(12, *mirrorMat);
	bvh->RenderInitialize();
	bvh->InitializeDebug();
	bvh->material->setTexture(4, "KleeHair", "kleehair.png");
	bvh->material->setTexture(5, "KleeFace", "kleeface.png");
	bvh->material->setTexture(6, "KleeCloth", "kleecloth.png");
	bvh->material->setTexture(7, "CubeHouseBasemap", "cubehouseBasemap.png");
	bvh->material->setTexture(8, "LightBasemap", "lightBasemap.png");
	bvh->material->setTexture(12, "MirrorBasemap", "lightBasemap.png");

	Shader* shaderp1 = new Shader("pathtracer.vs", "pass1.fr");
	Shader* shaderp2 = new Shader("pathtracer.vs", "pass2.fr");
	
	rt = new RayTracing(bvh, bvh->material->shader, shaderp1, shaderp2, maincam);
	rt->RayTracingPrepare();
	std::cout << "��ʼ��ȫ�����" << std::endl;
}
void ReleaseObjects() {
	klee->DeleteObject();
	console->DeleteObject();
	//spacecraft->DeleteObject();
	cubehouse->DeleteObject();
	cubelight->DeleteObject();
}
void RenderObjects() {
	int pathTracing = 1;
	//���Ҫ�ù�դ����Debug�����������Ϊ0����·��׷���������Ϊ1

	if (!pathTracing) {
		klee->Render();
		bvh->DrawDebugLine(maincam);
		cubelight->Render();
		cubelight2->Render();
		cubelight3->Render();
		cubelight4->Render();
		mirror->Render();
		cubehouse->Render();
	}
	else {
		bvh->material->setInt("bounce", 8);
		bvh->material->setFloat("frame", (float)timer.frame);
		rt->RayTracingRender(maincam);
		timer.frame++;
		if (timer.frame % 20 == 0)std::cout << "��ǰ������                                                                                               �ڣ�" << timer.frame << " ֡" << std::endl;
	}
	 
	/*klee->Render();
	cubehouse->Render();
	cubelight->Render();*/

	
}
void RenderFunctions() {
	glClearColor(0.1, 0.1, 0.1, 0.1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	if (timer.frame >-1) {
		RenderObjects();
	}
	glfwSwapBuffers(window);
	glfwPollEvents();
}
void processInput() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}
int main() {
	window = InitializeGL();
	maincam = new Camera();
	if (!window)return -1;
	InitializeObjects();
	while (!glfwWindowShouldClose(window)) {
		processInput();
		UpdatePerFrame();
		RenderFunctions();
	}
	ReleaseObjects();
	glfwTerminate();
	return 0;
}