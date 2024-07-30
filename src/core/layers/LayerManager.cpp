#include "LayerManager.h"

LayerManager::~LayerManager()
{
	for (Layer* layer : m_Layers) {
		delete layer;
	}
}

void LayerManager::AddLayer(Layer* layer)
{
	layer->OnAttach();
	m_Layers.push_back(layer);
}

void LayerManager::RemoveLayer(Layer* layer)
{
	layer->OnDetach();
	m_Layers.pop_back();
}

const std::string& LayerManager::CurrentLayer()
{
	return m_Layers[m_SelectedLayer]->GetName();
}

std::vector<Layer*>::iterator LayerManager::begin()
{
	return m_Layers.begin();
}

std::vector<Layer*>::iterator LayerManager::end()
{
	return m_Layers.end();
}

std::vector<Layer*>::reverse_iterator LayerManager::rbegin()
{
	return m_Layers.rbegin();
}

std::vector<Layer*>::reverse_iterator LayerManager::rend()
{
	return m_Layers.rend();
}

std::vector<Layer*>::const_iterator LayerManager::begin() const
{
	return m_Layers.begin();
}

std::vector<Layer*>::const_iterator LayerManager::end() const
{
	return m_Layers.end();
}

std::vector<Layer*>::const_reverse_iterator LayerManager::rbegin() const
{
	return m_Layers.rbegin();
}

std::vector<Layer*>::const_reverse_iterator LayerManager::rend() const
{
	return m_Layers.rend();
}
