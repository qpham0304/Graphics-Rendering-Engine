#pragma once

#include <vector>
#include "Layer.h"
#include <stack>
class LayerManager
{
private:
	std::stack<Layer*> m_Layers;
	std::stack<Layer*> m_RecentLayers; // recently poped

public:
	void AddLayer();
	void RemoveLayer();
	std::string CurrentLayer();
};