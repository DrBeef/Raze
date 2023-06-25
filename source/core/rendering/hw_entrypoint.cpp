// 
//---------------------------------------------------------------------------
//
// Copyright(C) 2004-2016 Christoph Oelckers
// All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/
//
//--------------------------------------------------------------------------
//
/*
** gl_scene.cpp
** manages the rendering of the player's view
**
*/

#include <gamestate.h>
#include <menustate.h>
#include "gi.h"
#include "build.h"
#include "v_draw.h"
//#include "a_dynlight.h"
#include "v_video.h"
#include "m_png.h"
#include "i_time.h"
#include "hw_dynlightdata.h"
#include "hw_clock.h"
#include "flatvertices.h"

#include "hw_renderstate.h"
#include "hw_lightbuffer.h"
#include "hw_cvars.h"
#include "hw_viewpointbuffer.h"
#include "hw_clipper.h"
//#include "hwrenderer/scene/hw_portal.h"
#include "hw_vrmodes.h"

#include "hw_drawstructs.h"
#include "hw_drawlist.h"
#include "hw_drawinfo.h"
#include "gamecvars.h"
#include "render.h"
#include "gamestruct.h"
#include "gamehud.h"

EXTERN_CVAR(Bool, cl_capfps)

extern float vrYaw;

PalEntry GlobalMapFog;
float GlobalFogDensity = 350.f;
TArray<PortalDesc> allPortals;
void Draw2D(F2DDrawer* drawer, FRenderState& state);

CVARD(Bool, hw_hightile, true, CVAR_ARCHIVE | CVAR_GLOBALCONFIG, "enable/disable hightile texture rendering")
bool hw_int_useindexedcolortextures;
CUSTOM_CVARD(Bool, hw_useindexedcolortextures, false, CVAR_ARCHIVE | CVAR_GLOBALCONFIG, "enable/disable indexed color texture rendering")
{
	if (screen) screen->SetTextureFilterMode();
}
CVARD(Bool, hw_models, false, CVAR_ARCHIVE | CVAR_GLOBALCONFIG, "enable/disable model rendering")



#if 0
void CollectLights(FLevelLocals* Level)
{
	IShadowMap* sm = &screen->mShadowMap;
	int lightindex = 0;

	// Todo: this should go through the blockmap in a spiral pattern around the player so that closer lights are preferred.
	for (auto light = Level->lights; light; light = light->next)
	{
		IShadowMap::LightsProcessed++;
		if (light->shadowmapped && light->IsActive() && lightindex < 1024)
		{
			IShadowMap::LightsShadowmapped++;

			light->mShadowmapIndex = lightindex;
			sm->SetLight(lightindex, (float)light->X(), (float)light->Y(), (float)light->Z(), light->GetRadius());
			lightindex++;
		}
		else
		{
			light->mShadowmapIndex = 1024;
		}

	}

	for (; lightindex < 1024; lightindex++)
	{
		sm->SetLight(lightindex, 0, 0, 0, 0);
	}
}
#endif

float RazeXR_GetFOV();
void VR_GetMove(float *joy_forward, float *joy_side, float *hmd_forward, float *hmd_side, float *up,
				float *yaw, float *pitch, float *roll);

//-----------------------------------------------------------------------------
//
// Renders one viewpoint in a scene
//
//-----------------------------------------------------------------------------

void RenderViewpoint(FRenderViewpoint& mainvp, IntRect* bounds, float fov, float ratio, float fovratio, bool mainview, bool toscreen)
{
	auto& RenderState = *screen->RenderState();

	/*
	if (mainview && toscreen)
	{
		screen->SetAABBTree(camera->Level->aabbTree);
		screen->mShadowMap.SetCollectLights([=] {
			CollectLights(camera->Level);
		});
		screen->UpdateShadowMap();
	}
	*/

	// Render (potentially) multiple views for stereo 3d
	// Fixme. The view offsetting should be done with a static table and not require setup of the entire render state for the mode.
	auto vrmode = VRMode::GetVRMode(mainview && toscreen);
	const int eyeCount = vrmode->mEyeCount;
	screen->FirstEye();

#ifndef __MOBILE__
	hw_int_useindexedcolortextures = eyeCount > 1? false : *hw_useindexedcolortextures;
#else
	hw_int_useindexedcolortextures = false;
#endif

	for (int eye_ix = 0; eye_ix < eyeCount; ++eye_ix)
	{
		const auto& eye = vrmode->mEyes[eye_ix];
		screen->SetViewportRects(bounds);

		if (mainview) // Bind the scene frame buffer and turn on draw buffers used by ssao
		{
			bool useSSAO = (gl_ssao != 0);
			screen->SetSceneRenderTarget(useSSAO);
			RenderState.SetPassType(useSSAO ? GBUFFER_PASS : NORMAL_PASS);
			RenderState.EnableDrawBuffers(RenderState.GetPassDrawBufferCount(), true);
		}

		auto di = HWDrawInfo::StartDrawInfo(nullptr, mainvp, nullptr);
		di->SetVisibility();
		di->eye = eye_ix;
		auto& vp = di->Viewpoint;
		vp = mainvp;

		di->Set3DViewport(RenderState);
		float flash = 8.f / (r_scenebrightness + 8.f);
		di->Viewpoint.FieldOfView = FAngle::fromDeg(fov);	// Set the real FOV for the current scene (it's not necessarily the same as the global setting in r_viewpoint)

		// Stereo mode specific perspective projection
		di->VPUniforms.mProjectionMatrix[0] = vrmode->mEyes[0].GetStereoProjection(fov, ratio, fovratio);
		di->VPUniforms.mProjectionMatrix[1] = vrmode->mEyes[1].GetStereoProjection(fov, ratio, fovratio);


		// Stereo mode specific viewpoint adjustment
		if (eye.mShiftFactor != 0)
		{
			vp.Pos += eye.GetViewShift(vp.HWAngles);
			sectortype* sect = &sector[vp.SectCount];
			updatesector(DVector2(vp.Pos.X, -vp.Pos.Y), &sect);
			vp.SectCount = sectindex(sect);
		}

		di->SetupView(RenderState, vp.Pos.X, vp.Pos.Y, vp.Pos.Z, false, false);
		di->ProcessScene(toscreen);

		if (mainview)
		{
			PostProcess.Clock();
			if (toscreen) di->EndDrawScene(RenderState); // do not call this for camera textures.

			if (RenderState.GetPassType() == GBUFFER_PASS) // Turn off ssao draw buffers
			{
				RenderState.SetPassType(NORMAL_PASS);
				RenderState.EnableDrawBuffers(1);
			}

			screen->PostProcessScene(false, CM_DEFAULT, flash, []() { 
				hw_int_useindexedcolortextures = false;
				PostProcess.Unclock();
				Draw2D(&twodpsp, *screen->RenderState()); // draws the weapon sprites
				PostProcess.Clock();
				});
			PostProcess.Unclock();
		}
		di->EndDrawInfo();
		if (eyeCount - eye_ix > 1)
			screen->NextEye(eyeCount);
	}
	hw_int_useindexedcolortextures = false;
}

//===========================================================================
//
// Set up the view point.
//
//===========================================================================

FRenderViewpoint SetupViewpoint(DCoreActor* cam, const DVector3& position, int sectnum, const DRotator& angles, float fov = -1)
{
	float dummy, yaw, pitch, roll;
	VR_GetMove(&dummy, &dummy, &dummy, &dummy, &dummy, &yaw, &pitch, &roll);

	//Special handling for Duke's security cameras
    bool renderingSecurityCamera = isDuke() && (cam && cam->spr.cstat & CSTAT_SPRITE_INVISIBLE);

	//Only do the following if not rendering a camera tex
	if (!renderingSecurityCamera)
	{
		//Yaw
		float hmdYawDeltaDegrees;
		{
			static float previousHmdYaw = yaw;
			hmdYawDeltaDegrees = yaw - previousHmdYaw;
			previousHmdYaw = yaw;
		}

		if (gamestate == GS_LEVEL)
		{
			// Special frame-yaw-resync code
			// Basically, if the game code changes the player's yaw, then we need to gradually resync
			// our "vrYaw" back to match it, doing the full amount on a frame causes it to glitch
			// but smoothly transitioning to it means the user doesn't notice and we should never be out
			// of sync with the game's yaw for very long
			{
				float diff = (float) (-90.f + angles.Yaw.Degrees()) - vrYaw;
				if (diff < -360.f)
					diff += 360.f;
				if (diff > 360.f)
					diff -= 360.f;
				vrYaw += (diff / 2.0f);
			}

			//And now apply the delta of hmd movement for this frame
			vrYaw -= hmdYawDeltaDegrees;
		}
    }

	FRenderViewpoint r_viewpoint{};
	r_viewpoint.CameraActor = cam;
	r_viewpoint.SectNums = nullptr;
	r_viewpoint.SectCount = sectnum;
	r_viewpoint.Pos = { position.X, -position.Y, -position.Z };
	if (renderingSecurityCamera)
	{
		r_viewpoint.HWAngles.Yaw = FAngle::fromDeg(-90.f + (float)angles.Yaw.Degrees());
	}
	else
	{
		r_viewpoint.HWAngles.Yaw = FAngle::fromDeg(vrYaw);
	}
	r_viewpoint.HWAngles.Pitch = FAngle::fromDeg(pitch);
	r_viewpoint.HWAngles.Roll = FAngle::fromDeg(roll);
	r_viewpoint.FieldOfView = FAngle::fromDeg((float)RazeXR_GetFOV());
	r_viewpoint.RotAngle = angles.Yaw.BAMs();
	double FocalTangent = tan(r_viewpoint.FieldOfView.Radians() / 2);
	DAngle an = DAngle::fromDeg(270. - r_viewpoint.HWAngles.Yaw.Degrees());
	r_viewpoint.TanSin = FocalTangent * an.Sin();
	r_viewpoint.TanCos = FocalTangent * an.Cos();
	r_viewpoint.ViewVector = an.ToVector();
	return r_viewpoint;
}


void DoWriteSavePic(FileWriter* file, uint8_t* scr, int width, int height, bool upsidedown)
{
	int pixelsize = 3;

	int pitch = width * pixelsize;
	if (upsidedown)
	{
		scr += ((height - 1) * width * pixelsize);
		pitch *= -1;
	}

	M_CreatePNG(file, scr, nullptr, SS_RGB, width, height, pitch, vid_gamma);
}

//===========================================================================
//
// Render the view to a savegame picture
//
// Currently a bit messy because the game side still needs to be able to
// handle Polymost.
//
//===========================================================================
bool writingsavepic;
FileWriter* savefile;
int savewidth, saveheight;

void WriteSavePic(FileWriter* file, int width, int height)
{
	writingsavepic = true;
	savefile = file;
	savewidth = width;
	saveheight = height;
	/*bool didit =*/ gi->GenerateSavePic();
	writingsavepic = false;
}

void RenderToSavePic(FRenderViewpoint& vp, FileWriter* file, int width, int height)
{
	IntRect bounds;
	bounds.left = 0;
	bounds.top = 0;
	bounds.width = width;
	bounds.height = height;
	auto& RenderState = *screen->RenderState();

	// we must be sure the GPU finished reading from the buffer before we fill it with new data.
	screen->WaitForCommands(false);

	// Switch to render buffers dimensioned for the savepic
	screen->SetSaveBuffers(true);
	screen->ImageTransitionScene(true);

	RenderState.SetVertexBuffer(screen->mVertexData);
	screen->mVertexData->Reset();
	screen->mLights->Clear();
	screen->mViewpoints->Clear();

	twodpsp.Clear();

	RenderViewpoint(vp, &bounds, vp.FieldOfView.Degrees(), 1.333f, 1.333f, true, false);


	int numpixels = width * height;
	uint8_t* scr = (uint8_t*)M_Malloc(numpixels * 3);
	screen->CopyScreenToBuffer(width, height, scr);

	DoWriteSavePic(file, scr, width, height, screen->FlipSavePic());
	M_Free(scr);

	// Switch back the screen render buffers
	screen->SetViewportRects(nullptr);
	screen->SetSaveBuffers(false);
}

//===========================================================================
//
// Renders the main view
//
//===========================================================================

static void CheckTimer(FRenderState &state, uint64_t ShaderStartTime)
{
	// if firstFrame is not yet initialized, initialize it to current time
	// if we're going to overflow a float (after ~4.6 hours, or 24 bits), re-init to regain precision
	if ((state.firstFrame == 0) || (screen->FrameTime - state.firstFrame >= 1 << 24) || ShaderStartTime >= state.firstFrame)
		state.firstFrame = screen->FrameTime;
}


void animatecamsprite(double s);


void render_drawrooms(DCoreActor* playersprite, const DVector3& position, sectortype* sect, const DRotator& angles, double interpfrac, float fov)
{
	updatesector(position.XY(), &sect);
	if (sect == nullptr) return;

	iter_dlightf = iter_dlight = draw_dlight = draw_dlightf = 0;
	checkBenchActive();

	// reset statistics counters
	ResetProfilingData();

	// Get this before everything else
	FRenderViewpoint r_viewpoint = SetupViewpoint(playersprite, position, sectindex(sect), angles, fov);
	r_viewpoint.TicFrac = !cl_capfps ? interpfrac : 1.;

	screen->mLights->Clear();
	screen->mViewpoints->Clear();
	screen->mVertexData->Reset();

	if (writingsavepic) // hack alert! The save code should not go through render_drawrooms, but we can only clean up the game side when Polymost is gone for good.
	{
		RenderToSavePic(r_viewpoint, savefile, savewidth, saveheight);
		return;
	}

	// Shader start time does not need to be handled per level. Just use the one from the camera to render from.
	auto RenderState = screen->RenderState();
	CheckTimer(*RenderState, 0/*ShaderStartTime*/);

	// prepare all camera textures that have been used in the last frame.
	gi->UpdateCameras(r_viewpoint.TicFrac);

	RenderState->SetVertexBuffer(screen->mVertexData);

	// now render the main view
	float fovratio;
	float ratio = ActiveRatio(screen->GetWidth(), screen->GetHeight());
	if (ratio >= 1.33f)
	{
		fovratio = 1.33f;
	}
	else
	{
		fovratio = ratio;
	}

	screen->ImageTransitionScene(true); // Only relevant for Vulkan.

	RenderViewpoint(r_viewpoint, nullptr, r_viewpoint.FieldOfView.Degrees(), ratio, fovratio, true, true);
	All.Unclock();
}

void render_camtex(DCoreActor* playersprite, const DVector3& position, sectortype* sect, const DRotator& angles, FGameTexture* camtex, IntRect& rect, double interpfrac)
{
	updatesector(position, &sect);
	if (!sect) return;

	screen->RenderState()->SetVertexBuffer(screen->mVertexData);

	// now render the main view
	float ratio = camtex->GetDisplayWidth() / camtex->GetDisplayHeight();

	FRenderViewpoint r_viewpoint = SetupViewpoint(playersprite, position, sectindex(sect), angles);
	r_viewpoint.TicFrac = !cl_capfps ? interpfrac : 1.;

	RenderViewpoint(r_viewpoint, &rect, r_viewpoint.FieldOfView.Degrees(), ratio, ratio, false, false);
	All.Unclock();
}

FSerializer& Serialize(FSerializer& arc, const char* key, PortalDesc& obj, PortalDesc* defval)
{
	if (arc.BeginObject(key))
	{
		arc("type", obj.type)
			("d", obj.delta)
			("targets", obj.targets)
			.EndObject();
	}
	return arc;
}
