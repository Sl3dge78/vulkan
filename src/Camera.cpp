#include "Camera.h"

void Camera::start() {
	position = glm::vec3(0.f, -2.f, 0.f);
	front = glm::vec3(0., 1.f, 0.f);
	up = glm::vec3(0.f, 0.f, 1.f);
}

void Camera::update(float delta_time) {
	int mouse_x, mouse_y;

	if (SDL_GetRelativeMouseState(&mouse_x, &mouse_y) == SDL_BUTTON_RIGHT) {
		yaw += mouse_x * camera_speed * delta_time;
		pitch -= mouse_y * camera_speed * delta_time;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;

		direction.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.z = sin(glm::radians(pitch));

		front = glm::normalize(direction);
		glm::vec3 right = glm::cross(front, glm::vec3(0.f, 0.f, -1.f));
		up = glm::cross(front, right);
	}

	float delta_speed = delta_time * move_speed;

	auto keyboard = SDL_GetKeyboardState(NULL);
	if (keyboard[SDL_SCANCODE_LSHIFT])
		delta_speed *= 2.0f;

	if (keyboard[SDL_SCANCODE_W])
		position += delta_speed * front;
	if (keyboard[SDL_SCANCODE_S])
		position -= delta_speed * front;
	if (keyboard[SDL_SCANCODE_A])
		position -= delta_speed * glm::normalize(glm::cross(front, up));
	if (keyboard[SDL_SCANCODE_D])
		position += delta_speed * glm::normalize(glm::cross(front, up));
	if (keyboard[SDL_SCANCODE_Q])
		position -= delta_speed * up;
	if (keyboard[SDL_SCANCODE_E])
		position += delta_speed * up;
}

void Camera::get_view_matrix(glm::mat4 &view) {
	view = glm::lookAt(position, position + front, up);
	//view = glm::lookAt(position, front, up);
}
