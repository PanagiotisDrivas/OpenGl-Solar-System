#include "Mesh.h"
//code from tutorial
std::vector<Vertex> Vertex::genList(float* vertices, int noVertices) {
	std::vector<Vertex> ret(noVertices);

	int stride = sizeof(Vertex) / sizeof(float);

	for (int i = 0; i < noVertices; i++) {
		ret[i].pos = glm::vec3(
			vertices[i * stride + 0],
			vertices[i * stride + 1],
			vertices[i * stride + 2]
		);
		ret[i].normal = glm::vec3(
			vertices[i * stride + 3],
			vertices[i * stride + 4],
			vertices[i * stride + 5]
		);
		ret[i].texCoord = glm::vec2(
			vertices[i * stride + 6],
			vertices[i * stride + 7]
		);
	}

	return ret;
}

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	: vertices(vertices), indices(indices), textures(textures) {
	setup();
}
// responsible for rendering the mesh using a specified shader
// iterates over the textures associated with the mesh, activates each texture, sets the shader value, and binds the texture
// binds the vertex array object (VAO) and draws the mesh
void Mesh::render(Shader shader) {
	// textures
	for (unsigned int i = 0; i < textures.size(); i++) {
			// textures
			unsigned int diffuseIdx = 0;
			unsigned int specularIdx = 0;
			// activate texture
			glActiveTexture(GL_TEXTURE0 + i);
			//glBindTexture(GL_TEXTURE_2D, textures[i].id);
			// retrieve texture info
			std::string name;
			switch (textures[i].type) {
			case aiTextureType_DIFFUSE:
				name = "diffuse" + std::to_string(diffuseIdx++);
				break;
			case aiTextureType_SPECULAR:
				name = "specular" + std::to_string(specularIdx++);
				break;
			}

			// set the shader value
			shader.setInt(name, i);
			// bind texture
			textures[i].bind();
		}
	// glBindTexture(GL_TEXTURE_2D, textures[i].id);
    //     glActiveTexture(GL_TEXTURE0 + i);
    //     textures[i].bind();
	
	// VAO stuff
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// reset
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::cleanup() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}
// generates the VAO, VBO, and EBO, binds the VAO, loads vertex and index data into the VBO and EBO
void Mesh::setup() {
	// create buffers/arrays
	glGenVertexArrays(1, &VAO); // vertex array object
	glGenBuffers(1, &VBO); // vertex buffer object
	glGenBuffers(1, &EBO); // element buffer object

	glBindVertexArray(VAO);

	// load data into VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// load data into EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set vertex attribute pointers
	// vertex.position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	
	//vertex.normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);
	
	// vertex.texCoord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoord)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}