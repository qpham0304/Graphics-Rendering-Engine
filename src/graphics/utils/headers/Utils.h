#pragma once

#include <glm/glm.hpp>
#include <string>
#include <random>
#include <sstream>
#include <windows.h>
#include <shobjidl.h> 
#include <stb/stb_image.h>

namespace Utils::Math {
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

}

namespace Utils::uuid {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::string get_uuid();

}

namespace Utils::OpenGL {
    namespace Draw {
        void drawQuad();
        void drawQuadNormals();
        void drawCube(unsigned int& cubeVAO, unsigned int& cubeVBO);
        void drawSphere(unsigned int& sphereVAO, unsigned int& indexCount);
    };

    std::string loadHDRTexture(const char* path, unsigned int& texture);
    unsigned int loadTexture(char const* path);
    unsigned int loadMTexture(const float* ltc);

    namespace GLFW {

    };
}

namespace Utils::Window {
    std::string WindowFileDialog();

}