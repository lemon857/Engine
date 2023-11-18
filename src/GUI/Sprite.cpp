#include "EngineCore/GUI/Sprite.h"

#include "EngineCore/Renderer/Renderer.h"
#include "EngineCore/Renderer/VertexArray.h"
#include "EngineCore/Renderer/VertexBuffer.h"
#include "EngineCore/Renderer/VertexBufferLayout.h"
#include "EngineCore/Renderer/IndexBuffer.h"
#include "EngineCore/Renderer/Material.h"

namespace GUI
{
	static int g_current_sprite_ID = 0;
	Sprite::Sprite(std::shared_ptr<RenderEngine::Material> pMaterial, std::string initSubTexture, glm::vec2 pos, glm::vec2 scale, std::string name)
		: GUI_element(name == "Sprite" ? name + std::to_string(g_current_sprite_ID++) : name, pMaterial)
		, m_vertexArray(std::make_shared<RenderEngine::VertexArray>())
		, m_vertexCoordsBuffer(new RenderEngine::VertexBuffer())
		, m_textureCoordsBuffer(new RenderEngine::VertexBuffer())
		, m_indexBuffer(new RenderEngine::IndexBuffer())
	{
		m_position_p = pos;
		m_scale_p = scale;
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

		auto aSubTexture = m_pMaterial->get_texture_ptr()->getSubTexture(initSubTexture);
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

	Sprite::~Sprite()
	{
		delete m_indexBuffer;
		delete m_textureCoordsBuffer;
		delete m_vertexCoordsBuffer;
		delete m_vertexArray.get();
	}

	void Sprite::setSubTexture(std::string subTexture)
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

	void Sprite::on_render_prj(glm::mat4& prj)
	{
		glm::mat4 scaleMat(
			m_scale[0], 0, 0, 0,
			0, m_scale[1], 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

		glm::mat4 translateMat(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			m_position[0], m_position[1], 0, 1);

		glm::mat4 model = translateMat * scaleMat;

		m_pMaterial->use();
		m_pMaterial->set_model_matrix(model);
		m_pMaterial->set_view_projection_matrix(prj);

		RenderEngine::Renderer::drawTriangles(*m_vertexArray, *m_indexBuffer);
	}

}