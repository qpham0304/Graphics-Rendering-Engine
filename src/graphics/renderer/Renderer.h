#pragma once

#include <memory>
#include <Shader.h>
#include "../../src/graphics/components/headers/Component.h"

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

