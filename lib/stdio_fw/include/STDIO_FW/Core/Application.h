#pragma once

struct GLFWwindow;

namespace stdio_fw
{
	class Graphics;
}
namespace stdio_fw
{
	class Application
	{
		GLFWwindow*			m_pWindow;		
		Graphics*			m_pGraphics;

		char				m_sWindowTitle[64];

	public:
		Application();
		virtual ~Application();

		// Init applicaion - use OpenGL 2.1
		// [in] screenW: screen width
		// [in] screenH: screen height
		// [in] title: game title
		virtual ErrorCode	Init(int screenW, int screenH, const char* title);

		// Update function, must be override
		// [in] deltaTime: time between 2 frames
		virtual void		Update(float deltaTime) = 0;

		virtual void		Render(Graphics* g) = 0;
		virtual void		Exit() = 0;

		// Get key state
		// [in] Key code
		// [out] State of key
		KeyState			getKeyState(KeyCode key);
		Graphics*			getGraphics();

		void				Run();

	protected:
		bool				m_isRunning;
	};
}
