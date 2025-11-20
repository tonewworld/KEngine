#include "ParallaxMapping.h"
#include "../RendererConfig.h"
ParallaxMapping::ParallaxMapping(std::string name) :name(name)
{
}

ParallaxMapping::~ParallaxMapping()
{
	Destroy();
}

void ParallaxMapping::Init()
{

	mainCamera = std::make_unique<KEngine::Camera>();
	
	{
		char* vertexSrc = R"(
					#version 420 core
					layout(location=0) in vec3 v_Position;
				
					uniform mat4 model;
					layout(std140,binding = 0) uniform VPMatrix
					{
						mat4 view;
						mat4 proj;
					};

					void main()
					{
						gl_Position = proj * view * model * vec4(v_Position,1.0);
					}
				)";
		char* fragmentSrc = R"(
					#version 420 core
					out vec4 color;
				
					void main()
					{
						color = vec4(1.0f);
					}

				)";
		l_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
	}


	m_Mesh = KEngine::ObjectFactory::CreateCube("cube1");

	m_DiffuseMap.reset(KEngine::Texture2D::Create("references/parallaxMapping/brickwall_diffuse.jpg"));
	m_DiffuseMap->SetTexSlot(TEX_SLOT_DIFFUSE_MAP);
	m_NormalMap.reset(KEngine::Texture2D::Create("references/parallaxMapping/brickwall_normal.jpg"));
	m_NormalMap->SetTexSlot(TEX_SLOT_NORMAL_MAP);
	m_ParallaxMap.reset(KEngine::Texture2D::Create("references/parallaxMapping/brickwall_parallax.jpg"));
	m_ParallaxMap->SetTexSlot(TEX_SLOT_PARALLAX_MAP);
	m_Mesh->SetDiffuseMap(m_DiffuseMap);
	m_Mesh->SetNormalMap(m_NormalMap);
	m_Mesh->SetParallaxMap(m_ParallaxMap);
	m_Mesh->UseDiffuseMap() = true;
	m_Mesh->UseNormalMap() = true;
	m_Mesh->UseParallaxMapMode() = 1;
	m_Mesh->AddTexture(m_DiffuseMap);
	m_Mesh->AddTexture(m_NormalMap);
	m_Mesh->AddTexture(m_ParallaxMap);

	pointLight0 = KEngine::ObjectFactory::CreatePointLight("pointLightCube1");

	pointLight0->SetLightAttributes({
		glm::vec3(0.2f,0.2f,0.2f),
		glm::vec3(0.5f,0.5f,0.5f),
		glm::vec3(1.0f,1.0f,1.0f),
		glm::vec3(1.0f,1.0f,1.0f)
		});
	pointLight0->SetPosition(glm::vec3(-2.0f, 2.0f, 0.0f));
	pointLight0->SetScale(glm::vec3(0.2f));
	pointLight0->UseDelayRender()=false;
	pointLight0->SetDrawState(l_Shader, true, false);

	pointLightList.push_back(pointLight0);

	Objects.push_back(m_Mesh);
	Objects.push_back(pointLight0);

}
void ParallaxMapping::OnUpdate(KEngine::TimeStep ts)
{
	mainCamera->Control(ts.GetTimeStep());



}
void ParallaxMapping::Destroy()
{
	m_Mesh.reset();
	pointLight0.reset();


	Objects.clear();
	pointLightList.clear();
	parallelLightList.clear();
}
