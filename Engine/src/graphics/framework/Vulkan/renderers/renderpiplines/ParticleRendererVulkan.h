#pragma once

#include "graphics/framework/vulkan/renderers/RendererVulkan.h"

class TextureVulkan;
class ParticleManager;

class ParticleRendererVulkan : public RendererVulkan
{
private:
	struct EmitterUBO{
		glm::vec3 position;	// placeholder only remove
	};

public:
	struct ParticlePushConstant {
		uint64_t containersRef;
		uint32_t containerIdx;
		uint32_t particleCount;
		float deltaTime;
	};

	ParticleRendererVulkan(std::string serviceName = "ParticleRendererVulkan");
	virtual ~ParticleRendererVulkan() override;

	virtual bool init(WindowConfig config) override;
	virtual bool onClose() override;
	virtual void onUpdate() override;
	virtual void render(Camera& camera) override;

protected:

	const int MAX_INSTANCES = 10000;
	const int numInstances = 1;
	
	virtual void _recreateResources() override;
	virtual void _cleanupResources() override;
	
	void _createResources();
	void _createPipelines();
	void _createDescriptor();
	void _updateDescriptor();

	void _computeParticle(VkCommandBuffer cmd, uint32_t currentFrame, Scene* scene);
	void _renderParticle(VkCommandBuffer cmd, uint32_t currentFrame, Scene* scene);

	std::unique_ptr<VulkanPipeline> pipeline;
	std::unique_ptr<VulkanPipeline> computePipeline;
	ParticlePushConstant pushConstant;
	
	TextureVulkan* outTexture { nullptr };
	TextureVulkan* depthTexture { nullptr };

	uint32_t layoutID;
	uint32_t poolID;
	uint32_t setsID;
	std::vector<VkDescriptorSet> descriptorSets;

	std::vector<UniformBufferVulkan*> emitterUniformBuffersList;
	EmitterUBO emitterUBO {};

	ParticleManager* particleManager { nullptr };
};