#include "camera.hpp"

Camera::Camera(int width, int height, glm::vec3 position) {  // NOLINT
    scrWidth = width;
    scrHeight = height;
    Camera::position = position;
}

void Camera::updateCameraMatrix(float viewingAngle, float nearPlane,
                                float farPlane) {
    glm::mat4 view = glm::mat4(1.0F);
    glm::mat4 projection = glm::mat4(1.0F);

    view = glm::lookAt(position, position + target, worldUp);
    projection = glm::perspective(
        glm::radians(viewingAngle),
        static_cast<float>(scrWidth) / static_cast<float>(scrHeight), nearPlane,
        farPlane);

    cameraMatrix = projection * view;
}

void Camera::exportMatrix(Shader& shader, const std::string& uniform) {
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform.c_str()), 1,
                       GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::userInput(GLFWwindow* window) {
    // ввод с клавиатуры
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += speedMovement * target;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position +=
            speedMovement * -glm::normalize(glm::cross(target, worldUp));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position += speedMovement * -target;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += speedMovement * glm::normalize(glm::cross(target, worldUp));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        position += speedMovement * worldUp;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        position += speedMovement * -worldUp;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        speedMovement = 0.4F;
    } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
        speedMovement = 0.1F;
    }

    // ввод мышки
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        // спрятать курсор
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if (firstClick) {
            glfwSetCursorPos(window, (static_cast<float>(scrWidth) / 2),
                             (static_cast<float>(scrHeight) / 2));
            firstClick = false;
        }

        double mouseX;
        double mouseY;

        glfwGetCursorPos(window, &mouseX, &mouseY);
        float rotX = sensitivity *
                     static_cast<float>(mouseY - (scrHeight / 2)) /  // NOLINT
                     scrHeight;                                      // NOLINT
        float rotY = sensitivity *
                     static_cast<float>(mouseX - (scrWidth / 2)) /  // NOLINT
                     scrWidth;                                      // NOLINT

        glm::vec3 newTarget =
            glm::rotate(target, glm::radians(-rotX),
                        glm::normalize(glm::cross(target, worldUp)));

        if (abs(glm::angle(newTarget, worldUp) - glm::radians(90.0F)) <=
            glm::radians(85.0F)) {
            target = newTarget;
        }

        target = glm::rotate(target, glm::radians(-rotY), worldUp);

        // поставить мышь в центр
        glfwSetCursorPos(window, (scrWidth / 2), (scrHeight / 2));  // NOLINT
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ==
               GLFW_RELEASE) {
        // Unhides cursor since camera is not looking around anymore
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Makes sure the next time the camera looks around it doesn't jump
        firstClick = true;
    }
}