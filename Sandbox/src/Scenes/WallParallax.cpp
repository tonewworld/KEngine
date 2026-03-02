#include "WallParallax.h"
#include "../RendererConfig.h"

WallParallax::WallParallax(std::string name) : name(name) {}
WallParallax::~WallParallax() { Destroy(); }

void WallParallax::Init() {
    mainCamera = std::make_shared<KEngine::Camera>();

    // create a thin wall using a cube scaled flat
    wallMesh = KEngine::ObjectFactory::CreateCube("brickWall");
    wallMesh->SetScale(glm::vec3(6.0f, 3.0f, 0.05f));
    wallMesh->SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));

    // placeholder textures; user can replace paths with actual brick maps
    std::shared_ptr<KEngine::Texture2D> diffuse;
    diffuse.reset(KEngine::Texture2D::Create("references/parallaxMapping/brickwall_diffuse.jpg"));
    std::shared_ptr<KEngine::Texture2D> normal;
    normal.reset(KEngine::Texture2D::Create("references/parallaxMapping/brickwall_normal.jpg"));
    std::shared_ptr<KEngine::Texture2D> parallax;
    parallax.reset(KEngine::Texture2D::Create("references/parallaxMapping/brickwall_parallax.jpg"));
    if (diffuse) { diffuse->SetTexSlot(TEX_SLOT_DIFFUSE_MAP); wallMesh->SetDiffuseMap(diffuse); wallMesh->AddTexture(diffuse); wallMesh->UseDiffuseMap() = true; }
    if (normal) { normal->SetTexSlot(TEX_SLOT_NORMAL_MAP); wallMesh->SetNormalMap(normal); wallMesh->AddTexture(normal); wallMesh->UseNormalMap() = true; }
    if (parallax) { parallax->SetTexSlot(TEX_SLOT_PARALLAX_MAP); wallMesh->SetParallaxMap(parallax); wallMesh->AddTexture(parallax); wallMesh->UseParallaxMapMode() = 1; }

    // add a point light to see normal/parallax effects
    auto p = KEngine::ObjectFactory::CreatePointLight("wall_pLight");
    p->SetPosition(glm::vec3(0.0f, 2.5f, 2.0f));
    p->SetLightAttributes({ glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f), glm::vec3(1.0f,0.9f,0.8f) });
    p->UseDelayRender() = false;

    Objects.push_back(wallMesh);
    Objects.push_back(p);
    pointLightList.push_back(p);
}

void WallParallax::OnUpdate(KEngine::TimeStep ts) {
    mainCamera->Control(ts.GetTimeStep());
}

void WallParallax::Destroy() {
    wallMesh.reset();
    Objects.clear();
    pointLightList.clear();
    parallelLightList.clear();
}
