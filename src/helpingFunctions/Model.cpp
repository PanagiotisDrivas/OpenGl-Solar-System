#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//code from tutorial
Model::Model(glm::vec3 pos, glm::vec3 size)
	:pos(pos), size(size){}

void Model::init() {}

glm::vec3 Model::render(Shader shader, int type, glm::vec3 point, float  totalElapsedTime) {
	//CHANGE THE VERTEX SHADER TO 
    glm::mat4 model = glm::mat4(1.0f);
	if(type == 3){//earth
		double xx = sin(totalElapsedTime * 0.55f)*15;
		double zz = cos(totalElapsedTime * 0.55f)*15;
		//model = glm::translate(model, point);
		model = glm::translate(model, glm::vec3(xx, 0.0f, zz));
		model = glm::scale(model, size);
		model = glm::rotate(model, glm::radians(totalElapsedTime*100), glm::vec3(0.0f, 1.0f, 0.0f));	

		pos = glm::vec3(xx, 0.0f, zz);
	}else if(type == 2){//moon
		double xx = sin(totalElapsedTime)*16;
		double zz = cos(totalElapsedTime)*16;

		model = glm::translate(model, point);
		model = glm::scale(model, size);
		model = glm::translate(model, glm::vec3(xx, 0.0f, zz));
		
		pos = glm::vec3(xx, 0.0f, zz);
	}else if(type == 1){//sun
		
		//model = glm::translate(model, glm::vec3(0.0f));
		model = glm::scale(model, size);
		model = glm::translate(model, glm::vec3(0.0f));
	}
	
    shader.setMat4("model", model);
    shader.setFloat("material.shininess", 0.5f);

    for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].render(shader);
	}
	if(type == 3){	
		return pos;
	}else{
		return glm::vec3(0.0f);
	} 
}

void Model::cleanup() {
    for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].cleanup();
	}
}

void Model::loadModel(std::string path) {
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Could not load model at " << path << std::endl << import.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of("/"));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// process all meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// process all child nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		// position
		vertex.pos = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);

		// normal vectors
		vertex.normal = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);

		// textures
		if (mesh->mTextureCoords[0]) {
			vertex.texCoord = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
		}
		else {
			vertex.texCoord = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// process material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// diffuse maps
		std::vector<Texture> diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// specular maps
		std::vector<Texture> specularMaps = loadTextures(material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadTextures(aiMaterial* mat, aiTextureType type) {
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);

		// prevent duplicate loading
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		
		if (!skip) {
			
			// not loaded yet
			Texture tex(directory, str.C_Str(), type);
			tex.load(false);
			textures.push_back(tex);
			textures_loaded.push_back(tex);
		}
	}

	return textures;
}