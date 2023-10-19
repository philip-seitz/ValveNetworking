#pragma once
#include <glm/glm.hpp>
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include "Window.h"

namespace Graphics
{
	class Camera
	{

	public:
		Camera();								// perspective is standard
		Camera(const bool& isPerspective);
		glm::vec3 Position();
		glm::vec3 LookDirection();
		glm::mat4x4 LookAtMatrix();
		glm::mat4x4 ProjectionMatrix();
		const void ManageInputs(Window& window, const float& dt);
		const void UpdatePosition(Window& window, const float& dt);
		const void UpdateDirection(Window& window, const float& dt);
		const void UpdateFromMouseCallback(const short& xoffset, const short& yoffset);
		const void UpdateFromScrollCallback(const float& xoffset, const float& yoffset);
		const void Zoom(Window& window, const float& dt);
		const void SetDirection(glm::vec3 direction);

	private:
		// 3D or 2D 
		bool m_proj_3D;
		bool m_UseMouseInput;

		// projection matrix
		float m_yFOV;
		float m_zoom;
		float m_aspectRatio;
		float m_zNear;
		float m_zFar;
		glm::mat4x4 m_perspective_proj;
		glm::mat4x4 m_orthographic_proj;

		// look at matrix
		glm::vec3 m_position;
		glm::vec3 m_target;
		glm::vec3 m_up;
		glm::mat4x4 m_look_at;

		// variables for movement
		glm::vec3 m_look_direction;
		float m_movement_speed;
		float m_camera_angle_yaw;
		float m_camera_angle_pitch;
	};
}