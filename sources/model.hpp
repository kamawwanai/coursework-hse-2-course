#pragma once

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

#include "mesh.hpp"

using json = nlohmann::json;

class Model {
   public:
    // загрузка модели
    explicit Model(const std::string& fileGLTF);

    void draw(Shader& shader, Camera& camera);

   private:
    const char* fileGLTF;
    std::vector<unsigned char> binData;
    json JSON;

    // все меши матриц трансформации
    std::vector<Mesh> meshes;
    std::vector<glm::vec3> translationsMeshes;
    std::vector<glm::quat> rotationsMeshes;
    std::vector<glm::vec3> scalesMeshes;
    std::vector<glm::mat4> matricesMeshes;

    // чтобы не загружать текстуру дважды
    std::vector<std::string> loadedTextureNames;
    std::vector<Texture> loadedTextures;

    void loadMesh(unsigned int indMesh);

    // Обходит узел рекурсивно, так что он, по сути, обходит все подключенные
    // узлы
    void nodeTraversal(unsigned int nextNode,
                       glm::mat4 matrix = glm::mat4(1.0F));

    // получить бинарник из файла
    auto getData() -> std::vector<unsigned char>;
    // интерпретация бинарника
    auto getFloats(json accessor) -> std::vector<float>;
    auto getIndices(json accessor) -> std::vector<GLuint>;
    auto getTextures() -> std::vector<Texture>;

    // сборка в вершины
    auto assemblingVertices(const std::vector<glm::vec3>& positions,
                            const std::vector<glm::vec3>& normals,
                            const std::vector<glm::vec2>& texCoorsTo3d)
        -> std::vector<Vertex>;

    // для сборки
    auto groupFloatsVec2(std::vector<float> floatVec) -> std::vector<glm::vec2>;
    auto groupFloatsVec3(std::vector<float> floatVec) -> std::vector<glm::vec3>;
    auto groupFloatsVec4(std::vector<float> floatVec) -> std::vector<glm::vec4>;
};