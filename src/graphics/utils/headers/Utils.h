#pragma once

#include <glm/glm.hpp>
#include <string>
#include <random>
#include <sstream>
#include <windows.h>
#include <shobjidl.h> 

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

namespace Utils::Draw {
    void drawQuad();

}

namespace Utils::Window {
    std::string WindowFileDialog();

}