#include "kepch.h"
#include "Renderer.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "Application.h"
namespace KEngine{
	void Renderer::Init()
	{
		RenderCommand::Init();

	}
	void Renderer::BeginScene()
	{
		RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		RenderCommand::Clear();

	}
	void Renderer::EndScene()
	{
		
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Mesh>& mesh)
	{

		shader->Bind();
		mesh->VAO->Bind();
		RenderCommand::DrawIndexed(mesh->VAO);
	}
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Model>& model)
	{
		shader->Bind();
		if (model->texture)
			model->texture->Bind();
		for (unsigned int i = 0; i < model->meshes.size(); i++)
			Submit(shader, std::make_shared<Mesh>(model->meshes[i]));
	}
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Light>& light)
	{

		shader->Bind();
		light->VAO->Bind();
		RenderCommand::DrawIndexed(light->VAO);
	}
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Object>& object)
	{
		if (Mesh* m = dynamic_cast<Mesh*>(object.get()))
		{
			std::shared_ptr<Mesh> ptr(m, [object](Mesh*) {}); // 不删除，只共享引用计数
			Submit(shader, ptr);
			return;
		}
		if (Model* md = dynamic_cast<Model*>(object.get()))
		{
			std::shared_ptr<Model> ptr(md, [object](Model*) {});
			Submit(shader, ptr);
			return;
		}
		if (Light* md = dynamic_cast<Light*>(object.get()))
		{
			std::shared_ptr<Light> ptr(md, [object](Light*) {});
			Submit(shader, ptr);
			return;
		}
	}


	void Renderer::SetStencilOpenOrClose(bool tag) {
		RenderCommand::SetStencilOpenOrClose(tag);
	}
	void Renderer::SetStencilFunc(GLenum func, GLint ref, GLuint mask)
	{
		RenderCommand::SetStencilFunc(func, ref, mask);
	}
	void Renderer::SetStencilMask(GLint tag) {
		RenderCommand::SetStencilMask(tag);
	}
	void Renderer::SetDepthOpenOrClose(bool tag) {
		RenderCommand::SetDepthOpenOrClose(tag);
	}
	void Renderer::SwitchFrameBuffer(unsigned int frameBuffer) {
		RenderCommand::SwitchFrameBuffer(frameBuffer);
	}

	std::array<unsigned char, 4> Renderer::ReadPixel(int rx ,int ry)
	{
		std::array<unsigned char, 4> pixel;
		glReadPixels(rx, ry, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel.data());
		return pixel;
	}

	void Renderer::ColorPickBegin()
	{
		RenderCommand::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);                             
		glDepthFunc(GL_LEQUAL);
	}
	void Renderer::ParallelLightShadowBegin()
	{
		RenderCommand::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glCullFace(GL_FRONT);
	}

	void Renderer::ParallelLightShadowEnd()
	{
		glCullFace(GL_BACK);

	}
	void Renderer::PointLightShadowBegin()
	{
		RenderCommand::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glViewport(0, 0, 1024, 1024);//hack
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glCullFace(GL_FRONT);
	}

	void Renderer::PointLightShadowEnd()
	{
		glCullFace(GL_BACK);
		int w = KEngine::Application::s_Instance->GetWindow().GetWidth();
		int h = KEngine::Application::s_Instance->GetWindow().GetHeight();
		glViewport(0, 0, w, h);
		
	}
	
	void Renderer::ResetGLState() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 重置纹理绑定
		/*for (int i = 0; i < 8; i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}*/

		// 重置其他OpenGL状态
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);

		// 重置视口
		int width = KEngine::Application::s_Instance->GetWindow().GetWidth();
		int height = KEngine::Application::s_Instance->GetWindow().GetHeight();
		glViewport(0, 0, width, height);

		// 清除错误状态
		while (glGetError() != GL_NO_ERROR);
	}
	void Renderer::Debug()
	{
		unsigned int debugCube;
		glGenTextures(1, &debugCube);
		glBindTexture(GL_TEXTURE_CUBE_MAP, debugCube);

		const int size = 1024;
		const int count = size * size;
		std::vector<float> color(count); 

		for (int i = 0; i < 6; ++i) {
			std::fill(color.begin(), color.end(), 0.5f); 
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F,
				size, size, 0, GL_RED, GL_FLOAT, color.data());
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, debugCube);
	}
}