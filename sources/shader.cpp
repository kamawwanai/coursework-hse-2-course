#include "shader.hpp"

// получить все строки из файла
auto get_file_contents(const std::string& file) -> std::string {
    std::ifstream in(file, std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], static_cast<std::streamsize>(contents.size()));
        in.close();
        return (contents);
    }
    throw(errno);
}

void compileErrors(unsigned int shader, const std::string& type);

Shader::Shader(const char* vertexShaderFile,  // NOLINT
               const char* fragmentShaderFile) {
    std::string vertexCode = get_file_contents(vertexShaderFile);
    std::string fragmentCode = get_file_contents(fragmentShaderFile);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    // создание вершинного шейдерного объекта
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    // проверка на ошибки
    compileErrors(vertexShader, "VERTEX");

    // создание фрагментного шейдерного объекта
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    // проверка на ошибки
    compileErrors(fragmentShader, "FRAGMENT");

    // создание шейдерной программы
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    // проверка на ошибки
    compileErrors(ID, "PROGRAM");

    // так как мы создали шейдерную программу
	// объекты нам больше не нужны
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() { glUseProgram(ID); }

void Shader::remove() { glDeleteProgram(ID); }

void compileErrors(unsigned int shader, const std::string& type) {
    GLint hasCompiled;
    char infoLog[1024];  // NOLINT
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "SHADER_COMPILATION_ERROR " << type << "\n"
                      << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "SHADER_LINKING_ERROR " << type << "\n"
                      << infoLog << std::endl;
        }
    }
}