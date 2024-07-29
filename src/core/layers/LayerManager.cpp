#include "LayerManager.h"

void LayerManager::AddLayer()
{

}

void LayerManager::RemoveLayer()
{
	m_RecentLayers.push(m_Layers.top());
	m_Layers.pop();
}

std::string LayerManager::CurrentLayer()
{
	return m_Layers.top()->getName();
}
