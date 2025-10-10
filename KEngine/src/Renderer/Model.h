#pragma once
#include "Core.h"
#include "Mesh.h"
#include "Texture.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace KEngine
{
	class KE_API Model {
	public:
		Model(const std::string& path);
		~Model();
		std::vector<Mesh> meshes;
	private:
		std::string directory;

		/*  私有成员函数   */
		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture3D> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	};
}