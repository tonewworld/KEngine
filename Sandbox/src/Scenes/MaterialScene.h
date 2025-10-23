#include "KEngine.h"
class MaterialScene :public KEngine::Scene {
public:
	MaterialScene(std::string name);
	~MaterialScene();
	inline virtual std::string GetName() override { return name; }
	inline std::vector<std::shared_ptr<KEngine::Object>> GetObjectsInScene() override { return Objects; }
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

	std::shared_ptr<KEngine::Shader> l_Shader;
	std::shared_ptr<KEngine::Light> pointLight;

	std::shared_ptr<KEngine::UniformBuffer> matrixUBO;
	std::shared_ptr<KEngine::UniformBuffer> materialUBO;
	std::shared_ptr<KEngine::UniformBuffer> pointLightUBO;

	glm::vec3 pl_Position = glm::vec3(1.2f, 1.0f, 0.0f);
	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	std::vector<std::shared_ptr<KEngine::Shader>> vpSL;
};