
#include <string>//删除
#include <cstdint>//删除

namespace KEngine{
    struct WindowProps{
        std::string Title;
        uint32_t Width; 
        uint32_t Height;
        WindowProps(const std::string& title="KEngine",uint32_t width=1600,uint32_t height=900)
            :Title(title),Width(width),Height(height){}

    };



    class Window{
        
    public:

        Windows()=default;
        virtual ~Window()=default;

        virtual void OnUpdate()=0;

        virtual uint32_t GetWidth() const=0; 
        virtual uint32_t GetHeight()const =0;

        virtual void SetVSync()=0;
        virtual bool IsVSync()=0;

        staic Window

        
    };
}