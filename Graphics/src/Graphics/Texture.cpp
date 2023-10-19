#include "Texture.h"
#include "stb_image/stb_image.h"

namespace Graphics
{

	Texture::Texture(const std::string& path, std::string& type, const int& id) :
		m_RendererID(id),
		m_FilePath(path),
		m_Type(type),
		m_LocalBuffer(nullptr),
		m_Slot(0),
		m_Width(0),
		m_Height(0),
		m_BPP(0)
	{
		// TODO: when implementation is clearer
	}

	Texture::Texture(const std::string& path, const unsigned short& slot, const unsigned short& wrap_mode) :
		m_RendererID(0),
		m_FilePath(path),
		m_Type(""),
		m_LocalBuffer(nullptr),
		m_Slot(slot),
		m_Width(0),
		m_Height(0),
		m_BPP(0)
	{
		short texture_slot = GL_TEXTURE0 + slot;
		stbi_set_flip_vertically_on_load(1);

		m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

		glActiveTexture(texture_slot);		// texture slot that should be used
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		// texture filtering 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);		// mipmaps switching only used when down scaling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// texture wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);


		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
		// generate mipmap
		glGenerateMipmap(GL_TEXTURE_2D);

		// Unbind and free
		glBindTexture(GL_TEXTURE_2D, 0);
		if (m_LocalBuffer)
		{
			stbi_image_free(m_LocalBuffer);
		}
	}

	Texture::~Texture()
	{
		// TODO: commented out to avoid deletion of id
		// glDeleteTextures(1, &m_RendererID);		// needed in case of dynamic texture (new)
	}

	void Texture::Bind()
	{
		glActiveTexture(GL_TEXTURE0 + m_Slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void Texture::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::Init()
	{
		static int slot = 0;
		// TODO: implement when ready
		short texture_slot = GL_TEXTURE0 + slot;
		stbi_set_flip_vertically_on_load(1);

		m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 4);

		glActiveTexture(texture_slot);		// texture slot that should be used
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		// texture filtering 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);		// mipmaps switching only used when down scaling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// texture wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
		// generate mipmap
		glGenerateMipmap(GL_TEXTURE_2D);

		// Unbind and free
		glBindTexture(GL_TEXTURE_2D, 0);
		if (m_LocalBuffer)
		{
			stbi_image_free(m_LocalBuffer);
		}
	}

	std::string Texture::GetType()
	{
		return m_Type;
	}

	std::string Texture::GetPath()
	{
		return m_FilePath;
	}

	unsigned int Texture::GetID()
	{
		return m_RendererID;
	}

	int Texture::GetWidth()
	{

		return m_Width;
	}

	int Texture::GetHeight()
	{
		return m_Height;
	}
}
