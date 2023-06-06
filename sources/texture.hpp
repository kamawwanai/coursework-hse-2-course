#pragma once

#include <glad/glad.h>
#include <stb_image.h>

#include "shader.hpp"

class Texture
{
public:
	GLuint ID;
	std::string type;
	GLuint unit;

	Texture(const std::string& image, const char* textureType, GLuint slot);

	// присвоить текстуре единицу текстуры
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// привязка
	void bind();
	// отвязка
	void unbind();
	// удаление
	void remove();
};