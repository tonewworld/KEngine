#include "kepch.h"
#include "ObjectFactory.h"
namespace KEngine
{
	

	std::shared_ptr<Cube> ObjectFactory::CreateCube(const std::string& name)
	{
		float m_Vertices[] = {
			// px, py, pz,  nx, ny, nz,  u, v,  tx, ty, tz
			//位置                 //法向量              //uv          //切线
			 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,


			 -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			  0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			  0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			 -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			  0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			 -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,


			  0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
			  0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
			  0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
			  0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
			  0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
			  0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,


			  -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
			  -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
			  -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
			  -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
			  -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
			  -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,


			  -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
			   0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
			   0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
			  -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
			   0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
			  -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,


			  -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			   0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			   0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			  -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			   0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			  -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,  -1.0f, 0.0f, 0.0f
		};
		unsigned int m_Indices[]{
			0,1,2,
			3,4,5,
			6,7,8,
			9,10,11,
			12,13,14,
			15,16,17,
			18,19,20,
			21,22,23,
			24,25,26,
			27,28,29,
			30,31,32,
			33,34,35
		};

		KEngine::BufferLayout m_Layout = {
			{KEngine::ShaderDataType::Float3,"position"} ,
			{KEngine::ShaderDataType::Float3,"normal"},
			{KEngine::ShaderDataType::Float2, "v_TexCoord"},
			{KEngine::ShaderDataType::Float3, "v_Tangent"}
		};

		return std::shared_ptr<Cube>(new Cube(m_Vertices, sizeof(m_Vertices) / sizeof(float),
			m_Layout,
			m_Indices, sizeof(m_Indices) / sizeof(unsigned int),
			name));
	}

	std::shared_ptr<PointLight> ObjectFactory::CreatePointLight(const std::string& name)
	{
		float l_Vertices[] = {
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f
		};
		unsigned int l_Indices[]{
			0,1,2,
			3,4,5,
			6,7,8,
			9,10,11,
			12,13,14,
			15,16,17,
			18,19,20,
			21,22,23,
			24,25,26,
			27,28,29,
			30,31,32,
			33,34,35
		};

		KEngine::BufferLayout l_Layout = {
			{KEngine::ShaderDataType::Float3,"position"}
		};
		auto light = std::make_shared<PointLight>(l_Vertices, sizeof(l_Vertices) / sizeof(float),
			l_Layout,
			l_Indices, sizeof(l_Indices) / sizeof(unsigned int),
			name);
		light->UseIsLight() = true;
		return light;
	}

	std::shared_ptr<ParallelLight> ObjectFactory::CreateParallelLight(const std::string& name)
	{
		float l_Vertices[] = {
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f
		};
		unsigned int l_Indices[]{
			0,1,2,
			3,4,5,
			6,7,8,
			9,10,11,
			12,13,14,
			15,16,17,
			18,19,20,
			21,22,23,
			24,25,26,
			27,28,29,
			30,31,32,
			33,34,35
		};

		KEngine::BufferLayout l_Layout = {
			{KEngine::ShaderDataType::Float3,"position"}
		};
		auto light = std::make_shared<ParallelLight>(l_Vertices, sizeof(l_Vertices) / sizeof(float),
			l_Layout,
			l_Indices, sizeof(l_Indices) / sizeof(unsigned int),
			name);
		light->UseIsLight() = true; 
		return light;
	}

	std::shared_ptr<Sphere> ObjectFactory::CreateSphere(int sectors, int stacks, float radius,const std::string& name)
	{

		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		vertices.reserve((sectors + 1) * (stacks + 1) * 8);
		indices.reserve(sectors * stacks * 6);

		for (int i = 0; i <= stacks; ++i) {
			float v = float(i) / float(stacks);
			float phi = v * glm::pi<float>();
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			for (int j = 0; j <= sectors; ++j) {
				float u = float(j) / float(sectors);
				float theta = u * 2.0f * glm::pi<float>(); 
				float sinTheta = sin(theta);
				float cosTheta = cos(theta);

				glm::vec3 pos(
					radius * sinPhi * cosTheta,
					radius * cosPhi,
					radius * sinPhi * sinTheta
				);
				glm::vec3 norm = glm::normalize(pos);
				glm::vec2 uv(u, 1.0f - v);

				vertices.push_back(pos.x);
				vertices.push_back(pos.y);
				vertices.push_back(pos.z);

				vertices.push_back(norm.x);
				vertices.push_back(norm.y);
				vertices.push_back(norm.z);

				vertices.push_back(uv.x);
				vertices.push_back(uv.y);
			}
		}

		for (int i = 0; i < stacks; ++i) {
			int row1 = i * (sectors + 1);
			int row2 = (i + 1) * (sectors + 1);
			for (int j = 0; j < sectors; ++j) {
				indices.push_back(row1 + j);
				indices.push_back(row2 + j);
				indices.push_back(row1 + j + 1);

				indices.push_back(row1 + j + 1);
				indices.push_back(row2 + j);
				indices.push_back(row2 + j + 1);
			}
		}

		KEngine::BufferLayout layout = {
			{ KEngine::ShaderDataType::Float3, "v_Position" },
			{ KEngine::ShaderDataType::Float3, "v_Normal" },
			{ KEngine::ShaderDataType::Float2, "v_TexCoord" }
		};

		auto mesh = std::make_shared<KEngine::Sphere>(vertices.data(), vertices.size(), layout, indices.data(), indices.size(),name);
		return mesh;
	}
	
}
