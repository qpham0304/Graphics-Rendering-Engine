#include "LayerManager.h"

std::unordered_map<std::string, std::shared_ptr<FrameBuffer>> LayerManager::frameBuffers = {};


LayerManager::~LayerManager()
{
	for (Layer* layer : m_Layers) {
		delete layer;
	}
}

bool LayerManager::addFrameBuffer(const std::string& name, FrameBuffer& FBO)
{
	if (frameBuffers.find(name) == frameBuffers.end()) {
		frameBuffers[name] = std::make_shared<FrameBuffer>(FBO);
		return true;
	}
	return false;
}

std::shared_ptr<FrameBuffer> LayerManager::getFrameBuffer(const std::string name)
{
	if (frameBuffers.find(name) != frameBuffers.end()) {
		return frameBuffers[name];
	}
	return nullptr;
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