#pragma once

#include"../Core.h"

#include<string>




//为什么这里要不生成实例就获得它的类型
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

namespace KEngine {
	enum class EventType {
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};
	enum EventCategory {
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)//用标志位方便，因为有些事件可能属于多个类别
	};

	class Event {
	public:
		Event()=default;
		virtual~Event()=default;

		bool Handled = false;//事件是否被处理

		virtual EventType GetEventType()const = 0;//这里是为了给子类用宏重载这几个函数
		virtual int GetCategoryFlags()const = 0;
		virtual const char* GetName()const = 0;

		virtual std::string ToString()const { return GetName(); }//这个tostring有问题别用

		bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

	};

	class EventDispatcher {
	public:
		EventDispatcher(Event& event):m_Event(event) {
		}

		template<typename T, typename F>
		bool Dispatch(const F& func) {
			if (m_Event.GetEventType() == T::GetStaticType()) {
				m_Event.Handled = func(static_cast<T&>(m_Event));//这里func一定要是个bool函数吗，我感觉不一定
				return true;
			}
			return false;
		}


	private:
		Event& m_Event;

	};

	//这里重载<<出问题了
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

}