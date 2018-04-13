#include "CharacterMovement.h"
#include "GameObject.h"

CharacterMovement::CharacterMovement(Transform& transform) : Transformable(transform)
{
    assetName = "CharacterMovement";
	cameraSpeed = 0.08f;
	cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = 0;
	yaw = 0;
	firstMouse = true;
	lastX = 400;
	lastY = 300;
	xpos = 0;
	ypos = 0;
    mouseDisable = 0;
    xoffset = 0;
    yoffset = 0;
    sensitivity = 0.002f;
}

CharacterMovement::~CharacterMovement()
{
}

void CharacterMovement::getInformation(float time)
{
	this->time += time / 1000;
}

void CharacterMovement::update(float deltaTime)
{
	/*
		Make stuff happen. 
		If player class: Update health, cold, hunger, thirst etc.
	*/
}

void CharacterMovement::processEvents(GLFWwindow * window, float deltaTime)
{
	//... Mouse Movement
	glfwGetCursorPos(window, &xpos, &ypos);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw = xoffset;
		pitch = yoffset;

		if (pitch > 90.f)
			pitch = 90.0f;
		if (pitch < -90.0f)
			pitch = -90.0f;

		glm::mat4 matrix = glm::mat4(1);

		glm::vec4 forward = glm::vec4(Transformable::transform.forward, 0);
		glm::vec4 right = glm::vec4(Transformable::transform.right, 0);
		glm::vec4 up = glm::vec4(Transformable::transform.up, 0);

		matrix = glm::rotate(matrix, pitch, Transformable::transform.right);
		matrix *= glm::rotate(matrix, -yaw, Transformable::transform.up);

		forward = matrix * forward;
		up = matrix * up;
		right = matrix * right;

		Transformable::transform.forward.x = forward.x;
		Transformable::transform.forward.y = forward.y;
		Transformable::transform.forward.z = forward.z;

		Transformable::transform.right.x = right.x;
		Transformable::transform.right.y = right.y;
		Transformable::transform.right.z = right.z;
	}

	if (firstMouse) {
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	xoffset = (float)xpos - lastX;
	yoffset = lastY - (float)ypos;
	lastX = (float)xpos;
	lastY = (float)ypos;

	//... WASD Movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && frontCollision == false)
		Transformable::transform.position += cameraSpeed * Transformable::transform.forward;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && backCollision == false)
		Transformable::transform.position -= cameraSpeed * Transformable::transform.forward;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && leftCollision == false)
		Transformable::transform.position -= Transformable::transform.right * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && rightCollision == false)
		Transformable::transform.position += Transformable::transform.right * cameraSpeed;

	//... Jump mechanic
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && inAir == false)
		transform.position += cameraSpeed * transform.up;
}
