#include "Games/Tower_Defense/Castle.h"

#include "Games/Tower_Defense/HealthBar.h"

#include "EngineCore/Components/MeshRenderer.h"
#include "EngineCore/Components/Transform.h"

#include "EngineCore/Resources/ResourceManager.h"

Castle::Castle(glm::vec3 initPos, const unsigned int hp, std::string objPath, std::shared_ptr<RenderEngine::Material> pMaterial, std::shared_ptr<RenderEngine::Material> pMaterialBar)
	: m_hp(hp)
	, m_isDestroyed(false)
	, m_bar(new HealthBar(pMaterialBar, initPos + glm::vec3(0.f, 3.f, 0.f), 25, 2, hp, glm::vec3(1.f), glm::vec3(1.f, 0.f, 0.f)))
	, IGameObject("MainCastle")
{
	addComponent<Transform>(initPos);
	addComponent<MeshRenderer>(ResourceManager::load_OBJ_file(objPath), pMaterial);
}

void Castle::update(const double delta)
{
	if (m_isDestroyed) return;
	m_bar->update();
	updateComponents(delta);
}

void Castle::damage(const unsigned int damage_hp)
{
	m_hp -= damage_hp;
	m_bar->set_value(m_hp);
	if (m_hp <= 0)
	{
		m_isDestroyed = true;
	}
}

glm::vec3 Castle::get_pos()
{
	return getComponent<Transform>()->get_position();
}

bool Castle::isDestroyed()
{
	return m_isDestroyed;
}