#pragma once

#include <glm/vec2.hpp>
#include <string>
#include <memory>

namespace RenderEngine
{
	class Sprite;
}

class IGameObject 
{
public:
	IGameObject(
		std::shared_ptr<RenderEngine::Sprite> sprite,
		const std::string name,
		const glm::vec2& position, 
		const glm::vec2& size, 
		const glm::vec2& moveOffset = glm::vec2(0.f),
		const double velocity = 1, 
		const double weight = 1)
		: m_pSprite(std::move(sprite)),
		m_name(name),
		m_velocity(velocity),
		m_weight(weight),
		m_position(position),
		m_size(size),
		m_moveOffset(moveOffset)
	{};
	~IGameObject()
	{

	};
	virtual void update(double delta) {};
	bool isMove() const { return m_move; };

	void setKinematicState(bool isKinematic) { m_isKinematic = isKinematic; };
	void getGroundState(bool isGrounded) { m_isGrounded = isGrounded; };

	double getVelocity() { return m_velocity; };
	double getWeight() { return m_weight; };
	bool getKinematicState() { return m_isKinematic; };
	bool getGroundState() { return m_isGrounded; };
	glm::vec2& getSize() { return m_size; };
	glm::vec2& getPosition() { return m_position; };
	glm::vec2& getOffset() { return m_moveOffset; };
	std::string& getName() { return m_name; };
	std::shared_ptr<RenderEngine::Sprite>& getpSprite() { return m_pSprite; };
protected:
	std::string m_name;

	glm::vec2 m_position;
	glm::vec2 m_size;
	glm::vec2 m_moveOffset;

	std::shared_ptr<RenderEngine::Sprite> m_pSprite;

	double m_weight;
	double m_velocity;
	bool m_move = false;
	bool m_isKinematic = false;
	bool m_isGrounded = false;
};