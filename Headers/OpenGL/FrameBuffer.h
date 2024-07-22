#pragma once

#include <glad/glad.h>
#include <iostream>

class FrameBuffer
{
private:
	void init(
		int width,
		int height,
		int internalFormat,
		int format,
		int type,
		const void* data);

public:
	unsigned int FBO;
	unsigned int RBO;
	unsigned int texture;

	FrameBuffer(int width, int height);
	FrameBuffer(int width, int height, int size);
	~FrameBuffer();

	void Bind();
	void Unbind();
	void Delete();

};

