#pragma once

#include "GraphicsUtils.h"


namespace Graphics
{
	class Grid
	{
	public:
		Grid();
		void InitGridxz();
		void LoadGridxz();
		void InitGridxy();
		void LoadGridxy();

		Line* m_LineBuffer;
		Line* m_LineBufferPointer;
		LineIndices* m_GridLineIndices;
		LineIndices* m_GridLineIndicesPointer;
		unsigned int m_LineCount;
		unsigned int m_va;
		unsigned int m_vb;
		unsigned int m_ib;

		float m_grid_size;
		float m_grid_line_distance;
		int m_max_grid_lines;

		~Grid();


	private:



	};
}