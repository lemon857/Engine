#pragma once

#include "Games/Tower_Defense/BaseEnemy.h"

#define P_PROFESSOR_VELOCITY 1.2
#define P_PROFESSOR_COOLDOWN 1.6
#define P_PROFESSOR_HP 28
#define P_PROFESSOR_DAMAGE 1.5
#define P_PROFESSOR_DESCRIPTION "Very angry professor"

class ProfessorEnemy : public BaseEnemy
{
public:
	ProfessorEnemy(ObjModel* model, Castle* target, std::vector<Target> targets, glm::vec3 pos, std::shared_ptr<RenderEngine::Material> pMaterial);
	~ProfessorEnemy() = default;

	std::string get_description() override { return P_PROFESSOR_DESCRIPTION; }
private:

};