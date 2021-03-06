#pragma once
//#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

enum ECamera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class CCamera
{
public:
	CCamera(glm::vec3 vPosition = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f), float vYaw = -90.0f, float vPitch = 0.0f);
	CCamera(float vPosX, float vPosY, float vPosZ, float vUpX, float vUpY, float vUpZ, float vYaw, float vPitch);

	void processKeyboard(ECamera_Movement vDirection, float vDeltaTime);
	void processMouseMovement(float vXOffset, float vYOffset, GLboolean vConstrainPitch = true);
	void processMouseScroll(float vYOffset);

	float getZoom() const { return m_Zoom; }
	const glm::vec3& getPosition() const { return m_Position; }
	const glm::vec3& getFront() const { return m_Front; }
	glm::mat4 getViewMatrix() const { return glm::lookAt(m_Position, m_Position + m_Front, m_Up); }

private:
	void __updateCameraVectors();

	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;

	float m_Yaw;
	float m_Pitch;
	float m_MovementSpeed;
	float m_MouseSensitivity;
	float m_Zoom;
};
