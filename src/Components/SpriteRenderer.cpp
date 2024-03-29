#include "EngineCore/Components/SpriteRenderer.h"

#include "EngineCore/Renderer/Renderer.h"
#include "EngineCore/Renderer/VertexArray.h"
#include "EngineCore/Renderer/VertexBuffer.h"
#include "EngineCore/Renderer/VertexBufferLayout.h"
#include "EngineCore/Renderer/IndexBuffer.h"
#include "EngineCore/Renderer/Material.h"
#include "EngineCore/Components/Transform.h"
#include "EngineCore/IGameObject.h"

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

SpriteRenderer::SpriteRenderer(RenderEngine::Material* pMaterial,
	std::string initialSubTexture)
	: IComponent()
	, m_vertexArray(new RenderEngine::VertexArray())
	, m_pMaterial(std::move(pMaterial))
	, m_vertexCoordsBuffer(new RenderEngine::VertexBuffer())
	, m_textureCoordsBuffer(new RenderEngine::VertexBuffer())
	, m_indexBuffer(new RenderEngine::IndexBuffer())
{
	const GLfloat vertexCoords[] = {
		//2--3  1
		//| / /	|
		//1	 3--2	
		//1--2
		//| /|
		//0--3
		//X --- Y
		-1.f,-1.f, 0.f,
		-1.f, 1.f, 0.f,
		 1.f, 1.f, 0.f,
		 1.f, -1.f, 0.f
	};

	auto aSubTexture = m_pMaterial->get_texture_ptr()->getSubTexture(initialSubTexture);
	const GLfloat textureCoords[] = {
		//U --- V
		aSubTexture.leftBottomUV.x, aSubTexture.leftBottomUV.y,
		aSubTexture.leftBottomUV.x, aSubTexture.rightTopUV.y,
		aSubTexture.rightTopUV.x, aSubTexture.rightTopUV.y,
		aSubTexture.rightTopUV.x, aSubTexture.leftBottomUV.y,
	};
	const GLuint indexes[] = { 0, 1, 2, 2, 3, 0 };

	m_vertexCoordsBuffer->init(&vertexCoords, 3 * 4 * sizeof(GLfloat), false);
	RenderEngine::VertexBufferLayout vertexCoordsLayout;
	vertexCoordsLayout.addElementLayoutFloat(3, false);
	m_vertexArray->addBuffer(*m_vertexCoordsBuffer, vertexCoordsLayout);

	m_textureCoordsBuffer->init(&textureCoords, 2 * 4 * sizeof(GLfloat), true);
	RenderEngine::VertexBufferLayout textureCoordsLayout;
	textureCoordsLayout.addElementLayoutFloat(2, false);
	m_vertexArray->addBuffer(*m_textureCoordsBuffer, textureCoordsLayout);

	m_indexBuffer->init(&indexes, 6);

	m_vertexArray->unbind();
	m_indexBuffer->unbind();
}

SpriteRenderer::~SpriteRenderer()
{
	delete m_indexBuffer;
	delete m_textureCoordsBuffer;
	delete m_vertexCoordsBuffer;
	delete m_vertexArray;
}

void SpriteRenderer::setSubTexture(std::string subTexture)
{
	auto aSubTexture = m_pMaterial->get_texture_ptr()->getSubTexture(subTexture);
	const GLfloat textureCoords[] = {
		//U --- V
		aSubTexture.leftBottomUV.x, aSubTexture.leftBottomUV.y,
		aSubTexture.leftBottomUV.x, aSubTexture.rightTopUV.y,
		aSubTexture.rightTopUV.x, aSubTexture.rightTopUV.y,
		aSubTexture.rightTopUV.x, aSubTexture.leftBottomUV.y,
	}; 
	m_textureCoordsBuffer->update(&textureCoords, 2 * 4 * sizeof(GLfloat));
}

void SpriteRenderer::render(RenderEngine::ShaderProgram* shader)
{
	Transform* transform = m_targetObj->getComponent<Transform>();
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;
	if (transform == nullptr)
	{
		scale = glm::vec3(1.f);;
		pos = glm::vec3(0.f);;
		rot = glm::vec3(0.f);
	}
	else
	{
		scale = transform->get_scale();
		pos = transform->get_position();
		rot = transform->get_rotation();
	}

	glm::mat4 scaleMat(
		scale[0], 0, 0, 0,
		0, scale[1], 0, 0,
		0, 0, scale[2], 0,
		0, 0, 0, 1);

	glm::mat4 translateMat(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		pos[0], pos[1], pos[2], 1);

	float xRotRadians = glm::radians(rot[0]);
	float yRotRadians = glm::radians(rot[1]);
	float zRotRadians = glm::radians(rot[2]);

	glm::mat4 rotateXmat(
		1, 0, 0, 0,
		0, cos(xRotRadians), -sin(xRotRadians), 0,
		0, sin(xRotRadians), cos(xRotRadians), 0,
		0, 0, 0, 1);

	glm::mat4 rotateYmat(
		cos(yRotRadians), 0, -sin(yRotRadians), 0,
		0, 1, 0, 0,
		sin(yRotRadians), 0, cos(yRotRadians), 0,
		0, 0, 0, 1);

	glm::mat4 rotateZmat(
		cos(zRotRadians), -sin(zRotRadians), 0, 0,
		sin(zRotRadians), cos(zRotRadians), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	glm::mat4 model = translateMat * rotateXmat * rotateYmat * rotateZmat * scaleMat;

	m_pMaterial->use();
	m_pMaterial->set_model_matrix(model);

	RenderEngine::Renderer::drawTriangles(*m_vertexArray, *m_indexBuffer);
}

RenderEngine::Material* SpriteRenderer::get_material_ptr()
{
	return m_pMaterial;
}
