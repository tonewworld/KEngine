#include "SSAORoom.h"

SSAORoom::SSAORoom(std::string name) : name(name) {}
SSAORoom::~SSAORoom() { Destroy(); }

void SSAORoom::Init() {
    mainCamera = std::make_shared<KEngine::Camera>();

    // Build a simple room using scaled cubes: floor, ceiling, walls
    auto floor = KEngine::ObjectFactory::CreateCube("floor");
    floor->SetScale(glm::vec3(10.0f, 0.1f, 10.0f));
    floor->SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));

    auto ceiling = KEngine::ObjectFactory::CreateCube("ceiling");
    ceiling->SetScale(glm::vec3(10.0f, 0.1f, 10.0f));
    ceiling->SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));

    auto backWall = KEngine::ObjectFactory::CreateCube("backWall");
    backWall->SetScale(glm::vec3(10.0f, 3.0f, 0.1f));
    backWall->SetPosition(glm::vec3(0.0f, 1.0f, -5.0f));

    auto leftWall = KEngine::ObjectFactory::CreateCube("leftWall");
    leftWall->SetScale(glm::vec3(0.1f, 3.0f, 10.0f));
    leftWall->SetPosition(glm::vec3(-5.0f, 1.0f, 0.0f));

    auto rightWall = KEngine::ObjectFactory::CreateCube("rightWall");
    rightWall->SetScale(glm::vec3(0.1f, 3.0f, 10.0f));
    rightWall->SetPosition(glm::vec3(5.0f, 1.0f, 0.0f));

    // Add some objects inside to see SSAO
    for (int i = 0; i < 4; ++i) {
        auto c = KEngine::ObjectFactory::CreateCube("obj" + std::to_string(i));
        float x = -2.5f + i * 1.7f;
        c->SetPosition(glm::vec3(x, 0.0f, 0.0f));
        c->SetScale(glm::vec3(0.8f));
        meshes.push_back(c);
    }

    // lights: one parallel + one point light
    auto dir = KEngine::ObjectFactory::CreateParallelLight("dirLight");
    dir->SetLightAttributes({ glm::vec3(-0.5f, -1.0f, -0.5f), glm::vec3(0.1f), glm::vec3(0.6f), glm::vec3(0.6f), glm::vec3(0.6f) });
    dir->SetPosition(glm::vec3(0.0f, 4.0f, 0.0f));
    dir->UseDelayRender() = false;
    parallelLightList.push_back(dir);

    auto p = KEngine::ObjectFactory::CreatePointLight("pointLight1");
    p->SetPosition(glm::vec3(0.0f, 2.0f, 2.0f));
    // point light attributes: ambient, diffuse, specular, color
    p->SetLightAttributes({ glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f), glm::vec3(1.0f,0.8f,0.6f) });
    pointLightList.push_back(p);

    // push into object list
    Objects.push_back(floor);
    Objects.push_back(ceiling);
    Objects.push_back(backWall);
    Objects.push_back(leftWall);
    Objects.push_back(rightWall);
    for (auto &m : meshes) Objects.push_back(m);
    Objects.push_back(dir);
    Objects.push_back(p);
}

void SSAORoom::OnUpdate(KEngine::TimeStep ts) {
    mainCamera->Control(ts.GetTimeStep());
}

void SSAORoom::Destroy() {
    meshes.clear();
    Objects.clear();
    pointLightList.clear();
    parallelLightList.clear();
}
