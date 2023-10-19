#pragma once
#include <iostream>
#include "GL/glew.h"

namespace Graphics
{
	class Texture
	{
	public:
		Texture(const std::string& path, std::string& type, const int& id);
		Texture(const std::string& path, const unsigned short& slot, const unsigned short& wrap_mode);
		~Texture();

		void Bind();
		void UnBind();

		std::string GetType();
		std::string GetPath();
		void Init();			// TODO: when mesh loading is implemented
		unsigned int GetID();

		int GetWidth();
		int GetHeight();

	private:
		unsigned int m_RendererID;
		std::string m_FilePath;
		unsigned char* m_LocalBuffer;
		std::string m_Type;
		short m_Slot;
		int m_Width;
		int m_Height;
		int m_BPP;
	};
}
