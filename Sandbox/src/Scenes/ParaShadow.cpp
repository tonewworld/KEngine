#include "ParaShadow.h"
ParaShadow::ParaShadow(std::string name) :name(name)
{
}

ParaShadow::~ParaShadow()
{
	Destroy();
}

void ParaShadow::Init()
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
	m_Mesh1 = KEngine::ObjectFactory::CreateCube("cube2");
	m_Mesh1->SetPosition(glm::vec3(0.0f, 0.f, -3.0f));

	parallelLight0 = KEngine::ObjectFactory::CreateParallelLight("parallelLightCube1");

	parallelLight0->SetLightAttributes({
		glm::vec3( 0.f, 0.f,-1.f),
		glm::vec3(0.2f,0.2f,0.2f),
		glm::vec3(0.5f,0.5f,0.5f),
		glm::vec3(1.0f,1.0f,1.0f),
		glm::vec3(1.0f,1.0f,1.0f)
		});
	parallelLight0->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
	parallelLight0->SetScale(glm::vec3(0.2f));
	parallelLight0->UseDelayRender() = false;
	parallelLight0->SetDrawState(l_Shader, true, false);
	parallelLightList.push_back(parallelLight0);

	Objects.push_back(m_Mesh);
	Objects.push_back(m_Mesh1);
	Objects.push_back(parallelLight0);

}
void ParaShadow::OnUpdate(KEngine::TimeStep ts)
{
	mainCamera->Control(ts.GetTimeStep());

	
}
void ParaShadow::Destroy()
{
	m_Mesh.reset();
	m_Mesh1.reset();
	parallelLight0.reset();


	Objects.clear();
	pointLightList.clear();
	parallelLightList.clear();
}
