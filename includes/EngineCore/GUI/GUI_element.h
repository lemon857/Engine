#pragma once

#include <string>
#include <memory>

#include <glm/vec2.hpp>

namespace RenderEngine
{
	class Material;

}

namespace GUI
{
	class GUI_element
	{
	public:
		GUI_element(std::string name, std::shared_ptr<RenderEngine::Material> pMaterial)
			: m_name(name)
			, m_pMaterial(std::move(pMaterial))
			, m_position(glm::ivec2(0))
			, m_scale(glm::ivec2(0))
		{
		}

		~GUI_element() = default;

		virtual void on_update(const double delta) {};
		virtual void on_render() {};

	protected:
		glm::ivec2 m_position;
		glm::ivec2 m_scale;

		std::string m_name;
		std::shared_ptr<RenderEngine::Material> m_pMaterial;
	};
}