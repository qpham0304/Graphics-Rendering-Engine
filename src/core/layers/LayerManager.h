#pragma once

#include <vector>
#include <stack>
#include "Layer.h"
#include <FrameBuffer.h>
#include <memory>

class LayerManager
{
private:
	static std::unordered_map<std::string,std::shared_ptr<FrameBuffer>> frameBuffers;
	std::vector<Layer*> m_Layers;
	int m_SelectedLayer;

	bool boundCheck(const int& index);


public:
	LayerManager() = default;
	~LayerManager();

	Layer& operator[](const int&& index);

	static bool addFrameBuffer(const std::string& name, FrameBuffer& FBO);
	static std::shared_ptr<FrameBuffer> getFrameBuffer(const std::string name);

	bool AddLayer(Layer* layer);
	bool RemoveLayer(const int&& index);
	void EnableLayer(const int&& index);
	void DisableLayer(const int&& index);
	const std::string& CurrentLayer();

	//std::vector<Layer*>::iterator begin();
	//std::vector<Layer*>::iterator end();
	//std::vector<Layer*>::reverse_iterator rbegin();
	//std::vector<Layer*>::reverse_iterator rend();

	const std::vector<Layer*>::const_iterator begin() const;
	const std::vector<Layer*>::const_iterator end() const;
	const std::vector<Layer*>::const_reverse_iterator rbegin() const;
	const std::vector<Layer*>::const_reverse_iterator rend() const;
};