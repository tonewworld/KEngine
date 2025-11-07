#pragma once
#include <KEngine.h>
#include "RendererConfig.h"
#include "Scenes/Skybox.h"
#include "Scenes/ParaShadow.h"
#include "Scenes/OmniShadow.h"
#include "Scenes/NormalMapping.h"
#include "Scenes/ParallaxMapping.h"
class RendererLayer : public KEngine::Layer {
public:
    RendererLayer();
    void OnAttach() override;
	void OnDetach() override;
    void OnUpdate(KEngine::TimeStep ts) override;
    void OnEvent(KEngine::Event& event) override;
    void ImGuiRender() override;
	void SetSceneAttri();
	void PickWithColor();
	void CalculateShadow();
	void HDRandBloom();

    inline glm::mat4 CalculateVP(glm::mat4 view, glm::mat4 proj) { return proj * view; }
private:

	RendererConfig m_Config;

	
	
	bool m_ShowGlobalSettings = true;

	std::shared_ptr<KEngine::Shader> screenShader;
	std::shared_ptr<KEngine::Mesh>quad_Mesh;
	std::shared_ptr<KEngine::FrameBuffer>FBO;
	std::shared_ptr<KEngine::Texture2D>quad_Texture;
	std::shared_ptr<KEngine::RenderBuffer>RBO;


	std::shared_ptr<KEngine::FrameBuffer> pickFBO;
	std::shared_ptr<KEngine::Texture2D>   pickTexture;
	std::shared_ptr<KEngine::RenderBuffer>pickRBO;
	std::shared_ptr<KEngine::Shader>      pickShader;
	
	std::shared_ptr<KEngine::FrameBuffer> depthFBO;
	std::shared_ptr<KEngine::Texture2D>   depthTexture;
	std::shared_ptr<KEngine::Shader>	  shadowShader;

	std::shared_ptr<KEngine::FrameBuffer> depthCubeFBO;
	std::shared_ptr<KEngine::TextureCube> depthCubeTexture;
	std::shared_ptr<KEngine::Shader>      shadowCubeShader;
	
	std::shared_ptr<KEngine::FrameBuffer> hdrFBO;
	std::shared_ptr<KEngine::Texture2D>   hdrTexture;
	std::shared_ptr<KEngine::Texture2D>   bloomTexture;
	std::shared_ptr<KEngine::Shader>      hdrShader;
	std::shared_ptr<KEngine::RenderBuffer>hdrRBO;

	std::shared_ptr<KEngine::FrameBuffer> pingpongFBO[2];
	std::shared_ptr<KEngine::Texture2D>   pingpongTexture[2];
	std::shared_ptr<KEngine::Shader>      blurShader;

	bool m_ShowSceneHierarchy = true;
	bool m_ShowInspector = true;
	int m_SelectedObjectID = -1;
	std::shared_ptr<KEngine::Object> m_SelectedObject = nullptr;

	std::shared_ptr<Skybox> skyboxScene;
	std::shared_ptr<ParaShadow>paraShadowScene;
	std::shared_ptr<OmniShadow>omniShadowScene;
	std::shared_ptr<NormalMapping>normalMappingScene;
	std::shared_ptr<ParallaxMapping>parallaxMappingScene;
	std::shared_ptr<KEngine::Scene> currentScene;
	std::vector<std::shared_ptr<KEngine::Scene>> sceneList;

	// 场景管理器相关方法
	void DrawSceneHierarchy();
	void DrawInspector();
	void DrawObjectProperties(std::shared_ptr<KEngine::Object> object);
	void DrawSceneList();          // 绘制场景列表窗口
	void SwitchToScene(int index); // 切换场景接口
	void DrawGlobalSettings();
	
};