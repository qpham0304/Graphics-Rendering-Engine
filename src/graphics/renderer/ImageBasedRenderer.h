#pragma once

#include <string>
#include <Shader.h>
#include <memory>

class ImageBasedRenderer
{
private:
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	unsigned int captureFBO, captureRBO;
	std::string texturePath;

	std::unique_ptr<Shader> equirectangularToCubemapShader;
	std::unique_ptr<Shader> irradianceShader;
	std::unique_ptr<Shader> backgroundShader;
	std::unique_ptr<Shader> prefilterShader;
	std::unique_ptr<Shader> brdfShader;

	void setupCubeMap();
	void renderCubeMap();
	void setupIrradianceMap();
	void renderIrradianceMap();
	void setupPrefilterMap();
	void renderPrefilterMap();
	void setupBRDF();
	void renderBRDF();

public:
	ImageBasedRenderer();

	void init(const std::string& path);
	void bindIrradianceMap();
	void bindPrefilterMap();
	void bindLUT();
	void onTextureReload(const std::string& path);

	unsigned int envCubemapTexture;
	unsigned int hdrTexture;

	unsigned int prefilterMap;
	unsigned int irradianceMap;
	unsigned int brdfLUTTexture;
};

