#include "KEngine/Layer.h"



namespace KEngine {

	class KE_API ImGuiLayer : public Layer {

	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnEvent(Event& event);
		void OnAttach();
		void OnDetach();
		void OnUpdate();

	private:
		float m_Time = 0.0f;
	};

}