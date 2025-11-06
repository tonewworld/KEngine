#include "ParallaxMapping.h"

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
	projMatrix = glm::perspective(glm::radians(45.f), (float)
		KEngine::Application::s_Instance->GetWindow().GetWidth()
		/ KEngine::Application::s_Instance->GetWindow().GetHeight(),
		0.1f, 300.f);
	{
		char* vertexSrc = R"(
					#version 420 core
					layout(location=0) in vec3 v_Position;
					layout(location=1) in vec3 v_Normal;
					layout(location = 2) in vec2 v_TexCoord; 
					layout(location = 3) in vec3 v_Tangent;  

					uniform mat4 model;
					uniform vec3 viewPos;
					uniform vec3 lightPos;

					layout(std140) uniform VPMatrix
					{
						mat4 view;
						mat4 proj;
					
					};
					
					out VS_OUT {
						vec3 fragPos;
						vec2 texCoord;
						vec3 tangentLightPos;
						vec3 tangentViewPos;
						vec3 tangentFragPos;
					} vs_out;
							

					void main()
					{
						gl_Position = proj * view * model * vec4(v_Position,1.0);

						vec3 T = normalize(mat3(model) * v_Tangent);
						vec3 N = normalize(mat3(transpose(inverse(model))) * v_Normal);
						T = normalize(T - dot(T, N) * N);
						vec3 B = cross(N, T);
						
						vs_out.fragPos = vec3(model*vec4(v_Position,1.0));
						vs_out.texCoord=v_TexCoord;

						mat3 TBN = transpose(mat3(T, B, N));  
						vs_out.tangentLightPos = TBN * lightPos;
						vs_out.tangentViewPos  = TBN * viewPos;
						vs_out.tangentFragPos  = TBN * vs_out.fragPos;
						
					}
				)";

		char* fragmentSrc = R"(#version 420 core
					out vec4 FragColor;
					in VS_OUT {
						vec3 fragPos;
						vec2 texCoord;
						vec3 tangentLightPos;
						vec3 tangentViewPos;
						vec3 tangentFragPos;
					} fs_in;

					layout(std140) uniform MaterialUboData{
						vec3 Ambient;
						float _pad0;
						vec3 Diffuse;
						float _pad1;
						vec3 Specular;
						float _pad2;
						float Shininess;
						float _pad3[3];
					}material;

					struct PointLight{
						vec3 Position; float _pad0;
						vec3 Ambient;  float _pad1;
						vec3 Diffuse;  float _pad2;
						vec3 Specular; float _pad3;
						vec3 Color;	   float _pad4;
					};
					
					layout(std140,binding=2) uniform PointLightUboData{
						PointLight pointLightList[10];
						int pointLightCount;
						int _pad0[3];
					};
					
					uniform sampler2D u_DiffuseMap;  
					uniform sampler2D u_NormalMap;  
					uniform sampler2D u_ParallaxMap;		
					uniform bool useBlin;
					uniform bool useNormalMap;
					uniform int useParallaxMapMode;

					float height_Scale = 0.1;

					vec2 ParallaxMapping(vec2 texCoord,vec3 viewDir){
						switch(useParallaxMapMode){
							case 1:{
								float height = texture(u_ParallaxMap,texCoord).r;
								vec2 p = viewDir.xy/viewDir.z * (height*height_Scale);
								return texCoord - p;
								break;
							}
							case 2:{
								const float minLayerNum = 8.0;
								const float maxLayerNum = 32.0;
								float layerNum = mix(maxLayerNum, minLayerNum, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));;
								const float layerDepth = 1.0/layerNum;
								float currentLayerDepth = 0.0;
								vec2 p = viewDir.xy * height_Scale;
								vec2 deltaTexCoord = p / layerNum;
								vec2 currentTexCoord = texCoord;
								float currentDepth = texture(u_ParallaxMap,currentTexCoord).r;
								while(currentDepth > currentLayerDepth){
									currentTexCoord -= deltaTexCoord;
									currentDepth = texture(u_ParallaxMap,currentTexCoord).r;
									currentLayerDepth += layerDepth;	
								}
								return currentTexCoord;
								break;
							}
							case 3:{
								const float minLayerNum = 8.0;
								const float maxLayerNum = 32.0;
								float layerNum = mix(maxLayerNum, minLayerNum, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
								const float layerDepth = 1.0/layerNum;
								float currentLayerDepth = 0.0;
								vec2 p = viewDir.xy * height_Scale;
								vec2 deltaTexCoord = p / layerNum;
								vec2 currentTexCoord = texCoord;
								float currentDepth = texture(u_ParallaxMap,currentTexCoord).r;
								while(currentDepth > currentLayerDepth){
									currentTexCoord -= deltaTexCoord;
									currentDepth = texture(u_ParallaxMap,currentTexCoord).r;
									currentLayerDepth += layerDepth;	
								}
								vec2 prevTexCoord = currentTexCoord += deltaTexCoord;
								float prevDepth = texture(u_ParallaxMap,prevTexCoord).r;
								float weight = currentDepth/(currentDepth-prevDepth);
								return prevTexCoord * weight + currentTexCoord * (1.0 - weight);
								break;
							}
						}
					}

					vec3 CalculatePointLight(){
						vec3 viewDir = normalize(fs_in.tangentViewPos - fs_in.tangentFragPos);
						vec2 texCoord;
						if(useParallaxMapMode>0){
							texCoord = ParallaxMapping(fs_in.texCoord,viewDir);
							texCoord = clamp(texCoord, 0.02, 0.98);
						}
						else{
							texCoord = fs_in.texCoord;
						}

						vec3 baseColor = texture(u_DiffuseMap, texCoord).rgb;
						vec3 norm;
						if (useNormalMap) {
							vec3 tangentNormal = texture(u_NormalMap,texCoord).rgb * 2.0 - 1.0;
							norm = normalize(tangentNormal);         
						} else {
							norm = vec3(0, 0, 1);                    
						}
						vec3 total   = vec3(0.0);          

						for (int i = 0; i < pointLightCount; ++i) {
							vec3 lightDir = normalize(fs_in.tangentLightPos-fs_in.tangentFragPos);

							vec3 ambient  = pointLightList[i].Color * pointLightList[i].Ambient * baseColor;
							float diff    = max(dot(norm, lightDir), 0.0);
							vec3 diffuse  = pointLightList[i].Color * pointLightList[i].Diffuse * baseColor * diff;

							vec3 halfwayDir = normalize(lightDir + viewDir);
							vec3 reflectDir = reflect(-lightDir,norm);
							float spec;
							if(useBlin)
								spec = pow(max(dot(norm,halfwayDir), 0.0), material.Shininess);
							else
								spec = pow(max(dot(viewDir,reflectDir),0.0),material.Shininess);
							vec3 specular   = pointLightList[i].Color * pointLightList[i].Specular * spec;

							float dist = length(pointLightList[i].Position - fs_in.fragPos);
							float attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);

							total += (ambient + diffuse + specular) * attenuation;
						}
						return total;
					}
					
					void main() {
						
						FragColor =vec4(CalculatePointLight(),1.0f);
					}
				)";

		m_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));

	}

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
						color = vec4(0.5f);
					}

				)";
		l_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
	}


	float m_Vertices[] = {
		// px, py, pz,  nx, ny, nz,  u, v,  tx, ty, tz
		//位置                 //法向量              //uv          //切线
		 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,


		 -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		  0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		  0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		 -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		  0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		 -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,


		  0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		  0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		  0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		  0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		  0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		  0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,


		  -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		  -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		  -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		  -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		  -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		  -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,


		  -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		   0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		   0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		  -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		   0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		  -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,


		  -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		   0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		   0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		  -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		   0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		  -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,  -1.0f, 0.0f, 0.0f
	};
	unsigned int m_Indices[]{
		0,1,2,
		3,4,5,
		6,7,8,
		9,10,11,
		12,13,14,
		15,16,17,
		18,19,20,
		21,22,23,
		24,25,26,
		27,28,29,
		30,31,32,
		33,34,35
	};

	KEngine::BufferLayout m_Layout = {
		{KEngine::ShaderDataType::Float3,"position"} ,
		{KEngine::ShaderDataType::Float3,"normal"},
		{KEngine::ShaderDataType::Float2, "v_TexCoord"},
		{KEngine::ShaderDataType::Float3, "v_Tangent"}
	};
	m_Mesh.reset(new KEngine::Mesh(m_Vertices, sizeof(m_Vertices) / sizeof(float),
		m_Layout,
		m_Indices, sizeof(m_Indices) / sizeof(unsigned int),
		"m"));
	m_Mesh->SetMaterial({ glm::vec3(1.0f,0.5f,0.31f),
		glm::vec3(1.0f,0.5f,0.31f),
		glm::vec3(0.5f,0.5f,0.5f),
		32.0f
		});
	m_DiffuseMap.reset(KEngine::Texture2D::Create("references/parallaxMapping/brickwall_diffuse.jpg"));
	m_DiffuseMap->SetTexSlot(1);
	m_NormalMap.reset(KEngine::Texture2D::Create("references/parallaxMapping/brickwall_normal.jpg"));
	m_NormalMap->SetTexSlot(2);
	m_ParallaxMap.reset(KEngine::Texture2D::Create("references/parallaxMapping/brickwall_parallax.jpg"));
	m_ParallaxMap->SetTexSlot(3);
	m_Mesh->AddTexture(m_DiffuseMap);
	m_Mesh->AddTexture(m_NormalMap);
	m_Mesh->AddTexture(m_ParallaxMap);

	float l_Vertices[] = {
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f
	};
	unsigned int l_Indices[]{
		0,1,2,
		3,4,5,
		6,7,8,
		9,10,11,
		12,13,14,
		15,16,17,
		18,19,20,
		21,22,23,
		24,25,26,
		27,28,29,
		30,31,32,
		33,34,35
	};

	KEngine::BufferLayout l_Layout = {
		{KEngine::ShaderDataType::Float3,"position"}
	};
	pointLight0.reset(new KEngine::PointLight(l_Vertices, sizeof(l_Vertices) / sizeof(float),
		l_Layout,
		l_Indices, sizeof(l_Indices) / sizeof(unsigned int),
		"pointLight0"));
	pointLight0->SetLightAttributes({
		glm::vec3(0.2f,0.2f,0.2f),
		glm::vec3(0.5f,0.5f,0.5f),
		glm::vec3(1.0f,1.0f,1.0f),
		glm::vec3(1.0f,1.0f,1.0f)
		});
	pointLight0->SetPosition(glm::vec3(-2.0f, 2.0f, 0.0f));
	pointLight0->SetScale(glm::vec3(0.2f));
	pointLight0->SetIsLight(true);



	vpSL.clear();
	vpSL = {
		m_Shader,
		l_Shader
	};
	//一个shader的列表,为他们每一个绑定相同的VP矩阵
	for (auto& shader : vpSL)
	{
		shader->BindUniformBufferPoint("VPMatrix", 0);
	}

	m_Shader->BindUniformBufferPoint("MaterialUboData", 1);
	m_Shader->BindUniformBufferPoint("PointLightUboData", 2);

	//生成uniform缓冲对象
	matrixUBO.reset(KEngine::UniformBuffer::Create(2 * sizeof(glm::mat4), 0));
	materialUBO.reset(KEngine::UniformBuffer::Create(sizeof(KEngine::MaterialUboData), 1));
	pointLightUBO.reset(KEngine::UniformBuffer::Create(11 * sizeof(KEngine::PointLightUboData), 2));

	pointLightList.push_back(pointLight0);

	Objects.push_back(m_Mesh);
	Objects.push_back(pointLight0);

}
void ParallaxMapping::OnUpdate(KEngine::TimeStep ts)
{
	mainCamera->Control(ts.GetTimeStep());

	// 填充数据到uniform缓冲对象
	matrixUBO->AddVPMatrix(mainCamera->GetViewMatrix(), projMatrix, 0);
	// 设置着色器uniform
	m_Shader->SetUniform3f(mainCamera->GetPosition(), "viewPos");
	m_Shader->SetUniform3f(pointLight0->GetPosition(), "lightPos");
	m_Shader->SetUniform1i(1, "u_DiffuseMap");
	m_Shader->SetUniform1i(2, "u_NormalMap");
	m_Shader->SetUniform1i(3, "u_ParallaxMap");
	// 填充UBO
	materialUBO->AddMaterial(KEngine::MaterialUboData{ m_Mesh->GetMaterial() });
	pointLightUBO->AddPointLight(pointLightList);

	pointLight0->SetDrawState(l_Shader, true, false, 0);
	m_Mesh->SetDrawState(m_Shader, true, false, 1, GL_LESS, 1, 0xFF);

}
void ParallaxMapping::Destroy()
{
	m_Mesh.reset();
	pointLight0.reset();

	matrixUBO.reset();
	materialUBO.reset();
	pointLightUBO.reset();

	Objects.clear();
	pointLightList.clear();
	parallelLightList.clear();
}
