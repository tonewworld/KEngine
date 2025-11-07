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

		shadowCubeShader.reset(new KEngine::Shader(vertexSrc, geometrySrc,fragmentSrc));
	}
	{
		char* vertexSrc = R"(
				#version 420 core
				layout (location = 0) in vec3 position;
				layout (location = 1) in vec2 texCoords;

				out vec2 TexCoords;

				void main()
				{
					gl_Position = vec4(position, 1.0f);
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

		blurShader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
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
	quad_Mesh.reset(new KEngine::Mesh(quad_Vertices, sizeof(quad_Vertices) / sizeof(float),
		quad_Layout,
		quadIndexes, sizeof(quadIndexes) / sizeof(unsigned int)));

	m_Config.textureResolutions.shadowMap = 1024;

	FBO.reset(KEngine::FrameBuffer::Create());
	quad_Texture.reset(KEngine::Texture2D::Create());
	quad_Texture->SetTexSlot(TEX_SLOT_QUAD);
	FBO->Add2DTexture(GL_COLOR_ATTACHMENT0,quad_Texture->GetRendererID(),GL_TRUE,GL_TRUE);
	RBO.reset(KEngine::RenderBuffer::Create(GL_DEPTH24_STENCIL8,m_Config.windowWidth, m_Config.windowHeight));
	
	pickFBO.reset(KEngine::FrameBuffer::Create());
	pickTexture.reset(KEngine::Texture2D::Create());
	pickFBO->Add2DTexture(GL_COLOR_ATTACHMENT0, pickTexture->GetRendererID(), GL_TRUE, GL_TRUE);
	pickRBO.reset(KEngine::RenderBuffer::Create(GL_DEPTH24_STENCIL8, m_Config.windowWidth, m_Config.windowHeight));

	depthFBO.reset(KEngine::FrameBuffer::Create());
	depthTexture.reset(KEngine::Texture2D::Create(GL_DEPTH_COMPONENT, m_Config.textureResolutions.shadowMap, m_Config.textureResolutions.shadowMap));
	depthFBO->Add2DTexture(GL_DEPTH_ATTACHMENT, depthTexture->GetRendererID(), GL_NONE, GL_NONE);

	depthCubeFBO.reset(KEngine::FrameBuffer::Create());
	depthCubeTexture.reset(KEngine::TextureCube::Create(GL_DEPTH_COMPONENT, m_Config.textureResolutions.shadowMap, m_Config.textureResolutions.shadowMap));
	depthCubeFBO->AddTexture(GL_DEPTH_ATTACHMENT, depthCubeTexture->GetRendererID(), GL_NONE, GL_NONE);

	hdrFBO.reset(KEngine::FrameBuffer::Create());
	hdrTexture.reset(KEngine::Texture2D::Create(GL_RGB16F, m_Config.windowWidth, m_Config.windowHeight));
	hdrTexture->SetTexSlot(TEX_SLOT_SCENE);
	bloomTexture.reset(KEngine::Texture2D::Create(GL_RGB16F, m_Config.windowWidth, m_Config.windowHeight));
	hdrRBO.reset(KEngine::RenderBuffer::Create(GL_DEPTH24_STENCIL8, m_Config.windowWidth, m_Config.windowHeight));
	unsigned int hdrTextures[2] = {hdrTexture->GetRendererID(),bloomTexture->GetRendererID()};
	hdrFBO->AddRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, hdrRBO->GetRendererID());
	hdrFBO->Add2DTextures(GL_COLOR_ATTACHMENT0, hdrTextures, GL_TRUE, GL_TRUE, 2);

	for (int i = 0; i < 2; i++) {
		pingpongFBO[i].reset(KEngine::FrameBuffer::Create());
		pingpongTexture[i].reset(KEngine::Texture2D::Create(GL_RGB16F, m_Config.windowWidth, m_Config.windowHeight));
		pingpongFBO[i]->Add2DTexture(GL_COLOR_ATTACHMENT0, pingpongTexture[i]->GetRendererID(), GL_TRUE, GL_TRUE);
	}
	pingpongTexture[0]->SetTexSlot(TEX_SLOT_BLOOM_BLUR);

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
	FBO.reset();
	RBO.reset();
	pickFBO.reset();
	pickRBO.reset();
}
void RendererLayer::OnUpdate(KEngine::TimeStep ts) {
	
	
	
	if(currentScene)
	{
		currentScene->OnUpdate(ts);
		PickWithColor();
		CalculateShadow();
		HDRandBloom();
		SetSceneAttri();
	}
	
	
	screenShader->SetUniform1i(TEX_SLOT_SCENE, "scene");
	screenShader->SetUniform1i(TEX_SLOT_BLOOM_BLUR, "bloomBlur");


	quad_Mesh->AddTexture(pingpongTexture[0]);
	quad_Mesh->AddTexture(hdrTexture);
	
	KEngine::Renderer::BeginScene();
	quad_Mesh->SetDrawState(screenShader, false, false);
	quad_Mesh->Draw(screenShader);
	KEngine::Renderer::EndScene();
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

void RendererLayer::SetSceneAttri()
{
	screenShader->SetUniform1f(m_Config.renderSettings.exposure, "exposure");
	screenShader->SetUniform1b(m_Config.renderSettings.enableBloom, "bloom");
	screenShader->SetUniform1b(m_Config.renderSettings.enableHDR, "hdr");
	screenShader->SetUniform1b(m_Config.renderSettings.enableGamma, "gamma");
}

void RendererLayer::PickWithColor()
{
	float mouseX = 0.f,mouseY = 0.f;
	
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
			if (obj->GetIsLight())
				continue;
			glm::mat4 model = obj->GetModelMatrix();
			shadowShader->SetUniformMatrix4fv(model, "model");
			obj->Draw(shadowShader); 
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
		}
		shadowCubeShader->SetUniform3f(light->GetPosition(), "lightPos");
		shadowCubeShader->SetUniform1f(25.f, "far_plane");

		for (const auto& obj : currentScene->GetObjectsInScene()) {
			if (obj->GetIsLight())
				continue;
			glm::mat4 model = obj->GetModelMatrix();
			shadowCubeShader->SetUniformMatrix4fv(model, "model");
			obj->Draw(shadowCubeShader); 
		}

		KEngine::Renderer::PointLightShadowEnd();
		depthCubeFBO->Unbind();

	}
}

void RendererLayer::HDRandBloom()
{
	//hdr
	hdrFBO->Bind();
	KEngine::Renderer::BeginScene();

	for (const auto& obj : currentScene->GetObjectsInScene())
	{
		obj->UpdateModelMatrix();
		obj->shader->SetUniformMatrix4fv(obj->GetModelMatrix(), "model");

		obj->shader->Bind();
		if (currentScene->GetParallelLightInScene().size() != 0) {
			depthTexture->Bind(TEX_SLOT_SHADOW_PARA);
			obj->shader->SetUniformMatrix4fv(currentScene->GetParallelLightInScene()[0]->CalculateLightSpace(), "lightSpaceMatrix");
		}

		if (currentScene->GetPointLightInScene().size() != 0) {
			depthCubeTexture->Bind(TEX_SLOT_SHADOW_CUBE);
		}

		obj->Draw(obj->shader);

	}
	hdrFBO->Unbind();
	//bloom
	bool horizontal = true, first_iteration = true;
	GLuint amount = 10;
	blurShader->Bind();
	blurShader->SetUniform1i(TEX_SLOT_BLOOM_SHADER, "image");
	for (GLuint i = 0; i < amount; i++)
	{
		pingpongFBO[horizontal]->Bind();
		blurShader->SetUniform1b(horizontal, "horizontal");
		if (first_iteration) {
			bloomTexture->Bind(TEX_SLOT_BLOOM_SHADER);
		}
		else {
			pingpongTexture[!horizontal]->Bind(TEX_SLOT_BLOOM_SHADER);
		}
		quad_Mesh->SetDrawState(blurShader, false, false);
		quad_Mesh->Draw(blurShader);

		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	pingpongFBO[!horizontal]->Unbind();

}

void RendererLayer::DrawSceneHierarchy()
{
	if (!m_ShowSceneHierarchy||!currentScene) return;

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
			if(!object->GetIsLight())
			{
				ImGui::Checkbox("BlinPhon: ", &object->UseBlin());
				ImGui::Checkbox("NormalMap: ", &object->UseNormalMap());

				ImGui::Separator();
				ImGui::Text("ParallaxMap :");
				std::array<std::string, 4> algo{ "Close", "Simple", "Steep","Occlusion"};
				for (std::size_t i = 0; i < algo.size(); ++i)
				{
					ImGui::RadioButton(algo[i].data(), &object->UseParallaxMapMode(), static_cast<int>(i));
				}
			
			}
		}
		else {
			ImGui::Text("Shader: null");
		}


		ImGui::Text("Matertial Attribution:");
		ImGui::Indent();

	
		if (object->GetIsLight()) {//将来这里可能要做枚举
			ImGui::Text("Type: light");
		}
		else {
			ImGui::Text("Type: Mesh");
		}

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
	if(currentScene)
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
