#include "KEngine.h"
class TestScene :public KEngine::Scene {
public:
	TestScene();
	~TestScene();

	void OnUpdate(KEngine::TimeStep ts);
	std::vector<std::shared_ptr<KEngine::Object>> Objects;
private:

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

	glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 0.0f);
	glm::mat4 projMatrix;
	std::shared_ptr<KEngine::Camera>  mainCamera;


};