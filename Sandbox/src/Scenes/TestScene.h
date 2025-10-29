#include "KEngine.h"
class TestScene :public KEngine::Scene {
public:
	TestScene(std::string name);
	~TestScene();
	inline virtual std::string GetName() override { return name; }
	inline std::vector<std::shared_ptr<KEngine::Object>> GetObjectsInScene() override { return Objects; }
	inline std::vector<std::shared_ptr<KEngine::ParallelLight>>GetParallelLightInScene()override { return parallelLightList; }
	virtual void OnUpdate(KEngine::TimeStep ts)override;
	virtual void Init()override;
	virtual void Destroy()override;
private:
	std::string name;
	std::vector<std::shared_ptr<KEngine::Object>> Objects;

	std::shared_ptr<KEngine::Shader> m_Shader;
	std::shared_ptr<KEngine::Mesh> m_Mesh;

	std::shared_ptr<KEngine::Shader> l_Shader;
	std::shared_ptr<KEngine::Mesh> l_Mesh;

	std::shared_ptr<KEngine::TextureCube>textureCube;
	std::shared_ptr<KEngine::Shader>sky_Shader;
	std::shared_ptr<KEngine::Mesh>sky_Mesh;

	std::shared_ptr<KEngine::Shader>backpack_Shader;
	std::shared_ptr<KEngine::Model>backpack_Model;

	std::shared_ptr<KEngine::UniformBuffer> matrixUBO;

	std::vector<std::shared_ptr<KEngine::Shader>> shaderList;
	std::vector<std::shared_ptr<KEngine::ParallelLight>> parallelLightList;

	glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 0.0f);
	glm::mat4 projMatrix;
	std::shared_ptr<KEngine::Camera>  mainCamera;


};