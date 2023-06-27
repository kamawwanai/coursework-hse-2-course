#include "model.hpp"
#include <cstdint>
#include "GLFW/glfw3.h"
#include "glm/fwd.hpp"

Model::Model(const std::string& fileGLTF) {
    std::string text = get_file_contents(fileGLTF);
    JSON = json::parse(text);

    Model::fileGLTF = fileGLTF.c_str();
    binData = getData();
    nodeTraversal(0);
}

void Model::draw(Shader& shader, Camera& camera) {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].Mesh::draw(shader, camera, matricesMeshes[i]);
    }
}

void Model::loadMesh(unsigned int indMesh) {
    unsigned int posAccInd =
        JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
    unsigned int normalAccInd =
        JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
    unsigned int texAccInd =
        JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
    unsigned int indAccInd =
        JSON["meshes"][indMesh]["primitives"][0]["indices"];

    std::vector<float> posVec = getFloats(JSON["accessors"][posAccInd]);
    std::vector<glm::vec3> positions = groupFloatsVec3(posVec);
    std::vector<float> normalVec = getFloats(JSON["accessors"][normalAccInd]);
    std::vector<glm::vec3> normals = groupFloatsVec3(normalVec);
    std::vector<float> texVec = getFloats(JSON["accessors"][texAccInd]);
    std::vector<glm::vec2> texUVs = groupFloatsVec2(texVec);

    std::vector<Vertex> vertices =
        assemblingVertices(positions, normals, texUVs);
    std::vector<GLuint> indices = getIndices(JSON["accessors"][indAccInd]);
    std::vector<Texture> textures = getTextures();

    // комбинируем вершины, индексы и текстуры в mesh
    meshes.emplace_back(vertices, indices, textures);
}

void Model::nodeTraversal(unsigned int nextNode, glm::mat4 matrix) {
    // текущий узел
    json node = JSON["nodes"][nextNode];
    glm::vec3 translation = glm::vec3(0.0F, 0.0F, 0.0F);
    if (node.find("translation") != node.end()) {
        float transValues[3];  // NOLINT
        for (unsigned int i = 0; i < node["translation"].size(); i++) {
            transValues[i] = (node["translation"][i]);
        }
        translation = glm::make_vec3(transValues);
    }
    glm::quat rotation = glm::quat(1.0F, 0.0F, 0.0F, 0.0F);
    if (node.find("rotation") != node.end()) {
        float rotValues[4] = {node["rotation"][3],  // NOLINT
                              node["rotation"][0], node["rotation"][1],
                              node["rotation"][2]};
        rotation = glm::make_quat(rotValues);
    }
    glm::vec3 scale = glm::vec3(1.0F, 1.0F, 1.0F);
    if (node.find("scale") != node.end()) {
        float scaleValues[3];  // NOLINT
        for (unsigned int i = 0; i < node["scale"].size(); i++) {
            scaleValues[i] = (node["scale"][i]);
        }
        scale = glm::make_vec3(scaleValues);
    }
    glm::mat4 matNode = glm::mat4(1.0F);
    if (node.find("matrix") != node.end()) {
        float matValues[16];  // NOLINT
        for (unsigned int i = 0; i < node["matrix"].size(); i++) {
            matValues[i] = (node["matrix"][i]);
        }
        matNode = glm::make_mat4(matValues);
    }

    glm::mat4 trans = glm::mat4(1.0F);
    glm::mat4 rot = glm::mat4(1.0F);
    glm::mat4 sca = glm::mat4(1.0F);

    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);

    glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

    // проверка содержит ли узел mesh
    if (node.find("mesh") != node.end()) {
        translationsMeshes.push_back(translation);
        rotationsMeshes.push_back(rotation);
        scalesMeshes.push_back(scale);
        matricesMeshes.push_back(matNextNode);

        loadMesh(node["mesh"]);
    }

    // есть ли дочерние узлы
    if (node.find("children") != node.end()) {
        for (unsigned int i = 0; i < node["children"].size(); i++) {  // NOLINT
            nodeTraversal(node["children"][i], matNextNode);
        }
    }
}

auto Model::getData() -> std::vector<unsigned char> {
    std::string bytesText;
    std::string uri = JSON["buffers"][0]["uri"];

    std::string fileStr = std::string(fileGLTF);
    std::string fileDirectory =
        fileStr.substr(0, fileStr.find_last_of('/') + 1);
    bytesText = get_file_contents(fileDirectory + uri);

    // преобразовать необработанные текстовые данные в байты и поместите их в
    // вектор
    std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
    return data;
}

auto Model::getFloats(json accessor) -> std::vector<float> {
    std::vector<float> floatVec;

    unsigned int buffViewInd = accessor.value("bufferView", 1);
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    std::string type = accessor["type"];

    json bufferView = JSON["bufferViews"][buffViewInd];
    unsigned int byteOffset = bufferView["byteOffset"];

    unsigned int numPerVert;
    if (type == "SCALAR") {
        numPerVert = 1;
    } else if (type == "VEC2") {
        numPerVert = 2;
    } else if (type == "VEC3") {
        numPerVert = 3;
    } else if (type == "VEC4") {
        numPerVert = 4;
    } else {
        throw std::invalid_argument(
            "Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");
    }

    // Переберать все байты в данных в нужном месте
    unsigned int beginningOfData = byteOffset + accByteOffset;
    unsigned int lengthOfData = count * 4 * numPerVert;
    for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData;
         i) {                                                 // NOLINT
        unsigned char bytes[] = {binData[i++], binData[i++],  // NOLINT
                                 binData[i++], binData[i++]};
        float value;
        std::memcpy(&value, bytes, sizeof(float));
        floatVec.push_back(value);
    }

    return floatVec;
}

auto Model::getIndices(json accessor) -> std::vector<GLuint> {
    std::vector<GLuint> indices;

    unsigned int buffViewInd = accessor.value("bufferView", 0);
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    unsigned int componentType = accessor["componentType"];

    json bufferView = JSON["bufferViews"][buffViewInd];
    unsigned int byteOffset = bufferView.value("byteOffset", 0);
    std::cout << byteOffset << std::endl;

    unsigned int beginningOfData = byteOffset + accByteOffset;
    if (componentType == 5125) {
        for (unsigned int i = beginningOfData;
             i < byteOffset + accByteOffset + count * 4; i) {     // NOLINT
            unsigned char bytes[] = {binData[i++], binData[i++],  // NOLINT
                                     binData[i++], binData[i++]};
            unsigned int value;
            std::memcpy(&value, bytes, sizeof(unsigned int));
            indices.push_back(static_cast<GLuint>(value));
        }
    } else if (componentType == 5123) {
        for (unsigned int i = beginningOfData;
             i < byteOffset + accByteOffset + count * 2; i) {      // NOLINT
            unsigned char bytes[] = {binData[i++], binData[i++]};  // NOLINT
            unsigned short value;                                  // NOLINT
            std::memcpy(&value, bytes, sizeof(unsigned short));    // NOLINT
            indices.push_back(static_cast<GLuint>(value));
        }
    } else if (componentType == 5122) {
        for (unsigned int i = beginningOfData;
             i < byteOffset + accByteOffset + count * 2; i) {      // NOLINT
            unsigned char bytes[] = {binData[i++], binData[i++]};  // NOLINT
            short value;                                           // NOLINT
            std::memcpy(&value, bytes, sizeof(short));             // NOLINT
            indices.push_back(static_cast<GLuint>(value));
        }
    }

    return indices;
}

auto Model::getTextures() -> std::vector<Texture> {
    std::vector<Texture> textures;

    std::string fileStr = std::string(fileGLTF);
    std::string fileDirectory =
        fileStr.substr(0, fileStr.find_last_of('/') + 1);

    // Переходим к каждой картинке
    for (unsigned int i = 0; i < JSON["images"].size(); i++) {
        // uri текущей текстуры
        std::string texPath = JSON["images"][i]["uri"];

        // проверка если текстура уже загружена
        bool skip = false;
        for (unsigned int j = 0; j < loadedTextureNames.size(); j++) {
            if (loadedTextureNames[j] == texPath) {
                textures.push_back(loadedTextures[j]);
                skip = true;
                break;
            }
        }

        // если текстура уже загружена, то скипаем
        if (!skip) {
            // Load diffuse texture
            if (texPath.find("airrplane_baseColor") != std::string::npos ||
                texPath.find("diffuse") != std::string::npos) {
                Texture diffuse = Texture((fileDirectory + texPath), "diffuse",
                                          loadedTextures.size());
                textures.push_back(diffuse);
                loadedTextures.push_back(diffuse);
                loadedTextureNames.push_back(texPath);
            }
            // Load specular texture
            else if (texPath.find("airplane_normal") != std::string::npos ||
                     texPath.find("specular") != std::string::npos) {
                Texture specular = Texture((fileDirectory + texPath),
                                           "specular", loadedTextures.size());
                textures.push_back(specular);
                loadedTextures.push_back(specular);
                loadedTextureNames.push_back(texPath);
            }
        }
    }

    return textures;
}

auto Model::assemblingVertices(
    const std::vector<glm::vec3>& positions,  // NOLINT
    const std::vector<glm::vec3>& normals,
    const std::vector<glm::vec2>& texCoorsTo3d) -> std::vector<Vertex> {
    std::vector<Vertex> vertices;
    for (int i = 0; i < static_cast<int>(positions.size()); i++) {
        vertices.push_back(Vertex{positions[i], normals[i],  // NOLINT
                                  glm::vec3(1.0F, 1.0F, 1.0F),
                                  texCoorsTo3d[i]});
    }
    return vertices;
}

auto Model::groupFloatsVec2(std::vector<float> floatVec)
    -> std::vector<glm::vec2> {
    std::vector<glm::vec2> vectors;
    for (int i = 0; i < static_cast<int>(floatVec.size());) {
        vectors.emplace_back(floatVec[i++], floatVec[i++]);  // NOLINT
    }
    return vectors;
}
auto Model::groupFloatsVec3(std::vector<float> floatVec)
    -> std::vector<glm::vec3> {
    std::vector<glm::vec3> vectors;
    for (int i = 0; i < static_cast<int>(floatVec.size());) {
        vectors.emplace_back(floatVec[i++], floatVec[i++],  // NOLINT
                             floatVec[i++]);
    }
    return vectors;
}
auto Model::groupFloatsVec4(std::vector<float> floatVec)
    -> std::vector<glm::vec4> {
    std::vector<glm::vec4> vectors;
    for (int i = 0; i < static_cast<int>(floatVec.size());) {
        vectors.emplace_back(floatVec[i++], floatVec[i++],  // NOLINT
                             floatVec[i++], floatVec[i++]);
    }
    return vectors;
}

void Model::changePos(float dt) {
    const float sinPsi = sin(rotation.z);

    // Вычисляем ускорения, действующие на модель
    glm::vec3 acceleration{0.0F, 0.0F, 0.0F};
    acceleration.x = T / m;
    acceleration.y = (Fy - m * g) / m;
    acceleration.z = (Mz - l * m * g * sinPsi) / Iz;

    // Обновляем скорость и позицию модели
    velocity += acceleration * dt;
    position += velocity * dt;

    // Обновляем углы поворота модели
    glm::vec3 angularAcceleration{0.0F, 0.0F, 0.0F};
    angularAcceleration.z =
        (-rotation.z * velocity.y + rotation.y * velocity.z) / Iy;

    rotation += angularAcceleration * dt;

    // Обновляем матрицу трансформации модели
    glm::mat4 modelMatrix{1.0F};
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = modelMatrix * glm::toMat4(glm::quat(rotation));

    // Обновляем матрицы трансформации всех мешей
    for (size_t i = 0; i < meshes.size(); ++i) {
        matricesMeshes[i] =
            modelMatrix *
            glm::translate(glm::mat4(1.0F), translationsMeshes[i]) *
            glm::toMat4(rotationsMeshes[i]) *
            glm::scale(glm::mat4(1.0F), scalesMeshes[i]);
    }
}

void Model::userInput(GLFWwindow* window) {
    const float speed = 0.1F;

    // Проверяем, нажата ли клавиша W
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        Fy += speed;
    }

    // Проверяем, нажата ли клавиша S
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        Fy -= speed;
    }

    // Проверяем, нажата ли клавиша D
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        Mz -= speed;
    }

    // Проверяем, нажата ли клавиша A
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Mz += speed;
    }

    // Проверяем, нажата ли клавиша Q
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        rotation.y += speed;
    }

    // Проверяем, нажата ли клавиша E
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        rotation.y -= speed;
    }

    // Проверяем, нажата ли клавиша Пробел
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        position.y += 5.0F;
    }

    // Ограничиваем значения Fy и Mz
    const float max_fy = 5000.0F;
    const float max_mz = 1000.0F;
    Fy = glm::clamp(Fy, -max_fy, max_fy);
    Mz = glm::clamp(Mz, -max_mz, max_mz);
}