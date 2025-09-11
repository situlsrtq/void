#ifndef VOID_RENDERPASS_H
#define VOID_RENDERPASS_H


#include "../vendor/glad/glad.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


//-------------------------HDR---------------------------

struct fb_hdr_t
{
	uint32_t FBO;
	uint32_t ColorBuffer;
	uint32_t DepthBuffer;

	int Init(uint32_t WindowWidth, uint32_t WindowHeight);
	void Release();
	void Bind();
};

//-------------------------------------------------------

//-----------------------PICKING-------------------------

struct texel_info_t
{
	float ID;
	float Type;
};


struct fb_mpick_t
{
	uint32_t FBO;
	uint32_t IndexTex;
	uint32_t DepthTex;

	texel_info_t Info;

	int Init(uint32_t WindowWidth, uint32_t WindowHeight);
	void Release();
	void Bind_W();
	void Unbind_W();
	texel_info_t GetInfo(uint32_t X, uint32_t Y);
};

//-----------------------------------------------------


#endif
