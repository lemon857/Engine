#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <glm/vec2.hpp>

struct INIdata
{
	glm::ivec2& window_size;
	glm::ivec2& window_position;
	bool& maximized_window;
};

namespace RenderEngine
{
	class ShaderProgram;
	class Texture2D;
	class Sprite;
	class Animator;
	class GraphicsObject;
}

	class ResourceManager {
	public:
		static void setExecutablePath(const std::string& executablePath);
		static void unloadAllResources();

		~ResourceManager() = delete;
		ResourceManager() = delete;

		ResourceManager(const ResourceManager&) = delete;
		ResourceManager(const ResourceManager&&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&&) = delete;

		static std::string getFileString(const std::string& relativeFilePath);

		static std::string getExeFilePath() { return m_path; };

		static bool loadJSONresources(const std::string& JSONpath);

		static bool loadINIsettings(const std::string& INIpath, INIdata& data, const bool isWrite);

		static std::shared_ptr<RenderEngine::ShaderProgram> loadShaders(
			const std::string& shaderName, 
			const std::string& vertexPath, 
			const std::string& fragmentPath);
		static std::shared_ptr<RenderEngine::ShaderProgram> getShaderProgram(
			const std::string& shaderName);

		static std::shared_ptr<RenderEngine::Texture2D>  loadTexture(
			const std::string& textureName, 
			const std::string& texturePath);
		static std::shared_ptr<RenderEngine::Texture2D> getTexture(const std::string& textureName);

		static std::shared_ptr<RenderEngine::Sprite>  loadSprite(
			const std::string& spriteName, 
			const std::string& textureName,
			const std::string& shaderName,
			const std::string& subTextureName = "default");
		static std::shared_ptr<RenderEngine::Sprite> getSprite(const std::string& spriteName);

		static std::shared_ptr<RenderEngine::GraphicsObject>  loadGraphicsObject(
			const std::string& objName,
			const std::string& shaderName,
			const std::string& source);
		static std::shared_ptr<RenderEngine::GraphicsObject> getGraphicsObject(const std::string& objName);

		/*static std::shared_ptr<RenderEngine::Animator>  loadAnimator(
			const std::string& animatorName,
			const std::string& spriteName);*/
		//static std::shared_ptr<RenderEngine::Animator> getAnimator(const std::string& animatorName);

		static std::shared_ptr<RenderEngine::Texture2D> loadTextureAtlas(
			std::string textureName,
			std::vector<std::string> subTextures,
			std::string texturePath,
			const unsigned int width,
			const unsigned int height,
			const unsigned int subTextureWidth,
			const unsigned int subTextureHeight);
	private:

		typedef std::map<const std::string, std::shared_ptr<RenderEngine::ShaderProgram>> ShaderProgramsMap;
		static ShaderProgramsMap m_ShaderPrograms;

		typedef std::map<const std::string, std::shared_ptr<RenderEngine::Texture2D>> TexturesMap;
		static TexturesMap m_textures;

		typedef std::map<const std::string, std::shared_ptr<RenderEngine::Sprite>> SpritesMap;
		static SpritesMap m_sprites;

		//typedef std::map<const std::string, std::shared_ptr<RenderEngine::Animator>> AnimatorsMap;
		//static AnimatorsMap m_animators;

		typedef std::map<const std::string, std::shared_ptr<RenderEngine::GraphicsObject>> GraphObjMap;
		static GraphObjMap m_graph_objs;

		static std::string m_path;
	};
