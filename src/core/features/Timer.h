#pragma once

#include <chrono>
#include <string>
class Timer
{
private:
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;
	std::chrono::duration<float> duration;

	void Start();
	void Stop();

public:
	Timer();
	~Timer();
};

