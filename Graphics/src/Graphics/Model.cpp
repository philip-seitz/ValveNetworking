#include "Model.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace Graphics {

	Model::Model() :
		TriCount(0),
		va(0),
		vb(0),
		ib(0),
		TriBuffer(nullptr),
		TriBufferPointer(nullptr),
		TriIndices(nullptr),
		TriIndicesPointer(nullptr)

	{
	}

	Model::~Model()
	{
		glDeleteBuffers(1, &vb);
		glDeleteBuffers(1, &ib);
		delete[] TriBuffer;
		delete[] TriIndices;

		glDeleteVertexArrays(1, &vb);
	}


	void Model::InitMeshFromData(const std::string& filepath)
	{
		LoadMesh(filepath);

		// vertex array object setup
		glCreateVertexArrays(1, &va);
		glBindVertexArray(va);

		glCreateBuffers(1, &vb);
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * TriCount, TriBuffer, GL_STATIC_DRAW);

		glCreateBuffers(1, &ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * TriCount * 3, TriIndices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texture_cords));
	}

	void Model::InitSimpleCube()
	{
		LoadSimpleCube();
		// vertex array object setup
		glCreateVertexArrays(1, &va);
		glBindVertexArray(va);

		glCreateBuffers(1, &vb);
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * TriCount, TriBuffer, GL_STATIC_DRAW);

		glCreateBuffers(1, &ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * TriCount * 3, TriIndices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	}
	void Model::LoadSimpleCube()
	{
		std::vector<Triangle> mesh;

		glm::vec3 center = { 0.0f, 0.0f, 0.0f };
		float side = 2.0f;

		glm::vec3 front_center = center + glm::vec3(0.0f, 0.0f, side / 2.0f);
		glm::vec3 back_center = center - glm::vec3(0.0f, 0.0f, side / 2.0f);
		glm::vec3 right_center = center + glm::vec3(side / 2.0f, 0.0f, 0.0f);
		glm::vec3 left_center = center - glm::vec3(side / 2.0f, 0.0f, 0.0f);
		glm::vec3 top_center = center + glm::vec3(0.0f, side / 2.0f, 0.0f);
		glm::vec3 bottom_center = center - glm::vec3(0.0f, side / 2.0f, 0.0f);


		// NORTH (back)
		mesh.push_back({ {back_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f)},
						{back_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f)},
						{back_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f)} });

		mesh.push_back({ {back_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f)},
						{back_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f)},
						{back_center + glm::vec3(side / 2.0f,  side / 2.0f, 0.0f)} });

		// SOUTH (front)
		mesh.push_back({ {front_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f)},
						{front_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f)},
						{front_center + glm::vec3(side / 2.0f, side / 2.0f, 0.0f)} });

		mesh.push_back({ {front_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f)},
						{front_center + glm::vec3(side / 2.0f, side / 2.0f, 0.0f)},
						{front_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f)} });

		// EAST (right)
		mesh.push_back({ {right_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f)},
						{right_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f)},
						{right_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f)} });

		mesh.push_back({ {right_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f)},
						{right_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f)},
						{right_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f)} });

		// WEST (left)
		mesh.push_back({ {left_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f)},
						{left_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f)},
						{left_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f)} });

		mesh.push_back({ {left_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f)},
						{left_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f)},
						{left_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f)} });

		// BOTTOM 
		mesh.push_back({ {bottom_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f)},
						{bottom_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f)},
						{bottom_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f)} });

		mesh.push_back({ {bottom_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f)},
						{bottom_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f)},
						{bottom_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f)} });

		// TOP 
		mesh.push_back({ {top_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f)},
						{top_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f)},
						{top_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f)} });

		mesh.push_back({ {top_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f)},
						{top_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f)},
						{top_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f)} });

		// allocating buffers
		int triNumber = mesh.size();
		TriBuffer = new Triangle[triNumber];
		TriIndices = new TriangleIndices[triNumber];
		TriBufferPointer = TriBuffer;
		TriIndicesPointer = TriIndices;

		for (unsigned int i = 0; i < mesh.size(); i++)
		{
			mesh[i].SetNormal();
			*(TriBufferPointer) = mesh[i];
			*(TriIndicesPointer) = { {0 + 3 * (TriCount), 1 + 3 * (TriCount), 2 + 3 * (TriCount)} };
			TriBufferPointer++;
			TriIndicesPointer++;
			TriCount++;
		}
	}

	void Model::InitCube(const bool& useTexture)
	{
		LoadCube();

		// vertex array object setup
		glCreateVertexArrays(1, &va);
		glBindVertexArray(va);

		glCreateBuffers(1, &vb);
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * TriCount, TriBuffer, GL_STATIC_DRAW);

		glCreateBuffers(1, &ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * TriCount * 3, TriIndices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texture_cords));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
	}

	void Model::LoadCube()
	{
		std::vector<Triangle> mesh;

		glm::vec3 center = { 0.0f, 0.0f, 0.0f };
		float side = 2.0f;

		glm::vec3 front_center = center + glm::vec3(0.0f, 0.0f, side / 2.0f);
		glm::vec3 back_center = center - glm::vec3(0.0f, 0.0f, side / 2.0f);
		glm::vec3 right_center = center + glm::vec3(side / 2.0f, 0.0f, 0.0f);
		glm::vec3 left_center = center - glm::vec3(side / 2.0f, 0.0f, 0.0f);
		glm::vec3 top_center = center + glm::vec3(0.0f, side / 2.0f, 0.0f);
		glm::vec3 bottom_center = center - glm::vec3(0.0f, side / 2.0f, 0.0f);


		// NORTH (back)
		mesh.push_back({ {back_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f),		{WHITE}, {0.0f, 0.0f}},
						{back_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f), {WHITE}, {1.0f, 0.0f}},
						{back_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f),	{WHITE}, {1.0f, 1.0f}} });

		mesh.push_back({ {back_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f),	{WHITE}, {0.0f, 0.0f}},
						{back_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f),	{WHITE}, {1.0f, 1.0f}},
						{back_center + glm::vec3(side / 2.0f,  side / 2.0f, 0.0f),	{WHITE}, {0.0f, 1.0f}} });

		// SOUTH (front)
		mesh.push_back({ {front_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f),	{WHITE}, {0.0f, 0.0f}},
						{front_center + glm::vec3(side / 2.0f, -side / 2.0f, 0.0f),		{WHITE}, {1.0f, 0.0f}},
						{front_center + glm::vec3(side / 2.0f, side / 2.0f, 0.0f),		{WHITE}, {1.0f, 1.0f}} });

		mesh.push_back({ {front_center + glm::vec3(-side / 2.0f, -side / 2.0f, 0.0f),	{WHITE}, {0.0f, 0.0f}},
						{front_center + glm::vec3(side / 2.0f, side / 2.0f, 0.0f),		{WHITE}, {1.0f, 1.0f}},
						{front_center + glm::vec3(-side / 2.0f, side / 2.0f, 0.0f),		{WHITE}, {0.0f, 1.0f}} });

		// EAST (right)
		mesh.push_back({ {right_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f), {WHITE}, {0.0f, 0.0f}},
						{right_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f), {WHITE}, {1.0f, 0.0f}},
						{right_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f), {WHITE}, {1.0f, 1.0f}} });

		mesh.push_back({ {right_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f), {WHITE}, {0.0f, 0.0f}},
						{right_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f), {WHITE}, {1.0f, 1.0f}},
						{right_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f), {WHITE}, {0.0f, 1.0f}} });

		// WEST (left)
		mesh.push_back({ {left_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f), {WHITE}, {0.0f, 0.0f}},
						{left_center + glm::vec3(0.0f, -side / 2.0f, side / 2.0f), {WHITE}, {1.0f, 0.0f}},
						{left_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f), {WHITE}, {1.0f, 1.0f}} });
		mesh.push_back({ {left_center + glm::vec3(0.0f, -side / 2.0f, -side / 2.0f), {WHITE}, {0.0f, 0.0f}},
						{left_center + glm::vec3(0.0f, side / 2.0f, side / 2.0f), {WHITE}, {1.0f, 1.0f}},
						{left_center + glm::vec3(0.0f, side / 2.0f, -side / 2.0f), {WHITE}, {0.0f, 1.0f}} });

		// BOTTOM 
		mesh.push_back({ {bottom_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {0.0f, 0.0f}},
						{bottom_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {1.0f, 0.0f}},
						{bottom_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {1.0f, 1.0f}} });
		mesh.push_back({ {bottom_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {0.0f, 0.0f}},
						{bottom_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {1.0f, 1.0f}},
						{bottom_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {0.0f, 1.0f}} });

		// TOP 
		mesh.push_back({ {top_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {0.0f, 0.0f}},
						{top_center + glm::vec3(side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {1.0f, 0.0f}},
						{top_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {1.0f, 1.0f}} });
		mesh.push_back({ {top_center + glm::vec3(-side / 2.0f, 0.0f, side / 2.0f), {WHITE}, {0.0f, 0.0f}},
						{top_center + glm::vec3(side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {1.0f, 1.0f}},
						{top_center + glm::vec3(-side / 2.0f, 0.0f, -side / 2.0f), {WHITE}, {0.0f, 1.0f}} });

		// allocating buffers
		int triNumber = mesh.size();
		TriBuffer = new Triangle[triNumber];
		TriIndices = new TriangleIndices[triNumber];
		TriBufferPointer = TriBuffer;
		TriIndicesPointer = TriIndices;

		for (unsigned int i = 0; i < mesh.size(); i++)
		{
			mesh[i].SetNormal();
			*(TriBufferPointer) = mesh[i];
			*(TriIndicesPointer) = { {0 + 3 * (TriCount), 1 + 3 * (TriCount), 2 + 3 * (TriCount)} };
			TriBufferPointer++;
			TriIndicesPointer++;
			TriCount++;
		}
	}

	void Model::InitTriangle()
	{
		LoadTriangle();

		// vertex array object setup
		glCreateVertexArrays(1, &va);
		glBindVertexArray(va);

		glCreateBuffers(1, &vb);
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * TriCount, TriBuffer, GL_STATIC_DRAW);

		glCreateBuffers(1, &ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * TriCount * 3, TriIndices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texture_cords));
	}

	void Model::LoadTriangle()
	{
		// init and setting data
		std::vector<Triangle> mesh;

		glm::vec3 center = { 0.0f, 0.0f, 0.0f };
		float side = 4.0f;

		// one triangle
		mesh.push_back({ {{  center.x - side / 2, center.y - side / 2, 0.0f}, {WHITE}, {0.0f, 0.0f}}, {{center.x + side / 2, center.y - side / 2, 0.0f}, {WHITE}, {1.0f, 0.0f}}, {{center.x, center.y + side / 2, 0.0f}, {WHITE}, {0.5f, 1.0f}} });

		// allocating buffers
		int triNumber = mesh.size();
		TriBuffer = new Triangle[triNumber];
		TriIndices = new TriangleIndices[triNumber];
		TriBufferPointer = TriBuffer;
		TriIndicesPointer = TriIndices;

		for (unsigned int i = 0; i < mesh.size(); i++)
		{
			mesh[i].SetNormal();
			*(TriBufferPointer) = mesh[i];
			*(TriIndicesPointer) = { {0 + 3 * (TriCount), 1 + 3 * (TriCount), 2 + 3 * (TriCount)} };
			TriBufferPointer++;
			TriIndicesPointer++;
			TriCount++;
		}
	}

	void Model::InitQuad(float side, glm::vec3 center)
	{
		LoadQuad(side, center);

		// vertex array object setup
		glCreateVertexArrays(1, &va);
		glBindVertexArray(va);

		glCreateBuffers(1, &vb);
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * TriCount, TriBuffer, GL_STATIC_DRAW);

		glCreateBuffers(1, &ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * TriCount * 3, TriIndices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texture_cords));

	}

	void Model::LoadQuad(float side, glm::vec3 center)
	{
		// init and setting data
		std::vector<Triangle> mesh;

		// two triangles = 1 quad
		mesh.push_back({ {{  center.x - side / 2, center.y - side / 2, center.z}, {WHITE}, {0.0f, 0.0f}}, {{center.x + side / 2, center.y - side / 2, center.z}, {WHITE}, {1.0f, 0.0f}}, {{center.x + side / 2, center.y + side / 2, center.z}, {WHITE}, {1.0f, 1.0f}} });
		mesh.push_back({ {{  center.x + side / 2, center.y + side / 2, center.z}, {WHITE}, {1.0f, 1.0f}}, {{center.x - side / 2, center.y + side / 2, center.z}, {WHITE}, {0.0f, 1.0f}}, {{center.x - side / 2, center.y - side / 2, center.z}, {WHITE}, {0.0f, 0.0f}} });

		// allocating buffers
		int triNumber = mesh.size();
		TriBuffer = new Triangle[triNumber];
		TriIndices = new TriangleIndices[triNumber];
		TriBufferPointer = TriBuffer;
		TriIndicesPointer = TriIndices;

		for (unsigned int i = 0; i < mesh.size(); i++)
		{
			mesh[i].SetNormal();
			*(TriBufferPointer) = mesh[i];
			*(TriIndicesPointer) = { {0 + 3 * (TriCount), 1 + 3 * (TriCount), 2 + 3 * (TriCount)} };
			TriBufferPointer++;
			TriIndicesPointer++;
			TriCount++;
		}
	}

	void Model::LoadMesh(const std::string& filepath)
	{
		std::vector <float>vert;
		std::vector<unsigned int> ind;
		glm::vec4 color = { PURPLE };
		std::vector<glm::vec3> mesh;
		std::vector< glm::highp_uvec3> indices;
		ReadVertexData(filepath, vert, ind);
		// put the data into the triangle structures...
		int vertexNumber = vert.size() / 3;			// number of unique vertex positions
		int triNumber = ind.size() / 3;

		// allocating buffers (CPU)
		TriBuffer = new Triangle[triNumber];
		TriIndices = new TriangleIndices[triNumber];
		TriBufferPointer = TriBuffer;
		TriIndicesPointer = TriIndices;

		for (int i = 0; i < vertexNumber; i++)
		{
			glm::vec3 pos;
			pos = { vert[0 + i * 3], vert[1 + i * 3], vert[2 + i * 3] };
			mesh.push_back(pos);
		}

		for (int i = 0; i < triNumber; i++)
		{
			glm::highp_uvec3 idx;
			idx = { ind[0 + i * 3], ind[1 + i * 3], ind[2 + i * 3] };
			indices.push_back(idx);
		}

		for (int i = 0; i < triNumber; i++)
		{
			*(TriBufferPointer) = Triangle{ {mesh[indices[i][0] - 1], color}, {mesh[indices[i][1] - 1], color}, {mesh[indices[i][2] - 1], color} };
			TriBufferPointer->SetNormal();
			*(TriIndicesPointer) = { {0 + 3 * (TriCount), 1 + 3 * (TriCount), 2 + 3 * (TriCount)} };
			TriBufferPointer++;
			TriIndicesPointer++;
			TriCount++;
		}
	}

}
