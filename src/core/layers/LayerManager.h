#pragma once

#include <vector>
#include <stack>
#include "Layer.h"

class LayerManager
{
private:
	std::vector<Layer*> m_Layers;
	int m_SelectedLayer;

public:
	LayerManager() = default;
	~LayerManager();

	void AddLayer(Layer* layer);
	void RemoveLayer(Layer* layer);
	const std::string& CurrentLayer();

	std::vector<Layer*>::iterator begin();
	std::vector<Layer*>::iterator end();
	std::vector<Layer*>::reverse_iterator rbegin();
	std::vector<Layer*>::reverse_iterator rend();

	std::vector<Layer*>::const_iterator begin() const;
	std::vector<Layer*>::const_iterator end() const;
	std::vector<Layer*>::const_reverse_iterator rbegin() const;
	std::vector<Layer*>::const_reverse_iterator rend() const;
};