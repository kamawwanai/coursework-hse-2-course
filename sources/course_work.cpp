#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <type_traits>
#include <array>

#include "shader.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// настройки
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

auto main() -> int
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // создаем окно glfw
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Coursework", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // загружаем все указатели на функции OpenGL
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0)
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ШЕЙДЕРЫ
    // ----------
    // создание и компиляция шейдеров
    Shader ourShader("../static/shaders/shader.vs", "../static/shaders/shader.fs");

    //вершины треугольника
    std::array<float,18> vertices = {
        // координаты (x,y,z)          //цвета (r,g,b)
        -0.5F, -0.5F, 0.0F,  1.0F, 0.0F, 0.0F, // правая точка 
         0.5F, -0.5F, 0.0F,  0.0F, 1.0F, 0.0F, // левая точка
         0.0F,  0.5F, 0.0F,  0.0F, 0.0F, 1.0F // вершина 
    }; 

    unsigned int VBO;
    unsigned int VAO;
    // VAO - Vertex Arrays Object
    glGenVertexArrays(1, &VAO);
    // VBO - Vertex Buffer Object позволяет отрендерить видеоустройством
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    // Настройка атрибутов вершин
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Настройка атрибутов цветов
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ЦИКЛ РЕНДЕРИНГА
    // цикл нам нужен для того, чтобы окно не закрылось без пользователя
    // -----------
    while (glfwWindowShouldClose(window) == 0)
    {
        // input

        processInput(window);
        

        // РЕНДЕРИНГ
        glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);

        // РИСУЕМ ТРЕУГОЛЬНИК

        ourShader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // ОБМЕН БУФФЕРОВ И СПРАШИВАЕМ ПОЛЬЗОВАТЕЛЯ О ВВОДЕ-ВЫВОДЕ (кнопки, мышка и т. д.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // освобождаем ресурсы
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(ourShader.ID);

    // ЗАВЕРШЕНИЕ РАБОТЫ ПРОГРАММЫ
    // НЕ ЗАБЫВАЕМ ОЧИСТИТЬ ВСЕ ВЫДЕЛЕННЫЕ РЕСУРСЫ GLFW
    glfwTerminate();
    return 0;
}

// ОБРАБОТКА ВХОДНЫХ ДАННЫХ ОТ ПОЛЬЗОВАТЕЛЯ
void processInput(GLFWwindow *window)
{
    // кнопка закрытия окна
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

// КОГДА МЕНЯЕТСЯ РАЗМЕР ОКНА (ОС ИЛИ ПОЛЬЗОВАТЕЛЕМ) ВЫЗЫВАЕТСЯ ЭТА ФУНКЦИЯ
void framebuffer_size_callback([[maybe_unused]]GLFWwindow* window, int width, int height)
{
    // окно просмотра должно соответствовать размерам нового окна
    glViewport(0, 0, width, height);
}