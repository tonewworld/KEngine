#include<KEngine.h>

class ExampleLayer : public KEngine::Layer {
	public:
	ExampleLayer()
		:Layer("Example") {
	}
	void OnUpdate() override {
		//KE_CLIENT_INFO("ExampleLayer::Update");�������־��֪��Ϊɶһֱ��bug
		
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