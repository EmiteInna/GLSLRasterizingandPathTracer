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
#include "imm.h"
#include "Texture.h"
#include <Windows.h>

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
}Timer;
Timer timer;
GLFWwindow* window;
Camera* maincam;

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(512, 512, "EMITEINNA PATH TRACING", NULL, NULL);
	if (window == NULL) {
		std::cout << "窗口创建失败" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "GLAD初始化失败" << std::endl;
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

Material* kleemat,*spacecraftmat;
Model* klee,*spacecraft;

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
	klee = new Model("klee.fbx", kleemat, maincam, false);
	/*spacecraftmat = new Material("0000.vs", "0000.fr", 1);
	Texture *spacecrafttex = new Texture("spacecraftBasemap.png");
	spacecraftmat->setTexture(0, "texture0", *spacecrafttex);
	spacecraft = new Model("spacecraft.fbx",spacecraftmat, maincam, false);*/
}
void ReleaseObjects() {
	klee->DeleteObject();
	//spacecraft->DeleteObject();
}
void RenderObjects() {
	klee->Render();

	//spacecraft->Render();
}
void RenderFunctions() {
	glClearColor(0.1, 0.1, 0.1, 0.1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	RenderObjects();
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