#include "Profiler.h"
#include "../../graphics/utils/Utils.h"

Profiler& Profiler::getInstance()
{
	static Profiler instance;
	return instance;
}

void Profiler::addTracker(ProfilerData&& data)
{
	profileList[data.name] = data.time;
}

void Profiler::addTracker(ProfilerData& data)
{
	profileList[data.name] = data.time;
}

void Profiler::display()
{
	ImGui::Begin("Profiler");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
		1000.0f / ImGui::GetIO().Framerate, 
		ImGui::GetIO().Framerate
	);
	for (const auto& [label, time] : profileList) {
		ImGui::Text(label.c_str());
		ImGui::SameLine();
		ImGui::Text(std::to_string(time).c_str());
	}
	ImGui::End();
}
