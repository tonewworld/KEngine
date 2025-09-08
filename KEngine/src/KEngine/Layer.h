#pragma once
#include "KEngine/Core.h"
#include "KEngine/Events/Event.h"

namespace KEngine{
	class KE_API Layer
	{
	public :
		Layer(const std::string& m_DebugName);
		virtual ~Layer()=default;

		virtual void OnEvent(Event&event){}
		
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}

		const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};

}

