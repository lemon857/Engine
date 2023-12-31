#pragma once

#include <memory>

#include "PongObject.h"

#include "EngineCore/Renderer/ShaderProgram.h"
#include "EngineCore/Renderer/Point.h"

class Ball : public PongObject
{
public:
	Ball(std::shared_ptr<RenderEngine::ShaderProgram>& program,
		const float sizePoint,
		const glm::vec2& position,
		const double velocity);

	void render() override;


private:
	std::shared_ptr<RenderEngine::Point> m_point;
};