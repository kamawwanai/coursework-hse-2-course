#include "Model.hpp"

using std::string;

const unsigned int width = 800;
const unsigned int height = 800;

float skyboxVertices[] = {  // NOLINT
    //   Coordinates
    -1.0F, -1.0F, 1.0F,   //        7--------6
    1.0F,  -1.0F, 1.0F,   //       /|       /|
    1.0F,  -1.0F, -1.0F,  //      4--------5 |
    -1.0F, -1.0F, -1.0F,  //      | |      | |
    -1.0F, 1.0F,  1.0F,   //      | 3------|-2
    1.0F,  1.0F,  1.0F,   //      |/       |/
    1.0F,  1.0F,  -1.0F,  //      0--------1
    -1.0F, 1.0F,  -1.0F};

unsigned int skyboxIndices[] = {  // NOLINT
    // Right
    1, 2, 6, 6, 5, 1,
    // Left
    0, 4, 7, 7, 3, 0,
    // Top
    4, 5, 6, 6, 7, 4,
    // Bottom
    0, 3, 2, 2, 1, 0,
    // Back
    0, 1, 5, 5, 4, 0,
    // Front
    3, 7, 6, 6, 2, 3};

auto main() -> int {
    // Initialize GLFW
    glfwInit();

    // Tell GLFW what version of OpenGL we are using
    // In this case we are using OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Tell GLFW we are using the CORE profile
    // So that means we only have the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFWwindow object of 800 by 800 pixels, naming it
    // "YoutubeOpenGL"
    GLFWwindow* window =
        glfwCreateWindow(width, height, "Coursework", nullptr, nullptr);
    // Error check if the window fails to create
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Introduce the window into the current context
    glfwMakeContextCurrent(window);

    // Load GLAD so it configures OpenGL
    gladLoadGL();
    // Specify the viewport of OpenGL in the Window
    // In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
    glViewport(0, 0, width, height);

    // Generates Shader objects
    Shader shaderProgram("../static/shaders/shader.vs",
                         "../static/shaders/shader.fs");
    Shader skyboxShader("../static/shaders/skybox.vs",
                        "../static/shaders/skybox.fs");

    // Take care of all the light related things
    glm::vec4 lightColor = glm::vec4(1.0F, 1.0F, 1.0F, 1.0F);
    glm::vec3 lightPos = glm::vec3(0.5F, 0.5F, 0.5F);

    shaderProgram.use();
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"),
                lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x,
                lightPos.y, lightPos.z);
    skyboxShader.use();
    glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

    // Enables the Depth Buffer
    glEnable(GL_DEPTH_TEST);

    // Enables Cull Facing
    glEnable(GL_CULL_FACE);
    // Keeps front faces
    glCullFace(GL_FRONT);
    // Uses counter clock-wise standard
    glFrontFace(GL_CCW);

    // Creates camera object
    Camera camera(width, height, glm::vec3(0.0F, 0.0F, 2.0F));
    std::string modelPath = "../static/models/lowpoly_airplane/scene.gltf";

    // Load in models
    Model model(modelPath);

    // Variables to create periodic event for FPS displaying
    double prevTime = 0.0;
    double crntTime = 0.0;
    double timeDiff;
    // Keeps track of the amount of frames in timeDiff
    unsigned int counter = 0;

    // Use this to disable VSync (not advized)
    // glfwSwapInterval(0);

    // Create VAO, VBO, and EBO for the skybox
    unsigned int skyboxVAO;
    unsigned int skyboxVBO;
    unsigned int skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);  // NOLINT
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // All the faces of the cubemap (make sure they are in this exact order)
    string facesCubemap[6] = {// NOLINT
                              "../static/textures/skybox/right.jpg",
                              "../static/textures/skybox/left.jpg",
                              "../static/textures/skybox/top.jpg",
                              "../static/textures/skybox/bottom.jpg",
                              "../static/textures/skybox/front.jpg",
                              "../static/textures/skybox/back.jpg"};

    // Creates the cubemap texture object
    unsigned int cubemapTexture;
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // These are very important to prevent seams
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // This might help with seams on some systems
    // glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Cycles through all the textures and attaches them to the cubemap object
    for (unsigned int i = 0; i < 6; i++) {
        int width;
        int height;
        int nrChannels;
        unsigned char* data =
            stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
        if (data != nullptr) {
            stbi_set_flip_vertically_on_load(0);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width,
                         height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Failed to load texture: " << facesCubemap[i]
                      << std::endl;
            stbi_image_free(data);
        }
    }

    // Main while loop
    while (glfwWindowShouldClose(window) == 0) {
        // Updates counter and times
        crntTime = glfwGetTime();
        timeDiff = crntTime - prevTime;
        counter++;

        if (timeDiff >= 1.0 / 30.0) {
            // Creates new title
            std::string FPS = std::to_string((1.0 / timeDiff) * counter);
            std::string ms = std::to_string((timeDiff / counter) * 1000);
            std::string newTitle =
                "COURSEWORK - " + FPS + "FPS / " + ms + "ms";  // NOLINT
            glfwSetWindowTitle(window, newTitle.c_str());

            // Resets times and counter
            prevTime = crntTime;
            counter = 0;
        }

        // Specify the color of the background
        glClearColor(0.07F, 0.13F, 0.17F, 1.0F);
        // Clean the back buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Handles camera inputs (delete this if you have disabled VSync)
        camera.userInput(window);
        // Updates and exports the camera matrix to the Vertex Shader
        camera.updateCameraMatrix(45.0F, 0.1F, 300.0F);

        // Draw the normal model
        model.draw(shaderProgram, camera);

        // Since the cubemap will always have a depth of 1.0, we need that equal
        // sign so it doesn't get discarded
        glDepthFunc(GL_LEQUAL);

        skyboxShader.use();
        glm::mat4 view = glm::mat4(1.0F);
        glm::mat4 projection = glm::mat4(1.0F);
        // We make the mat4 into a mat3 and then a mat4 again in order to get
        // rid of the last row and column The last row and column affect the
        // translation of the skybox (which we don't want to affect)
        view = glm::mat4(glm::mat3(glm::lookAt(
            camera.position, camera.position + camera.target, camera.worldUp)));
        projection = glm::perspective(
            glm::radians(45.0F),
            static_cast<float>(width) / static_cast<float>(height), 0.1F,
            100.0F);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1,
                           GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"),
                           1, GL_FALSE, glm::value_ptr(projection));

        // Draws the cubemap as the last object so we can save a bit of
        // performance by discarding all fragments where an object is present (a
        // depth of 1.0f will always fail against any object's depth value)
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        // Switch back to the normal depth function
        glDepthFunc(GL_LESS);

        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window);
        // Take care of all GLFW events
        glfwPollEvents();
    }

    // Delete all the objects we've created
    shaderProgram.remove();
    skyboxShader.remove();
    // Delete window before ending the program
    glfwDestroyWindow(window);
    // Terminate GLFW before ending the program
    glfwTerminate();
    return 0;
}