#pragma once

#include<string>

#include "VAO.hpp"
#include "EBO.hpp"
#include "Camera.hpp"
#include "Texture.hpp"

class Mesh
{
public:
	std::vector <Vertex> vertices;
	std::vector <GLuint> indices;
	std::vector <Texture> textures;
	// VAO используется при отрисовке
	VAO meshVAO;

	Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

	// отрисовка
	void draw
	(
		Shader& shader, 
		Camera& camera,
		glm::mat4 matrix = glm::mat4(1.0F),
		glm::vec3 translation = glm::vec3(0.0F, 0.0F, 0.0F),
		glm::quat rotation = glm::quat(1.0F, 0.0F, 0.0F, 0.0F),
		glm::vec3 scale = glm::vec3(1.0F, 1.0F, 1.0F)
	);
};