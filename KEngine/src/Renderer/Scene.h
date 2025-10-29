#pragma once
#include "Core.h"
#include "Object.h"
#include "TimeStep.h"
#include "Light.h"
namespace KEngine {
	class KE_API Scene {
	
	public:
		Scene() = default;
		virtual ~Scene() = default;
		virtual std::string GetName() = 0;
		virtual std::vector<std::shared_ptr<KEngine::Object>> GetObjectsInScene() = 0;
		virtual std::vector<std::shared_ptr<KEngine::ParallelLight>> GetParallelLightInScene() = 0;
		virtual void OnUpdate(KEngine::TimeStep ts) = 0;
		virtual void Init() = 0;
		virtual void Destroy() = 0;
	};
}