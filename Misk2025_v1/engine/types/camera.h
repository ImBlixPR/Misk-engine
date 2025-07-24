#pragma once

#include <glm\glm.hpp>





class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, float startYaw, float startPitch, float startMoveSpeed, float startTurnSpeed);

	void free_camera(float deltaTime);
	void mouse_control(float xOffset, float yOffset);

	glm::mat4 calculate_view_matrix();
	inline glm::vec3 get_front() { return front; }
	inline glm::vec3 get_up_vector() { return up; }
	inline glm::vec3 get_up_world_vector() { return glm::vec3(0.0f, 1.0f, 0.0f); }

	glm::vec3 get_camera_position();
	glm::vec3 get_camera_direction();

	~Camera();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	float yaw;
	float pitch;

	float moveSpeed;
	float turnSpeed;
	void update();
};
