#include "stdio_fw.h"

#include "GL\glew.h"
#include "Video\Graphics.h"
#include "Video\Image.h"

namespace stdio_fw
{
	GLuint createProgram(const char* vertex_src, const char* fragment_src);
	GLuint loadShader(GLenum type, const char* src);

	Graphics::Graphics()
	{

	}

	Graphics::~Graphics()
	{

	}

	ErrorCode Graphics::initGraphics(int screenW, int screenH)
	{
		m_iScreenW = screenW;
		m_iScreenH = screenH;

		const char* solid_obj_vertex_shader_src =
			"#version 120\n"
			"attribute vec2 a_pos;\n"			
			"void main()\n"
			"{\n"			
			"gl_Position = vec4(a_pos, 1.0, 1.0);\n"
			"}\n";

		const char* solid_obj_fragment_shader_src = 
			"#version 120\n"
			"uniform vec4 u_color;\n"
			"void main()\n"
			"{\n"
			"gl_FragColor = u_color;\n"
			"}\n";

		m_aPrograms[0] = createProgram(solid_obj_vertex_shader_src, solid_obj_fragment_shader_src);

		m_cachedLocs[CACHED_LOC::ATRIB_POSITION0] = glGetAttribLocation(m_aPrograms[0], "a_pos");		
		m_cachedLocs[CACHED_LOC::UNIFO_COLOR] = glGetUniformLocation(m_aPrograms[0], "u_color");

		// For image
		const char* img_vertex_shader_src =
			"#version 120\n"
			"attribute vec2 a_pos;\n"
			"attribute vec2 a_uv;\n"
			"uniform mat3 u_mat;\n"
			"varying vec2 v_uv;\n"
			"void main()\n"
			"{\n"
			"gl_Position = vec4(a_pos, 1.0, 1.0);\n"
			"v_uv = a_uv;\n"
			"}\n";

		const char* img_fragment_shader_src =
			"#version 120\n"
			"uniform sampler2D u_tex;\n"
			"varying vec2 v_uv;\n"
			"void main()\n"
			"{\n"
			"gl_FragColor = texture2D(u_tex, v_uv);\n"
			"}\n";

		m_aPrograms[1] = createProgram(img_vertex_shader_src, img_fragment_shader_src);
		
		m_cachedLocs[CACHED_LOC::ATRIB_POSITION1] = glGetAttribLocation(m_aPrograms[1], "a_pos");		
		m_cachedLocs[CACHED_LOC::ATRIB_TEXCOORD] = glGetAttribLocation(m_aPrograms[1], "a_uv");
		m_cachedLocs[CACHED_LOC::UNIFO_TEXTURE] = glGetUniformLocation(m_aPrograms[1], "u_tex");

		return ErrorCode::ERR_NO_ERROR;
	}

	void Graphics::fillRect(int x, int y, int width, int height)
	{
		draw(x, y, width, height);
	}

	void Graphics::fillRect(Rect rect)
	{
		draw(rect.x, rect.y, rect.width, rect.height);
	}

	void Graphics::drawRect(int x, int y, int width, int height, int weight)
	{
		//Top
		fillRect(x, y, width, weight);
		//Bottom
		fillRect(x, y + height - weight, width, weight);
		//Left
		fillRect(x, y + weight, weight, height - (weight << 1));
		//Right
		fillRect(x + width - weight, y + weight, weight, height - (weight << 1));
	}

	void Graphics::drawRect(Rect rect, int weight)
	{
		drawRect(rect.x, rect.y, rect.width, rect.height, weight);
	}

	void Graphics::drawImage(Image* img, int x, int y, unsigned int flipping)
	{
		draw(x, y, img->getWidth(), img->getHeight(), nullptr, img->m_texID, flipping);
	}

	void Graphics::drawImage(Image* img, Rect rect, unsigned int flipping)
	{
		draw(rect.x, rect.y, rect.width, rect.height, nullptr, img->m_texID, flipping);
	}

	void Graphics::drawRegion(Image* img, int x, int y, int width, int height, int src_x, int src_y, int src_w, int src_h, unsigned int flipping)
	{
		float uv[]
		{
			X2UVGL(src_x, img->getWidth()), Y2UVGL(src_y, img->getHeight()),
			X2UVGL(src_x, img->getWidth()), Y2UVGL(src_y + src_h, img->getHeight()),
			X2UVGL(src_x + src_w, img->getWidth()), Y2UVGL(src_y + src_h, img->getHeight()),
			X2UVGL(src_x, img->getWidth()), Y2UVGL(src_y, img->getHeight()),
			X2UVGL(src_x + src_w, img->getWidth()), Y2UVGL(src_y + src_h, img->getHeight()),
			X2UVGL(src_x + src_w, img->getWidth()), Y2UVGL(src_y, img->getHeight())
		};
		draw(x, y, width, height, uv, img->m_texID, flipping);
	}

	void Graphics::drawRegion(Image* img, Rect src, Rect dest, unsigned int flipping)
	{
		drawRegion(img, src.x, src.y, src.width, src.height, dest.x, dest.y, dest.width, dest.height, flipping);
	}

	void Graphics::drawLine(int x1, int y1, int x2, int y2)
	{
		// Compute vertices array
		float vertices[] = {
			XSCREEN2GL(x1, m_iScreenW), YSCREEN2GL(y1, m_iScreenH),
			XSCREEN2GL(x2, m_iScreenW), YSCREEN2GL(y2, m_iScreenH)
		};
		
		glUseProgram(m_aPrograms[0]);

		// Transfer data to verties
		GLint posLoc = m_cachedLocs[CACHED_LOC::ATRIB_POSITION0];
		if (posLoc != -1)
		{
			glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, vertices);
			glEnableVertexAttribArray(posLoc);
		}

		GLint colorUniLoc = m_cachedLocs[CACHED_LOC::UNIFO_COLOR];
		if (colorUniLoc != -1)
		{
			glUniform4fv(colorUniLoc, 1, &m_drawColor[0]);
		}

		glDrawArrays(GL_LINES, 0, 2);
	}

	void Graphics::drawLine(Vec2 p1, Vec2 p2)
	{
		drawLine(p1.x, p1.y, p2.x, p2.y);
	}

	void Graphics::draw(int x, int y, int width, int height, float *uv, unsigned int texture_id, unsigned int flipping)
	{
		//enable blend
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Compute vertices value
		Vec3 v1(x, y, 1.0f);
		Vec3 v2(x + width, y, 1.0f);
		Vec3 v3(x, y + height, 1.0f);
		Vec3 v4(x + width, y + height, 1.0f);
		if (m_listMat.empty() == false)
		{
			Mat3 finalMat;
			finalMat.SetIdentity();

			while (m_listMat.size() > 0)
			{
				finalMat = finalMat * m_listMat.front();
				m_listMat.pop_front();
			}

			v1 = v1 * finalMat;
			v2 = v2 * finalMat;
			v3 = v3 * finalMat;
			v4 = v4 * finalMat;
		}

		// Compute vertices array
		float vertices[] = {
			XSCREEN2GL(v1.x, m_iScreenW), YSCREEN2GL(v1.y, m_iScreenH),
			XSCREEN2GL(v3.x, m_iScreenW), YSCREEN2GL(v3.y, m_iScreenH),
			XSCREEN2GL(v4.x, m_iScreenW), YSCREEN2GL(v4.y, m_iScreenH),
			XSCREEN2GL(v1.x, m_iScreenW), YSCREEN2GL(v1.y, m_iScreenH),
			XSCREEN2GL(v4.x, m_iScreenW), YSCREEN2GL(v4.y, m_iScreenH),
			XSCREEN2GL(v2.x, m_iScreenW), YSCREEN2GL(v2.y, m_iScreenH)
		};

		GLint activeProgram = 0;
		if (texture_id <= 0)	// We draw solid object
			activeProgram = m_aPrograms[0];
		else
		{
			activeProgram = m_aPrograms[1];

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_id);
			glUniform1i(m_cachedLocs[CACHED_LOC::UNIFO_TEXTURE], 0);
		}
		
		glUseProgram(activeProgram);

		// Transfer data to verties
		GLint posLoc = texture_id == 0 ? m_cachedLocs[CACHED_LOC::ATRIB_POSITION0] : m_cachedLocs[CACHED_LOC::ATRIB_POSITION1];
		if (posLoc != -1)
		{
			glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, vertices);
			glEnableVertexAttribArray(posLoc);
		}

		GLint uvLoc = m_cachedLocs[CACHED_LOC::ATRIB_TEXCOORD];
		if (uvLoc != -1)
		{
			float default_uv[] = {
				0.0f, 1.0f,
				0.0f, 0.0f,
				1.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 0.0f,
				1.0f, 1.0f
			};
			if (uv == nullptr)
				uv = default_uv;

			for (int i = 0; i < 12; i += 2)
			{
				if (flipping & FLIP_X)
					uv[i] -= 1.0f;
				
				if (flipping & FLIP_Y)
					uv[i+1] -= 1.0f;
			}

			glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, 0, uv);
			glEnableVertexAttribArray(uvLoc);
		}

		// Transfer uniform
		GLint colorUniLoc = m_cachedLocs[CACHED_LOC::UNIFO_COLOR];
		if (colorUniLoc != -1)
		{
			glUniform4fv(colorUniLoc, 1, &m_drawColor[0]);
		}		

		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Disable blend
		glDisable(GL_BLEND);
	}

	void Graphics::setColor(unsigned int color)
	{
		// [0000 0000] [0000 0000] [0000 0000] [0000 0000]
		m_drawColor[0] = ((color >> 24) & 0xFF) / 255.0f;
		m_drawColor[1] = ((color >> 16) & 0xFF) / 255.0f;
		m_drawColor[2] = ((color >> 8) & 0xFF) / 255.0f;
		m_drawColor[3] = (color & 0xFF) / 255.0f;
		
	}

	void Graphics::setClearColor(unsigned int color)
	{
		// [0000 0000] [0000 0000] [0000 0000] [0000 0000]
		m_clearColor[0] = ((color >> 24) & 0xFF) / 255.0f;
		m_clearColor[1] = ((color >> 16) & 0xFF) / 255.0f;
		m_clearColor[2] = ((color >> 8) & 0xFF) / 255.0f;
		m_clearColor[3] = 1.0f;
	}

	void Graphics::cleanScreen()
	{
		glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}

	void Graphics::pushMatrix(Mat3 mat)
	{
		m_listMat.push_back(mat);
	}

	void Graphics::popMatrix()
	{
		m_listMat.clear();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	GLuint loadShader(GLenum type, const char* src)
	{
		GLuint shader = glCreateShader(type);
		if (shader)
		{
			glShaderSource(shader, 1, &src, NULL);
			glCompileShader(shader);
			GLint compiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
			if (!compiled)
			{
				GLint infoLen = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
				if (infoLen)
				{
					char* buf = (char*)malloc(infoLen);
					if (buf)
					{
						glGetShaderInfoLog(shader, infoLen, NULL, buf);
						char msg[512];
						sprintf_s(msg, 512, "Could not compile shader %d:\n%s\n", type, buf);
						ASSERT_MSG(false, msg);
						free(buf);
					}
					glDeleteShader(shader);
					shader = 0;
				}
			}
		}
		return shader;
	}

	GLuint createProgram(const char* vertex_src, const char* fragment_src)
	{
		GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertex_src);
		if (!vertexShader)
		{
			return 0;
		}

		GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, fragment_src);
		if (!pixelShader)
		{
			return 0;
		}

		GLuint program = glCreateProgram();
		if (program)
		{
			glAttachShader(program, vertexShader);
			glAttachShader(program, pixelShader);

			glLinkProgram(program);
			GLint linkStatus = GL_FALSE;
			glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
			if (linkStatus != GL_TRUE)
			{
				GLint bufLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
				if (bufLength)
				{
					char* buf = (char*)malloc(bufLength);
					if (buf)
					{
						glGetProgramInfoLog(program, bufLength, NULL, buf);
						char msg[512];
						sprintf_s(msg, 512, "Could not link program:\n%s\n", buf);
						ASSERT_MSG(false, msg);
						free(buf);
					}
				}
				glDeleteProgram(program);
				program = 0;
			}
		}
		if (!program)
		{
			ASSERT_MSG(false, "Could not create program.");			
		}
		return program;
	}
}
