#include "Timer.h"
#include "../../src/graphics/utils/headers/Utils.h"

Timer::Timer()
{
    this->label = "default";
    Start();
}

Timer::Timer(const char* label)
{
    Start();
    this->label = label;
}

Timer::~Timer() {
    Stop();
}

void Timer::Start()
{
    start = std::chrono::high_resolution_clock::now();
}

void Timer::Stop()
{
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    float ms = duration.count() * 1000.0f;
    //const std::string timeTaken = "Operation took: " + std::to_string(ms) + " ms " + "label: " + label;
    
    std::ostringstream oss;
    oss << "Operation took: " << ms << " ms " << "label: " << label;
    std::string timeTaken = oss.str();
    Console::println(timeTaken);
}
