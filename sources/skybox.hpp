#pragma once

#include "glad/glad.h"
#include <stb_image.h>

#include <iostream>
#include <array>
#include <string>

class Skybox {
   public:
    Skybox() = default;
    unsigned int skyboxVAO;
    unsigned int cubemapTexture;

    void intializeArrayBufferObjects();

    void createTexture();

    void draw();

   private:
    std::array<float, 24> skyboxVertices = {
        //   Coordinates
        -1.0F, -1.0F, 1.0F,    // 0
        1.0F,  -1.0F, 1.0F,    // 1
        1.0F,  -1.0F, -1.0F,   // 2
        -1.0F, -1.0F, -1.0F,   // 3
        -1.0F, 1.0F,  1.0F,    // 4
        1.0F,  1.0F,  1.0F,    // 5
        1.0F,  1.0F,  -1.0F,   // 6
        -1.0F, 1.0F,  -1.0F};  // 7

    std::array<unsigned int, 36> skyboxIndices = {
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

    std::string facesCubemap[6] = {  // NOLINT
        "../static/textures/skybox/right.jpg",
        "../static/textures/skybox/left.jpg",
        "../static/textures/skybox/top.jpg",
        "../static/textures/skybox/bottom.jpg",
        "../static/textures/skybox/front.jpg",
        "../static/textures/skybox/back.jpg"};
};
