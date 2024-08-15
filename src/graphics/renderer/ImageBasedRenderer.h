#pragma once

#include <string>
#include <Shader.h>
#include <memory>

class ImageBasedRenderer
{
private:

public:
	ImageBasedRenderer();

	void init(const std::string& path);
	void onTextureReload(const std::string& path);

	std::unique_ptr<Shader> equirectangularToCubemapShader;
	std::unique_ptr<Shader> irradianceShader;
	std::unique_ptr<Shader> backgroundShader;
	std::unique_ptr<Shader> prefilterShader;
	std::unique_ptr<Shader> brdfShader;

	unsigned int captureFBO, captureRBO;
	unsigned int envCubemapTexture;
	unsigned int hdrTexture;
	std::string texturePath;

	unsigned int prefilterMap;
	unsigned int irradianceMap;
	unsigned int brdfLUTTexture;
};

