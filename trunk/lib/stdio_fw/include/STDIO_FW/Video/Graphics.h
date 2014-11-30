#pragma once

#include <list>

namespace stdio_fw
{
	enum CACHED_LOC
	{		
		// Solid object
		ATRIB_POSITION0 = 0,		
		UNIFO_COLOR,
		// Image
		ATRIB_POSITION1,
		ATRIB_TEXCOORD,
		UNIFO_TEXTURE,
		// Max cached loc
		MAX_LOC
	};

	class Image;
	class Graphics
	{
		// We use 2 program: 0 for SOLID OBJECT and 1 for IMAGE
		unsigned int		m_aPrograms[2];

		// Cache attribute and uniforms location here
		int					m_cachedLocs[MAX_LOC];


		float				m_drawColor[4];
		float				m_clearColor[4];

		int					m_iScreenW;
		int					m_iScreenH;

		std::list<Mat3>		m_listMat;
	public:
		Graphics();
		~Graphics();

		ErrorCode			initGraphics(int screenW, int screenH);

		void				fillRect(int x, int y, int width, int height);
		void				fillRect(Rect rect);


		void				drawRect(int x, int y, int width, int height, int weight = 1);
		void				drawRect(Rect rect, int weight = 1);

		void				drawLine(int x1, int y1, int x2, int y2);
		void				drawLine(Vec2 p1, Vec2 p2);

		void				drawImage(Image* img, int x, int y, unsigned int flipping = 0);
		void				drawImage(Image* img, Rect rect, unsigned int flipping = 0);

		void				drawRegion(Image* img, int x, int y, int width, int height, int src_x, int src_y, int src_w, int src_h, unsigned int flipping);
		void				drawRegion(Image* img, Rect src, Rect dest, unsigned int flipping = 0);

		// Set color function: 0xRRGGBBAA
		void				setColor(unsigned int color);
		void				setClearColor(unsigned int color);

		void				cleanScreen();	

		// Push and pop matrix
		void				pushMatrix(Mat3 mat);
		void				popMatrix();

	private:
		void				draw(int x, int y, int width, int height, float *uv = nullptr, unsigned int texture_id = 0, unsigned int flipping = 0);
	};
}