#pragma once
#include <KEngine.h>

class RendererLayer : public KEngine::Layer {
public:
    RendererLayer();
    void OnAttach() override;
    void OnUpdate(KEngine::TimeStep ts) override;
    void OnEvent(KEngine::Event& event) override;
    void ImGuiRender() override;
	void PickWithColor();
    inline glm::mat4 CalculateVP(glm::mat4 view, glm::mat4 proj) { return proj * view; }
private:
	std::shared_ptr<KEngine::Shader> m_Shader;
	std::shared_ptr<KEngine::Mesh> m_Mesh;

	std::shared_ptr<KEngine::Shader> l_Shader;
	std::shared_ptr<KEngine::Mesh> l_Mesh;

	std::shared_ptr<KEngine::Shader> s_Shader;

	std::shared_ptr<KEngine::Shader> screenShader;
	std::shared_ptr<KEngine::Mesh>quad_Mesh;

	std::shared_ptr<KEngine::TextureCube>textureCube;
	std::shared_ptr<KEngine::Shader>sky_Shader;
	std::shared_ptr<KEngine::Mesh>sky_Mesh;

	std::shared_ptr<KEngine::FrameBuffer>FBO;
	std::shared_ptr<KEngine::Texture2D>quad_Texture;
	std::shared_ptr<KEngine::RenderBuffer>RBO;

	std::shared_ptr<KEngine::Shader>backpack_Shader;
	std::shared_ptr<KEngine::Model>backpack_Model;

	std::shared_ptr<KEngine::UniformBuffer> matrixUBO;

	std::vector<std::shared_ptr<KEngine::Shader>> shaderList;

	glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 0.0f);
	glm::mat4 projMatrix;
	std::shared_ptr<KEngine::Camera>  mainCamera;

	std::shared_ptr<KEngine::FrameBuffer> pickFBO;
	std::shared_ptr<KEngine::Texture2D>   pickTexture;
	std::shared_ptr<KEngine::RenderBuffer>pickRBO;
	std::shared_ptr<KEngine::Shader>      pickShader;

	std::vector<std::shared_ptr<KEngine::Object>> Objects;
	
};