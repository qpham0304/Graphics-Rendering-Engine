#include "ParticleManager.h"
#include "core/components/MComponent.h"
#include "core/features/Random.h"
#include "core/resources/managers/BufferManager.h"
#include "graphics/framework/Vulkan/resources/buffers/BufferManagerVulkan.h"  //TODO: move to vulkan particle renderer
#include "graphics/framework/Vulkan/resources/buffers/DeviceAddressBufferVulkan.h"  //TODO: move to vulkan particle renderer
#include "core/features/ServiceLocator.h"

ParticleManager::ParticleManager()
    : Manager("ParticleManager")
{
    
}

ParticleManager::~ParticleManager()
{

}

bool ParticleManager::init(WindowConfig config)
{
    Service::init(config);
    
    BufferManager* bufferManager = &ServiceLocator::GetService<BufferManager>("BufferManagerVulkan");
    m_containerRefsBDA = bufferManager->createBufferDeviceAddress(MAX_CONTAINERS * sizeof(ParticleContainer::ContainerRef));
    bufferManager->updateBufferDeviceAddress(m_containerRefsBDA, m_containerRefs.data(), MAX_CONTAINERS * sizeof(ParticleContainer::ContainerRef));

    return true;
}

bool ParticleManager::onClose()
{
    return true;
}

void ParticleManager::destroy(uint32_t id)
{
    if(id >= m_containers.size() || id < 0) {
        m_logger->error("particle container id: {} does not exist", id);
    } else {    //TODO: not thread safe, 2 request to delete the same id will causes issues
        m_toBeDestroyed.emplace_back(id);
    }
}

std::vector<uint32_t> ParticleManager::listIDs() const
{
    m_logger->error("listIDs is unimplmented for ParticleManager");

    return std::vector<uint32_t>();
}

void ParticleManager::onUpdate()
{
    if(!m_toBeDestroyed.empty()) {
        for(auto& containerID : m_toBeDestroyed) {
            m_containers.erase(m_containers.begin() + containerID);
            m_containerRefs.erase(m_containerRefs.begin() + containerID);
        }
    }
}

uint32_t ParticleManager::getContainerRef() const
{
    return m_containerRefsBDA;
}

ParticleContainer ParticleManager::getContainer(uint32_t id)
{
    if(id >= m_containers.size() || id < 0) {
        m_logger->error("particle container id: {} does not exist", id);
    }
    return m_containers[id];
}

uint32_t ParticleManager::createContainer(uint32_t size, glm::vec3 minSpacing, glm::vec3 maxSpacing)
{
    //NOTE: m_ids starts at 1 which matches container's index 1 with dummy container at 0
    m_containers.emplace_back(ParticleContainer(size));
    ParticleContainer& container = m_containers[m_ids]; //these only have the ID

    // container.m_containerBufferRefs only have the id to the buffer not the gpu memory address pointer
    BufferManager* tmp = &ServiceLocator::GetService<BufferManager>("BufferManagerVulkan");
    BufferManagerVulkan* bufferManager = dynamic_cast<BufferManagerVulkan*>(tmp);
    
    BufferVulkan* lifetimeBuffer = bufferManager->getBuffer(container.m_containerBufferRefs.lifetimeBufferRef);
    BufferVulkan* positionsBuffer = bufferManager->getBuffer(container.m_containerBufferRefs.positionsBufferRef);
    BufferVulkan* scalesBuffer = bufferManager->getBuffer(container.m_containerBufferRefs.scalesBufferRef);
    BufferVulkan* velocitiesBuffer = bufferManager->getBuffer(container.m_containerBufferRefs.velocitiesBufferRef);
    BufferVulkan* colorsBuffer = bufferManager->getBuffer(container.m_containerBufferRefs.colorsBufferRef);
    
    ParticleContainer::ContainerRef actualAddressBDA {};
    actualAddressBDA.lifetimeBufferRef = lifetimeBuffer->getAddress();
    actualAddressBDA.positionsBufferRef = positionsBuffer->getAddress();
    actualAddressBDA.scalesBufferRef = scalesBuffer->getAddress();
    actualAddressBDA.velocitiesBufferRef = velocitiesBuffer->getAddress();
    actualAddressBDA.colorsBufferRef = colorsBuffer->getAddress();

    m_containerRefs.emplace_back(actualAddressBDA);

    for(int i = 0; i < container.m_positions.size(); i++) {
        float x = Random::GenFloat(minSpacing.x, maxSpacing.x);
        float y = Random::GenFloat(minSpacing.y, maxSpacing.y);
        float z = Random::GenFloat(minSpacing.z, maxSpacing.z);
        
        container.m_positions[i] = glm::vec3(x, y ,z);
    }

    container._updateBufferReferences();
    bufferManager->updateBufferDeviceAddress(m_containerRefsBDA, m_containerRefs.data(),  MAX_CONTAINERS * sizeof(ParticleContainer::ContainerRef));

    return _assignID();
}
