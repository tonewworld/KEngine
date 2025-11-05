#include "KEngine.h"
class Skybox :public KEngine::Scene {
public:
	Skybox(std::string name);
	~Skybox();
	inline virtual std::string GetName() override { return name; }
	inline std::vector<std::shared_ptr<KEngine::Object>> GetObjectsInScene() override { return Objects; }
	inline std::vector<std::shared_ptr<KEngine::PointLight>>GetPointLightInScene()override { return pointLightList; }
	inline std::vector<std::shared_ptr<KEngine::ParallelLight>>GetParallelLightInScene()override { return parallelLightList; }
	virtual void OnUpdate(KEngine::TimeStep ts)override;
	virtual void Init()override;
	virtual void Destroy()override;
private:
	std::string name;
	std::vector<std::shared_ptr<KEngine::Object>> Objects;

	std::shared_ptr<KEngine::Shader> m_Shader;
	std::shared_ptr<KEngine::Mesh> m_Mesh;

	std::shared_ptr<KEngine::TextureCube>textureCube;
	std::shared_ptr<KEngine::Shader>sky_Shader;
	std::shared_ptr<KEngine::Mesh>sky_Mesh;

	std::shared_ptr<KEngine::Shader>backpack_Shader;
	std::shared_ptr<KEngine::Model>backpack_Model;

	std::shared_ptr<KEngine::UniformBuffer> matrixUBO;

	std::vector<std::shared_ptr<KEngine::Shader>> shaderList;
	std::vector<std::shared_ptr<KEngine::PointLight>> pointLightList;
	std::vector<std::shared_ptr<KEngine::ParallelLight>> parallelLightList;

	glm::mat4 projMatrix;
	std::shared_ptr<KEngine::Camera>  mainCamera;


};