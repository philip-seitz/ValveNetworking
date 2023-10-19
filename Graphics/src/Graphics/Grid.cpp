#include "Grid.h"
#include "GraphicsUtils.h"
#include <GL/glew.h>

namespace Graphics
{
	Grid::Grid() :
		m_va(0),
		m_vb(0),
		m_ib(0),
		m_LineCount(0),
		m_LineBuffer(nullptr),
		m_LineBufferPointer(nullptr),
		m_GridLineIndices(nullptr),
		m_GridLineIndicesPointer(nullptr)
	{
		m_grid_size = 100.0f;
		m_grid_line_distance = 1.0f;
		m_max_grid_lines = CalculateNumberOfLines(float(m_grid_size), float(m_grid_line_distance));
	}

	Grid::~Grid()
	{
		delete[] m_LineBuffer;
		delete[] m_GridLineIndices;
	}

	void Grid::InitGridxz()
	{
		LoadGridxz();

		// setup for grid lines
		glCreateVertexArrays(1, &m_va);
		glBindVertexArray(m_va);

		glCreateBuffers(1, &m_vb);
		glBindBuffer(GL_ARRAY_BUFFER, m_vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Line) * m_max_grid_lines, m_LineBuffer, GL_STATIC_DRAW);

		glCreateBuffers(1, &m_ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_max_grid_lines * 2, m_GridLineIndices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));
	}

	void Grid::LoadGridxz()
	{
		m_LineBuffer = new Line[m_max_grid_lines];
		m_GridLineIndices = new LineIndices[m_max_grid_lines];
		m_LineBufferPointer = m_LineBuffer;
		m_GridLineIndicesPointer = m_GridLineIndices;

		Line z_line = { {{0.0f, 0.0f, int(m_grid_size) / 2.0f}}, {{0.0f, 0.0f, -float(m_grid_size) / 2.0f}} };
		Line x_line = { {{float(m_grid_size) / 2.0f, 0.0f, 0.0f}}, {{-float(m_grid_size) / 2.0f, 0.0f, 0.0f}} };

		glm::vec3 interval_z = { 0.0f, 0.0f, m_grid_line_distance };
		glm::vec3 interval_x = { m_grid_line_distance, 0.0f, 0.0f };

		int j = 0;
		int k = 0;
		int l = 0;
		int limit = m_max_grid_lines - 2;		// minus inner x and z line plus outer lines
		for (int i = 0; i < limit; i++)
		{
			m_LineBufferPointer->p0.color = { GREY };
			m_LineBufferPointer->p1.color = { GREY };

			if (i < limit * 1 / 4)
			{
				if (i == 0)
				{
					m_LineBufferPointer->p0.color = { BLUE };
					m_LineBufferPointer->p1.color = { BLUE };
				}
				m_LineBufferPointer->p0.position = z_line.p0.position + interval_x * float(i);
				m_LineBufferPointer->p1.position = z_line.p1.position + interval_x * float(i);
			}

			else if (i < limit * 2 / 4)
			{
				m_LineBufferPointer->p0.position = z_line.p0.position - interval_x * float(j);
				m_LineBufferPointer->p1.position = z_line.p1.position - interval_x * float(j);
				j++;
			}

			else if (i < limit * 3 / 4)
			{
				if (k == 0)
				{
					m_LineBufferPointer->p0.color = { RED };
					m_LineBufferPointer->p1.color = { RED };
				}
				m_LineBufferPointer->p0.position = x_line.p0.position + interval_z * float(k);
				m_LineBufferPointer->p1.position = x_line.p1.position + interval_z * float(k);
				k++;
			}

			else
			{
				m_LineBufferPointer->p0.position = x_line.p0.position - interval_z * float(l);
				m_LineBufferPointer->p1.position = x_line.p1.position - interval_z * float(l);
				l++;
			}

			*(m_GridLineIndicesPointer) = { {2 * i, 2 * i + 1} };
			m_LineCount++;
			m_GridLineIndicesPointer++;
			m_LineBufferPointer++;
		}
	}
	void Grid::InitGridxy()
	{
		LoadGridxy();

		// setup for grid lines
		glCreateVertexArrays(1, &m_va);
		glBindVertexArray(m_va);

		glCreateBuffers(1, &m_vb);
		glBindBuffer(GL_ARRAY_BUFFER, m_vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Line) * m_max_grid_lines, m_LineBuffer, GL_STATIC_DRAW);

		glCreateBuffers(1, &m_ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_max_grid_lines * 2, m_GridLineIndices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

	}
	void Grid::LoadGridxy()
	{
		m_LineBuffer = new Line[m_max_grid_lines];
		m_GridLineIndices = new LineIndices[m_max_grid_lines];
		m_LineBufferPointer = m_LineBuffer;
		m_GridLineIndicesPointer = m_GridLineIndices;

		Line y_line = { {{0.0f, int(m_grid_size) / 2.0f, 0.0f}}, {{0.0f, -float(m_grid_size) / 2.0f, 0.0f}} };
		Line x_line = { {{float(m_grid_size) / 2.0f, 0.0f, 0.0f}}, {{-float(m_grid_size) / 2.0f, 0.0f, 0.0f}} };

		glm::vec3 interval_y = { 0.0f, m_grid_line_distance, 0.0f };
		glm::vec3 interval_x = { m_grid_line_distance, 0.0f, 0.0f };

		int j = 0;
		int k = 0;
		int l = 0;
		int limit = m_max_grid_lines - 2;		// minus inner x and z line plus outer lines
		for (int i = 0; i < limit; i++)
		{
			m_LineBufferPointer->p0.color = { GREY };
			m_LineBufferPointer->p1.color = { GREY };

			if (i < limit * 1 / 4 )
			{
				if (i == 0)
				{
					m_LineBufferPointer->SetColor(0.3f, 0.8f, 0.2f, 1.0f );
				}
				m_LineBufferPointer->p0.position = y_line.p0.position + interval_x * float(i);
				m_LineBufferPointer->p1.position = y_line.p1.position + interval_x * float(i);
			}

			else if (i < limit * 2 / 4)
			{
				m_LineBufferPointer->p0.position = y_line.p0.position - interval_x * float(j);
				m_LineBufferPointer->p1.position = y_line.p1.position - interval_x * float(j);
				j++;
			}

			else if (i < limit * 3 / 4)
			{
				if (k == 0)
				{
					m_LineBufferPointer->SetColor(0.8f, 0.3f, 0.2f, 1.0f);
				}
				m_LineBufferPointer->p0.position = x_line.p0.position + interval_y * float(k);
				m_LineBufferPointer->p1.position = x_line.p1.position + interval_y * float(k);
				k++;
			}

			else
			{
				m_LineBufferPointer->p0.position = x_line.p0.position - interval_y * float(l);
				m_LineBufferPointer->p1.position = x_line.p1.position - interval_y * float(l);
				l++;
			}

			*(m_GridLineIndicesPointer) = { {2 * i, 2 * i + 1} };
			m_LineCount++;
			m_GridLineIndicesPointer++;
			m_LineBufferPointer++;
		}
	}
}