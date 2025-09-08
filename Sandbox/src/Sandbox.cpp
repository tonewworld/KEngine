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
};


class Sandbox : public KEngine::Application {


public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		PushOverlayer(new KEngine::ImGuiLayer());
	}
	~Sandbox() {
	}

};



KEngine::Application* KEngine::CreateApplication() {
	return new Sandbox();
}