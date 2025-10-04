#include "renderpass.h"

//-------------------------HDR---------------------------

int fb_hdr_t::Init(uint32_t Width, uint32_t Height)
{
	if(FBO != 0)
	{
		printf("System: attempt to reinitialize existing framebuffer. Call Release() first\n");
		return EXIT_FAILURE;
	}

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Index Buffer
	glGenTextures(1, &ColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_FLOAT, 0x0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBuffer, 0);

	// Depth Buffer
	glGenTextures(1, &DepthBuffer);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0x0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if(Status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("System: Framebuffer (picking) gen error: 0x%x\n", Status);
		return EXIT_FAILURE;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return EXIT_SUCCESS;
}

void fb_hdr_t::Release()
{
	if(FBO != 0)
	{
		glDeleteFramebuffers(1, &FBO);
	}

	if(ColorBuffer != 0)
	{
		glDeleteTextures(1, &ColorBuffer);
	}

	if(DepthBuffer != 0)
	{
		glDeleteTextures(1, &DepthBuffer);
	}

	FBO = 0;
}

void fb_hdr_t::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

//-------------------------------------------------------

//---------------------- PICKING-------------------------

int fb_mpick_t::Init(uint32_t Width, uint32_t Height)
{
	if(FBO != 0)
	{
		printf("System: attempt to reinitialize existing framebuffer. Call Release() first\n");
		return EXIT_FAILURE;
	}

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Index Buffer
	glGenTextures(1, &IndexTex);
	glBindTexture(GL_TEXTURE_2D, IndexTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, Width, Height, 0, GL_RG, GL_FLOAT, 0x0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, IndexTex, 0);

	// Depth Buffer
	glGenTextures(1, &DepthTex);
	glBindTexture(GL_TEXTURE_2D, DepthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0x0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTex, 0);

	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if(Status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("System: Framebuffer (picking) gen error: 0x%x\n", Status);
		return EXIT_FAILURE;
	}

	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return EXIT_SUCCESS;
}

void fb_mpick_t::Release()
{
	if(FBO != 0)
	{
		glDeleteFramebuffers(1, &FBO);
	}

	if(IndexTex != 0)
	{
		glDeleteTextures(1, &IndexTex);
	}

	if(DepthTex != 0)
	{
		glDeleteTextures(1, &DepthTex);
	}

	FBO = 0;
}

void fb_mpick_t::Bind_W()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
}

void fb_mpick_t::Unbind_W()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

texel_info_t fb_mpick_t::GetInfo(uint32_t X, uint32_t Y)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	texel_info_t res;
	glReadPixels(X, Y, 1, 1, GL_RG, GL_FLOAT, &res);

	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	return res;
}

//-----------------------------------------------------
