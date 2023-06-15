#include "Camera.h"
Camera::Camera(glm::vec3 Position, glm::vec3 Target, glm::vec3 Up, float _FOV, float NearPlane, float FarPlane, float Aspect) {
	position = Position;
	target = Target;
	up = Up;
	FOV = _FOV;
	nearPlane = NearPlane;
	farPlane = FarPlane;
	aspect = Aspect;
}
Camera::Camera() {
	position = glm::vec3(0);
	target = glm::vec3(0, 0, -1);
	up = glm::vec3(0, 1, 0);
	FOV = 45;
	nearPlane = 0.2;
	farPlane = 50;
	aspect = 1;
}
void Camera::UpdateCamera(GLFWwindow* window,float delta) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += target*delta*2.5f;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position += -target * delta * 2.5f;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		position += up * delta * 2.5f;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		position += -up * delta * 2.5f;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position += glm::normalize(glm::cross(up,target)) * delta * 2.5f;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += -glm::normalize(glm::cross(up, target)) * delta * 2.5f;

}