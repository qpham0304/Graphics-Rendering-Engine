#include "DepthMap.h"

float DepthMap::shadowCalculation(glm::vec4 fragPosLightSpace)
{
	return 0.0f;
}

DepthMap::DepthMap()
{
	glGenFramebuffers(1, &ID);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//attach the generated depth texture as the framebuffer's depth buffer:
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


void DepthMap::depthMapViewUpdate()
{
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	lightSpaceMatrix = lightProjection * lightView;
}

void DepthMap::depthMapViewObject(GLuint shaderID, const char* uniform) {
	lightSpaceMatrix = lightProjection * lightView;
	GLuint uniformLocation = glGetUniformLocation(shaderID, "lightSpaceMatrix");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
}

void DepthMap::Draw()
{

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glClear(GL_DEPTH_BUFFER_BIT);
	//RenderScene(simpleDepthShader);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
