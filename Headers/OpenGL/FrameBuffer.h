#pragma once

#include <glad/glad.h>
#include <iostream>

class FrameBuffer
{
public:

	unsigned int FBO;
	unsigned int RBO;
	unsigned int texture;

	FrameBuffer() = default;
	FrameBuffer(int width, int height);
	FrameBuffer(int width, int height, int size);
	~FrameBuffer();

	void Init(
		int width,
		int height,
		int internalFormat,
		int format,
		int type,
		const void* data
	);
	void Bind();
	void Unbind();
	void Delete();

};

