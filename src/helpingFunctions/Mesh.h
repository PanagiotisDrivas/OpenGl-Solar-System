#ifndef MESH_H
#define MESH_H

#include "../../src/glad.h"
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include "../../src/helpingFunctions/Shader.h"
#include "Texture.h"
//code from tutorial
struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal; // for lighting
	glm::vec2 texCoord;

	static std::vector<Vertex> genList(float* vertices, int noVertices);
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	void render(Shader shader);

	void cleanup();

private:
	unsigned int VAO, VBO, EBO;
	bool noTex;
	void setup();
};

#endif