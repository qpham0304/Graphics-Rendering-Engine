#pragma once

#include <memory>
#include <Shader.h>
#include "../../src/core/components/Component.h"

class Renderer
{
private:
	std::unique_ptr<Shader> shader;

public:
	Renderer();

	void RegisterShader();
	void Render(std::vector<Component> components);
	void Init();
	void Delete();
};

