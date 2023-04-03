#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <array>
#include <iostream>
#include <type_traits>

#include "shader.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// настройки
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

auto main() -> int {
  // glfw: initialize and configure
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // создаем окно glfw
  GLFWwindow* window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Coursework", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // загружаем все указатели на функции OpenGL
  if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) ==
      0) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // ШЕЙДЕРЫ
  // ----------
  // создание и компиляция шейдеров
  Shader ourShader("../static/shaders/shader.vs",
                   "../static/shaders/shader.fs");

  // вершины квадрата
  std::array<float, 32> vertices = {
      // координаты (x,y,z)          // цвета (r,g,b)       // координаты
      // текстур (s,t)
      0.5F, 0.5F, 0.0F, 1.0F, 0.0F, 0.0F, 1.0F, 1.0F,  
      0.5F,  -0.5F, 0.0F, 0.0F, 1.0F, 0.0F, 1.0F, 0.0F,
      -0.5F,  -0.5F,  0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F,
      -0.5F, 0.5F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F
  };

  std::array<unsigned int, 6> indices = {  
        0, 1, 3, // первый треугольник
        1, 2, 3  // второй треугольник
    };

  unsigned int VBO;
  unsigned int VAO;
  unsigned int EBO;
  // VAO - Vertex Arrays Object
  glGenVertexArrays(1, &VAO);
  // VBO - Vertex Buffer Object позволяет отрендерить видеоустройством
  glGenBuffers(1, &VBO);
  // EBO - Индексный буферный объект
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

  // Настройка атрибутов вершин
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // Настройка атрибутов цветов
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Настройка атрибутов вершин
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void*>(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Создание текстуры
  unsigned int texture;
  glGenTextures(1, &texture);
  // привязываем как и другие команды
  glBindTexture(GL_TEXTURE_2D, texture);
  // перенос текстур
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
  // фильтрация
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // загружаем изображение(текстуру) и генерируем мип-карту(для работы с
  // прорисовкой текстур объектов в дали)
  int width;
  int height;
  int nrChannels;
  stbi_set_flip_vertically_on_load(1); // чтобы изображение не было вверх ногами
  unsigned char* data =
      stbi_load("../static/textures/smile_cat.jpg", &width, &height, &nrChannels, 0);
  if (data != nullptr) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  // освободим память изображений
  stbi_image_free(data);

  // ЦИКЛ РЕНДЕРИНГА
  // цикл нам нужен для того, чтобы окно не закрылось без пользователя
  // -----------
  while (glfwWindowShouldClose(window) == 0) {
    // input

    processInput(window);

    // РЕНДЕРИНГ
    glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);

    // РИСУЕМ ТРЕУГОЛЬНИК

    ourShader.use();
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // ОБМЕН БУФФЕРОВ И СПРАШИВАЕМ ПОЛЬЗОВАТЕЛЯ О ВВОДЕ-ВЫВОДЕ (кнопки, мышка и
    // т. д.)
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
void processInput(GLFWwindow* window) {
  // кнопка закрытия окна
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
}

// КОГДА МЕНЯЕТСЯ РАЗМЕР ОКНА (ОС ИЛИ ПОЛЬЗОВАТЕЛЕМ) ВЫЗЫВАЕТСЯ ЭТА ФУНКЦИЯ
void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width,
                               int height) {
  // окно просмотра должно соответствовать размерам нового окна
  glViewport(0, 0, width, height);
}