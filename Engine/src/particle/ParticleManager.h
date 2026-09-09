#include "core/resources/managers/Manager.h"
#include "ParticleContainer.h"

class ParticleManager : public Manager
{
public:
	ParticleManager();	
	virtual ~ParticleManager();

	virtual bool init(WindowConfig config) override;
    virtual bool onClose() override;
	virtual void destroy(uint32_t id) override;
	virtual std::vector<uint32_t> listIDs() const override;
    virtual void onUpdate() override;

	uint32_t getContainerRef() const;	// TODO: move BDA to rendererVulkan so as ParticleContainer
	ParticleContainer getContainer(uint32_t id);
	uint32_t createContainer(uint32_t size, glm::vec3 minSpacing, glm::vec3 maxSpacing);

private:

    const uint32_t MAX_CONTAINERS = 100;
	std::vector<ParticleContainer> m_containers { {} };
	std::vector<ParticleContainer::ContainerRef> m_containerRefs { {} };
	std::vector<uint32_t> m_toBeDestroyed;
	uint64_t m_containerRefsBDA;

};