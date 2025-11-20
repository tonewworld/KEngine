#include "SDFMix.h"
SDFMix::SDFMix(std::string name) :name(name)
{
}

SDFMix::~SDFMix()
{
	Destroy();
}

void SDFMix::Init()
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

	m_Sphere1 = KEngine::ObjectFactory::CreateSphere(50, 50, 0.3f, "sphere1");
	m_Sphere2 = KEngine::ObjectFactory::CreateSphere(50, 50, 0.3f, "sphere2");

	pointLight0 = KEngine::ObjectFactory::CreatePointLight("pointLightCube1");

	pointLight0->SetLightAttributes({
		glm::vec3(0.2f,0.2f,0.2f),
		glm::vec3(0.5f,0.5f,0.5f),
		glm::vec3(1.0f,1.0f,1.0f),
		glm::vec3(1.0f,1.0f,1.0f)
		});
	pointLight0->SetPosition(glm::vec3(-2.0f, 2.0f, 0.0f));
	pointLight0->SetScale(glm::vec3(0.2f));
	pointLight0->UseDelayRender() = false;
	pointLight0->SetDrawState(l_Shader, true, false);

	pointLightList.push_back(pointLight0);

	Objects.push_back(m_Sphere1);
	Objects.push_back(m_Sphere2);
	Objects.push_back(pointLight0);
}
void SDFMix::OnUpdate(KEngine::TimeStep ts)
{
	mainCamera->Control(ts.GetTimeStep());

}
void SDFMix::Destroy()
{

	pointLight0.reset();

	Objects.clear();
	pointLightList.clear();
	parallelLightList.clear();
}
