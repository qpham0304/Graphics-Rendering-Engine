#pragma once

#include <string>

class Layer
{
private:
	std::string m_layerName;

public:
	Layer(const std::string& name = "default") : m_layerName(name) {};
	virtual ~Layer() = default;

	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnEvent() = 0;

	const std::string& getName() const { return m_layerName; }
};

