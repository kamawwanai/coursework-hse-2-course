#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include "glm/fwd.hpp"

class Shader{
public:
    // ID программы
    unsigned int ID;

    // конструктор
    Shader(const char* vertexPath, const char* fragmentPath){
        // извлечение исходного кода вершины/фрагмента из filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        // устанавливаем флаги исключений для отлова ошибок вводв-вывода файла
        // failbit - ошибка при чтении файла
        // badbit - серьезная внутренняя ошибка потока
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        // блок для обработки исключений
        try
        {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream;
            std::stringstream fShaderStream;
            // считываем все содержимое файла в строковый поток(stream)
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // закрываем файлы
            vShaderFile.close();
            fShaderFile.close();
            // конвертируем streamstring в string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        // отлавливаем исключения
        catch(std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        // компилируем шейдеры
        unsigned int vertex;
        unsigned int fragment;
        int success;
        std::array<char,512> infoLog;

        // вершинный шейдер
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        // проверка на ошибки компиляции
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(success == 0)
        {
            glGetShaderInfoLog(vertex, 512, nullptr, infoLog.data());
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << &infoLog << std::endl;
        };

        // фрагментный шейдер
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        // проверка на ошибки компиляции
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if(success == 0)
        {
            glGetShaderInfoLog(vertex, 512, nullptr, infoLog.data());
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << &infoLog << std::endl;
        };

        // программа Шейдера
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        // проверка на ошибки линковки
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(success == 0)
        {
            glGetProgramInfoLog(ID, 512, nullptr, infoLog.data());
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << &infoLog << std::endl;
        }
        // удаляем шейдеры здесь, так как они теперь связаны с самой программой
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // активация шейдеров
    void use(){
        glUseProgram(ID);
    }
    // задать uniform-переменную
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value)); 
    }
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }

    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
    }
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
    }
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
    }
    void setMat2(const std::string &name, const glm::mat2 &value) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
    }
    void setMat3(const std::string &name, const glm::mat3 &value) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
    }
    void setMat4(const std::string &name, const glm::mat4 &value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
    }
};