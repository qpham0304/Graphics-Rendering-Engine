#include <pch.h>
#include "Timer.h"
#include "../../src/graphics/utils/headers/Utils.h"

Timer::Timer()
{
    Start();
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
    const std::string timeTaken = "Operation took: " + std::to_string(ms) + " ms";
    Console::println(timeTaken);
}
