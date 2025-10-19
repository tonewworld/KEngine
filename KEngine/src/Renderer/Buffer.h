#pragma once 
#include "Core.h"
#include "Log.h"
#include "glm.hpp"

namespace KEngine{

    enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static unsigned int ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:    return 4;
			case ShaderDataType::Float2:   return 4 * 2;
			case ShaderDataType::Float3:   return 4 * 3;
			case ShaderDataType::Float4:   return 4 * 4;
			case ShaderDataType::Mat3:     return 4 * 3 * 3;
			case ShaderDataType::Mat4:     return 4 * 4 * 4;
			case ShaderDataType::Int:      return 4;
			case ShaderDataType::Int2:     return 4 * 2;
			case ShaderDataType::Int3:     return 4 * 3;
			case ShaderDataType::Int4:     return 4 * 4;
			case ShaderDataType::Bool:     return 1;
		}
		return 0;
	}


    struct BufferElement {
        std::string name;
        ShaderDataType type;
        unsigned int size;
		unsigned int offset;
        bool normalized;
        BufferElement() {}
        BufferElement(const ShaderDataType& type,const std::string& name , bool normalized = false)
			: type(type),name(name) , size(ShaderDataTypeSize(type)), offset(0), normalized(normalized) {
		}
        uint32_t GetComponentCount() const
		{
			switch (type)
			{
				case ShaderDataType::Float:   return 1;
				case ShaderDataType::Float2:  return 2;
				case ShaderDataType::Float3:  return 3;
				case ShaderDataType::Float4:  return 4;
				case ShaderDataType::Mat3:    return 3 * 3;
				case ShaderDataType::Mat4:    return 4 * 4;
				case ShaderDataType::Int:     return 1;
				case ShaderDataType::Int2:    return 2;
				case ShaderDataType::Int3:    return 3;
				case ShaderDataType::Int4:    return 4;
				case ShaderDataType::Bool:    return 1;
			}
			return 0;
		}
    };
    class BufferLayout {
    public:
        BufferLayout() { }
        BufferLayout(const std::initializer_list<BufferElement>& elements):m_Elements(elements) {
			CalculateOffsetsAndStride();
        }
        inline unsigned int GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
    private:
		void CalculateOffsetsAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.offset = offset;
				offset += element.size;
				m_Stride += element.size;
			}
		}
        std::vector<BufferElement> m_Elements;
        unsigned int m_Stride=0;
    };
	

    class KE_API VertexBuffer{
    public :
        VertexBuffer()=default;
        virtual ~VertexBuffer() = default;
        virtual void Bind() const =0;
        virtual void Unbind() const =0;
        static VertexBuffer* Create(const std::vector<float>&data);
		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;
    };
    class KE_API IndexBuffer{
    public :
        IndexBuffer()=default;
        virtual ~IndexBuffer() = default;
        virtual void Bind() const =0;
        virtual void Unbind() const =0;
        virtual unsigned int GetCount() const =0;
        static IndexBuffer* Create(std::vector<unsigned int>);
    };
	class KE_API FrameBuffer
	{
	public:
		FrameBuffer()=default;
		virtual ~FrameBuffer() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		static FrameBuffer* Create();
		virtual void AddTexture(unsigned int textureID) = 0;
	};
	class KE_API RenderBuffer
	{
	public:
		RenderBuffer() = default;
		virtual ~RenderBuffer() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		static RenderBuffer* Create();
	};

	class KE_API UniformBuffer
	{
	public:
		UniformBuffer() = default;
		virtual ~UniformBuffer() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void AddVPMatrix(glm::mat4& view, glm::mat4& proj, std::size_t offset = 0) = 0;
		//这里要输入size
		static UniformBuffer* Create(unsigned int size);
	};

}