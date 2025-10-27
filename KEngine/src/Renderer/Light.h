#pragma once
#include "Core.h"
#include "Object.h"
#include "VertexArray.h"
namespace KEngine {
	struct LightAttri {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	class KE_API Light :public Object {
	public:
		Light(float* m_Vertices, unsigned int vertexCount,
			BufferLayout& layout,
			unsigned int* m_Indexes, unsigned int indexCount,
			const std::string& name = "Mesh");
		virtual ~Light() = default;

		std::shared_ptr<VertexArray> VAO;
		std::shared_ptr<VertexBuffer> VBO;
		std::shared_ptr<IndexBuffer> IBO;

		void SetLightAttributes(const LightAttri& attri) { m_LightAttri = attri; }
		LightAttri& GetLightAttributes() { return m_LightAttri; }
	private:

		BufferLayout m_layout;
		LightAttri m_LightAttri;


	};
}