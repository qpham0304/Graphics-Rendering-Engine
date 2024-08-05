#include "Timer.h"
#include "../../src/graphics/utils/Utils.h"
#include "Profiler.h"

Timer::Timer() : label("default")
{
    Start();
}

Timer::Timer(const char* label, const bool guiOn) : label(label), guiOn(guiOn)
{
    Start();
}

Timer::~Timer() {
    float ms = Stop();
    std::ostringstream oss;
    if (!guiOn) {
        oss << "Operation took: " << ms << " ms " << "label: " << label;
        std::string timeTaken = oss.str();
        Console::println(timeTaken);
    }
    else {
        Profiler::getInstance().addTracker({ label, ms });
    }
}

void Timer::Start()
{
    start = std::chrono::high_resolution_clock::now();
}

float Timer::Stop()
{
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    float ms = duration.count() * 1000.0f;
    return ms;
}
