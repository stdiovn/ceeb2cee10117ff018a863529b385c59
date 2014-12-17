#pragma once

namespace stdio_fw
{
	class Image
	{
		friend class Graphics;
		char		m_imgPath[256];
		uint		m_texID;

		uint		m_imgWidth;
		uint		m_imgHeight;
		uint		m_imgBPP;

		bool		m_isLoaded;
		bool		m_useMipmap;

	public:
		Image(const char* path, bool useMipmap = false);
		~Image();

		ErrorCode			loadImage();
		void				unloadImage();

		uint		getWidth() { return m_imgWidth; };
		uint		getHeight() { return m_imgHeight; };

	private:
		byte*		loadImageData(uint &pow2w, uint &pow2h);
		uint		roundUpPow2(uint n);
	};
}
