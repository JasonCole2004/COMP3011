#pragma once

#include <stdio.h>
#include <glm/glm.hpp>

struct SCamera
{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;

	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	const float MovementSpeed = 2.0f;
	float MouseSensitivity = 1.5f;



};


void InitCamera(SCamera& in)
{
	in.Front = glm::vec3(0.0f, 0.0f, -1.0f);
	in.Position = glm::vec3(0.0f, 0.0f, 0.0f);
	in.Up = glm::vec3(0.0f, 1.0f, 0.0f);
	in.WorldUp = in.Up;
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));

	in.Yaw = -90.f;
	in.Pitch = 0.f;
}

float cam_dist = 2.f;

void MoveAndOrientCamera(SCamera& in,glm::vec3 target,float distance,float xoffset,float yoffset)
{
	// Update yaw and pitch (inverted for orbit camera)
	in.Yaw -= xoffset * in.MovementSpeed;
	in.Pitch -= yoffset * in.MovementSpeed;

	if (in.Pitch > 89.0f) in.Pitch = 89.0f;
	if (in.Pitch < -89.0f) in.Pitch = -89.0f;

	// Convert to radians
	float yawRad = glm::radians(in.Yaw);
	float pitchRad = glm::radians(in.Pitch);

	// Calculate new camera position
	in.Position.x = target.x + distance * cos(pitchRad) * cos(yawRad);
	in.Position.y = target.y + distance * sin(pitchRad);
	in.Position.z = target.z + distance * cos(pitchRad) * sin(yawRad);

	// Calculate new Front vector 
	in.Front = glm::normalize(target - in.Position);

	// Calculate new Right vector 
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));

	// Calculate new Up vector
	in.Up = glm::normalize(glm::cross(in.Right, in.Front));
}