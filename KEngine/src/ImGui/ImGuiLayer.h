#include "Layer.h"

namespace KEngine {

	class KE_API ImGuiLayer : public Layer {

	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override ;
		virtual void OnDetach() override ;
		virtual void OnUpdate() override ;

		void ImGuiBegin();
		void ImGuiEnd();
		virtual void ImGuiRender() override ;
	private:
		float m_Time = 0.0f;
	};

}