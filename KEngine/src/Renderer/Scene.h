#pragma once
#include "Core.h"
#include "Object.h"
#include "TimeStep.h"
#include "Light.h"
#include "Camera.h"
namespace KEngine {
	class KE_API Scene {
	
	public:
		bool isInited = false;
		Scene() = default;
		virtual ~Scene() = default;
		virtual std::string GetName() = 0;
		
		virtual std::shared_ptr<KEngine::Camera> GetMainCamera() = 0;
		virtual std::vector<std::shared_ptr<KEngine::Object>> GetObjectsInScene() = 0;
		virtual std::vector<std::shared_ptr<KEngine::PointLight>>GetPointLightInScene() = 0;
		virtual std::vector<std::shared_ptr<KEngine::ParallelLight>> GetParallelLightInScene() = 0;

		virtual void OnUpdate(KEngine::TimeStep ts) = 0;
		virtual void Init() = 0;
		virtual void Destroy() = 0;
	};
}