#pragma once

#include <vector>

class ParticleManager;

class ParticleContainer
{
public:
	struct ContainerRef {
		uint64_t lifetimeBufferRef;
		uint64_t positionsBufferRef;
		uint64_t scalesBufferRef;
		uint64_t velocitiesBufferRef;
		uint64_t colorsBufferRef;
	};

    ParticleContainer() = default;
	ParticleContainer(uint32_t size);
	ParticleContainer(uint32_t size, uint32_t rows, uint32_t cols);

    ContainerRef m_containerBufferRefs;
    uint32_t m_textureID { 0 };
    uint32_t m_size { 0 };
    uint32_t m_numRows { 1 };
    uint32_t m_numCols { 1 };
    std::vector<double> m_lifetime {};
    std::vector<glm::vec3> m_positions {};
    std::vector<glm::vec3> m_scales {};
    std::vector<glm::vec3> m_velocities {};
    std::vector<glm::vec4> m_colors {};


private:
    friend class ParticleManager;

	void _allocate(uint32_t size);
    void _createBufferReferences();
    void _updateBufferReferences();


};