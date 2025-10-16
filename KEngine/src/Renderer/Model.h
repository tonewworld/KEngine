#pragma once
#include "Core.h"
#include "Mesh.h"
#include "Texture.h"
#include "Object.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace KEngine
{
	class KE_API Model :public Object{
	public:
		Model(const std::string& path);
		~Model();
		std::vector<Mesh> meshes;
		std::shared_ptr<Texture2D> texture;	
	private:
		std::string directory;
		/*  私有成员函数   */
		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	};
}