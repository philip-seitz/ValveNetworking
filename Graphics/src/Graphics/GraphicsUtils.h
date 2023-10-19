#pragma once
// for file reading
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <glm/glm.hpp>

#define RED 0.8f, 0.3f, 0.3f, 1.0f
#define GREEN 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE 0.3f, 0.3f, 0.7f, 1.0f
#define PURPLE 0.6f, 0.0f, 0.8f, 1.0f
#define ORANGE 0.9f, 0.3f, 0.3f, 1.0f
#define B_BLUE 0.0f, 0.5f, 0.9f, 1.0f
#define B_BLUE_SHADE 0.0f, 0.3f, 0.7f, 1.0f
#define B_BLUE_DARK 0.0f, 0.1f, 0.5f, 1.0f
#define WHITE 1.0f, 1.0f, 1.0f, 1.0f
#define GREY 0.3f, 0.3f, 0.3f, 1.0f 

namespace Graphics
{
	int CalculateNumberOfLines(float size, float distance);
	void TextureFromFile(const std::string& path, unsigned int& id);
	void ReadVertexData(const std::string& filepath, std::vector <float>& vert, std::vector<unsigned int>& ind);

	struct Vertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texture_cords;
		glm::vec3 normal;
	};

	struct Triangle
	{

		Vertex p0, p1, p2;
		void SetColor(const float& r, const float& g, const float& b, const float& a)
		{
			p0.color = { r, g, b, a };
			p1.color = { r, g, b, a };
			p2.color = { r, g, b, a };

		}

		void SetColor(glm::vec4 color)
		{
			p0.color = color;
			p1.color = color;
			p2.color = color;
		}

		void SetNormal()
		{
			glm::vec3 p01 = p1.position - p0.position;
			glm::vec3 p02 = p2.position - p0.position;
			glm::vec3 normal = glm::normalize(glm::cross(p01, p02));

			p0.normal = normal;
			p1.normal = normal;
			p2.normal = normal;
		}
	};

	struct TriangleIndices
	{
		glm::highp_uvec3 indices;
	};

	struct Line
	{
		Vertex p0, p1;
		void SetColor(const float& r, const float& g, const float& b, const float& a)
		{
			p0.color = { r, g, b, a };
			p1.color = { r, g, b, a };
		}

		void SetColor(glm::vec4 color)
		{
			p0.color = color;
			p1.color = color;
		}

		void SetColor(glm::vec4 color1, glm::vec4 color2)
		{
			p0.color = color1;
			p1.color = color2;
		}
	};

	struct LineIndices
	{
		glm::highp_uvec2 indices;
	};
}