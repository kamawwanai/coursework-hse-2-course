#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "vertex.hpp"

class VBO {
   public:
    GLuint ID;
    // конструктор генерирует буффер
    explicit VBO(std::vector<Vertex>& vertices);

    // привзяка
    void bind();
    // отвязка
    void unbind();
    // удаление
    void remove();
};