#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <glm/mat4x4.hpp>

#include "EngineCore/Renderer/ShaderProgramLayout.h"

namespace RenderEngine
{
	class ShaderProgram;
	class Texture2D;

	class Material
	{
	public:
		Material(std::shared_ptr<ShaderProgram> pShaderProgram, std::shared_ptr<Texture2D> pTexture);

		~Material();

		void use();

		std::vector<std::string> get_shader_prop_names(ETypeData type);

		void set_model_matrix(glm::mat4& model);
		void set_view_projection_matrix(glm::mat4& vp);

		std::shared_ptr<Texture2D> get_texture_ptr();
		std::shared_ptr<ShaderProgram> get_shader_ptr();

		void set_shader(std::shared_ptr<ShaderProgram> pShaderProgram);

		void set_texture(std::shared_ptr<Texture2D> pTexture);

		template<typename _Ty>
		_Ty* get_data(std::string name)
		{
			std::map<std::string, void*>::const_iterator it = m_shader_data_map.find(name);
			if (it != m_shader_data_map.end())
			{
				return (_Ty*)it->second;
			}
			return nullptr;
		}

	private:
		std::shared_ptr<ShaderProgram> m_pShaderProgram;
		std::shared_ptr<Texture2D> m_pTexture;

		std::map<std::string, void*> m_shader_data_map;
		std::vector<RenderEngine::ShaderProgramLayoutElement> m_shader_layout_elemrnts;
	};
}