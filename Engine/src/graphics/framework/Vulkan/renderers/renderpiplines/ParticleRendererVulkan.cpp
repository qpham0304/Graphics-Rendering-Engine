#include "ParticleRendererVulkan.h"

#include <particle/ParticleManager.h>
#include <graphics/framework/Vulkan/resources/textures/TextureVulkan.h>
#include <graphics/framework/Vulkan/resources/descriptors/DescriptorManagerVulkan.h>
#include <graphics/framework/Vulkan/resources/materials/MaterialManagerVulkan.h>
#include <graphics/framework/Vulkan/resources/textures/TextureManagerVulkan.h>
#include <graphics/framework/Vulkan/renderers/RendererManagerVulkan.h>
#include <graphics/framework/vulkan/core/VulkanPipeline.h>
#include <graphics/framework/Vulkan/renderers/RenderDeviceVulkan.h>
#include <graphics/renderers/RenderDevice.h>
#include <core/resources/managers/TextureManager.h>
#include <core/resources/managers/MeshManager.h>
#include <core/resources/managers/ModelManager.h>
#include <core/resources/managers/DescriptorManager.h>
#include <core/scene/SceneManager.h>
#include <core/features/ServiceLocator.h>
#include <core/events/EventManager.h>
#include <core/features/Mesh.h>
#include <core/features/Camera.h>

ParticleRendererVulkan::ParticleRendererVulkan(std::string serviceName)
	:	RendererVulkan(serviceName)
{

}

ParticleRendererVulkan::~ParticleRendererVulkan() 
{

}

bool ParticleRendererVulkan::init(WindowConfig config)
{
	RendererVulkan::init(config);

    particleManager = &ServiceLocator::GetService<ParticleManager>("ParticleManager");
	uint32_t bdaID = particleManager->getContainerRef();
    pushConstant.containersRef = bufferManagerVulkan->getBuffer(bdaID)->getAddress();

    bufferManagerVulkan->createUniformBuffers(emitterUniformBuffersList, sizeof(EmitterUBO));

    _createResources();
    _createDescriptor();
    _createPipelines();

    return true;
}

bool ParticleRendererVulkan::onClose()
{
	renderDeviceVulkan->waitIdle();
    _cleanupResources();

	_createPipelines();

    return true;
}

void ParticleRendererVulkan::onUpdate()
{
}

void ParticleRendererVulkan::render(Camera &camera)
{
    Timer timer(m_name, true);

    RendererVulkan::_resize();
    rendererManagerVulkan->setDisplayImage(outTexture);

	SceneManager& sceneManager = SceneManager::getInstance();
	Scene* scene = sceneManager.getActiveScene();
	if(!scene){
		m_logger->error("No scene to render");
	}

    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = outTexture->textureImageView;
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue = { {0.1f, 0.1f, 0.1f, 1.0f} };

    std::vector<VkRenderingAttachmentInfo> colorAttachments = { colorAttachment };

    // Optional depth attachment if needed for particle
    // VkRenderingAttachmentInfo depthAttachment{};
    // depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    // depthAttachment.imageView = depthTexture->textureImageView;
    // depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    // depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // depthAttachment.clearValue.depthStencil = {1.0f, 0};

    auto ubo = rendererManagerVulkan->getUBO();
    uint32_t width = ubo.width;
    uint32_t height = ubo.height;

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea = { {0, 0}, {width, height} };
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = colorAttachments.size();
    renderingInfo.pColorAttachments = colorAttachments.data();
    // renderingInfo.pDepthAttachment = &depthAttachment;
    renderingInfo.pDepthAttachment = nullptr;


	VkCommandBuffer cmd = renderDeviceVulkan->commandPool.currentBuffer();
    uint32_t currentFrame = renderDeviceVulkan->getCurrentFrameIndex();
    
    renderDeviceVulkan->beginLabel(cmd, "Particle Render Pass", {1.0, 0.0, 1.0, 1.0});
    vkCmdBeginRendering(cmd, &renderingInfo);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
    
    auto func = std::function<void(Entity)>([&](Entity entity) -> void {
        ParticleEmitter& emitter = entity.getComponent<ParticleEmitter>();

        pushConstant.containerIdx = emitter.containerID;

        ParticleContainer container = particleManager->getContainer(emitter.containerID);

        vkCmdPushConstants(cmd, pipeline->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ParticlePushConstant), &pushConstant);

        //TODO: use drawindirect with indirect buffer for efficiency ignore for now
        vkCmdDraw(cmd, container.m_size * 6, 1, 0, 0);
    });
    // vkCmdDraw(cmd, 3, 1, 0, 0);

    scene->forEnitiesWith<ParticleEmitter>(func);
    
    vkCmdEndRendering(cmd);
	renderDeviceVulkan->endLabel(cmd);
    
}

void ParticleRendererVulkan::_recreateResources()
{
	renderDeviceVulkan->waitIdle();
    _cleanupResources();

    _createResources();
    _createPipelines();
    _updateDescriptor();
}

void ParticleRendererVulkan::_cleanupResources()
{
    TextureManager& textureManager = ServiceLocator::GetService<TextureManager>("TextureManagerVulkan");
    textureManager.destroy(outTexture->id());
    pipeline->destroy();
}

void ParticleRendererVulkan::_createResources()
{
    VulkanSwapChain& swapchain = renderDeviceVulkan->swapchain;

    auto createTexture = [&] (uint32_t& id){
        id = textureManagerVulkan->createTexture();
        TextureVulkan* texture = dynamic_cast<TextureVulkan*>(textureManagerVulkan->getTexture(id));
        
        assert(texture && "failed to cast texture into vulkan texture");
        
        
        TextureManagerVulkan::createImage(
            swapchain.swapChainExtent.width,
            swapchain.swapChainExtent.height,
            VK_FORMAT_R16G16B16A16_SFLOAT,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            texture->textureImage,
            texture->textureImageMemory,
            1,
            renderDeviceVulkan->device
        );

        //TODO: remove general creation or change this to RGBA16FLOAT
        TextureManagerVulkan::createImageView(
            texture->textureImage,
            texture->textureImageView,
            VK_FORMAT_R16G16B16A16_SFLOAT,
            VK_IMAGE_ASPECT_COLOR_BIT,
            1,
            renderDeviceVulkan->device
        );

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        TextureManagerVulkan::createTextureSampler(
            texture->textureSampler, 
            renderDeviceVulkan->device,
            samplerInfo
        );

		return texture;
    };
    
	uint32_t textureID;
	outTexture = createTexture(textureID);
	rendererManagerVulkan->addRenderTexture("particleColorOut", textureID);

    // uint32_t depthTextureID = textureManagerVulkan->createTexture();
    // depthTexture = dynamic_cast<TextureVulkan*>(textureManagerVulkan->getTexture(depthTextureID));
	// VkFormat depthFormat = TextureManagerVulkan::findDepthFormat(renderDeviceVulkan->device);

	// TextureManagerVulkan::createImage(
	// 	swapchain.swapChainExtent.width,
	// 	swapchain.swapChainExtent.height,
	// 	depthFormat,
	// 	VK_IMAGE_TILING_OPTIMAL,
	// 	VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
	// 	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	// 	depthTexture->textureImage,
	// 	depthTexture->textureImageMemory,
	// 	1,
	// 	renderDeviceVulkan->device
	// );
	
	// TextureManagerVulkan::createImageView(depthTexture->textureImage,
    //     depthTexture->textureImageView,
    //     depthFormat,
    //     VK_IMAGE_ASPECT_DEPTH_BIT,
    //     1,
    //     renderDeviceVulkan->device
    // );

	// rendererManagerVulkan->addRenderTexture("particleDepthOut", textureID);

}

void ParticleRendererVulkan::_createPipelines()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;

	PipelineConfigInfo config = VulkanPipeline::defaultPipelineConfigInfo(1);
	config.renderPass = VK_NULL_HANDLE;

	AttachmentsInfo attachmentsInfo{};
    attachmentsInfo.colorAttachmentFormats = { VK_FORMAT_R16G16B16A16_SFLOAT };
    attachmentsInfo.depthAttachmentFormat = TextureManagerVulkan::findDepthFormat(renderDeviceVulkan->device);
    attachmentsInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;


    VkDescriptorSetLayout descriptorSetLayout = descriptorManagerVulkan->getDescriptorLayout(layoutID);
	VkDescriptorPool descriptorPool = descriptorManagerVulkan->getDescriptorPool(poolID);

	uint32_t bindlessLayoutID = textureManagerVulkan->getBindlessTextureLayout();
	auto bindlessLayout = descriptorManagerVulkan->getDescriptorLayout(bindlessLayoutID);

	void* handle = materialManager->getMaterialLayout();
	auto materialLayout = reinterpret_cast<VkDescriptorSetLayout>(handle);


    pipeline = std::make_unique<VulkanPipeline>(renderDeviceVulkan->device);
    pipeline->createGraphicsPipelineDynamic(
        "assets/shaders/spv/particle.vert.spv",
        "assets/shaders/spv/particle.frag.spv",
        config,
        attachmentsInfo,
        vertexInputInfo,
        { descriptorSetLayout, bindlessLayout, materialLayout },
        sizeof(pushConstant)
    );

}

void ParticleRendererVulkan::_createDescriptor()
{
    layoutID = descriptorManagerVulkan->createLayout({
		{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
		{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
	});

	uint32_t frameCount = VulkanUtils::numFrames();
	std::vector<VkDescriptorPoolSize> poolSizes {
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, frameCount * 2},
	};
	
	poolID = descriptorManagerVulkan->createPool(poolSizes, frameCount, VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT);
	setsID = descriptorManagerVulkan->createSets(layoutID, poolID, frameCount);

    _updateDescriptor();

}

void ParticleRendererVulkan::_updateDescriptor()
{
	descriptorSets = descriptorManagerVulkan->getDescriptorSet(setsID);

    auto uniformBuffersList = rendererManagerVulkan->getBufferUBO();
	for (size_t i = 0; i < VulkanSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
		DescriptorWriter writer{{}, descriptorSets[i] };
		descriptorManagerVulkan->writeUniform2(writer, uniformBuffersList[i]->getDescUniformBufferInfo());
		descriptorManagerVulkan->writeUniform2(writer, emitterUniformBuffersList[i]->getDescUniformBufferInfo());
		descriptorManagerVulkan->updateDescriptorSets(&writer.writes);
	}
}
