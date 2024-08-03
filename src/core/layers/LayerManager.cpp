#include "LayerManager.h"

#define OUT_OF_BOUND_ERROR(index) { \
    std::string msg = "Index out of bound: \"" + std::to_string(index) + "\""; \
    throw std::runtime_error(msg); \
}

#define FRAME_NOT_FOUND_ERROR(name) { \
    std::string msg = "Framebuffer does not exist: \"" + name + "\""; \
    throw std::runtime_error(msg); \
}


std::unordered_map<std::string, std::shared_ptr<FrameBuffer>> LayerManager::frameBuffers = {};

bool LayerManager::boundCheck(const int& index) {
	return !(index < 0 || index >= m_Layers.size());
}

LayerManager::~LayerManager()
{
	for (Layer* layer : m_Layers) {
		delete layer;
	}
}

Layer& LayerManager::operator[](const int&& index)
{
	if (!boundCheck(index)) {
		OUT_OF_BOUND_ERROR(index);
	}
	return *m_Layers[index];
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
	//FRAME_NOT_FOUND_ERROR(name);
	return nullptr;
}

bool LayerManager::AddLayer(Layer* layer)
{
	layer->OnAttach();
	m_Layers.push_back(layer);
	return true;
}

void LayerManager::RemoveLayer(const int&& index)
{
	//layer->OnDetach();
	if (!boundCheck(index)) {
		OUT_OF_BOUND_ERROR(index);
	}
	m_Layers.pop_back();
}

void LayerManager::EnableLayer(const int&& index)
{
	if (!boundCheck(index)) {
		OUT_OF_BOUND_ERROR(index);
	}
	m_Layers[index]->m_Enabled = true;
}

void LayerManager::DisableLayer(const int&& index)
{
	if (!boundCheck(index)) {
		OUT_OF_BOUND_ERROR(index);
	}
	m_Layers[index]->m_Enabled = false;
}

const std::string& LayerManager::CurrentLayer()
{
	return m_Layers[m_SelectedLayer]->GetName();
}

//std::vector<Layer*>::iterator LayerManager::begin()
//{
//	return m_Layers.begin();
//}
//
//std::vector<Layer*>::iterator LayerManager::end()
//{
//	return m_Layers.end();
//}

const std::vector<Layer*>::const_iterator LayerManager::begin() const
{
	return m_Layers.begin();
}

const std::vector<Layer*>::const_iterator LayerManager::end() const
{
	return m_Layers.end();
}

//std::vector<Layer*>::reverse_iterator LayerManager::rbegin()
//{
//	return m_Layers.rbegin();
//}
//
//std::vector<Layer*>::reverse_iterator LayerManager::rend()
//{
//	return m_Layers.rend();
//}

const std::vector<Layer*>::const_reverse_iterator LayerManager::rbegin() const
{
	return m_Layers.rbegin();
}

const std::vector<Layer*>::const_reverse_iterator LayerManager::rend() const
{
	return m_Layers.rend();
}
