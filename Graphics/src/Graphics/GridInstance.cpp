#include "GridInstance.h"
#include <GL/glew.h>

namespace Graphics
{
	GridInstance::GridInstance() :
		m_position(0.0f, 0.0f, 0.0f),
		m_angles(0.0f, 0.0f, 0.0f),
		m_scale(1.0f, 1.0f, 1.0f)
	{}

	void GridInstance::Draw(const Grid& base_model, Shader* shader)
	{
		glBindVertexArray(base_model.m_va);

		// improve appearence of the grid (anti z-fighting and thinner lines)
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glLineWidth(1.0f);

		shader->SetUniformMat4f("u_Model", ModelMatrix());
		glDrawElements(GL_LINES, 2 * base_model.m_LineCount, GL_UNSIGNED_INT, nullptr);

		// disable blending when done
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}

	glm::mat4x4 GridInstance::ModelMatrix()
	{
		glm::mat4x4 combinedRotation(0);
		glm::mat4x4 translation(0);
		glm::mat4x4 scaling(0);

		combinedRotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_angles.x), glm::vec3(1.0, 0.0, 0.0)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_angles.y), glm::vec3(0.0, 1.0, 0.0)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_angles.z), glm::vec3(0.0, 0.0, 1.0));
		translation = glm::translate(glm::mat4(1.0f), m_position);
		scaling = glm::scale(glm::mat4(1.0f), m_scale);

		return translation * combinedRotation * scaling;
	}

}