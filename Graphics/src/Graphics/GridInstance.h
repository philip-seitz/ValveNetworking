#pragma once
#include "Grid.h"
#include "Shader.h"

namespace Graphics {
	class GridInstance
	{
	public:
		GridInstance();
		void Draw(const Grid& base_model, Shader* shader);
		glm::mat4x4 ModelMatrix();

		glm::vec3 m_position;
		glm::vec3 m_angles;
		glm::vec3 m_scale;

	private:

	};
}
