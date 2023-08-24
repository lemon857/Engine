#pragma once

#include "EngineCore/IComponent.h"

#include <memory>
#include <string>

namespace RenderEngine
{
	class VertexArray;
	class IndexBuffer;
	class ShaderProgram;
	class Texture2D;
}

struct GraphicsObject;

class MeshRenderer : public IComponent
{
public:
	MeshRenderer(
		std::shared_ptr<GraphicsObject> obj,
		std::shared_ptr<RenderEngine::ShaderProgram> pShaderProgram,
		std::shared_ptr<RenderEngine::Texture2D> pTexture);
	~MeshRenderer();

	void update(const double delta) override;

	void update_object(std::shared_ptr<GraphicsObject> obj);
private:
	std::shared_ptr<RenderEngine::ShaderProgram> m_pShaderProgram;
	std::shared_ptr<RenderEngine::Texture2D> m_pTexture;
	std::shared_ptr<GraphicsObject> m_obj;
};