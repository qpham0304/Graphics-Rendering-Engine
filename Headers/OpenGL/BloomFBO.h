#pragma once

#include<glm/gtc/type_ptr.hpp>
#include<glad/glad.h>
#include<vector>
#include<iostream>

struct BloomMip {
	glm::vec2 size;
	glm::ivec2 intSize;
	unsigned int texture;
};

class BloomFBO {
public:
    BloomFBO();
    ~BloomFBO();
    bool Init(unsigned int windowWidth, unsigned int windowHeight, unsigned int mipChainLength);
    void Destroy();
    void Bind();
    void UnBind();
    const std::vector<BloomMip>& MipChain() const;

private:
    bool mInit;
    unsigned int mFBO;
    std::vector<BloomMip> mMipChain;
};

