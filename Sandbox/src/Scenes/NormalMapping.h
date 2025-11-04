#include "KEngine.h"
class NormalMapping :public KEngine::Scene {
public:
	NormalMapping(std::string name);
	~NormalMapping();
	inline virtual std::string GetName() override { return name; }
	inline std::vector<std::shared_ptr<KEngine::Object>> GetObjectsInScene() override { return Objects; }
	inline std::vector<std::shared_ptr<KEngine::PointLight>>GetPointLightInScene()override { return pointLightList; }
	inline std::vector<std::shared_ptr<KEngine::ParallelLight>>GetParallelLightInScene()override { return parallelLightList; }
	virtual void Init()override;
	virtual void OnUpdate(KEngine::TimeStep ts)override;
	virtual void Destroy()override;
private:
	std::string name;
	std::vector<std::shared_ptr<KEngine::Object>> Objects;

	glm::mat4 projMatrix;
	std::shared_ptr<KEngine::Camera>  mainCamera;

	std::shared_ptr<KEngine::Shader> m_Shader;
	std::shared_ptr<KEngine::Mesh> m_Mesh;
	std::shared_ptr<KEngine::Texture2D> m_DiffuseMap;
	std::shared_ptr<KEngine::Texture2D> m_NormalMap;

	std::shared_ptr<KEngine::Shader> l_Shader;

	std::vector<std::shared_ptr<KEngine::PointLight>> pointLightList;

	std::shared_ptr<KEngine::ParallelLight>parallelLight0;
	std::vector<std::shared_ptr<KEngine::ParallelLight>> parallelLightList;

	std::shared_ptr<KEngine::UniformBuffer> matrixUBO;
	std::shared_ptr<KEngine::UniformBuffer> materialUBO;
	std::shared_ptr<KEngine::UniformBuffer> parallelLightUBO;

	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	std::vector<std::shared_ptr<KEngine::Shader>> vpSL;
};