#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

// структура стандатизирует вершины в mesh
struct Vertex {
    // позиция вершины
    glm::vec3 position;
    // нормаль
    glm::vec3 normal;
    // цвет
    glm::vec3 color;
    // текстура
    glm::vec2 texUV;
};

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