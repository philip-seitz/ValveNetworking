#pragma once
#include <glm/glm.hpp>
#include "Model.h"
#include "ModelInstance.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Graphics
{
	ModelInstance::ModelInstance() :
		position(0.0f, 0.0f, 0.0f),
		angles(0.0f, 0.0f, 0.0f),
		scale(1.0f, 1.0f, 1.0f),
		color(0.5f, 0.5f, 0.5f),
		hasOutline(true)
	{
	}

	glm::mat4x4 ModelInstance::ModelMatrix()
	{
		glm::mat4x4 combinedRotation(0);
		glm::mat4x4 translation(0);
		glm::mat4x4 scaling(0);
		combinedRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angles.x),
			glm::vec3(1.0, 0.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
				glm::radians(angles.y), glm::vec3(0.0, 1.0, 0.0)) * glm::rotate(glm::mat4(1.0f),
					glm::radians(angles.z), glm::vec3(0.0, 0.0, 1.0));
		translation = glm::translate(glm::mat4(1.0f), position);
		scaling = glm::scale(glm::mat4(1.0f), scale);

		return translation * combinedRotation * scaling;
	}

	glm::mat3x3 ModelInstance::NormalMatrix()
	{
		return glm::transpose(glm::inverse(ModelMatrix()));
	}

	void ModelInstance::Draw(const Model& base_model, Shader* shader)
	{
		glBindVertexArray(base_model.va);

		// enable textures [0 -> no; 1 -> yes]
		// shader->SetUniform1i("u_Use_Texture", 1);
		shader->SetUniformMat4f("u_Model", ModelMatrix());

		// draw all vertices 
		glDrawElements(GL_TRIANGLES, 3 * base_model.TriCount, GL_UNSIGNED_INT, nullptr);

		// === Draw the points === 
		// glDrawArrays(GL_POINTS, 0, 3 * sData.TriToRenderCount);
	}

	void ModelInstance::DrawOutline(const Model& base_model, Shader* shader, Shader* outline)
	{
		glBindVertexArray(base_model.va);
		// scale model for outline
		float border = 0.01f;
		glm::vec3 temp = scale;
		scale = { scale.x * (scale.x + border) / (scale.x),scale.y * (scale.y + border) / (scale.y), scale.z * (scale.z + border) / (scale.z) };
		outline->SetUniformMat4f("u_Model", ModelMatrix());

		glDrawElements(GL_TRIANGLES, 3 * base_model.TriCount, GL_UNSIGNED_INT, nullptr);
		scale = temp;
	}
}
