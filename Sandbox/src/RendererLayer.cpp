#include "RendererLayer.h"
#include "imgui.h"

RendererLayer::RendererLayer() :Layer("Renderer") {
	m_Config.windowWidth = KEngine::Application::s_Instance->GetWindow().GetWidth();
	m_Config.windowHeight = KEngine::Application::s_Instance->GetWindow().GetHeight();

	{
		const char* pickVS = R"(
				#version 420 core
				layout(location=0) in vec3 v_Position;
				uniform mat4 model;
				layout(std140) uniform VPMatrix
				{
					mat4 view;
					mat4 proj;
				};

				void main(){ 
					gl_Position = proj * view * model * vec4(v_Position,1.0); 
				}
			)";

		const char* pickFS = R"(
				#version 420 core
				out vec4 OutColor;
				uniform vec3 pickColor;
				void main(){
					OutColor = vec4(pickColor,1.0);
				 }			
			)";

		pickShader.reset(new KEngine::Shader(pickVS, pickFS));
	}
	pickShader->BindUniformBufferPoint("VPMatrix", 0);
	{
		char* vertexSrc = R"(
				#version 420 core
				layout(location = 0) in vec3 v_Position;

				uniform mat4 model;
				uniform mat4 lightSpaceMatrix;

				void main()
				{
					gl_Position = lightSpaceMatrix * model * vec4(v_Position, 1.0);
				}
				)";
		char* fragmentSrc = R"(
				#version 420 core
				out vec4  FragColor;

				void main()
				{
					gl_FragDepth = gl_FragCoord.z;
				}
				)";

		shadowShader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
	}
	{
		char* vertexSrc = R"(
				#version 420 core
				layout(location = 0) in vec3 v_Position;

				uniform mat4 model;
				

				void main()
				{
					gl_Position =  model * vec4(v_Position, 1.0);
				}
				)";
		char* geometrySrc = R"(
				#version 420 core
				layout(triangles) in;
				layout(triangle_strip, max_vertices=18) out;

				uniform mat4 shadowMatrices[6];

				out vec4 FragPos;

				void main() {
					for(int face = 0; face < 6; ++face) {
						gl_Layer = face;
						for(int i = 0; i < 3; ++i) {
							FragPos = gl_in[i].gl_Position;
							gl_Position = shadowMatrices[face] * FragPos;
							EmitVertex();
						}
						EndPrimitive();
					}
				}
				)";
		char* fragmentSrc = R"(
				#version 420 core
				in vec4 FragPos;

				uniform vec3 lightPos;
				uniform float far_plane;

				void main() {
					float lightDistance = length(FragPos.xyz - lightPos);
					lightDistance = lightDistance / far_plane;
					gl_FragDepth = lightDistance;
				}
				)";

		shadowCubeShader.reset(new KEngine::Shader(vertexSrc, geometrySrc, fragmentSrc));
	}
	{
		char* vertexSrc = R"(
				#version 420 core
				layout(location=0) in vec3 v_Position;
				layout(location=1) in vec3 v_Normal;
		
				uniform mat4 model;
				layout(std140) uniform VPMatrix {
					mat4 view;
					mat4 proj;
				};
		
				void main() {
					gl_Position = proj * view * model * vec4(v_Position, 1.0);
				}
				)";

		char* fragmentSrc = R"(
				#version 420 core
				out vec4 FragColor;
				uniform sampler2D cubeMap;
				uniform bool useCubeMap;
				void main() {
					FragColor = vec4(1.0);
					if(useCubeMap) 
						FragColor=texture(cubeMap,vec2(0.0,0.0));
				}
				)";

		forwardShader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
		forwardShader->BindUniformBufferPoint("VPMatrix", 0);
	}
	{
		char* vertexSrc = R"(
					#version 420 core
					layout(location=0) in vec3 v_Position;
					layout(location=1) in vec3 v_Normal;
					layout(location=2) in vec2 v_TexCoord;
					layout(location=3) in vec3 v_Tangent;


					uniform mat4 model;
					uniform vec3 viewPos;
					layout(std140) uniform VPMatrix
					{
						mat4 view;
						mat4 proj;
					};
					
					out VS_OUT {
						vec3 fragPos;
						vec2 texCoord;
						mat3 TBN;				
						vec3 tangentViewPos;
						vec3 tangentFragPos;
					} vs_out;
							
					void main()
					{
						gl_Position = proj * view * model * vec4(v_Position,1.0);
						vs_out.fragPos = vec3(model * vec4(v_Position,1.0));
						vs_out.texCoord = v_TexCoord;

						vec3 T = normalize(mat3(model) * v_Tangent);
						vec3 N = normalize(mat3(transpose(inverse(model))) * v_Normal);
						T = normalize(T - dot(T, N) * N);
						vec3 B = cross(N, T);
						vs_out.TBN = mat3(T, B, N);

						vs_out.tangentViewPos = vs_out.TBN * viewPos;
						vs_out.tangentFragPos = vs_out.TBN * vs_out.fragPos;
					}

				)";

		char* fragmentSrc = R"(#version 420 core
					layout(location=0) out vec4 gPosition;
					layout(location=1) out vec3 gNormal;
					layout(location=2) out vec4 gALbedoSpec;

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
					uniform bool useDiffuseMap;
					uniform sampler2D u_DiffuseMap;
					uniform bool useNormalMap;
					uniform sampler2D u_NormalMap;
					uniform int useParallaxMapMode;
					uniform sampler2D u_ParallaxMap;

					float heightScale= 0.1;

					in VS_OUT {
						vec3 fragPos;
						vec2 texCoord;
						mat3 TBN;
						vec3 tangentViewPos;
						vec3 tangentFragPos;
					} fs_in;
					vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir)
					{
						switch(useParallaxMapMode) {
							case 1: { 
								float height = texture(u_ParallaxMap, texCoord).r;
								vec2 p = viewDir.xy / viewDir.z * (height * heightScale);
								return texCoord - p;
							}
							case 2: { 
								const float minLayerNum = 8.0;
								const float maxLayerNum = 32.0;
								float layerNum = mix(maxLayerNum, minLayerNum, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
								const float layerDepth = 1.0 / layerNum;
								float currentLayerDepth = 0.0;
								vec2 p = viewDir.xy * heightScale;
								vec2 deltaTexCoord = p / layerNum;
								vec2 currentTexCoord = texCoord;
								float currentDepth = texture(u_ParallaxMap, currentTexCoord).r;
                
								while(currentDepth > currentLayerDepth) {
									currentTexCoord -= deltaTexCoord;
									currentDepth = texture(u_ParallaxMap, currentTexCoord).r;
									currentLayerDepth += layerDepth;    
								}
								return currentTexCoord;
							}
							case 3: { 
								const float minLayerNum = 8.0;
								const float maxLayerNum = 32.0;
								float layerNum = mix(maxLayerNum, minLayerNum, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
								const float layerDepth = 1.0 / layerNum;
								float currentLayerDepth = 0.0;
								vec2 p = viewDir.xy * heightScale;
								vec2 deltaTexCoord = p / layerNum;
								vec2 currentTexCoord = texCoord;
								float currentDepth = texture(u_ParallaxMap, currentTexCoord).r;
                
								while(currentDepth > currentLayerDepth) {
									currentTexCoord -= deltaTexCoord;
									currentDepth = texture(u_ParallaxMap, currentTexCoord).r;
									currentLayerDepth += layerDepth;    
								}
                
								vec2 prevTexCoord = currentTexCoord + deltaTexCoord;
								float prevDepth = texture(u_ParallaxMap, prevTexCoord).r;
								float weight = currentLayerDepth - currentDepth;
								weight /= (currentLayerDepth - prevDepth) + 0.00001; 
                
								return prevTexCoord * weight + currentTexCoord * (1.0 - weight);
							}
							default:
								return texCoord;
						}
					}
					const float NEAR = 0.1; 
					const float FAR = 300.0f; 
					float LinearizeDepth(float depth)
					{
						float z = depth * 2.0 - 1.0; 
						return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));    
					}
					void main() {
						
						gPosition.xyz = fs_in.fragPos;
						gPosition.a = -fs_in.fragPos.z;
						vec3 finalNormal = normalize(fs_in.TBN[2]);
						vec3 viewDir = normalize(fs_in.tangentViewPos - fs_in.tangentFragPos);
						vec2 texCoord = fs_in.texCoord;
						if(useParallaxMapMode > 0 && heightScale > 0.0) {
							texCoord = ParallaxMapping(fs_in.texCoord, viewDir);
							texCoord = clamp(texCoord, 0.02, 0.98); 
						}
						if (useNormalMap) {
							vec3 tangentNormal = texture(u_NormalMap, texCoord).rgb * 2.0 - 1.0;
							finalNormal = normalize(fs_in.TBN * tangentNormal);
						}
						gNormal = finalNormal;
						vec3 color = material.Diffuse;
						if(useDiffuseMap){
							color = texture(u_DiffuseMap,fs_in.texCoord).rgb;
						}		
						gALbedoSpec = vec4(color, material.Specular.r);

					}
				)";

		geometryPassShader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));

	}
	geometryPassShader->BindUniformBufferPoint("VPMatrix", 0);
	geometryPassShader->BindUniformBufferPoint("MaterialUboData", 1);
	{
		char* vertexSrc = R"(
				#version 420 core
				layout(location = 0) in vec2 v_Position;
				layout(location = 1) in vec2 v_TexCoords;
				out vec2 TexCoords;	
				void main()
				{
					gl_Position = vec4(v_Position,0.0,1.0);
					TexCoords=v_TexCoords;
				}
				)";

		char* fragmentSrc = R"(#version 420 core
					out float FragColor;
					in vec2 TexCoords;

					uniform sampler2D gPositionDepth;
					uniform sampler2D gNormal;
					uniform sampler2D texNoise;

					uniform vec3 samples[64];

					uniform int kernelSize;
					uniform float radius;

					uniform vec2 noiseScale;

					uniform mat4 projection;

					void main()
					{
						vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;
						float fragDepth = texture(gPositionDepth, TexCoords).a;

						vec3 normal = texture(gNormal, TexCoords).rgb;
						vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;
						
						vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
						vec3 bitangent = cross(normal, tangent);
						mat3 TBN = mat3(tangent, bitangent, normal);
						
						float occlusion = 0.0;
						for(int i = 0; i < kernelSize; ++i)
						{
						
							vec3 sampleVec = TBN * samples[i]; 
							vec3 samplePos = fragPos + sampleVec * radius; 
        
							vec4 offset = projection * vec4(samplePos, 1.0);//这里要转换到视图空间
							offset.xyz /= offset.w;
							vec2 sampleUV = offset.xy * 0.5 + 0.5; 

							if(sampleUV.x < 0.0 || sampleUV.x > 1.0 || sampleUV.y < 0.0 || sampleUV.y > 1.0)
								continue;        

							float sampleDepth = -texture(gPositionDepth, sampleUV).a;
							float sampleViewDepth = samplePos.z;        
							float rangeCheck = smoothstep(0.0, 1.0, radius / (abs(fragDepth - sampleViewDepth) + 1e-5));

							occlusion += (sampleDepth >= sampleViewDepth ? 1.0 : 0.0) * rangeCheck;         
						}
						occlusion = 1.0 - (occlusion / float(kernelSize));
    
						FragColor = occlusion;
					}
				)";

		ssaoShader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
	}
	{
		char* vertexSrc = R"(
				#version 420 core
				layout(location = 0) in vec2 v_Position;
				layout(location = 1) in vec2 v_TexCoords;
				out vec2 TexCoords;	
				void main()
				{
					gl_Position = vec4(v_Position,0.0,1.0);
					TexCoords=v_TexCoords;
				}
				)";

		char* fragmentSrc = R"(#version 420 core
					in vec2 TexCoords;
					out float fragColor;

					uniform sampler2D ssaoInput;
					const int blurSize = 4; // use size of noise texture (4x4)

					void main() 
					{
					   vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
					   float result = 0.0;
					   for (int x = 0; x < blurSize; ++x) 
					   {
						  for (int y = 0; y < blurSize; ++y) 
						  {
							 vec2 offset = (vec2(-2.0) + vec2(float(x), float(y))) * texelSize;
							 result += texture(ssaoInput, TexCoords + offset).r;
						  }
					   }
 
					   fragColor = result / float(blurSize * blurSize);
					}
				)";

		ssaoBlurShader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
	}
	{
		char* vertexSrc = R"(
				#version 420 core
				layout(location = 0) in vec2 v_Position;
				layout(location = 1) in vec2 v_TexCoords;
				out vec2 TexCoords;	
				void main()
				{
					gl_Position = vec4(v_Position,0.0,1.0);
					TexCoords=v_TexCoords;
				}
				)";
		char* fragmentSrc = R"(
				#version 420 core
				layout(location=0) out vec4 FragColor;
				layout(location=1) out vec4 BrightColor;
				in vec2 TexCoords;

				uniform sampler2D gPosition;
				uniform sampler2D gNormal;
				uniform sampler2D gAlbedoSpec;
				uniform sampler2D ssaoTexture;
				uniform bool enableSSAO;

				uniform sampler2D shadowMap;             
				uniform samplerCube shadowCubeMap;        
				uniform mat4 lightSpaceMatrix;           
				uniform vec3 mainLightDir;            
				uniform vec3 mainLightPos;              
				uniform float far_plane;                 
    


				struct PointLight {
					vec3 Position;
					vec3 Color;
					vec3 Ambient;
					vec3 Diffuse;
					vec3 Specular;
				};

				layout(std140) uniform PointLightUboData {
					PointLight pointLightList[10];
					int pointLightCount;
					int _pad0[3];
				};

				struct ParallelLight {
					vec3 Direction;
					vec3 Color;
					vec3 Ambient;
					vec3 Diffuse;
					vec3 Specular;
				};

				layout(std140) uniform ParallelLightUboData {
					ParallelLight parallelLightList[10];
					int parallelLightCount;
					int _pad1[3];
				};

				uniform vec3 viewPos;
				float Shininess = 32.0f;
				float CalculatePointShadow(vec3 fragPos, vec3 lightPos, float farPlane);
				vec3 calculatePointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float specularStrength);
				float CalculateParallelShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
				vec3 calculateParallelLight(ParallelLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float specularStrength,vec4 fragPosLightSpace);
				void main() {
					vec3 fragPos = texture(gPosition, TexCoords).rgb;
					vec3 normal = texture(gNormal, TexCoords).rgb;
					vec4 albedoSpec = texture(gAlbedoSpec, TexCoords);
    
					vec3 albedo = albedoSpec.rgb;
					float specularStrength = albedoSpec.a;
    
					vec3 viewDir = normalize(viewPos - fragPos);
					vec3 lightingResult = vec3(0.1) * albedo; 

					vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

					if(enableSSAO) {
						float ambientOcclusion = texture(ssaoTexture, TexCoords).r;
						lightingResult *= ambientOcclusion;
					}    

					for(int i = 0; i < pointLightCount; i++) {
						lightingResult += calculatePointLight(pointLightList[i], fragPos, normal, viewDir, albedo, specularStrength);
					}
    
					for(int i = 0; i < parallelLightCount; i++) {
						lightingResult += calculateParallelLight(parallelLightList[i], fragPos, normal, viewDir, albedo, specularStrength,fragPosLightSpace);
					}
    
					FragColor = vec4(lightingResult, 1.0);
    
					float brightness = dot(lightingResult, vec3(0.2126, 0.7152, 0.0722));
					BrightColor = brightness > 1.0 ? vec4(lightingResult, 1.0) : vec4(0.0);
				}

				float CalculatePointShadow(vec3 fragPos, vec3 lightPos, float farPlane)
				{
					vec3 fragToLight = fragPos - lightPos;
					float currentDepth = length(fragToLight);
    
					// 采样立方体贴图
					float closestDepth = texture(shadowCubeMap, fragToLight).r * farPlane;
    
					// 添加bias
					float bias = 0.05;
					return currentDepth - bias > closestDepth ? 1.0 : 0.0;
				}
				vec3 calculatePointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float specularStrength) {
					vec3 lightDir = normalize(light.Position - fragPos);

					float diff = max(dot(normal, lightDir), 0.0);
					vec3 diffuse = light.Color * diff * albedo * light.Diffuse;
    
					vec3 reflectDir = reflect(-lightDir, normal);
					float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
					vec3 specular = light.Color * spec * specularStrength * light.Specular;

					float distance = length(light.Position - fragPos);
					float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    
					float shadow = CalculatePointShadow(fragPos, light.Position, far_plane);
    
					return (diffuse + specular) * attenuation * (1.0-shadow);
				}
				float CalculateParallelShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
				{
					// 执行透视除法
					vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
					// 变换到[0,1]范围
					projCoords = projCoords * 0.5 + 0.5;
					
					// 获取当前fragment在光源视角下的深度
					float currentDepth = projCoords.z;
    
					// PCF软阴影采样
					float shadow = 0.0;
					vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
					for(int x = -1; x <= 1; ++x)
					{
						for(int y = -1; y <= 1; ++y)
						{
							float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
							shadow += currentDepth  > pcfDepth ? 1.0 : 0.0;
						}
					}
					shadow /= 9.0;
    
					return shadow;
				}
				vec3 calculateParallelLight(ParallelLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float specularStrength,vec4 fragPosLightSpace) {
					vec3 lightDir = normalize(-light.Direction);
    
					float diff = max(dot(normal, lightDir), 0.0);
					vec3 diffuse = light.Color * diff * albedo * light.Diffuse;

					vec3 reflectDir = reflect(-lightDir, normal);
					float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
					vec3 specular = light.Color * spec * specularStrength * light.Specular;

					float shadow = CalculateParallelShadow(fragPosLightSpace, normal, lightDir);

					return (diffuse + specular) * (1.0f-shadow);
				}
			)";

		lightingPassShader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
	}
	lightingPassShader->BindUniformBufferPoint("PointLightUboData", 2);
	lightingPassShader->BindUniformBufferPoint("ParallelLightUboData", 3);
	{
		char* vertexSrc = R"(
				#version 420 core
				layout (location = 0) in vec2 position;
				layout (location = 1) in vec2 texCoords;

				out vec2 TexCoords;

				void main()
				{
					gl_Position = vec4(position,0.0f,1.0f);
					TexCoords = texCoords;
				}
				)";
		char* fragmentSrc = R"(
				#version 420 core
				out vec4 FragColor;
				in vec2 TexCoords;

				uniform sampler2D image;

				uniform bool horizontal;

				uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

				void main()
				{             
					vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
					vec3 result = texture(image, TexCoords).rgb * weight[0]; // current fragment's contribution
					if(horizontal)
					{
						for(int i = 1; i < 5; ++i)
						{
							result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
							result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
						}
					}
					else
					{
						for(int i = 1; i < 5; ++i)
						{
							result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
							result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
						}
					}
					FragColor = vec4(result, 1.0);
				}
				)";

		hdrAndBlurShader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
	}
	{
		char* vertexSrc = R"(
				#version 420 core
				layout (location = 0) in vec2 position;
				layout (location = 1) in vec2 texCoords;

				out vec2 TexCoords;

				void main()
				{
					gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
					TexCoords = texCoords;
				}
				)";
		char* fragmentSrc = R"(
				out vec4 FragColor;
				in vec2 TexCoords;

				uniform sampler2D scene;
				uniform sampler2D bloomBlur;
				uniform bool hdr;
				uniform bool bloom;
				uniform bool gamma;
				uniform float exposure;

				void main()
				{             
					const float g = 2.2;
					vec3 hdrColor = texture(scene, TexCoords).rgb;      
					vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
					if(bloom)
						hdrColor += bloomColor; 
					vec3 result;
					if(hdr){
						result = vec3(1.0) - exp(-hdrColor * exposure);
					}
					else{
						 result = clamp(hdrColor, 0.0, 1.0);
					}		
					if(gamma){
						result = pow(result, vec3(1.0 / g));
					}	
					   
					FragColor = vec4(result, 1.0f);
				}
				)";

		screenShader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
	}
	float quad_Vertices[] = {   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// Positions   // TexCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	unsigned int quadIndexes[] = {
		0,1,2,
		3,4,5
	};
	KEngine::BufferLayout quad_Layout = {
		{KEngine::ShaderDataType::Float2,"position"} ,
		{KEngine::ShaderDataType::Float2,"texCoords"}
	};

	m_Config.textureResolutions.shadowMap = 1024;


	pickFBO.reset(KEngine::FrameBuffer::Create());
	pickTexture.reset(KEngine::Texture2D::Create());
	pickFBO->Add2DTexture(GL_COLOR_ATTACHMENT0, pickTexture->GetRendererID(), GL_TRUE, GL_TRUE);
	pickRBO.reset(KEngine::RenderBuffer::Create(GL_DEPTH24_STENCIL8, m_Config.windowWidth, m_Config.windowHeight));

	depthFBO.reset(KEngine::FrameBuffer::Create());
	depthTexture.reset(KEngine::Texture2D::Create(GL_DEPTH_COMPONENT, m_Config.textureResolutions.shadowMap, m_Config.textureResolutions.shadowMap));
	depthFBO->Add2DTexture(GL_DEPTH_ATTACHMENT, depthTexture->GetRendererID(), GL_NONE, GL_NONE);
	depthTexture->SetTexSlot(TEX_SLOT_DEPTH_MAP);

	depthCubeFBO.reset(KEngine::FrameBuffer::Create());
	depthCubeTexture.reset(KEngine::TextureCube::Create(GL_DEPTH_COMPONENT, m_Config.textureResolutions.shadowMap, m_Config.textureResolutions.shadowMap));
	depthCubeFBO->AddTexture(GL_DEPTH_ATTACHMENT, depthCubeTexture->GetRendererID(), GL_NONE, GL_NONE);
	depthCubeTexture->SetTexSlot(TEX_SLOT_DEPTH_CUBE_MAP);

	//重构：几何管线
	gBuffer.reset(KEngine::FrameBuffer::Create());
	gPosition.reset(KEngine::Texture2D::Create(GL_RGBA16F, m_Config.windowWidth, m_Config.windowHeight));
	gNormal.reset(KEngine::Texture2D::Create(GL_RGB16F, m_Config.windowWidth, m_Config.windowHeight));
	gAlbedoSpec.reset(KEngine::Texture2D::Create(GL_RGBA, m_Config.windowWidth, m_Config.windowHeight));
	unsigned int gTextures[3] = { gPosition->GetRendererID(), gNormal->GetRendererID(),gAlbedoSpec->GetRendererID()};
	gBuffer->Add2DTextures(GL_COLOR_ATTACHMENT0, gTextures, GL_TRUE, GL_TRUE, 3);
	gRBO.reset(KEngine::RenderBuffer::Create(GL_DEPTH24_STENCIL8, m_Config.windowWidth, m_Config.windowHeight));
	gBuffer->AddRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, gRBO->GetRendererID());
	gPosition->SetTexSlot(TEX_SLOT_GPOSITION);
	gNormal->SetTexSlot(TEX_SLOT_GNORMAL);
	gAlbedoSpec->SetTexSlot(TEX_SLOT_GALBEDOSPEC);
	matrixUBO.reset(KEngine::UniformBuffer::Create(2 * sizeof(glm::mat4), 0));
	materialUBO.reset(KEngine::UniformBuffer::Create(4 * sizeof(glm::vec4), 1));

	//SSAO管线
	ssaoPassMesh.reset(new KEngine::Mesh(quad_Vertices, sizeof(quad_Vertices) / sizeof(float),
		quad_Layout,
		quadIndexes, sizeof(quadIndexes) / sizeof(unsigned int)));
	ssaoFBO.reset(KEngine::FrameBuffer::Create());
	ssaoTexture.reset(KEngine::Texture2D::Create(GL_R16F, m_Config.windowWidth, m_Config.windowHeight));
	ssaoFBO->Add2DTexture(GL_COLOR_ATTACHMENT0, ssaoTexture->GetRendererID(), GL_TRUE, GL_TRUE);
	ssaoTexture->SetTexSlot(TEX_SLOT_SSAO_OUTPUT);

	srand(static_cast<unsigned>(time(nullptr)));
	ssaoKernel.reserve(64);
	for (GLuint i = 0; i < 64; ++i) {
		glm::vec3 sample(
			RandFloat() * 2.0f - 1.0f,
			RandFloat() * 2.0f - 1.0f,
			RandFloat()
		);
		sample = glm::normalize(sample);
		sample *= RandFloat();
		if (sample.z < 0.0f) sample.z = -sample.z; // 强制在法线方向的半球内
		GLfloat scale = GLfloat(i) / 64.0f;
		scale = 0.1f + scale * scale * (1.0f - 0.1f);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	std::vector<glm::vec3> ssaoNoise;
	ssaoNoise.reserve(16);
	for (unsigned int i = 0; i < 16; i++) {
		ssaoNoise.push_back(glm::vec3(
			RandFloat() * 2.0f - 1.0f,
			RandFloat() * 2.0f - 1.0f,
			0.0f
		));
	}
	ssaoNoiseTexture.reset(KEngine::Texture2D::Create(GL_RGB16F, 4, 4, &ssaoNoise));
	ssaoNoiseTexture->SetTexSlot(TEX_SLOT_SSAO_NOISE);
	ssaoPassMesh->AddTexture(gPosition);
	ssaoPassMesh->AddTexture(gNormal);
	ssaoPassMesh->AddTexture(ssaoNoiseTexture);
	//SSAO模糊管线
	ssaoBlurPassMesh.reset(new KEngine::Mesh(quad_Vertices, sizeof(quad_Vertices) / sizeof(float),
		quad_Layout,
		quadIndexes, sizeof(quadIndexes) / sizeof(unsigned int)));
	ssaoBlurFBO.reset(KEngine::FrameBuffer::Create());
	ssaoBlurTexture.reset(KEngine::Texture2D::Create(GL_R16F, m_Config.windowWidth, m_Config.windowHeight));
	ssaoBlurFBO->Add2DTexture(GL_COLOR_ATTACHMENT0, ssaoBlurTexture->GetRendererID(), GL_TRUE, GL_TRUE);
	ssaoBlurPassMesh->AddTexture(ssaoTexture);

	//重构：光照管线
	lightingPassMesh.reset(new KEngine::Mesh(quad_Vertices, sizeof(quad_Vertices) / sizeof(float),
		quad_Layout,
		quadIndexes, sizeof(quadIndexes) / sizeof(unsigned int)));
	lightingPassMesh->AddTexture(gPosition);
	lightingPassMesh->AddTexture(gNormal);
	lightingPassMesh->AddTexture(gAlbedoSpec);
	lightingPassMesh->AddTexture(ssaoBlurTexture);
	lightingPassMesh->AddTexture(depthTexture);
	lightingPassMesh->AddTexture(depthCubeTexture);

	lightingFBO.reset(KEngine::FrameBuffer::Create());
	lightingTexture.reset(KEngine::Texture2D::Create(GL_RGB16F, m_Config.windowWidth, m_Config.windowHeight));
	lightingTexture->SetTexSlot(TEX_SLOT_SCENE);
	bloomTexture.reset(KEngine::Texture2D::Create(GL_RGB16F, m_Config.windowWidth, m_Config.windowHeight));
	unsigned int lightTextures[2] = { lightingTexture->GetRendererID(),bloomTexture->GetRendererID() };
	lightingFBO->Add2DTextures(GL_COLOR_ATTACHMENT0, lightTextures, GL_TRUE, GL_TRUE, 2);
	lightingRBO.reset(KEngine::RenderBuffer::Create(GL_DEPTH24_STENCIL8, m_Config.windowWidth, m_Config.windowHeight));
	lightingFBO->AddRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, lightingRBO->GetRendererID());

	pointLightUBO.reset(KEngine::UniformBuffer::Create(11 * sizeof(KEngine::PointLightUboData), 2));
	parallelLightUBO.reset(KEngine::UniformBuffer::Create(11 * sizeof(KEngine::ParallelLightUboData), 3));

	//重构：后处理
	postProcessMesh.reset(new KEngine::Mesh(quad_Vertices, sizeof(quad_Vertices) / sizeof(float),
		quad_Layout,
		quadIndexes, sizeof(quadIndexes) / sizeof(unsigned int)));
	for (int i = 0; i < 2; i++) {
		pingpongFBO[i].reset(KEngine::FrameBuffer::Create());
		pingpongTexture[i].reset(KEngine::Texture2D::Create(GL_RGB16F, m_Config.windowWidth, m_Config.windowHeight));
		pingpongFBO[i]->Add2DTexture(GL_COLOR_ATTACHMENT0, pingpongTexture[i]->GetRendererID(), GL_TRUE, GL_TRUE);
	}

	//重构：屏幕渲染
	quad_Mesh.reset(new KEngine::Mesh(quad_Vertices, sizeof(quad_Vertices) / sizeof(float),
		quad_Layout,
		quadIndexes, sizeof(quadIndexes) / sizeof(unsigned int)));
	quad_Mesh->AddTexture(lightingTexture);
	quad_Mesh->AddTexture(pingpongTexture[0]);

	skyboxScene.reset(new Skybox("SkyboxScene"));
	paraShadowScene.reset(new ParaShadow("ParaShadow"));
	omniShadowScene.reset(new OmniShadow("OmniShadow"));
	normalMappingScene.reset(new NormalMapping("NormalMapping"));
	parallaxMappingScene.reset(new ParallaxMapping("ParallaxMapping"));

	sceneList.push_back(skyboxScene);
	sceneList.push_back(paraShadowScene);
	sceneList.push_back(omniShadowScene);
	sceneList.push_back(normalMappingScene);
	sceneList.push_back(parallaxMappingScene);
}

void RendererLayer::OnAttach() {
	KEngine::Renderer::Init();

}
void RendererLayer::OnDetach()
{

}
void RendererLayer::OnUpdate(KEngine::TimeStep ts) {

	if (!currentScene)return;

	currentScene->OnUpdate(ts);
	//光影
	CalculateShadow();
	//重构
	GeometryPass();
	//SSAO
	if (m_Config.renderSettings.enableSSAO) {
		SSAOPass();
		SSAOBlurPass();
	}

	LightingPass();
	HDRandBloom();

	ScreenPass();
	ForwardRenderPass();

	PickWithColor();

}

void RendererLayer::OnEvent(KEngine::Event& event)
{
}

void RendererLayer::ImGuiRender()
{
	DrawSceneHierarchy();

	// 绘制检视窗口
	DrawInspector();

	DrawSceneList();

	DrawGlobalSettings();
}


void RendererLayer::PickWithColor()
{
	float mouseX = 0.f, mouseY = 0.f;

	if (KEngine::Camera::CheckLeftMouseButtonPress())
	{
		std::pair<float, float> mousePosition = KEngine::Input::GetMousePosition();
		mouseX = mousePosition.first;
		mouseY = mousePosition.second;

		auto EncodeIDToColor = [](int id)->glm::vec3 {
			unsigned char r = id & 0xFF;
			unsigned char g = (id >> 8) & 0xFF;
			unsigned char b = (id >> 16) & 0xFF;
			return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
			};
		auto DecodeColorToID = [](unsigned char r, unsigned char g, unsigned char b)->int {
			return r + (g << 8) + (b << 16);
			};

		// 离屏渲染到 FBO 的颜色缓冲
		pickFBO->Bind();
		KEngine::Renderer::ColorPickBegin();

		// 渲染每个对象为其 ID color（只写颜色）
		for (const auto& obj : currentScene->GetObjectsInScene())
		{
			int id = obj->GetID();            // 需要存在
			glm::vec3 color = EncodeIDToColor(id);
			glm::mat4 model = obj->GetModelMatrix(); // 需要存在

			pickShader->SetUniformMatrix4fv(model, "model");
			pickShader->SetUniform3f(color, "pickColor");

			// 需要对象能以 shader 绘制自己（或提供 mesh/model 访问）
			KEngine::Renderer::Submit(pickShader, obj);
		}

		// 读取像素（窗口坐标到 GL 底部原点）
		int width = KEngine::Application::s_Instance->GetWindow().GetWidth();
		int height = KEngine::Application::s_Instance->GetWindow().GetHeight();
		int rx = static_cast<int>(mouseX);
		int ry = height - 1 - static_cast<int>(mouseY);

		auto pixel = KEngine::Renderer::ReadPixel(rx, ry);

		pickFBO->Unbind();

		int pickedID = DecodeColorToID(pixel[0], pixel[1], pixel[2]);
		if (pickedID == 0) {

			return;
		}

		// 在 Objects 中查找 pickedID
		for (const auto& obj : currentScene->GetObjectsInScene()) {
			if (obj->GetID() == pickedID) {
				m_SelectedObjectID = pickedID;
				m_SelectedObject = obj;

				return;
			}
		}

	}
	return;
}

void RendererLayer::CalculateShadow()
{

	//parallel
	for (const auto& light : currentScene->GetParallelLightInScene()) {

		depthFBO->Bind();
		KEngine::Renderer::ParallelLightShadowBegin();
		shadowShader->Bind();

		shadowShader->SetUniformMatrix4fv(light->CalculateLightSpace(), "lightSpaceMatrix");

		for (const auto& obj : currentScene->GetObjectsInScene()) {
			if (obj->UseIsLight()) {
				continue;
			}
			glm::mat4 model = obj->GetModelMatrix();
			shadowShader->SetUniformMatrix4fv(model, "model");
			obj->SetDrawState(shadowShader, true, false);
			obj->Draw();
		}

		depthFBO->Unbind();

		KEngine::Renderer::ParallelLightShadowEnd();
	}
	//pointlight
	for (const auto& light : currentScene->GetPointLightInScene()) {
		depthCubeFBO->Bind();
		KEngine::Renderer::PointLightShadowBegin();

		const auto& matrices = light->CalculateLightSpace();
		shadowCubeShader->Bind();
		for (int i = 0; i < 6; ++i) {
			shadowCubeShader->SetUniformMatrix4fv(matrices[i], ("shadowMatrices[" + std::to_string(i) + "]").c_str());
		}//这里出问题
		shadowCubeShader->SetUniform3f(light->GetPosition(), "lightPos");
		shadowCubeShader->SetUniform1f(25.f, "far_plane");

		for (const auto& obj : currentScene->GetObjectsInScene()) {
			if( obj->UseIsLight()) {
				continue;
			}
			glm::mat4 model = obj->GetModelMatrix();
			shadowCubeShader->SetUniformMatrix4fv(model, "model");
			obj->SetDrawState(shadowCubeShader, true, false);
			obj->Draw();
		}

		KEngine::Renderer::PointLightShadowEnd();
		depthCubeFBO->Unbind();

	}
}


void RendererLayer::GeometryPass()
{
	gBuffer->Bind();
	KEngine::Renderer::GeometryPassBegin();
	for (const auto& obj : currentScene->GetObjectsInScene())
	{
		if (!obj->UseDelayRender())continue;
		obj->SetDrawState(geometryPassShader, true, false);
		obj->UpdateModelMatrix();
		geometryPassShader->SetUniformMatrix4fv(obj->GetModelMatrix(), "model");
		geometryPassShader->SetUniform3f(currentScene->GetMainCamera()->GetPosition(), "viewPos");
		if (obj->GetDiffuseMap()) {
			geometryPassShader->SetUniform1b(obj->UseDiffuseMap(), "useDiffuseMap");
			geometryPassShader->SetUniform1i(TEX_SLOT_DIFFUSE_MAP, "u_DiffuseMap");
		}
		else {
			geometryPassShader->SetUniform1b(false, "useDiffuseMap");
		}

		if (obj->GetNormalMap()) {
			geometryPassShader->SetUniform1b(obj->UseNormalMap(), "useNormalMap");
			geometryPassShader->SetUniform1i(TEX_SLOT_NORMAL_MAP, "u_NormalMap");
		}
		else {
			geometryPassShader->SetUniform1b(false, "useNormalMap");
		}
		if (obj->GetParallaxMap()) {
			geometryPassShader->SetUniform1i(obj->UseParallaxMapMode(), "useParallaxMapMode");
			geometryPassShader->SetUniform1i(TEX_SLOT_PARALLAX_MAP, "u_ParallaxMap");
		}
		else {
			geometryPassShader->SetUniform1i(0, "useParallaxMapMode");
		}
		materialUBO->AddMaterial(KEngine::MaterialUboData{ obj->GetMaterial() });
		matrixUBO->AddVPMatrix(currentScene->GetMainCamera()->GetViewMatrix(), currentScene->GetMainCamera()->GetProjMatrix(), 0);
		obj->Draw();
	}
	KEngine::Renderer::GeometryPassEnd();
	gBuffer->Unbind();

}

void RendererLayer::SSAOPass()
{
	ssaoFBO->Bind();
	KEngine::Renderer::SSAOPassBegin();

	ssaoShader->SetUniform1i(TEX_SLOT_GPOSITION, "gPositionDepth");
	ssaoShader->SetUniform1i(TEX_SLOT_GNORMAL, "gNormal");
	ssaoShader->SetUniform1i(TEX_SLOT_SSAO_NOISE, "texNoise");
	ssaoShader->SetUniformMatrix4fv(currentScene->GetMainCamera()->GetProjMatrix(), "projection");
	ssaoShader->SetUniform1f(m_Config.renderSettings.ssaoRadius, "radius");
	ssaoShader->SetUniform1f(m_Config.renderSettings.ssaoBias, "bias");
	ssaoShader->SetUniform1i(m_Config.renderSettings.ssaoKernelSize, "kernelSize");

	for (int i = 0; i < ssaoKernel.size(); ++i) {
		ssaoShader->SetUniform3f(ssaoKernel[i], ("samples[" + std::to_string(i) + "]").c_str());
	}

	// 计算噪声缩放
	glm::vec2 noiseScale(m_Config.windowWidth / 4.0f, m_Config.windowHeight / 4.0f);
	ssaoShader->SetUniform2f(noiseScale, "noiseScale");

	// 绘制全屏quad
	ssaoPassMesh->SetDrawState(ssaoShader, false, false);
	ssaoPassMesh->Draw();

	KEngine::Renderer::SSAOPassEnd();
	ssaoFBO->Unbind();
}

void RendererLayer::SSAOBlurPass()
{
	ssaoBlurFBO->Bind();
	KEngine::Renderer::SSAOBlurPassBegin();

	ssaoBlurShader->Bind();

	ssaoBlurShader->SetUniform1i(TEX_SLOT_SSAO_OUTPUT, "ssaoInput");

	ssaoBlurPassMesh->SetDrawState(ssaoBlurShader, false, false);
	ssaoBlurPassMesh->Draw();

	KEngine::Renderer::SSAOBlurPassEnd();
	ssaoBlurFBO->Unbind();
}

void RendererLayer::LightingPass()
{
	lightingFBO->Bind();
	KEngine::Renderer::LightingPassBegin();
	lightingPassShader->Bind();
	lightingPassMesh->SetDrawState(lightingPassShader, false, false);
	lightingPassShader->SetUniform1i(TEX_SLOT_GPOSITION, "gPosition");
	lightingPassShader->SetUniform1i(TEX_SLOT_GNORMAL, "gNormal");
	lightingPassShader->SetUniform1i(TEX_SLOT_GALBEDOSPEC, "gAlbedoSpec");

	lightingPassShader->SetUniform1i(TEX_SLOT_DEPTH_MAP, "shadowMap");
	lightingPassShader->SetUniform1i(TEX_SLOT_DEPTH_CUBE_MAP, "shadowCubeMap");

	auto parallelLights = currentScene->GetParallelLightInScene();
	if (!parallelLights.empty()) {
		glm::mat4 lightSpaceMatrix = parallelLights[0]->CalculateLightSpace();
		lightingPassShader->SetUniformMatrix4fv(lightSpaceMatrix, "lightSpaceMatrix");
		lightingPassShader->SetUniform3f(glm::vec3(0.0,0.0,-1.0), "mainLightDir");
	}

	auto pointLights = currentScene->GetPointLightInScene();
	if (!pointLights.empty()) {
		lightingPassShader->SetUniform3f(pointLights[0]->GetPosition(), "mainLightPos");
		lightingPassShader->SetUniform1f(25.0f, "far_plane"); // 与CalculateShadow中的值一致
	}

	if (m_Config.renderSettings.enableSSAO)
	{
		lightingPassShader->SetUniform1i(TEX_SLOT_SSAO_OUTPUT, "ssaoTexture");
		lightingPassShader->SetUniform1b(true, "enableSSAO");
	}
	else
		lightingPassShader->SetUniform1b(false, "enableSSAO");

	pointLightUBO->AddPointLight(currentScene->GetPointLightInScene());
	parallelLightUBO->AddParallelLight(currentScene->GetParallelLightInScene());

	lightingPassShader->SetUniform3f(currentScene->GetMainCamera()->GetPosition(), "viewPos");
	lightingPassMesh->Draw();
	KEngine::Renderer::LightingPassEnd();
	lightingFBO->Unbind();
}
void RendererLayer::HDRandBloom()
{
	//bloom
	bool horizontal = true, first_iteration = true;
	GLuint amount = 10;
	hdrAndBlurShader->Bind();
	hdrAndBlurShader->SetUniform1i(TEX_SLOT_BLOOM_BLUR, "image");
	for (GLuint i = 0; i < amount; i++)
	{
		pingpongFBO[horizontal]->Bind();
		KEngine::Renderer::HDRandBloomBegin();
		hdrAndBlurShader->SetUniform1b(horizontal, "horizontal");
		if (first_iteration) {
			bloomTexture->Bind(TEX_SLOT_BLOOM_BLUR);
		}
		else {
			pingpongTexture[!horizontal]->Bind(TEX_SLOT_BLOOM_BLUR);
		}
		postProcessMesh->SetDrawState(hdrAndBlurShader, false, false);
		postProcessMesh->Draw();

		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	m_FinalBloomIndex = !horizontal ? 1 : 0;
	KEngine::Renderer::HDRandBloomEnd();
	pingpongFBO[!horizontal]->Unbind();

}

void RendererLayer::ScreenPass()
{
	KEngine::Renderer::ScreenPassBegin();

	lightingTexture->Bind(TEX_SLOT_SCENE);
	pingpongTexture[m_FinalBloomIndex]->Bind(TEX_SLOT_BLOOM_BLUR);

	screenShader->SetUniform1f(m_Config.renderSettings.exposure, "exposure");
	screenShader->SetUniform1b(m_Config.renderSettings.enableBloom, "bloom");
	screenShader->SetUniform1b(m_Config.renderSettings.enableHDR, "hdr");
	screenShader->SetUniform1b(m_Config.renderSettings.enableGamma, "gamma");
	screenShader->SetUniform1i(TEX_SLOT_SCENE, "scene");
	screenShader->SetUniform1i(TEX_SLOT_BLOOM_BLUR, "bloomBlur");
	quad_Mesh->SetDrawState(screenShader, false, false);
	quad_Mesh->Draw();
	KEngine::Renderer::ScreenPassEnd();
}

void RendererLayer::ForwardRenderPass()
{
	KEngine::Renderer::BlitFrameBuffer(gBuffer, m_Config.windowWidth, m_Config.windowHeight);
	KEngine::Renderer::ForwardRenderPassBegin();
	forwardShader->Bind();
	for (const auto& obj : currentScene->GetObjectsInScene()) {
		if (obj->UseDelayRender())continue;
		obj->UpdateModelMatrix();
		obj->shader->SetUniformMatrix4fv(obj->GetModelMatrix(), "model");
		matrixUBO->AddVPMatrix(currentScene->GetMainCamera()->GetViewMatrix(), currentScene->GetMainCamera()->GetProjMatrix(), 0);
		obj->Draw();
	}
	KEngine::Renderer::ForwardRenderPassEnd();
}

void RendererLayer::DrawSceneHierarchy()
{
	if (!m_ShowSceneHierarchy || !currentScene) return;

	ImGui::Begin("SceneManager", &m_ShowSceneHierarchy);

	// 窗口设置
	ImGui::Text("Objects (%d)", currentScene->GetObjectsInScene().size());
	ImGui::Separator();

	// 对象列表
	for (const auto& obj : currentScene->GetObjectsInScene()) {
		// 为每个对象创建一个可选择的行
		bool isSelected = (m_SelectedObjectID == obj->GetID());

		// 使用Selectable来创建可选择项
		if (ImGui::Selectable(obj->GetName().c_str(), isSelected)) {
			m_SelectedObjectID = obj->GetID();
			m_SelectedObject = obj;
		}

		// 右键菜单
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete")) {
				// 这里可以添加删除逻辑
				std::cout << "Delete Object: " << obj->GetName() << std::endl;
			}
			if (ImGui::MenuItem("Copy")) {
				// 这里可以添加复制逻辑
				std::cout << "Copy Object: " << obj->GetName() << std::endl;
			}
			ImGui::EndPopup();
		}

		// 显示对象基本信息（可选）
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("ID: %d", obj->GetID());
			ImGui::Text("Type: %s", typeid(*obj).name());
			ImGui::EndTooltip();
		}
	}

	// 添加对象按钮
	ImGui::Separator();
	if (ImGui::Button("+ Add Object")) {
		// 这里可以弹出添加对象的菜单
		ImGui::OpenPopup("add_object_popup");
	}

	// 添加对象弹出菜单
	if (ImGui::BeginPopup("add_object_popup")) {
		if (ImGui::MenuItem("Cube")) {
			// 添加立方体逻辑
			std::cout << "Add Cube" << std::endl;
		}
		if (ImGui::MenuItem("Light")) {
			// 添加光源逻辑
			std::cout << "Add Light" << std::endl;
		}
		if (ImGui::MenuItem("Model")) {
			// 添加模型逻辑
			std::cout << "Add Model" << std::endl;
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}

void RendererLayer::DrawInspector()
{
	if (!m_ShowInspector) return;

	ImGui::Begin("Inspector", &m_ShowInspector);

	if (m_SelectedObject) {
		// 显示选中对象的名称和ID
		ImGui::Text("Name: %s", m_SelectedObject->GetName().c_str());
		ImGui::Text("ID: %d", m_SelectedObject->GetID());
		ImGui::Separator();

		// 绘制对象属性
		DrawObjectProperties(m_SelectedObject);
	}
	else {
		ImGui::Text("Nothing Selected");
		ImGui::Text("Selected one thing to check its attribution");
	}

	ImGui::End();
}

void RendererLayer::DrawObjectProperties(std::shared_ptr<KEngine::Object> object)
{
	// 变换组件
	if (ImGui::CollapsingHeader("Transition", ImGuiTreeNodeFlags_DefaultOpen)) {
		// 直接使用对象的属性，不需要从矩阵分解
		glm::vec3 position = object->GetPosition();
		glm::vec3 rotation = object->GetRotation();
		glm::vec3 scale = object->GetScale();

		// 位置
		float pos[3] = { position.x, position.y, position.z };
		if (ImGui::DragFloat3("Position", pos, 0.1f)) {
			object->SetPosition(glm::vec3(pos[0], pos[1], pos[2]));
		}

		// 旋转
		float rot[3] = { rotation.x, rotation.y, rotation.z };
		if (ImGui::DragFloat3("Rotation", rot, 1.0f)) {
			object->SetRotation(glm::vec3(rot[0], rot[1], rot[2]));
		}

		// 缩放
		float scl[3] = { scale.x, scale.y, scale.z };
		if (ImGui::DragFloat3("Scale", scl, 0.1f, 0.01f, 100.0f)) {
			object->SetScale(glm::vec3(scl[0], scl[1], scl[2]));
		}
	}


	if (ImGui::CollapsingHeader("Render", ImGuiTreeNodeFlags_DefaultOpen)) {

		if (object->shader) {
			ImGui::Text("Shader: %s", "Allocated");

			ImGui::Checkbox("BlinPhon: ", &object->UseBlin());
			ImGui::Checkbox("NormalMap: ", &object->UseNormalMap());

			ImGui::Separator();
			ImGui::Text("ParallaxMap :");
			std::array<std::string, 4> algo{ "Close", "Simple", "Steep","Occlusion" };
			for (std::size_t i = 0; i < algo.size(); ++i)
			{
				ImGui::RadioButton(algo[i].data(), &object->UseParallaxMapMode(), static_cast<int>(i));
			}

		}
		else {
			ImGui::Text("Shader: null");
		}

		ImGui::Text("Matertial Attribution:");
		ImGui::Indent();

		ImGui::Unindent();
	}


	if (ImGui::CollapsingHeader("Other Attribution")) {

		ImGui::Text("Object Attribution: %s", typeid(*object).name());

		if (auto model = std::dynamic_pointer_cast<KEngine::Model>(object)) {
			ImGui::Text("Model:");
			ImGui::Indent();
			ImGui::Text("Path: %s", model->GetPath());
			ImGui::Unindent();
		}
	}

	// 自定义属性扩展点
	if (ImGui::CollapsingHeader("Custom Attribution")) {
		/*ImGui::BulletText("物理属性");
		ImGui::BulletText("脚本组件");
		ImGui::BulletText("动画组件");
		ImGui::BulletText("粒子系统");*/
		// ... 更多自定义组件
	}
}
void RendererLayer::DrawSceneList()
{
	ImGui::Begin("Scenes");

	for (size_t i = 0; i < sceneList.size(); ++i)
	{
		const auto& sc = sceneList[i];
		ImGui::PushID(static_cast<int>(i));

		// 主项
		bool selected = (currentScene == sc);
		if (ImGui::Selectable(sc->GetName().c_str(), selected))
			if (currentScene != sc)
				SwitchToScene(static_cast<int>(i));

		// 右键菜单
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Switch to this scene"))
				if (currentScene != sc)
					SwitchToScene(static_cast<int>(i));
			ImGui::EndPopup();
		}

		ImGui::PopID();
	}
	ImGui::End();
}
void RendererLayer::SwitchToScene(int index)
{
	if (index < 0 || index >= static_cast<int>(sceneList.size())) return;
	if (currentScene)
		currentScene->Destroy();
	KEngine::Renderer::ResetGLState();
	currentScene = sceneList[index];
	currentScene->Init();
}

void RendererLayer::DrawGlobalSettings()
{
	if (!m_ShowGlobalSettings) return;

	ImGui::Begin("Global Render Settings", &m_ShowGlobalSettings);

	// ========== Post-Processing Settings ==========
	if (ImGui::CollapsingHeader("Post-Processing", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Exposure slider
		ImGui::SliderFloat("Exposure", &m_Config.renderSettings.exposure, 0.1f, 5.0f, "%.2f");
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Control scene exposure\nLow=darker, High=brighter");

		// Bloom toggle
		ImGui::Checkbox("Enable Bloom", &m_Config.renderSettings.enableBloom);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Enable bloom effect\nGlowing objects will produce halo");

		// HDR toggle
		ImGui::Checkbox("Enable HDR", &m_Config.renderSettings.enableHDR);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Enable HDR rendering\nAllow color values to exceed 1.0");

		// Gamma correction toggle
		ImGui::Checkbox("Enable Gamma Correction", &m_Config.renderSettings.enableGamma);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Enable Gamma correction\nCorrect color space");

		//SSAO Settings
		if (ImGui::CollapsingHeader("SSAO Settings", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Enable SSAO", &m_Config.renderSettings.enableSSAO);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Toggle Screen Space Ambient Occlusion");

			ImGui::SliderFloat("Radius", &m_Config.renderSettings.ssaoRadius, 0.1f, 5.0f, "%.2f");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("SSAO sampling radius");

			ImGui::SliderInt("Kernel Size", &m_Config.renderSettings.ssaoKernelSize, 16, 64);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Number of SSAO samples (higher=better quality but slower)");

			ImGui::SliderFloat("Bias", &m_Config.renderSettings.ssaoBias, 0.0f, 0.1f, "%.3f");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Depth bias to prevent self-shadowing");
		}
	}

	// ========== Performance Info ==========
	if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// FPS
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::SameLine();
		ImGui::Text(" (%.2f ms/frame)", 1000.0f / ImGui::GetIO().Framerate);

		// Post-processing time (optional)
		static float bloomTime = 0.0f;
		ImGui::Text("Bloom Blur: %.2f ms", bloomTime);
	}

	// ========== Window Info ==========
	if (ImGui::CollapsingHeader("Window Info", ImGuiTreeNodeFlags_DefaultOpen))
	{
		int width = KEngine::Application::s_Instance->GetWindow().GetWidth();
		int height = KEngine::Application::s_Instance->GetWindow().GetHeight();

		ImGui::Text("Window Size: %d x %d", width, height);

		// Display all FBO resolutions
		if (ImGui::TreeNode("Framebuffer Resolutions"))
		{
			ImGui::Text("HDR FBO: %d x %d", width, height);
			ImGui::Text("Shadow Map: 1024 x 1024");
			ImGui::Text("Pick FBO: %d x %d", width, height);
			ImGui::TreePop();
		}
	}

	// ========== Quick Presets ==========
	if (ImGui::CollapsingHeader("Presets", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Day Time")) {
			m_Config.renderSettings.exposure = 1.5f;
			m_Config.renderSettings.enableBloom = true;
			m_Config.renderSettings.enableHDR = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Night Mode")) {
			m_Config.renderSettings.exposure = 0.5f;
			m_Config.renderSettings.enableBloom = true;
			m_Config.renderSettings.enableHDR = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("LDR Mode")) {
			m_Config.renderSettings.exposure = 1.0f;
			m_Config.renderSettings.enableBloom = false;
			m_Config.renderSettings.enableHDR = false;
			m_Config.renderSettings.enableGamma = true;
		}
	}

	ImGui::End();
}