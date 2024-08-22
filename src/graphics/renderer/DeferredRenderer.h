#pragma once

#include <memory>
#include <Shader.h>
#include "../../src/core/components/legacy/component.h"

class DeferredRenderer
{
private:
	unsigned int gBuffer;
	unsigned int gPosition;
	unsigned int gNormal;
	unsigned int gColorSpec;
	unsigned int gAlbedoSpec;
	unsigned int rboDepth;
    unsigned int gDepth;
	int width;
	int height;


public:
	DeferredRenderer(const int width, const int height);
	std::unique_ptr<Shader> geometryShader;
	std::unique_ptr<Shader> colorShader;
	void renderGeometry(Camera& camera, std::vector<Component*>& components);
	void renderGeometry(Camera& camera, Component& component);
	void renderColor(Camera& camera, std::vector<Light>& lights);

	unsigned int getGBuffer();
	unsigned int getGPosition();
	unsigned int getGNormal();
	unsigned int getGColorspec();
	unsigned int getGAlbedoSpec();
	unsigned int getGDepth();
};

