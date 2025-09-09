#include "Event.h"


namespace KEngine {
	
	class KeyEvent : public Event {
	public:
	
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

		//这里要有个keycode
			int GetKeyCode() { return m_KeyCode; }

	protected:
		KeyEvent(unsigned int keycode) :m_KeyCode(keycode) {}//这里为什么要protected 防止实例化实现工厂模式
		unsigned int m_KeyCode;

	};
	class KeyPressedEvent : public KeyEvent {
	public:

		KeyPressedEvent(int keycode, bool isRepeat=false)
			:KeyEvent(keycode), m_IsRepeat(isRepeat) {
		}

		std::string ToString() const override {
			return GetName() + std::to_string(m_KeyCode) + ": (" + (m_IsRepeat ? "repeats" : "no repeats") + ")";
		}


		EVENT_CLASS_TYPE(KeyPressed)
	private:
		bool m_IsRepeat = false;
	
	};

	class KeyReleasedEvent : public KeyEvent {
	
		public:
		KeyReleasedEvent(int keycode)
			:KeyEvent(keycode) {
		}
		
		EVENT_CLASS_TYPE(KeyReleased)

		std::string ToString() const override {
			return GetName() + std::to_string(m_KeyCode) ;
		}

	};

	class KeyTypedEvent : public KeyEvent {
		
		public:
			KeyTypedEvent(int keycode) 
				:KeyEvent(keycode){
			}
		std::string ToString() const override {
			return GetName() + std::to_string(m_KeyCode);
		}
		EVENT_CLASS_TYPE(KeyTyped)
		
	};

}