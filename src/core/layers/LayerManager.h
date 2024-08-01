#pragma once

#include <vector>
#include <stack>
#include "Layer.h"
#include <FrameBuffer.h>
#include <memory>

class LayerManager
{
private:
	std::vector<Layer*> m_Layers;
	int m_SelectedLayer;
	static std::unordered_map<std::string,std::shared_ptr<FrameBuffer>> frameBuffers;

public:
	LayerManager() = default;
	~LayerManager();

	static bool addFrameBuffer(const std::string& name, FrameBuffer& FBO);
	static std::shared_ptr<FrameBuffer> getFrameBuffer(const std::string name);


	void AddLayer(Layer* layer);
	void RemoveLayer(Layer* layer);
	const std::string& CurrentLayer();

	std::vector<Layer*>::iterator begin();
	std::vector<Layer*>::iterator end();
	std::vector<Layer*>::reverse_iterator rbegin();
	std::vector<Layer*>::reverse_iterator rend();

};