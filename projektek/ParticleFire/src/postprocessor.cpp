#include "../include/postprocessor.hpp"
#include "../include/texture.hpp"

PostProcessor::PostProcessor(std::string shaderName, int screenWidth, int screenHeigth)
{
	m_iScreenHeigth = screenHeigth;
	m_iScreenWidth = screenWidth;

	// load shaders
	std::vector<tdogl::Shader> shaders;
	shaders.push_back(tdogl::Shader::shaderFromFile((shaderName + ".vs"), GL_VERTEX_SHADER));
	shaders.push_back(tdogl::Shader::shaderFromFile((shaderName + ".fs"), GL_FRAGMENT_SHADER));
	m_pShader = new tdogl::Program(shaders);

	initializeFBO();
	generateVAO();
}

PostProcessor::~PostProcessor()
{
	glDeleteTextures(1, &m_iRbo_depth);
	glDeleteTextures(1, &m_iFbo_texture);
	glDeleteFramebuffers(1, &m_iFbo);
}

void PostProcessor::startRenderingOnTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_iFbo);
}

void PostProcessor::stopRenderingOnTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::draw()
{
	m_pShader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_iFbo_texture);
	m_pShader->setUniform("tex", 0); //set 0, because it is bound to GL_TEXTURE0

	if (m_iAdditionalTexture != NULL)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_iAdditionalTexture);
		m_pShader->setUniform("tex2", 1);
	}
	if (m_iForegroundDepth != NULL)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_iForegroundDepth);
		m_pShader->setUniform("foregroundDepth", 2);
	}
	if (m_iBackgroundDepth != NULL)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_iBackgroundDepth);
		m_pShader->setUniform("backgroundDepth", 3);
	}

	glBindVertexArray(m_iQuadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	m_pShader->stopUsing();
}

void PostProcessor::setOffset(float offset)
{
	m_pShader->use();
	m_pShader->setUniform("offset", offset);
	m_pShader->stopUsing();
}

void PostProcessor::addTexture(GLuint textureId)
{
	m_iAdditionalTexture = textureId;
}

void PostProcessor::addBackgroundDepth(GLuint textureId)
{
	m_iBackgroundDepth = textureId;
}

void PostProcessor::addForegroundDepth(GLuint textureId)
{
	m_iForegroundDepth = textureId;
}

void PostProcessor::initializeFBO()
{
	// Generate empty texture
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &m_iFbo_texture);
	glBindTexture(GL_TEXTURE_2D, m_iFbo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iScreenWidth, m_iScreenHeigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Generate depth buffer
	//glGenRenderbuffers(1, &m_iRbo_depth);
	//glBindRenderbuffer(GL_RENDERBUFFER, m_iRbo_depth);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_iScreenWidth, m_iScreenHeigth);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glGenTextures(1, &m_iRbo_depth);
	glBindTexture(GL_TEXTURE_2D, m_iRbo_depth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_iScreenWidth, m_iScreenHeigth, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);



	// Generate FBO
	glGenFramebuffers(1, &m_iFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_iFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_iFbo_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_iRbo_depth, 0);
	// check if FBO is active
	GLenum status;
	if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "glCheckFramebufferStatus: error %p", status);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::generateVAO()
{
	GLfloat vertices[] = {
		-1, -1,
		1, -1,
		-1,  1,
		1,  1
	};

	glGenVertexArrays(1, &m_iQuadVAO);
	glBindVertexArray(m_iQuadVAO);

	glGenBuffers(1, &m_iVerticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iVerticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(m_pShader->attrib("vert"));
	glVertexAttribPointer(m_pShader->attrib("vert"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(0);
}

GLuint PostProcessor::getTextureId()
{
	return m_iFbo_texture;
}

GLuint PostProcessor::getDepthId()
{
	return m_iRbo_depth;
}

void PostProcessor::reshape(int newWidth, int newHeigth)
{
	m_iScreenWidth = newWidth;
	m_iScreenHeigth = newHeigth;

	glBindTexture(GL_TEXTURE_2D, m_iFbo_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iScreenWidth, m_iScreenHeigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	//glBindRenderbuffer(GL_RENDERBUFFER, m_iRbo_depth);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_iScreenWidth, m_iScreenHeigth);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, m_iRbo_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_iScreenWidth, m_iScreenHeigth, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}
