#include "ShaderProgram.h"
#include <iostream>

namespace Renderer 
{
	ShaderProgram::ShaderProgram(const std::string& vertexShader, const std::string& fragmentShader)
	{
		GLuint vertexShaderID;
		if (!createShader(vertexShader, GL_VERTEX_SHADER, vertexShaderID)) 
		{
			std::cerr << "VERTEX SHADER ERROR COMPILE\n";
			return;
		}
		GLuint fragmentShaderID;
		if (!createShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderID))
		{
			std::cerr << "FRAGMENT SHADER ERROR COMPILE\n";
			glDeleteShader(vertexShaderID);
			return;
		}
		m_ID = glCreateProgram();
		glAttachShader(m_ID, vertexShaderID);
		glAttachShader(m_ID, fragmentShaderID);
		glLinkProgram(m_ID);

		GLint success;
		glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			GLchar infoLog[1024];
			glGetProgramInfoLog(m_ID, 1024, nullptr, infoLog);
			std::cerr << "ERROR: PROGRAM: Link:\n" << infoLog << "\n";
		}
		else 
		{
			m_isCompiled = true;
		}
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
	}
	ShaderProgram::~ShaderProgram() 
	{
		glDeleteProgram(m_ID);
	}
	bool ShaderProgram::createShader(const std::string& sousce, const GLenum shaderType, GLuint& shaderID) 
	{
		shaderID = glCreateShader(shaderType);
		const char* code = sousce.c_str();
		glShaderSource(shaderID, 1, &code, nullptr);
		glCompileShader(shaderID);

		GLint success;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLchar infoLog[1024];
			glGetShaderInfoLog(shaderID, 1024, nullptr, infoLog);
			std::cerr << "ERROR: SHADER: Compile:\n" << infoLog << "\n";
			return false;
		}
		return true;
	}
	void ShaderProgram::use() const {
		glUseProgram(m_ID);
	}
	bool ShaderProgram::isCompiled() const {
		return m_isCompiled;
	}
	ShaderProgram& ShaderProgram::operator=(ShaderProgram && shaderProgram) noexcept
	{
		glDeleteProgram(m_ID);
		m_ID = shaderProgram.m_ID;
		m_isCompiled = shaderProgram.m_isCompiled;

		shaderProgram.m_ID = 0;
		shaderProgram.m_isCompiled = false;
		return *this;
	}
	ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram) noexcept
	{
		m_ID = shaderProgram.m_ID;
		m_isCompiled = shaderProgram.m_isCompiled;

		shaderProgram.m_ID = 0;
		shaderProgram.m_isCompiled = false;
	}
}
