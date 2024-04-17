#pragma once

#include <glad/glad.h>
#include <iostream>

class FrameBuffer
{
private:

public:
	unsigned int FBO;
	unsigned int RBO;
	unsigned int texture;

	FrameBuffer(int width, int height);

	void Bind();
	void Unbind();
	void Delete();

};

