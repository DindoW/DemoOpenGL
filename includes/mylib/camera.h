#pragma once

#include <glm/glm.hpp>
#include <iostream>

enum class Camera_Movement : std::uint8_t
{
	none,
	front,
	back,
	left,
	right
};


class Camera
{
private:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	// 相机fov
	float fov = 45.0f;

	// 当前相机front的转角
	float pitch = 0.0;
	float yaw = 0.0;

	// 相机鼠标灵敏度
	float sensitivity = 0.05f;

	// 相机键盘灵敏度
	float posSensitivity = 1.0f;

	// 观察矩阵
	glm::mat4 viewMatrix;

	// 透视矩阵
	glm::mat4 projMatrix;

	void _UpdateViewMatrix()
	{
		viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	void _UpdateProjMatrix()
	{
		projMatrix = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
	}

public:
	Camera():
		cameraPos(glm::vec3(0.0f, 0.0f, 3.0f)),
		cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
		cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
		viewMatrix(glm::mat4(1.0f)),
		projMatrix(glm::mat4(1.0f))
	{
		_UpdateViewMatrix();
		_UpdateProjMatrix();
	}

	const glm::vec3& GetPos()
	{
		return cameraPos;
	}

	const glm::vec3& GetDir()
	{
		return cameraFront;
	}

	void SetPos(Camera_Movement move, const float deltaTime)
	{
		float cameraSpeed = posSensitivity * deltaTime; // adjust accordingly
		switch (move)
		{
		case Camera_Movement::front:
			cameraPos += cameraSpeed * cameraFront;
			break;
		case Camera_Movement::back:
			cameraPos -= cameraSpeed * cameraFront;
			break;
		case Camera_Movement::left:
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			break;
		case Camera_Movement::right:
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			break;
		default:
			return;
		}
		_UpdateViewMatrix();
	}

	void SetPitchYaw(double xoffset, double yoffset)
	{
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;
		pitch = std::max(std::min(pitch, 89.0f), -89.0f);

		glm::vec3 front;
		front.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = -cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		cameraFront = glm::normalize(front);
		_UpdateViewMatrix();
	}

	void SetFOV(float yOffset)
	{
		if (fov >= 1.0f && fov <= 45.0f)
			fov -= yOffset;
		if (fov <= 1.0f)
			fov = 1.0f;
		if (fov >= 45.0f)
			fov = 45.0f;
		_UpdateProjMatrix();
	}

	const glm::mat4& GetViewMatrix()
	{
		return viewMatrix;
	}

	const glm::mat4& GetProjectMatrix()
	{
		return projMatrix;
	}
};
