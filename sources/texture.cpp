#include "texture.hpp"

Texture::Texture(const std::string& image, const char* textureType, GLuint slot) {

    type = textureType;

    // ширина, высота и количество цветовых каналов
    int widthImg;
    int heightImg;
    int nrChannels;
    // чтобы картинка была не перевернута
    stbi_set_flip_vertically_on_load(1);
    // картинка в байтах
    unsigned char* bytesImg =
        stbi_load(image.c_str(), &widthImg, &heightImg, &nrChannels, 0);

    // генерация текстурного объекта OpenGl
    glGenTextures(1, &ID);
    glActiveTexture(GL_TEXTURE0 + slot);
    unit = slot;
    glBindTexture(GL_TEXTURE_2D, ID);

    // чтобы была возможность увеличивать и уменьшать изображение
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // в зависимости от цветового канала используем разные загрузки
    if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, bytesImg);
    } else if (nrChannels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, bytesImg);
    } else if (nrChannels == 1) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RED,
                     GL_UNSIGNED_BYTE, bytesImg);
    } else {
        throw std::invalid_argument(
            "Automatic Texture type recognition failed");
    }

    // очень важно сгенерировать мип карту
    glGenerateMipmap(GL_TEXTURE_2D);

    // удаляем битовое значение, так как изображение уже в текстуре
    stbi_image_free(bytesImg);

    // отвязка, чтобы случайно не изменить
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit) {
    // получить местоположение униформы
    GLuint texUni = glGetUniformLocation(shader.ID, uniform);
    // активирем шейдеры до установки в него в текстуры
    shader.use();
    glUniform1i(static_cast<GLint>(texUni), static_cast<GLint>(unit));
}

void Texture::bind() {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::remove() { glDeleteTextures(1, &ID); }