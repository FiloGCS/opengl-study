#pragma once
#include "Shader.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
	Model(char* path)
	{
		loadModel(path);
	}
	void Draw(Shader &shader);

	//model data
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;

	//Model information functions
	unsigned int getVertexCount();
	unsigned int getMeshCount();
private:

	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	

};

