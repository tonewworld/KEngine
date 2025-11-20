#pragma once
#include "Core.h"
#include "Cube.h"
#include "Sphere.h"
#include "Light.h"
namespace KEngine {

	class KE_API ObjectFactory {
	public:
		

		static std::shared_ptr<Cube> CreateCube(const std::string& name = "Cube");
		static std::shared_ptr<PointLight> CreatePointLight(const std::string& name = "Light");
		static std::shared_ptr<ParallelLight> CreateParallelLight(const std::string& name = "Light");
		static std::shared_ptr<Sphere> CreateSphere(int sectors, int stacks, float radius,const std::string& name = "Sphere");

	private:
		
	};


}