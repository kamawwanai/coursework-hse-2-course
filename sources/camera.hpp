#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "glm/gtc/type_ptr.hpp"
#include "shader.hpp"

class Camera {
   public:
    glm::vec3 position;
    glm::vec3 target = glm::vec3(0.0F, 0.0F, -1.0F);
    glm::vec3 worldUp = glm::vec3(0.0F, 1.0F, 0.0F);
    glm::mat4 cameraMatrix = glm::mat4(1.0F);

    // проверка было ли нажатие, чтобы камера не прыгала
    bool firstClick = true;

    // размеры окна
    int scrWidth;
    int scrHeight;

    // скорость и чувствительность
    float speedMovement = 0.1F;
    float sensitivity = 100.0F;

    Camera(int width, int height, glm::vec3 position);

    // обновить вершинный шейдер
    void updateCameraMatrix(float viewingAngle, float nearPlane,
                            float farPlane);
    // экспортировать камеру в шейдер
    void exportMatrix(Shader& shader, const std::string& uniform);
    // движения юзера
    void userInput(GLFWwindow* window);
};