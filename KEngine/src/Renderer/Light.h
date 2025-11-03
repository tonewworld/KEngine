#pragma once
#include "Core.h"
#include "Object.h"
#include "VertexArray.h"
namespace KEngine {
	struct KE_API PointLightAttri {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 color;
	};
	struct alignas(16) KE_API PointLightUboData {
		glm::vec3 Position; float _pad0;
		glm::vec3 Ambient;  float _pad1;
		glm::vec3 Diffuse;  float _pad2;
		glm::vec3 Specular; float _pad3;
		glm::vec3 Color;    float _pad4;
		PointLightUboData(glm::vec3 position,PointLightAttri pla)
			:Position(position), _pad0(0),
			Ambient(pla.ambient), _pad1(0),
			Diffuse(pla.diffuse), _pad2(0),
			Specular(pla.specular), _pad3(0),
			Color(pla.color),_pad4(0)
		{
		}
	};
	struct KE_API ParallelLightAttri {
		glm::vec3 direct;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 color;
	};
	struct alignas(16) KE_API ParallelLightUboData {
		glm::vec3 Direct; float _pad0;
		glm::vec3 Ambient;  float _pad1;
		glm::vec3 Diffuse;  float _pad2;
		glm::vec3 Specular; float _pad3;
		glm::vec3 Color;    float _pad4;
		ParallelLightUboData(ParallelLightAttri pla)
			:Direct(pla.direct), _pad0(0),
			Ambient(pla.ambient), _pad1(0),
			Diffuse(pla.diffuse), _pad2(0),
			Specular(pla.specular), _pad3(0),
			Color(pla.color), _pad4(0)
		{
		}
	};
	class KE_API Light :public Object {
	public:
		Light(float* m_Vertices, unsigned int vertexCount,
			BufferLayout layout,
			unsigned int* m_Indexes, unsigned int indexCount,
			const std::string& name = "Mesh");
		virtual ~Light() = default;

		std::shared_ptr<VertexArray> VAO;
		std::shared_ptr<VertexBuffer> VBO;
		std::shared_ptr<IndexBuffer> IBO;

	private:

		BufferLayout m_layout;


	};
	class KE_API PointLight :public Light {
	public:
		PointLight(float* m_Vertices, unsigned int vertexCount,
			BufferLayout layout,
			unsigned int* m_Indexes, unsigned int indexCount,
			const std::string& name = "Mesh")
			:Light(m_Vertices, vertexCount,
				layout,
				m_Indexes, indexCount,
				name) {
		}
		void SetLightAttributes(const PointLightAttri& attri) { m_LightAttri = attri; }
		PointLightAttri& GetLightAttributes() { return m_LightAttri; }
		std::array<glm::mat4,6> CalculateLightSpace();
	private:

		glm::mat4 lightProjection = glm::perspective(
			glm::radians(90.0f),  // 视野角度
			1.0f,                 // 宽高比 1:1
			1.0f,                 // 近平面
			25.0f                // 远平面
		);
		PointLightAttri m_LightAttri;

	};

	class KE_API ParallelLight :public Light {
	public:
		ParallelLight(float* m_Vertices, unsigned int vertexCount,
			BufferLayout layout,
			unsigned int* m_Indexes, unsigned int indexCount,
			const std::string& name = "Mesh")
			:Light(m_Vertices, vertexCount,
				layout,
				m_Indexes, indexCount,
				name) {
		}
		void SetLightAttributes(const ParallelLightAttri& attri) { m_LightAttri = attri; }
		ParallelLightAttri& GetLightAttributes() { return m_LightAttri; }
		glm::mat4 CalculateLightSpace();
	private:
		

		ParallelLightAttri m_LightAttri;
	};
};