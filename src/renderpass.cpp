#include "renderpass.h"

//-------------------------HDR---------------------------

int fb_hdr_t::Init(uint32_t Width, uint32_t Height)
{
	if(FBO != 0)
	{
		printf("System: attempt to reinitialize existing framebuffer. Call Release() first\n");
		return EXIT_FAILURE;
	}

	glCreateFramebuffers(1, &FBO);

	// Index Buffer
	glCreateTextures(GL_TEXTURE_2D, 1, &ColorBuffer);
	glTextureParameteri(ColorBuffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(ColorBuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(ColorBuffer, 1, GL_RGBA16F, Width, Height);
	glNamedFramebufferTexture(FBO, GL_COLOR_ATTACHMENT0, ColorBuffer, 0);

	// Depth Buffer
	glCreateTextures(GL_TEXTURE_2D, 1, &DepthBuffer);
	glTextureStorage2D(DepthBuffer, 1, GL_DEPTH_COMPONENT16, Width, Height);
	glNamedFramebufferTexture(FBO, GL_DEPTH_ATTACHMENT, DepthBuffer, 0);

	GLenum Status = glCheckNamedFramebufferStatus(FBO, GL_FRAMEBUFFER);

	if(Status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("System: Framebuffer (HDR) gen error: 0x%x\n", Status);
		return EXIT_FAILURE;
	}

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

	glCreateFramebuffers(1, &FBO);

	// Index Buffer
	glCreateTextures(GL_TEXTURE_2D, 1, &IndexTex);
	glTextureStorage2D(IndexTex, 1, GL_RG16F, Width, Height);
	glNamedFramebufferTexture(FBO, GL_COLOR_ATTACHMENT0, IndexTex, 0);

	// Depth Buffer
	glCreateTextures(GL_TEXTURE_2D, 1, &DepthTex);
	glTextureStorage2D(DepthTex, 1, GL_DEPTH_COMPONENT16, Width, Height);
	glNamedFramebufferTexture(FBO, GL_DEPTH_ATTACHMENT, DepthTex, 0);

	GLenum Status = glCheckNamedFramebufferStatus(FBO, GL_FRAMEBUFFER);

	if(Status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("System: Framebuffer (picking) gen error: 0x%x\n", Status);
		return EXIT_FAILURE;
	}

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

void fb_mpick_t::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

texel_info_t fb_mpick_t::GetInfo(uint32_t X, uint32_t Y)
{
	glNamedFramebufferReadBuffer(FBO, GL_COLOR_ATTACHMENT0);

	texel_info_t res;
	glReadPixels(X, Y, 1, 1, GL_RG, GL_FLOAT, &res);

	return res;
}

//-----------------------------------------------------
