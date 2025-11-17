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
	void PickWithColor();
	void CalculateShadow();
	//����ͨ��
	void GeometryPass();
	//�������ڱ�ͨ��
	void SSAOPass();
	inline float RandFloat() { return static_cast<float>(rand()) / RAND_MAX; }
	//ģ��ͨ��
	void SSAOBlurPass();
	//����ͨ��
	void LightingPass();
	//HDR��Bloomͨ��
	void HDRandBloom();
	//��Ļ��Ⱦͨ��
	void ScreenPass();

	//ǰ����Ⱦ
	void ForwardRenderPass();

    inline glm::mat4 CalculateVP(glm::mat4 view, glm::mat4 proj) { return proj * view; }
private:

	RendererConfig m_Config;

	
	
	bool m_ShowGlobalSettings = true;

	// debug flag removed

	

	std::shared_ptr<KEngine::FrameBuffer> depthFBO;
	std::shared_ptr<KEngine::Texture2D>   depthTexture;
	std::shared_ptr<KEngine::Shader>	  shadowShader;

	std::shared_ptr<KEngine::FrameBuffer> depthCubeFBO;
	std::shared_ptr<KEngine::TextureCube> depthCubeTexture;
	std::shared_ptr<KEngine::Shader>      shadowCubeShader;
	
	//ǰ����Ⱦ
	std::shared_ptr<KEngine::Shader>      forwardShader;	

	//�ӳ���ɫ
	//�ع�:���ι���
	std::shared_ptr<KEngine::Shader>      geometryPassShader;
	std::shared_ptr<KEngine::FrameBuffer> gBuffer;
	std::shared_ptr<KEngine::Texture2D>   gPosition;
	std::shared_ptr<KEngine::Texture2D>   gNormal;
	std::shared_ptr<KEngine::Texture2D>   gAlbedoSpec;
	std::shared_ptr<KEngine::Texture2D>   gRoughness;
	std::shared_ptr<KEngine::RenderBuffer>gRBO;
	std::shared_ptr<KEngine::UniformBuffer>materialUBO;
	std::shared_ptr<KEngine::UniformBuffer>matrixUBO;
	
	//SSAO����
	std::shared_ptr<KEngine::Mesh>		  ssaoPassMesh;
	std::shared_ptr<KEngine::Shader>      ssaoShader;
	std::shared_ptr<KEngine::FrameBuffer> ssaoFBO;
	std::shared_ptr<KEngine::Texture2D>   ssaoTexture;
	std::vector<glm::vec3> ssaoKernel;
	std::shared_ptr<KEngine::Texture2D>   ssaoNoiseTexture;
	//SSAOģ������
	std::shared_ptr<KEngine::Mesh>        ssaoBlurPassMesh;
	std::shared_ptr<KEngine::Shader>      ssaoBlurShader;
	std::shared_ptr<KEngine::FrameBuffer> ssaoBlurFBO;
	std::shared_ptr<KEngine::Texture2D>   ssaoBlurTexture;

	//�ع������չ���
	std::shared_ptr<KEngine::Mesh>        lightingPassMesh;
	std::shared_ptr<KEngine::Shader>      lightingPassShader;
	std::shared_ptr<KEngine::FrameBuffer> lightingFBO;
	std::shared_ptr<KEngine::Texture2D>   lightingTexture;
	std::shared_ptr<KEngine::Texture2D>   bloomTexture;
	std::shared_ptr<KEngine::RenderBuffer>lightingRBO;

	std::shared_ptr<KEngine::UniformBuffer>pointLightUBO;
	std::shared_ptr<KEngine::UniformBuffer>parallelLightUBO;

	//�ع������ڴ�������
	std::shared_ptr<KEngine::Mesh>        postProcessMesh;
	int m_FinalBloomIndex = 0;
	std::shared_ptr<KEngine::FrameBuffer> pingpongFBO[2];
	std::shared_ptr<KEngine::Texture2D>   pingpongTexture[2];
	std::shared_ptr<KEngine::Shader>      hdrAndBlurShader;

	//�ع�����Ļ��Ⱦ����
	std::shared_ptr<KEngine::Mesh>quad_Mesh;
	std::shared_ptr<KEngine::Shader> screenShader;

	//��ɫʰȡ
	std::shared_ptr<KEngine::FrameBuffer> pickFBO;
	std::shared_ptr<KEngine::Texture2D>   pickTexture;
	std::shared_ptr<KEngine::RenderBuffer>pickRBO;
	std::shared_ptr<KEngine::Shader>      pickShader;

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

	// ������������ط���
	void DrawSceneHierarchy();
	void DrawInspector();
	void DrawObjectProperties(std::shared_ptr<KEngine::Object> object);
	void DrawSceneList();          // ���Ƴ����б�����
	void SwitchToScene(int index); // �л������ӿ�
	void DrawGlobalSettings();
	
};