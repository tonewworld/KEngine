#include "../Window.h"

namespace KEngine{

    class WindowsWindow:public Window{
    public:
        struct WindowData{
            uint32_t width;
            uint32_t height;
            bool Vsync;

        };


        WindowsWindow(const WindowProps&props);

        virtual~WindowsWindow();

        void OnUpdate()override;

        uint32_t GetWidth() const override{return myData.width;}; 
        uint32_t GetHeight()const override{return myData.height;};

        void SetVSync(bool sign) override;
        bool IsVSync() override;

    private:
        WindowData myData;

        virtual void Init(const WindowProps& props);
        virtual void Shutdown();

    };


}