#pragma once

#include "imgui.h"
#include <vector>
#include <string>
#include <unordered_map>

struct ProfilerData {
	const char* name;
	double time;
};

class Profiler
{
private:
	std::unordered_map<std::string, double> profileList;
	
	Profiler() = default;

public:
	~Profiler() = default;

	static Profiler& getInstance();

	void addTracker(ProfilerData&& data);
	void addTracker(ProfilerData& data);
	void display();
};

