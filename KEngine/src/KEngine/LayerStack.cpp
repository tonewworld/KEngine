#include "kepch.h"
#include "LayerStack.h"

namespace KEngine
{
	LayerStack::LayerStack()
	{
		m_LayerInsertPoint = m_Layers.begin();
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_LayerInsertPoint=m_Layers.emplace(m_LayerInsertPoint, layer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{	
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if(it != m_Layers.end())
		{
			m_Layers.erase(it);
			m_LayerInsertPoint--;//这里是为什么,我感觉不用移动这个指针，这个指针应该只要在插入的时候改变就行了
		}	
	}

	void LayerStack::PushOverlayer(Layer* overlayer)
	{
		m_Layers.emplace_back(overlayer);
	}

	void LayerStack::PopOverlayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if(it != m_Layers.end())
		{
			m_Layers.erase(it);
		}
	}

}
