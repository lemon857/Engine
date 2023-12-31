#pragma once

#include "EngineCore/GUI/GUI_element.h"

#include "EngineCore/System/List.h"

#include <string>
#include <glm/vec3.hpp>
namespace GUI
{
	class GUI_place;
	class Sprite;
	class ScrollBox : public GUI_element
	{
	public:
		ScrollBox(Sprite* backgrond, glm::vec2 pos, glm::vec2 scale,
			std::string name, int max_count_elements, bool has_displacement = false, GUI_place* place = nullptr);
		~ScrollBox();

		void on_render_prj(glm::mat4& prj) override;

		std::vector<GUI_element*> get_elements() override;

		void on_scroll(int offset);

		void add_element(GUI_element* element);

		void clear();

		void set_open(bool isOpen);
	private:
		bool m_has_shift;
		unsigned int m_max_count_elements;
		GUI_place* m_place;
		linked_list<GUI_element*> m_elements;
		Sprite* m_background;
	};
}