#pragma once

#include <glad/glad.h>
#include <array>
#include <vector>

class EBO {
   public:
    GLuint ID;
    // генерирует EBO и привязывает к нему индексы
    explicit EBO(std::vector<GLuint>& indices);

    // привязка
    void bind();
    // отвязка
    void unbind();
    // удаление
    void remove();
};