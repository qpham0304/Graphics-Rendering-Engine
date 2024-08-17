#pragma once

#include <memory>
#include <Shader.h>
#include "../../src/core/components/legacy/component.h"

class Renderer
{
private:
	

public:
	Renderer();
	~Renderer();

	//void registerShader();
	virtual void render() = 0;
	virtual void init() = 0;
};

