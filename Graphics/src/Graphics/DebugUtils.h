#pragma once

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
x;\
ASSERT(GLLogCall(#x, __FILE__, __LINE__));

#include <GL/glew.h>
namespace Graphics
{
	// alle Sachen, um die Fehler zu handeln usw.


	void PrintErrorMsg(const int& errorcode);

	bool GLLogCall(const char* function, const char* file, int line);

	void GLClearError();
}