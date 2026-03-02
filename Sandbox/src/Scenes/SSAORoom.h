#pragma once
#include "KEngine.h"

class SSAORoom : public KEngine::Scene {
public:
    SSAORoom(std::string name);
    ~SSAORoom();
    inline virtual std::string GetName() override { return name; }
    inline virtual std::shared_ptr<KEngine::Camera> GetMainCamera() override { return mainCamera; }
    inline std::vector<std::shared_ptr<KEngine::Object>> GetObjectsInScene() override { return Objects; }
    inline std::vector<std::shared_ptr<KEngine::PointLight>> GetPointLightInScene() override { return pointLightList; }
    inline std::vector<std::shared_ptr<KEngine::ParallelLight>> GetParallelLightInScene() override { return parallelLightList; }
    virtual void Init() override;
    virtual void OnUpdate(KEngine::TimeStep ts) override;
    virtual void Destroy() override;
private:
    std::string name;
    std::vector<std::shared_ptr<KEngine::Object>> Objects;

    std::shared_ptr<KEngine::Camera> mainCamera;

    // scene objects
    std::vector<std::shared_ptr<KEngine::Mesh>> meshes;

    // lights
    std::vector<std::shared_ptr<KEngine::PointLight>> pointLightList;
    std::vector<std::shared_ptr<KEngine::ParallelLight>> parallelLightList;
};
