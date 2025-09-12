#include<KEngine.h>

class ExampleLayer : public KEngine::Layer {
	public:
	ExampleLayer()
		:Layer("Example") {
	}
	void OnUpdate() override {
	}
	void OnEvent(KEngine::Event& event) override {
	}
	void ImGuiRender()override{
		
		//ImGui::Begin("TestWindow");
		//ImGui::Text("111");
		//ImGui::End();
	}
};


class Sandbox : public KEngine::Application {


public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}
	~Sandbox() {
	}

};



KEngine::Application* KEngine::CreateApplication() {
	return new Sandbox();
}