#pragma once


#ifdef  KENGINE_PLATFORM_WINDOWS

extern KEngine::Application* KEngine::CreateApplication();

int main(int argc,char** argv) {
	
	KEngine::Log::Init();
	
	auto app = KEngine::CreateApplication();
	app->Run();
	delete app;

	return 0;
} 

#endif 

 