#include "KEngine.h"
class ParaShadow :public KEngine::Scene {
public:
	ParaShadow(std::string name);
	~ParaShadow();
	inline virtual std::string GetName() override { return name; }
	inline virtual std::shared_ptr<KEngine::Camera>GetMainCamera()override { return mainCamera; }
	inline std::vector<std::shared_ptr<KEngine::Object>> GetObjectsInScene() override { return Objects; }
	inline std::vector<std::shared_ptr<KEngine::PointLight>>GetPointLightInScene()override { return pointLightList; }
	inline std::vector<std::shared_ptr<KEngine::ParallelLight>>GetParallelLightInScene()override { return parallelLightList; }
	virtual void Init()override;
	virtual void OnUpdate(KEngine::TimeStep ts)override;
	virtual void Destroy()override;
private:
	std::string name;
	std::vector<std::shared_ptr<KEngine::Object>> Objects;

	std::shared_ptr<KEngine::Camera>  mainCamera;

	std::shared_ptr<KEngine::Mesh> m_Mesh;
	std::shared_ptr<KEngine::Mesh> m_Mesh1;

	std::vector<std::shared_ptr<KEngine::PointLight>> pointLightList;

	std::shared_ptr<KEngine::Shader> l_Shader;
	std::shared_ptr<KEngine::ParallelLight>parallelLight0;
	std::vector<std::shared_ptr<KEngine::ParallelLight>> parallelLightList;


};