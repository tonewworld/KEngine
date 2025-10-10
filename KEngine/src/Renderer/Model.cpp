#include "kepch.h"	
#include "Model.h"
namespace KEngine {
	Model::Model(const std::string& path)
	{
        this->loadModel(path);
	}

	Model::~Model()
	{
	}

	void Model::loadModel(std::string path)
	{
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			return;
		}
		this->directory = path.substr(0, path.find_last_of('/'));

		this->processNode(scene->mRootNode, scene);
	}

	void Model::processNode(aiNode* node, const aiScene* scene)
	{
		// 添加当前节点中的所有Mesh
		for (GLuint i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			this->meshes.push_back(this->processMesh(mesh, scene));
		}
		// 递归处理该节点的子孙节点
		for (GLuint i = 0; i < node->mNumChildren; i++)
		{
			this->processNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
	{
        KEngine::BufferLayout layout = {
            { KEngine::ShaderDataType::Float3, "Position" },
            { KEngine::ShaderDataType::Float3, "Normal"   },
            { KEngine::ShaderDataType::Float2, "TexCoord" }
        };
        const unsigned floatPerVertex = layout.GetStride() / 4; 

        // 2. 一次性分配好 float 数组
        std::vector<float> vertices;
        vertices.reserve(mesh->mNumVertices * floatPerVertex);

        // 3. 逐顶点写入
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            // position
            vertices.push_back(mesh->mVertices[i].x);
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);

            // normal
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);

            // texcoord（没有就填 0）
            if (mesh->mTextureCoords[0]) {
                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            }
            else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
        }

        // 4. 索引数组
        std::vector<unsigned int> indices;
        indices.reserve(mesh->mNumFaces * 3);
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
            for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
                indices.push_back(mesh->mFaces[i].mIndices[j]);

        
       
        //// Process materials
        //if (mesh->mMaterialIndex >= 0)
        //{
        //    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        //    // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
        //    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        //    // Same applies to other texture as the following list summarizes:
        //    // Diffuse: texture_diffuseN
        //    // Specular: texture_specularN
        //    // Normal: texture_normalN

        //    // 1. Diffuse maps
        //    std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        //    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        //    // 2. Specular maps
        //    std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        //    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        //}

        // Return a mesh object created from the extracted mesh data
       // 5. 直接构造 Mesh，不再出现 Vertex 结构
        return Mesh(vertices.data(),
            (unsigned int)vertices.size(),
            layout,
            indices.data(),
            (unsigned int)indices.size());
	}

	std::vector<Texture3D> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		return std::vector<Texture3D>();
	}
}
