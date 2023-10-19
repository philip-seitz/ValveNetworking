#include "DebugUtils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

namespace Graphics
{

    void PrintErrorMsg(const int& errorcode)
    {
        std::ifstream stream("../Dependencies/GLEW/include/GL/glew.h");
        std::string line;

        std::stringstream ss;
        ss << std::hex << errorcode;
        std::string hexcode = ss.str();
        while (getline(stream, line))
        {
            if (line.find(hexcode) != std::string::npos)
            {
                hexcode = "0x0" + hexcode;
                auto pos = line.find(hexcode);
                line.erase(pos, hexcode.length());
                pos = line.find("#define ");
                line.erase(pos, 8);
                std::cout << "Error Description: " << line << std::endl;
                break;
            }
        }
    }

    bool GLLogCall(const char* function, const char* file, int line)
    {
        if (GLenum error = glGetError())
        {
            std::cout << "================================ ERROR MESSAGE =================================" << std::endl;
            std::cout << "[OpenGL Error] (" << error << "), " << "Function Name: " << function << std::endl;
            std::cout << "File Name: " << file << ", " << "Line #: " << line << std::endl;
            PrintErrorMsg(error);
            std::cout << "================================================================================" << std::endl;
            return false;
        }
        else
            return true;
    }

    void GLClearError()
    {
        while (glGetError() != GL_NO_ERROR)
        {

        }
    }
}