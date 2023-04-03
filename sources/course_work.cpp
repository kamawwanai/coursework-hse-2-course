#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <stb_image.h>

//ИСПОЛЬЗУЕМ МОЩНУЮ БИБЛИОТЕКУ ДЛЯ РАБОТЫ С МАТРИЦАМИ GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <iostream>
#include <type_traits>

#include "shader.hpp"

using glm::vec3;

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
  glEnable(GL_DEPTH_TEST); // включить тестирование глубины
  // ----------
  // создание и компиляция шейдеров
  Shader ourShader("../static/shaders/shader.vs",
                   "../static/shaders/shader.fs");

  // вершины квадрата
  std::array<float, 180> vertices = {
      // координаты (x,y,z)      // координаты текстур (s,t)
    -0.5F, -0.5F, -0.5F,  0.0F, 0.0F,
     0.5F, -0.5F, -0.5F,  1.0F, 0.0F,
     0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
     0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
    -0.5F,  0.5F, -0.5F,  0.0F, 1.0F,
    -0.5F, -0.5F, -0.5F,  0.0F, 0.0F,

    -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
     0.5F, -0.5F,  0.5F,  1.0F, 0.0F,
     0.5F,  0.5F,  0.5F,  1.0F, 1.0F,
     0.5F,  0.5F,  0.5F,  1.0F, 1.0F,
    -0.5F,  0.5F,  0.5F,  0.0F, 1.0F,
    -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,

    -0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
    -0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
    -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
    -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
    -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
    -0.5F,  0.5F,  0.5F,  1.0F, 0.0F,

     0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
     0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
     0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
     0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
     0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
     0.5F,  0.5F,  0.5F,  1.0F, 0.0F,

    -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
     0.5F, -0.5F, -0.5F,  1.0F, 1.0F,
     0.5F, -0.5F,  0.5F,  1.0F, 0.0F,
     0.5F, -0.5F,  0.5F,  1.0F, 0.0F,
    -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
    -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,

    -0.5F,  0.5F, -0.5F,  0.0F, 1.0F,
     0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
     0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
     0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
    -0.5F,  0.5F,  0.5F,  0.0F, 0.0F,
    -0.5F,  0.5F, -0.5F,  0.0F, 1.0F
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // // Настройка атрибутов цветов
  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
  //                       reinterpret_cast<void*>(3 * sizeof(float)));
  // glEnableVertexAttribArray(1);

  // Настройка атрибутов вершин
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

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

  ourShader.use(); 
  ourShader.setInt("texture1", 0);

  // ЦИКЛ РЕНДЕРИНГА
  // цикл нам нужен для того, чтобы окно не закрылось без пользователя
  // -----------
  while (glfwWindowShouldClose(window) == 0) {
    // input

    processInput(window);

    // РЕНДЕРИНГ ЭКРАНА
    glClearColor(0.2F, 0.3F, 0.3F, 1.0F); // цвет экрана
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // АКТИВИРУЕМ ШЕЙДЕРЫ
    ourShader.use(); 
    
    // ПЕРЕМЕЩЕНИЕ
    glm::mat4 model = glm::mat4(1.0F); // это матрица модели - наша трансформация
    glm::mat4 view = glm::mat4(1.0F); // камера
    glm::mat4 projection = glm::mat4(1.0F); // проекция

    // переводим матрицу преобразования на нужный нам вектор
    // model = glm::translate(model, glm::vec3(0.25F, 0.25F, 0.0F));

    // постоянный поворот на 50 градусов по (0.5,1,0)
    model = glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(50.0F), glm::vec3(0.5F, 1.0F, 0.0F));

    // куда расположем камеру
    view = glm::translate(view, glm::vec3(0.0F,0.0F,-3.0F));

    // используем переспективную проекцию
    // угол обзора - соотношение сторон сцены - ближайшая усеченная - дальнаяя усеченная
    projection =  glm::perspective(glm::radians(45.0F), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1F, 300.0F);

    // ПЕРЕДАЧА МАТРИЦ ПРЕОБРАЗОВАНИЯ ШЕЙДЕРУ
    ourShader.setMat4("model", model);
    ourShader.setMat4("view", view);
    ourShader.setMat4("projection", projection);

    // РЕНДЕРИНГ
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

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