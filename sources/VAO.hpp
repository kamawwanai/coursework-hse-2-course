#pragma once
#include <glad/glad.h>
#include "VBO.hpp"

class VAO {
public:
	GLuint ID;
	// конструктор гнерирует VAO id
	VAO();

	// связка атрибутов с VBO
	void linkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	// привязка
	void bind();
	// отвязка
	void unbind();
	// удаление
	void remove();
};