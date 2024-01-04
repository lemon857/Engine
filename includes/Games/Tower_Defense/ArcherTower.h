#pragma once

#include "Games/Tower_Defense/BaseTower.h"

class ArcherTower : public BaseTower
{
public:
	ArcherTower(std::shared_ptr<GraphicsObject> obj, std::shared_ptr<RenderEngine::Material> pMaterial,
		linked_list<BaseEnemy*>* list, glm::vec3 pos, RenderEngine::Line* line);
	~ArcherTower() = default;

	void upgrade() override;

	static const unsigned int p_damage = 3;
	static const unsigned int p_distance = 10;
	static const unsigned int p_cooldown = 1;

private:

};