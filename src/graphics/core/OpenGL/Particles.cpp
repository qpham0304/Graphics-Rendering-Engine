#include "Particles.h"


const float g = 9.8f; // Gravity
const float rho = 1.225f; // Air density
const float Cd = 1.0f; // Drag coefficient
const float cross_sectional_area = 0.5f; // Cross-sectional area
const float dt = 0.016f; // Time step (assuming 60 FPS)

glm::vec3 generateRandomDirection() {
    float theta = glm::radians(static_cast<float>(rand() % 360)); // Random angle in the xy-plane
    float phi = glm::radians(static_cast<float>(rand() % 180)); // Random angle for z-axis
    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);
    return glm::normalize(glm::vec3(x, y, z));
}

glm::vec3 generateRandomCircularDirection() {
    float angle = glm::radians(static_cast<float>(rand() % 360)); // Random angle in the XY plane
    float x = cos(angle);
    float y = sin(angle);
    return glm::normalize(glm::vec3(x, y, 0.0f)); // Z is 0 to keep it in the XY plane
}

float calculate_terminal_velocity(float mass, float cross_sectional_area) {
    float combined_constant = 2.0f * g / rho;
    return sqrt((combined_constant * mass) / (Cd * cross_sectional_area));
}

// Function to update velocity
float update_velocity(float current_velocity, float mass, float cross_sectional_area) {
    float terminal_velocity = calculate_terminal_velocity(mass, cross_sectional_area);

    if (current_velocity < terminal_velocity) {
        current_velocity += g * dt;

        float drag_force = 0.5f * rho * current_velocity * current_velocity * Cd * cross_sectional_area;
        float drag_acceleration = drag_force / mass;

        current_velocity -= drag_acceleration * dt;

        if (current_velocity > terminal_velocity) {
            current_velocity = terminal_velocity;
        }
    }

    return current_velocity;
}

Particles::Particles() {
    direction = glm::vec3(0.0, 0.0, 0.0);
    scale = glm::vec3(1.0);
    model = glm::mat4(1.0);
    upperBound = 0.0;
    lowerBound = 0.0;
    VAO = 0;
    VBO = 0;
}

void Particles::init(const ParticleControl& control) {
    scale = control.size;
    upperBound = control.upperBound;
    lowerBound = control.lowerBound;
    numInstances = control.numInstances;

    if (!matrixModels.empty())
        Console::println("Please clear particle before reinitialize");
    else {
        upperBound = control.upperBound;
        lowerBound = control.lowerBound;
        Console::println(std::to_string(control.numInstances));
        for (int i = 0; i < control.numInstances; i++) {
            matrixModels.push_back(Utils::Random::createRandomTransform(control.spawnArea, scale));
            weights.push_back(Utils::Random::randomFloat(control.randomRange.x, control.randomRange.y));
            flyDirections.push_back(glm::vec3(0.0, 0.0, 0.0));
            glm::vec3 rand = generateRandomDirection();
            //glm::vec3 rand = generateRandomCircularDirection();
            randomDirs.push_back(rand);
        }
    }
}

void Particles::clear() {
    matrixModels.clear();
}

void Particles::reset() {
    std::fill(flyDirections.begin(), flyDirections.end(), glm::vec3(0.0));
}

void Particles::render(Shader& shader, Camera& camera, int& numRender, float& speed, bool& pause) {
    shader.Activate();
    shader.setMat4("mvp", camera.getMVP());
    shader.setVec3("lightColor", glm::vec3(0.7, 0.8, 1.0));
    if (!matrixModels.empty()) {
        //if (numRender >= numInstances) {
        //    Console::println("Cannot render more than the number of instances")
        //}
        for (unsigned int i = 0; i < numInstances; i++) {
            model = matrixModels[i];

            if (!pause) {
                float velocity = speed * camera.getDeltaTime();
                //flyDirections[i].x -= weights[i] * velocity;
                flyDirections[i].y -= weights[i] * velocity;
                //flyDirections[i].z -= weights[i] * velocity;

                if(!randomDirs.empty()) // randomdirs to generate pattern
                    flyDirections[i] += randomDirs[i];
            }
            if (flyDirections[i].y <= lowerBound || flyDirections[i].y >= upperBound) {
                flyDirections[i].x = 0.0;
                flyDirections[i].y = 0.0;
                flyDirections[i].z = 0.0;
            }
            direction.x = flyDirections[i].x;
            direction.y = flyDirections[i].y;
            direction.z = flyDirections[i].z;

            translateMatrix = glm::translate(glm::mat4(1.0), direction);
            scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(1.0));
            transformMatrix = translateMatrix * scaleMatrix * model;
            model = transformMatrix;
            shader.setMat4("matrix", model);
            //Utils::OpenGL::Draw::drawInstancedCube(VAO, VBO, numInstances);
            Utils::OpenGL::Draw::drawQuad(VAO, VBO);
        }
    }
    else {
        Console::println("No particle to render");
    }
}
