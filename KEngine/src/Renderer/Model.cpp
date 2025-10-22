#include "kepch.h"	
#include "Model.h"
#include "SOIL2/SOIL2.h"
namespace KEngine {
    Model::Model(const std::string& path,const std::string& name) :Object(name)
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
        size_t lastSlash = path.find_last_of("/\\");
        directory = (lastSlash != std::string::npos)
            ? path.substr(0, lastSlash)
            : ".";
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
    //缓存
    static std::unordered_map<std::string, std::weak_ptr<Texture2D>> sTextureCache;

	Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
	{
        KEngine::BufferLayout layout = {
            { KEngine::ShaderDataType::Float3, "Position" },
            { KEngine::ShaderDataType::Float3, "Normal"   },
            { KEngine::ShaderDataType::Float2, "TexCoord" }
        };
        const unsigned floatPerVertex = layout.GetStride() / 4; 

        std::vector<float> vertices;
        vertices.reserve(mesh->mNumVertices * floatPerVertex);

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

        std::vector<unsigned int> indices;
        indices.reserve(mesh->mNumFaces * 3);
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
            for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
                indices.push_back(mesh->mFaces[i].mIndices[j]);

        std::shared_ptr<Texture2D> diffuseTex = nullptr;
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
            aiString str;
            if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0 &&
                mat->GetTexture(aiTextureType_DIFFUSE, 0, &str) == AI_SUCCESS)
            {
                std::string fileName = str.C_Str();
                size_t pos = fileName.find_last_of("/\\");
                if (pos != std::string::npos)
                    fileName = fileName.substr(pos + 1);

                auto it = sTextureCache.find(fileName);
                if (it != sTextureCache.end() && !it->second.expired())
                {
                    diffuseTex = it->second.lock();   
                }
                else
                {      
                    std::string fullPath = directory + '/' + fileName;
                    diffuseTex.reset(Texture2D::Create(fullPath));
                    sTextureCache[fileName] = diffuseTex; 
                }
            }
        }

		this->texture = diffuseTex;
        return Mesh(vertices.data(),
            (unsigned int)vertices.size(),
            layout,
            indices.data(),
            (unsigned int)indices.size());
	}

   
	
}

