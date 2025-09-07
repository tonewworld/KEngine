#include<KEngine.h>

class ExampleLayer : public KEngine::Layer {
	public:
	ExampleLayer()
		:Layer("Example") {
	}
	void OnUpdate() override {
		//KE_CLIENT_INFO("ExampleLayer::Update");这里的日志不知道为啥一直有bug
		
	}
	void OnEvent(KEngine::Event& event) override {
		//std::cout<<event.ToString();
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