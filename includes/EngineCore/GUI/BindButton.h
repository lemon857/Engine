#pragma once

#include "EngineCore/GUI/GUI_element.h"
#include <string>
#include <memory>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

enum class KeyCode;

namespace RenderEngine
{
	class Material;
	class ShaderProgram;
}

namespace GUI
{
	class TextRenderer;
	class Font;
	class Sprite;

	class BindButton : public GUI_element
	{
	public:
		BindButton(Sprite* face, glm::vec2 pos, glm::vec2 scale,
			 std::shared_ptr<RenderEngine::ShaderProgram> textShader, std::shared_ptr<Font> font, glm::vec3 textColor, KeyCode* targetBind, std::string name = "default");
		~BindButton();

		void on_render_prj(glm::mat4& prj) override;

		void on_press() override;
		void on_release() override;

		std::vector<GUI_element*> get_elements() override;

		void press_button(KeyCode key);
	private:
		Sprite* m_face;
		KeyCode* m_targetBind;
		TextRenderer* m_textRenderer;
		KeyCode m_last;
		bool m_isClicked = false;
	};
}