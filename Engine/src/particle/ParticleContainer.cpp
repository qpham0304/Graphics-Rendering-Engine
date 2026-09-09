#include "ParticleContainer.h"
#include "core/resources/managers/BufferManager.h"
#include "core/features/ServiceLocator.h"

ParticleContainer::ParticleContainer(uint32_t size) : m_size(size)
{
    _allocate(size);
}

ParticleContainer::ParticleContainer(uint32_t size, uint32_t rows, uint32_t cols)
    : m_size(size), m_numRows(rows), m_numCols(cols)
{
    _allocate(size);
}

void ParticleContainer::_allocate(uint32_t size)
{
    m_lifetime.resize(size, 1.0);
    m_positions.resize(size, glm::vec3(1.0));
    m_scales.resize(size, glm::vec3(0.5));
    m_velocities.resize(size, glm::vec3(0.5));
    m_colors.resize(size, glm::vec4(1.0, 0.5, 1.0, 1.0));
    
    //TODO: move to particle renderer to let them create the BDA
    _createBufferReferences();
    _updateBufferReferences();
}

void ParticleContainer::_createBufferReferences()
{
    size_t lifetimeSize = m_lifetime.size() * sizeof(double);
    size_t positionsSize = m_positions.size() * sizeof(glm::vec3);
    size_t scalesSize = m_scales.size() * sizeof(glm::vec3);
    size_t velocitiesSize = m_velocities.size() * sizeof(glm::vec3);
    size_t colorsSize = m_colors.size() * sizeof(glm::vec4);

    BufferManager* bufferManager = &ServiceLocator::GetService<BufferManager>("BufferManagerVulkan");
    m_containerBufferRefs.lifetimeBufferRef = bufferManager->createBufferDeviceAddress(lifetimeSize);
    m_containerBufferRefs.positionsBufferRef = bufferManager->createBufferDeviceAddress(positionsSize);
    m_containerBufferRefs.scalesBufferRef = bufferManager->createBufferDeviceAddress(scalesSize);
    m_containerBufferRefs.velocitiesBufferRef = bufferManager->createBufferDeviceAddress(velocitiesSize);
    m_containerBufferRefs.colorsBufferRef = bufferManager->createBufferDeviceAddress(colorsSize);
}

// NOTE: set once and done on the cpu, only compute shader is allowed to modify these values onward
void ParticleContainer::_updateBufferReferences()
{
    size_t lifetimeSize = m_lifetime.size() * sizeof(double);
    size_t positionsSize = m_positions.size() * sizeof(glm::vec3);
    size_t scalesSize = m_scales.size() * sizeof(glm::vec3);
    size_t velocitiesSize = m_velocities.size() * sizeof(glm::vec3);
    size_t colorsSize = m_colors.size() * sizeof(glm::vec4);

    BufferManager* bufferManager = &ServiceLocator::GetService<BufferManager>("BufferManagerVulkan");
    bufferManager->updateBufferDeviceAddress(m_containerBufferRefs.lifetimeBufferRef, m_lifetime.data(), lifetimeSize);
    bufferManager->updateBufferDeviceAddress(m_containerBufferRefs.positionsBufferRef, m_positions.data(), positionsSize);
    bufferManager->updateBufferDeviceAddress(m_containerBufferRefs.scalesBufferRef, m_scales.data(), scalesSize);
    bufferManager->updateBufferDeviceAddress(m_containerBufferRefs.velocitiesBufferRef, m_velocities.data(), velocitiesSize);
    bufferManager->updateBufferDeviceAddress(m_containerBufferRefs.colorsBufferRef, m_colors.data(), colorsSize);
}
