#include "RendererLayer.h"
#include "imgui.h"

RendererLayer::RendererLayer() :Layer("Renderer") {
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
	pickFBO.reset(KEngine::FrameBuffer::Create());
	pickTexture.reset(KEngine::Texture2D::Create());
	pickFBO->AddTexture(pickTexture->GetRendererID());
	pickRBO.reset(KEngine::RenderBuffer::Create());

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
				#version 420 core
				in vec2 TexCoords;
				out vec4 color;

				uniform sampler2D screenTexture;

				void main()
				{
					color =vec4(vec3(texture(screenTexture,TexCoords)),1.f);
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
	quad_Mesh.reset(new KEngine::Mesh(quad_Vertices, sizeof(quad_Vertices) / sizeof(float),
		quad_Layout,
		quadIndexes, sizeof(quadIndexes) / sizeof(unsigned int)));

	FBO.reset(KEngine::FrameBuffer::Create());
	quad_Texture.reset(KEngine::Texture2D::Create());
	FBO->AddTexture(quad_Texture->GetRendererID());
	RBO.reset(KEngine::RenderBuffer::Create());

	testScene.reset(new TestScene("testScene"));
	materialScene.reset(new MaterialScene("materialScene"));
	sceneList.push_back(testScene);
	sceneList.push_back(materialScene);
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

	
	FBO->Bind();
	KEngine::Renderer::BeginScene();
	
	if(currentScene)
	{
		currentScene->OnUpdate(ts);
		for (const auto& obj : currentScene->GetObjectsInScene())
		{
			obj->UpdateModelMatrix();
			obj->shader->SetUniformMatrix4fv(obj->GetModelMatrix(), "model");
			obj->Draw(obj->shader);
		}
		PickWithColor();
	}
	FBO->Unbind();

	quad_Mesh->SetDrawState(quad_Texture, screenShader, false, false);
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
		std::cout << "Read pixel at (" << rx << ", " << ry << ") -> RGB: "
			<< (int)pixel[0] << ", " << (int)pixel[1] << ", " << (int)pixel[2] << std::endl;

		pickFBO->Unbind();

		int pickedID = DecodeColorToID(pixel[0], pixel[1], pixel[2]);
		if (pickedID == 0) {
			// 背景或未命中
			std::cout << "Pick: nothing\n";
			return;
		}

		// 在 Objects 中查找 pickedID（根据你的容器方式调整）
		for (const auto& obj : currentScene->GetObjectsInScene()) {
			if (obj->GetID() == pickedID) {
				std::cout << "Pick: ID=" << pickedID << " Name=" << obj->GetName() << "\n";
				// 在这里把属性返回、触发事件或填充 UI
				return;
			}
		}
		std::cout << "Pick: unknown ID=" << pickedID << "\n";
	}
	return;
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

	// 渲染组件
	if (ImGui::CollapsingHeader("Render")) {
		// 显示使用的着色器
		if (object->shader) {
			ImGui::Text("Shader: %s", "Allocated"); // 可以显示着色器名称
		}
		else {
			ImGui::Text("Shader: null");
		}

		// 显示材质属性（如果有）
		ImGui::Text("Matertial Attribution:");
		ImGui::Indent();

		// 这里可以根据对象类型显示不同的材质属性
		if (object->GetName() == "light") {
			ImGui::Text("Type: light");
		}
		else if (object->GetName() == "skybox") {
			ImGui::Text("Type: skybox");
		}
		else {
			ImGui::Text("Type: Mesh");
		}

		ImGui::Unindent();
	}

	// 其他组件可以根据需要添加
	if (ImGui::CollapsingHeader("Other Attribution")) {
		// 显示对象类型
		ImGui::Text("Object Attribution: %s", typeid(*object).name());

		// 显示顶点数等信息（如果可用）
		if (auto mesh = std::dynamic_pointer_cast<KEngine::Mesh>(object)) {
			// 这里可以显示网格的详细信息
			ImGui::Text("Mesh Data:");
			ImGui::Indent();
			ImGui::Text("Vertics: %d", /* 获取顶点数 */ 0);
			ImGui::Text("Triangles: %d", /* 获取三角形数 */ 0);
			ImGui::Unindent();
		}

		if (auto model = std::dynamic_pointer_cast<KEngine::Model>(object)) {
			ImGui::Text("Model:");
			ImGui::Indent();
			ImGui::Text("Path: %s", /* 获取模型路径 */ "references/backpack/backpack.obj");
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
	currentScene = sceneList[index];
	currentScene->Init();
}