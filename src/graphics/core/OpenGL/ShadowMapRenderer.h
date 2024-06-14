#pragma once

#include <DepthMap.h>
#include <memory>
#include <Shader.h>
#include "../../components/headers/component.h"

class ShadowMapRenderer
{

private:
	std::unique_ptr<Shader> shader;
	std::unique_ptr<DepthMap> depthMap;
	

public:
	ShadowMapRenderer(const unsigned int width, const unsigned int height);

	void renderShadow(Light& light, std::vector<Model>& models, std::vector<glm::mat4>& modelMatrices);
	unsigned int depthTexture();
	unsigned int getWidth();
	unsigned int getHeight();

};