#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <memory>
#include <vector>

#include "ShaderProgram.h"
#include "EngineCore/IGameObject.h"
#include "EngineCore/Physics/PhysicsEngine.h"
#include "EngineCore/Renderer/Sprite.h"

enum ETypeCamera
{
	Orthographic,
	Perspective
};

class Camera : public IGameObject
{
public:
	// ��������������� �����������
	Camera(glm::vec2 position, glm::vec2 size, float zNear, float zFar);
	// ������������� �����������
	Camera(glm::vec2 position, float fov, float zNear, float zFar);

	void addShaderProgram(std::shared_ptr<RenderEngine::ShaderProgram>& program);

	void update(const double delta) override;

	void updateSize(glm::vec2 size);
private:
	std::vector<std::shared_ptr<RenderEngine::ShaderProgram>> m_shaderPrograms;

	ETypeCamera m_type;
	glm::mat4 m_projectionMatrix;
	Physics::EDirection m_eOrentation;
};