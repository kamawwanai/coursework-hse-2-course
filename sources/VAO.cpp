#include "VAO.hpp"

VAO::VAO() { glGenVertexArrays(1, &ID); }

void VAO::linkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type,
                     GLsizeiptr stride, void* offset) {
    VBO.bind();
    glVertexAttribPointer(layout, static_cast<GLint>(numComponents), type,
                          GL_FALSE, static_cast<GLsizei>(stride), offset);
    glEnableVertexAttribArray(layout);
    VBO.unbind();
}

void VAO::bind() { glBindVertexArray(ID); }

void VAO::unbind() { glBindVertexArray(0); }

void VAO::remove() { glDeleteVertexArrays(1, &ID); }