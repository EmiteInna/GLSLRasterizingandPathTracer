#ifndef EMITEINNA_CAMERA
#define EMITEINNA_CAMERA

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <bits/stdc++.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"



class Camera {
public:
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 up;
	float FOV;
	float nearPlane;
	float farPlane;
	float aspect;
	Camera(glm::vec3 Position, glm::vec3 Target, glm::vec3 Up, float _FOV, float NearPlane, float FarPlane, float Aspect);
	Camera();
	void UpdateCamera(GLFWwindow* window,float delta);
};
#endif // !EMITEINNA_CAMERA
