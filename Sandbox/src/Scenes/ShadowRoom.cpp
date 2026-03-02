#include "ShadowRoom.h"

ShadowRoom::ShadowRoom(std::string name) : name(name) {}
ShadowRoom::~ShadowRoom() { Destroy(); }

void ShadowRoom::Init() {
    mainCamera = std::make_shared<KEngine::Camera>();

    // simple ground and two spheres to cast shadows
    auto ground = KEngine::ObjectFactory::CreateCube("ground");
    ground->SetScale(glm::vec3(10.0f, 0.1f, 10.0f));
    ground->SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
    auto sphA = KEngine::ObjectFactory::CreateSphere(64, 64, 1.0f, "sphereA");
    sphA->SetPosition(glm::vec3(-1.0f, 0.0f, -1.0f));
    sphA->SetScale(glm::vec3(1.0f,1.0f,1.0f));

    auto sphB = KEngine::ObjectFactory::CreateSphere(64, 64, 1.0f, "sphereB");
    sphB->SetPosition(glm::vec3(2.0f, 0.0f, 0.5f));
    sphB->SetScale(glm::vec3(1.0f));

    // directional light for shadow mapping
    auto dir = KEngine::ObjectFactory::CreateParallelLight("mainDir");
    dir->SetLightAttributes({ glm::vec3(-0.3f,-1.0f,-0.3f), glm::vec3(0.2f), glm::vec3(0.5f), glm::vec3(0.6f), glm::vec3(0.9f) });
    dir->SetPosition(glm::vec3(0.0f, 4.0f, 0.0f));
    dir->SetScale(glm::vec3(0.3f));
    dir->UseDelayRender() = false;

    // point light to add more shadow variety
    auto p = KEngine::ObjectFactory::CreatePointLight("pLight");
    p->SetPosition(glm::vec3(0.0f, 2.5f, -2.0f));
    // point light attributes: ambient, diffuse, specular, color
    p->SetLightAttributes({ glm::vec3(0.1f), glm::vec3(0.8f), glm::vec3(1.0f), glm::vec3(1.0f,1.0f,1.0f) });

    // add to lists
    Objects.push_back(ground);
    Objects.push_back(sphA);
    Objects.push_back(sphB);
    Objects.push_back(dir);
    Objects.push_back(p);

    parallelLightList.push_back(dir);
    pointLightList.push_back(p);
}

void ShadowRoom::OnUpdate(KEngine::TimeStep ts) {
    mainCamera->Control(ts.GetTimeStep());
}

void ShadowRoom::Destroy() {
    meshes.clear();
    Objects.clear();
    pointLightList.clear();
    parallelLightList.clear();
}
