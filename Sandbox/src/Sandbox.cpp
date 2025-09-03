#include<KEngine.h>


class Sandbox : public KEngine::Application {


public:
	Sandbox() {
	}
	~Sandbox() {
	}

};



KEngine::Application* KEngine::CreateApplication() {
	return new Sandbox();
}