#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices,
           std::vector<Texture>& textures) {
    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::textures = textures;

    meshVAO.bind();
    VBO meshVBO(vertices);
    EBO meshEBO(indices);
    meshVAO.linkAttrib(meshVBO, 0, 3, GL_FLOAT, sizeof(Vertex),
                       (void*)0);  // NOLINT
    meshVAO.linkAttrib(meshVBO, 1, 3, GL_FLOAT, sizeof(Vertex),
                       (void*)(3 * sizeof(float)));  // NOLINT
    meshVAO.linkAttrib(meshVBO, 2, 3, GL_FLOAT, sizeof(Vertex),
                       (void*)(6 * sizeof(float)));  // NOLINT
    meshVAO.linkAttrib(meshVBO, 3, 2, GL_FLOAT, sizeof(Vertex),
                       (void*)(9 * sizeof(float)));  // NOLINT
    // отвязка, чтобы не изменить
    meshVAO.unbind();
    meshVBO.unbind();
    meshEBO.unbind();
}

void Mesh::draw(Shader& shader, Camera& camera, glm::mat4 matrix,
                glm::vec3 translation, glm::quat rotation, glm::vec3 scale) {
    shader.use();
    meshVAO.bind();

    // как много текстур мы используем
    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;

    for (unsigned int i = 0; i < textures.size(); i++) {
        std::string num;
        std::string type = textures[i].type;
        if (type == "diffuse") {
            num = std::to_string(numDiffuse++);
        } else if (type == "specular") {
            num = std::to_string(numSpecular++);
        }
        textures[i].texUnit(shader, (type + num).c_str(), i);
        textures[i].bind();
    }
    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.position.x,
                camera.position.y, camera.position.z);
    camera.exportMatrix(shader, "camMatrix");

    glm::mat4 trans = glm::mat4(1.0F);
    glm::mat4 rot = glm::mat4(1.0F);
    glm::mat4 sca = glm::mat4(1.0F);

    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);

    // добавить матрицы в шейдер
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1,
                       GL_FALSE, glm::value_ptr(trans));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE,
                       glm::value_ptr(rot));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE,
                       glm::value_ptr(sca));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE,
                       glm::value_ptr(matrix));

    // нарисовать mesh
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                   GL_UNSIGNED_INT, nullptr);
}