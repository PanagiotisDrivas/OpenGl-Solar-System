#ifndef OBJECT_H
#define OBJECT_H

#include "../../src/glad.h"
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "Mesh.h"
//code from tutorial
class Model {
public:
	glm::vec3 lightColor = glm::vec3(1.0f);

    glm::vec3 ambient = glm::vec3(0.2f);
    glm::vec3 diffuse = glm::vec3(0.5f);
    glm::vec3 specular = glm::vec3(1.0f);

	glm::vec3 pos;
	glm::vec3 size;

	Model(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f));

	void init();
	void loadModel(std::string path);
	glm::vec3 render(Shader shader, int flag, glm::vec3 point = glm::vec3(0.0f), float  totalElapsedTime = 0.0f);

	void cleanup();
protected:
	std::vector<Mesh> meshes;
	std::string  directory;

	std::vector<Texture> textures_loaded;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);
};

#endif