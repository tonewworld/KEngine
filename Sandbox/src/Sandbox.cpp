#include "KEngine.h"
#include "RendererLayer.h"	
#include "EntryPoint.h"
class Sandbox : public KEngine::Application {


public:
	Sandbox() {
		PushLayer(new RendererLayer());
		
	}
	~Sandbox() {
	}


};



KEngine::Application* KEngine::CreateApplication() {
	return new Sandbox();
}

