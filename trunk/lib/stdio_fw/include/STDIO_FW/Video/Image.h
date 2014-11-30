#pragma once

namespace stdio_fw
{
	class Image
	{
		friend class Graphics;
		char				m_imgPath[256];
		unsigned int		m_texID;

		unsigned int		m_imgWidth;
		unsigned int		m_imgHeight;

		bool				m_isLoaded;

	public:
		Image(const char* path);
		~Image();

		ErrorCode			loadImage();
		void				unloadImage();

		unsigned int		getWidth() { return m_imgWidth; };
		unsigned int		getHeight() { return m_imgHeight; };

	private:
		unsigned char*		loadImageData();
	};
}
