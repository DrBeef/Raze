/*
** sdlglvideo.cpp
**
**---------------------------------------------------------------------------
** Copyright 2005-2016 Christoph Oelckers et.al.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/

// HEADER FILES ------------------------------------------------------------

#include "i_module.h"
#include "i_soundinternal.h"
#include "i_system.h"
#include "i_video.h"
#include "m_argv.h"
#include "v_video.h"
#include "version.h"
#include "c_console.h"
#include "c_dispatch.h"
#include "printf.h"

#include "hardware.h"
#include "gl_sysfb.h"
#include "gl_system.h"

#include "gl_renderer.h"
#include "gl_framebuffer.h"
#ifdef HAVE_GLES2
#include "gles_framebuffer.h"
#endif
 
#ifdef HAVE_VULKAN
#include "vulkan/system/vk_framebuffer.h"
#endif

#ifdef HAVE_SOFTPOLY
#include "poly_framebuffer.h"
#endif

void RazeXR_GetScreenRes(uint32_t *width, uint32_t *height);

// MACROS ------------------------------------------------------------------

#if defined HAVE_VULKAN
#include <SDL_vulkan.h>
#endif // HAVE_VULKAN

// TYPES -------------------------------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------
extern IVideo *Video;

EXTERN_CVAR (Int, vid_adapter)
EXTERN_CVAR (Int, vid_displaybits)
EXTERN_CVAR (Int, vid_defwidth)
EXTERN_CVAR (Int, vid_defheight)
EXTERN_CVAR (Int, vid_preferbackend)
EXTERN_CVAR (Bool, cl_capfps)

// PUBLIC DATA DEFINITIONS -------------------------------------------------

CUSTOM_CVAR(Bool, gl_debug, false, CVAR_ARCHIVE | CVAR_GLOBALCONFIG | CVAR_NOINITCALL)
{
	Printf("This won't take effect until " GAMENAME " is restarted.\n");
}
CUSTOM_CVAR(Bool, gl_es, false, CVAR_ARCHIVE | CVAR_GLOBALCONFIG | CVAR_NOINITCALL)
{
	Printf("This won't take effect until " GAMENAME " is restarted.\n");
}

CVAR (Int, vid_adapter, 0, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)

CUSTOM_CVAR(String, vid_sdl_render_driver, "", CVAR_ARCHIVE | CVAR_GLOBALCONFIG | CVAR_NOINITCALL)
{
	Printf("This won't take effect until " GAMENAME " is restarted.\n");
}

CCMD(vid_list_sdl_render_drivers)
{
	
}

// PRIVATE DATA DEFINITIONS ------------------------------------------------


class GLVideo : public IVideo
{
public:
	GLVideo ();
	~GLVideo ();

	DFrameBuffer *CreateFrameBuffer ();

private:
#ifdef HAVE_VULKAN
	VulkanDevice *device = nullptr;
#endif
};

// CODE --------------------------------------------------------------------

#ifdef HAVE_VULKAN
void I_GetVulkanDrawableSize(int *width, int *height)
{
	assert(Priv::vulkanEnabled);
	assert(Priv::window != nullptr);
	SDL_Vulkan_GetDrawableSize(Priv::window, width, height);
}

bool I_GetVulkanPlatformExtensions(unsigned int *count, const char **names)
{
	assert(Priv::vulkanEnabled);
	assert(Priv::window != nullptr);
	return SDL_Vulkan_GetInstanceExtensions(Priv::window, count, names) == SDL_TRUE;
}

bool I_CreateVulkanSurface(VkInstance instance, VkSurfaceKHR *surface)
{
	assert(Priv::vulkanEnabled);
	assert(Priv::window != nullptr);
	return SDL_Vulkan_CreateSurface(Priv::window, instance, surface) == SDL_TRUE;
}
#endif

#ifdef HAVE_SOFTPOLY
namespace
{
	SDL_Renderer* polyrendertarget = nullptr;
	SDL_Texture* polytexture = nullptr;
	int polytexturew = 0;
	int polytextureh = 0;
	bool polyvsync = false;
	bool polyfirstinit = true;
}

void I_PolyPresentInit()
{
	assert(Priv::softpolyEnabled);
	assert(Priv::window != nullptr);

	if (strcmp(vid_sdl_render_driver, "") != 0)
	{
		SDL_SetHint(SDL_HINT_RENDER_DRIVER, vid_sdl_render_driver);
	}
}

uint8_t *I_PolyPresentLock(int w, int h, bool vsync, int &pitch)
{
	// When vsync changes we need to reinitialize
	if (polyrendertarget && polyvsync != vsync)
	{
		I_PolyPresentDeinit();
	}

	if (!polyrendertarget)
	{
		polyvsync = vsync;

		polyrendertarget = SDL_CreateRenderer(Priv::window, -1, vsync ? SDL_RENDERER_PRESENTVSYNC : 0);
		if (!polyrendertarget)
		{
			I_FatalError("Could not create render target for softpoly: %s\n", SDL_GetError());
		}

		// Tell the user which render driver is being used, but don't repeat
		// outselves if we're just changing vsync.
		if (polyfirstinit)
		{
			polyfirstinit = false;

			SDL_RendererInfo rendererInfo;
			if (SDL_GetRendererInfo(polyrendertarget, &rendererInfo) == 0)
			{
				Printf("Using render driver %s\n", rendererInfo.name);
			}
			else
			{
				Printf("Failed to query render driver\n");
			}
		}

		// Mask color
		SDL_SetRenderDrawColor(polyrendertarget, 0, 0, 0, 255);
	}

	if (!polytexture || polytexturew != w || polytextureh != h)
	{
		if (polytexture)
		{
			SDL_DestroyTexture(polytexture);
			polytexture = nullptr;
			polytexturew = polytextureh = 0;
		}
		if ((polytexture = SDL_CreateTexture(polyrendertarget, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h)) == nullptr)
			I_Error("Failed to create %dx%d render target texture.", w, h);
		polytexturew = w;
		polytextureh = h;
	}

	uint8_t* pixels;
	SDL_LockTexture(polytexture, nullptr, (void**)&pixels, &pitch);
	return pixels;
}

void I_PolyPresentUnlock(int x, int y, int width, int height)
{
	SDL_UnlockTexture(polytexture);

	int ClientWidth, ClientHeight;
	SDL_GetRendererOutputSize(polyrendertarget, &ClientWidth, &ClientHeight);

	SDL_Rect clearrects[4];
	int count = 0;
	if (y > 0)
	{
		clearrects[count].x = 0;
		clearrects[count].y = 0;
		clearrects[count].w = ClientWidth;
		clearrects[count].h = y;
		count++;
	}
	if (y + height < ClientHeight)
	{
		clearrects[count].x = 0;
		clearrects[count].y = y + height;
		clearrects[count].w = ClientWidth;
		clearrects[count].h = ClientHeight - clearrects[count].y;
		count++;
	}
	if (x > 0)
	{
		clearrects[count].x = 0;
		clearrects[count].y = y;
		clearrects[count].w = x;
		clearrects[count].h = height;
		count++;
	}
	if (x + width < ClientWidth)
	{
		clearrects[count].x = x + width;
		clearrects[count].y = y;
		clearrects[count].w = ClientWidth - clearrects[count].x;
		clearrects[count].h = height;
		count++;
	}

	if (count > 0)
		SDL_RenderFillRects(polyrendertarget, clearrects, count);

	SDL_Rect dstrect;
#ifdef __MOBILE__ // Make it fit the screen properly
	dstrect.x = 0;
	dstrect.y = 0;
	dstrect.w = ClientWidth;
	dstrect.h = ClientHeight;
#else
	dstrect.x = x;
	dstrect.y = y;
	dstrect.w = width;
	dstrect.h = height;
#endif
	SDL_RenderCopy(polyrendertarget, polytexture, nullptr, &dstrect);

	SDL_RenderPresent(polyrendertarget);
}

void I_PolyPresentDeinit()
{
	if (polytexture)
	{
		SDL_DestroyTexture(polytexture);
		polytexture = nullptr;
	}

	if (polyrendertarget)
	{
		SDL_DestroyRenderer(polyrendertarget);
		polyrendertarget = nullptr;
	}
}
#endif


GLVideo::GLVideo ()
{
#ifdef HAVE_SOFTPOLY
	Priv::softpolyEnabled = vid_preferbackend == 2;
#endif
#ifdef HAVE_VULKAN
	Priv::vulkanEnabled = vid_preferbackend == 1;

	if (Priv::vulkanEnabled)
	{
		Priv::CreateWindow(SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN | (vid_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));

		if (Priv::window == nullptr)
		{
			Priv::vulkanEnabled = false;
		}
	}
#endif
#ifdef HAVE_SOFTPOLY
	if (Priv::softpolyEnabled)
	{
		Priv::CreateWindow(SDL_WINDOW_HIDDEN);
		if (Priv::window == nullptr)
		{
			I_FatalError("Could not create SoftPoly window:\n%s\n",SDL_GetError());
		}
	}
#endif
}

GLVideo::~GLVideo ()
{
#ifdef HAVE_VULKAN
	delete device;
#endif
}

DFrameBuffer *GLVideo::CreateFrameBuffer ()
{
	SystemBaseFrameBuffer *fb = nullptr;

	// first try Vulkan, if that fails OpenGL
#ifdef HAVE_VULKAN
	if (Priv::vulkanEnabled)
	{
		try
		{
			assert(device == nullptr);
			device = new VulkanDevice();
			fb = new VulkanFrameBuffer(nullptr, vid_fullscreen, device);
		}
		catch (CVulkanError const &error)
		{
			if (Priv::window != nullptr)
			{
				Priv::DestroyWindow();
			}

			Printf(TEXTCOLOR_RED "Initialization of Vulkan failed: %s\n", error.what());
			Priv::vulkanEnabled = false;
		}
	}
#endif

#ifdef HAVE_SOFTPOLY
	if (Priv::softpolyEnabled)
	{
		fb = new PolyFrameBuffer(nullptr, vid_fullscreen);
	}
#endif
	if (fb == nullptr)
	{
		fb = new OpenGLRenderer::OpenGLFrameBuffer(0, vid_fullscreen);
	}

	return fb;
}


IVideo *gl_CreateVideo()
{
	return new GLVideo();
}


// FrameBuffer Implementation -----------------------------------------------

SystemBaseFrameBuffer::SystemBaseFrameBuffer (void *, bool fullscreen)
: DFrameBuffer (vid_defwidth, vid_defheight)
{

}

int SystemBaseFrameBuffer::GetClientWidth()
{
	uint32_t w, h;
    RazeXR_GetScreenRes(&w, &h);
	int width = w;
	return width;
}

int SystemBaseFrameBuffer::GetClientHeight()
{
	uint32_t w, h;
    RazeXR_GetScreenRes(&w, &h);
	int height = h;
	return height;
}

bool SystemBaseFrameBuffer::IsFullscreen ()
{
	return true;
}

void SystemBaseFrameBuffer::ToggleFullscreen(bool yes)
{
	
}

void SystemBaseFrameBuffer::SetWindowSize(int w, int h)
{

}


SystemGLFrameBuffer::SystemGLFrameBuffer(void *hMonitor, bool fullscreen)
: SystemBaseFrameBuffer(hMonitor, fullscreen)
{

}

SystemGLFrameBuffer::~SystemGLFrameBuffer ()
{

}

int SystemGLFrameBuffer::GetClientWidth()
{
	uint32_t w, h;
    RazeXR_GetScreenRes(&w, &h);
	int width = w;
	return width;
}

int SystemGLFrameBuffer::GetClientHeight()
{
	uint32_t w, h;
    RazeXR_GetScreenRes(&w, &h);
	int height = h;
	return height;
}

void SystemGLFrameBuffer::SetVSync( bool vsync )
{

}

void SystemGLFrameBuffer::SwapBuffers()
{

}


// each platform has its own specific version of this function.
void I_SetWindowTitle(const char* caption)
{

}

