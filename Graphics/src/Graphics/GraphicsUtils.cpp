#include "GraphicsUtils.h"
#include <iostream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include <GL/glew.h>

namespace Graphics
{
	void TextureFromFile(const std::string& path, unsigned int& id)
	{
		stbi_set_flip_vertically_on_load(1);
		int width;
		int height;
		int bpp;

		unsigned char* buffer = stbi_load(path.c_str(), &width, &height, &bpp, 4);
		// glActiveTexture(texture_slot);		// texture slot that should be used
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		// texture filtering 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);		// mipmaps switching only used when down scaling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// texture wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if (buffer)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			// generate mipmap
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glBindTexture(GL_TEXTURE_2D, 0);		// unbind
		stbi_image_free(buffer);
	}

	int CalculateNumberOfLines(float size, float distance)
	{

		float t = (size / 2.0) / distance;	// number of lines per half 
		if (std::fmod(t, 2.0) != 0.0)	// odd
		{
			if (t < 2.0)
			{
				t = t + (2 - t);	// fill to be even
			}
			else if (std::fmod(t, 2.0) < 1.0)
			{
				t = t + (2.0 - std::fmod(t, 2.0)); // t = t - std::fmod(t, 2.0);
				return  2 + 4 * t - 4;
			}
			else
			{
				t = t + (2.0 - std::fmod(t, 2.0));
				return  2 + 4 * t;
			}
		}
		else
		{
			return 2 + 4 * t + 4;
		}

	}

	void StringToFloat(const std::vector<std::vector <char>>& stream, std::vector <float>& vert)
	{
		std::vector<float> res_vec;
		unsigned int vert_count = 0;
		for (int i = 0; i < stream.size(); i++)
		{
			int len = stream[i].size();
			int p_idx = 0;
			float result = 0.0f;
			bool negative = false;
			if (stream[i][0] == 45)
			{
				negative = true;
			}

			while (stream[i][p_idx] != 46)					// dot
			{
				p_idx++;
			}

			for (int j = 0 + negative; j < p_idx; j++)
			{
				int digit = stream[i][j] - 48;				// char to digit
				result += digit * pow(10, p_idx - (1 + j));
			}

			for (int j = p_idx + 1; j < len; j++)
			{
				int digit = stream[i][j] - 48;
				float tmp = digit * pow(10, p_idx - j);
				result += digit * pow(10, p_idx - j);
			}

			if (negative)
				result *= -1.0f;

			res_vec.push_back(result);
			vert_count++;
		}
		vert = res_vec;
	}

	void StringToInt(const std::vector<std::vector <char>>& stream, std::vector <unsigned int>& ind)
	{
		std::vector<unsigned int> res_vec;
		unsigned int idx_count = 0;
		for (int i = 0; i < stream.size(); i++)
		{
			int len = stream[i].size();
			unsigned int  result = 0;

			for (int j = 0; j < len; j++)
			{
				int digit = stream[i][j] - 48;
				result += digit * pow(10, len - (1 + j));
			}

			res_vec.push_back(result);
			idx_count++;
		}

		ind = res_vec;
	}

	void ReadVertexData(const std::string& filepath, std::vector <float>& vert, std::vector<unsigned int>& ind)
	{
		std::string line;
		std::ifstream stream(filepath);
		std::vector<std::vector <char>> vertex_vectors;
		std::vector<std::vector <char>> index_vectors;

		while (getline(stream, line))
		{
			if (line.find("v") != std::string::npos)
			{
				for (int i = 1; i < line.size(); i++)
				{
					if ((line[i] >= 48 && line[i] <= 57) || line[i] == 45)		// numbers from 0 to 9 or minus
					{
						std::vector<char> number_char;

						while (line[i] != 32)				// space
						{
							number_char.push_back(line[i]);
							if (i + 1 < line.size())
								i++;
							else
								break;
						}
						vertex_vectors.push_back(number_char);
					}
				}
			}
			else if (line.find("f") != std::string::npos)
			{
				for (int i = 1; i < line.size(); i++)
				{
					if ((line[i] >= 48 && line[i] <= 57) || line[i] == 45)		// numbers from 0 to 9 or minus
					{
						std::vector<char> number_char;

						while (line[i] != 32)				// space
						{
							number_char.push_back(line[i]);
							if (i + 1 < line.size())
								i++;
							else
								break;
						}
						index_vectors.push_back(number_char);
					}
				}
			}
			else
			{
			}
		}
		// converts strings/char arrays to numbers
		StringToFloat(vertex_vectors, vert);
		StringToInt(index_vectors, ind);
	}
}