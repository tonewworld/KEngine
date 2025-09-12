#pragma once
#include "Core.h"
#include "Layer.h"
namespace KEngine {
	class KE_API LayerStack
	{
	public:
		
		LayerStack();
		~LayerStack();

		void PushLayer(Layer*layer);
		void PopLayer(Layer* layer);

		void PushOverlayer(Layer* overlayer);
		void PopOverlayer(Layer* layer);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};
}

