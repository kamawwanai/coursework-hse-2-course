#include <glm/glm.hpp>

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