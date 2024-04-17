#pragma once

#include <string>
#include <vector>
#include <map>
#include "components/headers/Component.h"

class GraphicsController {
private:

protected:
	std::vector<Component> Components;
	std::unordered_map<std::string, Shader> shaders;

	GraphicsController() {};

public:
	virtual void init() {};
	virtual void render() {}
	virtual ~GraphicsController() {}
};


class OpenGLController : public GraphicsController {
private:

public:
	OpenGLController();
	~OpenGLController();

	void init() override;
	void render() override;
};

