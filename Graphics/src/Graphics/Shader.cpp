#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "DebugUtils.h"

namespace Graphics
{
    Shader::Shader()
        :m_FilePath(""), m_RendererID(0)
    {

    }

    Shader::Shader(const std::string& filepath)
        : m_FilePath(filepath), m_RendererID(0)
    {
        ShaderProgramSource source = ParseShader(filepath);                         // Parsing = string analysis / durchgehen
        m_RendererID = CreateShader(source.vertex, source.fragment);               // shader erstellen compilen und linken. 
    }

    Shader::~Shader()
    {
        GLCall(glDeleteProgram(m_RendererID));
    }

    unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
    {
        unsigned int program = glCreateProgram();
        // ähnlich wie genBuffer Funktion aber nicht konstant mit ID-Übergabe :(
        unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
        unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

        GLCall(glAttachShader(program, vs));
        GLCall(glAttachShader(program, fs));
        GLCall(glLinkProgram(program));
        GLCall(glValidateProgram(program));         // Prüfen ob die Programme aktuell ausgeführt werden können

        GLCall(glDeleteShader(vs));                 // Shaderspeicherplatz freimachen, der durch CreateShader 
        GLCall(glDeleteShader(fs));                 // angelegt wurde. Wenn attached wird erst freigegeben, wenn detach gecalled

        return program;
    }

    unsigned Shader::CompileShader(unsigned int type, const std::string& source)
    {
        unsigned int id = glCreateShader(type);
        const char* src = &source[0];
        GLCall(glShaderSource(id, 1, &src, nullptr));       // shader source code hinzufügen
        GLCall(glCompileShader(id));                        // shader code kompilieren

        // error handling
        int result;
        GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
        if (result == GL_FALSE)
        {
            int length;
            GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
            char* message = (char*)_malloca(length * sizeof(char));
            // alloca() lässt einen 'dynamisch' auf dem stack speicher anlegen (geht ohne nur auf dem heap)
            GLCall(glGetShaderInfoLog(id, length, &length, message));
            std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
            std::cout << message << std::endl;
            GLCall(glDeleteShader(id));
            return 0;
        }
        return id;
    }

    ShaderProgramSource Shader::ParseShader(const std::string& filepath)
    {
        enum ShaderType {
            NONE = -1, VERTEX = 0, FRAGMENT = 1
        };
        ShaderType type = NONE;
        ShaderProgramSource output;
        std::stringstream ss[2];
        std::ifstream stream(filepath);

        std::string line;
        while (getline(stream, line))
        {
            if (line.find("shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                {
                    type = VERTEX;
                }
                else if (line.find("fragment") != std::string::npos)
                {
                    type = FRAGMENT;
                }
            }
            else
            {
                if ((int)type >= 0)
                {
                    ss[(int)type] << line << '\n';
                }
            }
        }

        output.vertex = ss[VERTEX].str();
        output.fragment = ss[FRAGMENT].str();
        return output;
    }

    void Shader::Bind() const
    {
        GLCall(glUseProgram(m_RendererID));
    }

    void Shader::UnBind() const
    {
        GLCall(glUseProgram(0));
    }

    unsigned int Shader::GetRendererID()
    {
        return m_RendererID;
    }

    void Shader::SetUniform1i(const std::string& name, int value)
    {
        Bind();
        GLCall(glUniform1i(GetUniformLocation(name), value));
    }

    void Shader::SetUniform1f(const std::string& name, float value)
    {
        Bind();
        GLCall(glUniform1f(GetUniformLocation(name), value));
    }

    void Shader::SetUniform3f(const std::string& name, const glm::vec3 vec)
    {
        Bind();
        GLCall(glUniform3f(GetUniformLocation(name), vec.x, vec.y, vec.z));
    }

    void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
    {
        Bind();
        GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
    }

    void Shader::SetUniformMat3f(const std::string& name, const glm::mat3& matrix)
    {
        Bind();
        GLCall(glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
    }

    void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
    {
        Bind();
        GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
    }

    int Shader::GetUniformLocation(const std::string& name)
    {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];
        GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
        if (location == -1)
            std::cout << "Warning: uniform " << name << " doesn't exist!" << std::endl;
        m_UniformLocationCache[name] = location;
        return location;
    }
}