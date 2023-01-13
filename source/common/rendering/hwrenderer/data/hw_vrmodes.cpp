/*
** hw_vrmodes.cpp
** Matrix handling for stereo 3D rendering
**
**---------------------------------------------------------------------------
** Copyright 2015 Christopher Bruns
** Copyright 2016-2021 Christoph Oelckers
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
**
*/

#include "vectors.h" // RAD2DEG
#include "hw_cvars.h"
#include "hw_vrmodes.h"
#include "v_video.h"
#include "v_draw.h"
#include "version.h"
#include "i_interface.h"
#include "RazeXR/mathlib.h"

extern vec3_t hmdPosition;
extern vec3_t hmdOrigin;
extern vec3_t hmdorientation;

float RazeXR_GetFOV();
void VR_GetMove(float *joy_forward, float *joy_side, float *hmd_forward, float *hmd_side, float *up,
				float *yaw, float *pitch, float *roll);

// Set up 3D-specific console variables:
CVAR(Int, vr_mode, 15, CVAR_GLOBALCONFIG|CVAR_ARCHIVE)

// switch left and right eye views
CVAR(Bool, vr_swap_eyes, false, CVAR_GLOBALCONFIG | CVAR_ARCHIVE)


// intraocular distance in meters
CVAR(Float, vr_ipd, 0.062f, CVAR_ARCHIVE|CVAR_GLOBALCONFIG) // METERS

// distance between viewer and the display screen
CVAR(Float, vr_screendist, 0.80f, CVAR_ARCHIVE | CVAR_GLOBALCONFIG) // METERS

CVAR(Float, vr_hunits_per_meter, 24.0f, CVAR_ARCHIVE | CVAR_GLOBALCONFIG) // METERS

CVAR(Float, vr_height_adjust, 0.0f, CVAR_ARCHIVE | CVAR_GLOBALCONFIG) // METERS
CVAR(Int, vr_control_scheme, 0, CVAR_ARCHIVE | CVAR_GLOBALCONFIG | CVAR_NOINITCALL)
CVAR(Bool, vr_move_use_offhand, false, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Bool, vr_teleport, false, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Float, vr_weaponRotate, -30, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Float, vr_weaponScale, 1.02f, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Float, vr_snapTurn, 45.0f, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Int, vr_move_speed, 19, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Float, vr_run_multiplier, 1.5, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Bool, vr_switch_sticks, false, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Bool, vr_secondary_button_mappings, true, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Bool, vr_two_handed_weapons, true, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Bool, vr_positional_tracking, true, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Bool, vr_crouch_use_button, true, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)

CVAR(Float, vr_pickup_haptic_level, 0.2, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Float, vr_quake_haptic_level, 0.8, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)

//HUD control
CVAR(Float, vr_hud_scale, 0.5f, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Float, vr_hud_stereo, 2.0f, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Float, vr_hud_rotate, 0.f, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Bool, vr_hud_fixed_pitch, false, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)
CVAR(Bool, vr_hud_fixed_roll, false, CVAR_ARCHIVE | CVAR_GLOBALCONFIG)

int playerHeight = 0;

float getHmdAdjustedHeightInMapUnit()
{
	if (playerHeight != 0)
	{
		return ((hmdPosition[1] + vr_height_adjust) * vr_hunits_per_meter) -
			   playerHeight;
	}

	//Just use offset from origin
	return ((hmdPosition[1] - hmdOrigin[1]) * vr_hunits_per_meter);
}

#define isqrt2 0.7071067812f
static VRMode vrmi_mono = { 1, 1.f, 1.f, 1.f,{ { 0.f, 1.f },{ 0.f, 0.f } } };
static VRMode vrmi_stereo = { 2, 1.f, 1.f, 1.f,{ { -.5f, 1.f },{ .5f, 1.f } } };
static VRMode vrmi_sbsfull = { 2, .5f, 1.f, 2.f,{ { -.5f, .5f },{ .5f, .5f } } };
static VRMode vrmi_sbssquished = { 2, .5f, 1.f, 1.f,{ { -.5f, 1.f },{ .5f, 1.f } } };
static VRMode vrmi_lefteye = { 1, 1.f, 1.f, 1.f, { { -.5f, 1.f },{ 0.f, 0.f } } };
static VRMode vrmi_righteye = { 1, 1.f, 1.f, 1.f,{ { .5f, 1.f },{ 0.f, 0.f } } };
static VRMode vrmi_topbottom = { 2, 1.f, .5f, 1.f,{ { -.5f, 1.f },{ .5f, 1.f } } };
static VRMode vrmi_checker = { 2, isqrt2, isqrt2, 1.f,{ { -.5f, 1.f },{ .5f, 1.f } } };

const VRMode *VRMode::GetVRMode(bool toscreen)
{
	int mode = !toscreen || (sysCallbacks.DisableTextureFilter && sysCallbacks.DisableTextureFilter()) ? 0 : vr_mode;

	switch (mode)
	{
	default:
	case VR_MONO:
		return &vrmi_mono;

	case VR_GREENMAGENTA:
	case VR_REDCYAN:
	case VR_QUADSTEREO:
	case VR_AMBERBLUE:
	case VR_SIDEBYSIDELETTERBOX:
	case VR_OPENXR:
		return &vrmi_stereo;

	case VR_SIDEBYSIDESQUISHED:
	case VR_COLUMNINTERLEAVED:
		return &vrmi_sbssquished;

	case VR_SIDEBYSIDEFULL:
		return &vrmi_sbsfull;

	case VR_TOPBOTTOM:
	case VR_ROWINTERLEAVED:
		return &vrmi_topbottom;

	case VR_LEFTEYEVIEW:
		return &vrmi_lefteye;

	case VR_RIGHTEYEVIEW:
		return &vrmi_righteye;

	case VR_CHECKERINTERLEAVED:
		return &vrmi_checker;
	}
}

void VRMode::AdjustViewport(DFrameBuffer *screen) const
{
	screen->mSceneViewport.height = (int)(screen->mSceneViewport.height * mVerticalViewportScale);
	screen->mSceneViewport.top = (int)(screen->mSceneViewport.top * mVerticalViewportScale);
	screen->mSceneViewport.width = (int)(screen->mSceneViewport.width * mHorizontalViewportScale);
	screen->mSceneViewport.left = (int)(screen->mSceneViewport.left * mHorizontalViewportScale);

	screen->mScreenViewport.height = (int)(screen->mScreenViewport.height * mVerticalViewportScale);
	screen->mScreenViewport.top = (int)(screen->mScreenViewport.top * mVerticalViewportScale);
	screen->mScreenViewport.width = (int)(screen->mScreenViewport.width * mHorizontalViewportScale);
	screen->mScreenViewport.left = (int)(screen->mScreenViewport.left * mHorizontalViewportScale);
}

extern float gameYaw;
float getViewpointYaw()
{
	return gameYaw;
}

VSMatrix VREyeInfo::GetHUDProjection(int width, int height) const
{
	// now render the main view
	float fovratio;
	float ratio = ActiveRatio(width, height);
	if (ratio >= 1.33f)
	{
		fovratio = 1.33f;
	}
	else
	{
		fovratio = ratio;
	}

	VSMatrix new_projection;
	new_projection.loadIdentity();

	float stereo_separation = (vr_ipd * 0.5) * vr_hunits_per_meter * vr_hud_stereo * (getEye() == 1 ? 1.0 : -1.0);
	new_projection.translate(stereo_separation, 0, 0);

	// doom_units from meters
	new_projection.scale(
			-vr_hunits_per_meter,
			vr_hunits_per_meter,
			-vr_hunits_per_meter);

	if (vr_hud_fixed_roll)
	{
		new_projection.rotate(-hmdorientation[ROLL], 0, 0, 1);
	}

	new_projection.rotate(vr_hud_rotate, 1, 0, 0);

	if (vr_hud_fixed_pitch)
	{
		new_projection.rotate(-hmdorientation[PITCH], 1, 0, 0);
	}

	// hmd coordinates (meters) from ndc coordinates
	// const float weapon_distance_meters = 0.55f;
	// const float weapon_width_meters = 0.3f;
	new_projection.translate(0.0, 0.0, 1.0);
	new_projection.scale(
			-vr_hud_scale,
            vr_hud_scale,
			-vr_hud_scale);

	// ndc coordinates from pixel coordinates
	new_projection.translate(-1.0, 1.0, 0);
	new_projection.scale(2.0 / width, -2.0 / height, -1.0);

	VSMatrix proj = GetProjection(RazeXR_GetFOV(), ratio, fovratio);
	proj.multMatrix(new_projection);
	new_projection = proj;

	return new_projection;
}

VSMatrix VREyeInfo::GetPlayerSpriteProjection(int width, int height) const
{
	// now render the main view
	float fovratio;
	float ratio = ActiveRatio(width, height);
	if (ratio >= 1.33f)
	{
		fovratio = 1.33f;
	}
	else
	{
		fovratio = ratio;
	}

	VSMatrix new_projection;
	new_projection.loadIdentity();

	float weapon_stereo_effect = 2.8f;
	float stereo_separation = (vr_ipd * 0.5) * vr_hunits_per_meter * weapon_stereo_effect * (getEye() == 1 ? -1.0 : 1.0);
	new_projection.translate(stereo_separation, 0, 0);

	// doom_units from meters
	new_projection.scale(
			-vr_hunits_per_meter,
			vr_hunits_per_meter,
			-vr_hunits_per_meter);

	new_projection.rotate(-hmdorientation[ROLL], 0, 0, 1);

	// hmd coordinates (meters) from ndc coordinates
	// const float weapon_distance_meters = 0.55f;
	// const float weapon_width_meters = 0.3f;
	new_projection.translate(0.0, 0.0, 1.0);

	float weapon_scale = 0.7f;
	new_projection.scale(
			-weapon_scale,
			weapon_scale,
			-weapon_scale);

	// ndc coordinates from pixel coordinates
	new_projection.translate(-1.0, 1.0, 0);
	new_projection.scale(2.0 / width, -2.0 / height, -1.0);

	VSMatrix proj = GetProjection(RazeXR_GetFOV(), ratio, fovratio);
	proj.multMatrix(new_projection);
	new_projection = proj;

	return new_projection;
}

float VREyeInfo::getShift() const
{
	return mShiftFactor * vr_ipd * vr_hunits_per_meter;
}

int VREyeInfo::getEye() const
{
	return mShiftFactor < 0 ? 0 : 1;
}

bool VR_GetVRProjection(int eye, float zNear, float zFar, float* projection);

VSMatrix VREyeInfo::GetProjection(float fov, float aspectRatio, float fovRatio) const
{
	VSMatrix result;

	if (mShiftFactor == 0)
	{
		float fovy = (float)(2 * RAD2DEG(atan(tan(DEG2RAD(fov) / 2) / fovRatio)));
		result.perspective(fovy, aspectRatio, screen->GetZNear(), screen->GetZFar());
		return result;
	}
	else
	{
		double zNear = screen->GetZNear();
		double zFar = screen->GetZFar();


		// For stereo 3D, use asymmetric frustum shift in projection matrix
		// Q: shouldn't shift vary with roll angle, at least for desktop display?
		// A: No. (lab) roll is not measured on desktop display (yet)
		double frustumShift = zNear * getShift() / vr_screendist; // meters cancel, leaving doom units
																  // double frustumShift = 0; // Turning off shift for debugging
		double fH = zNear * tan(DEG2RAD(fov) / 2) / fovRatio;
		double fW = fH * aspectRatio * mScaleFactor;
		double left = -fW - frustumShift;
		double right = fW - frustumShift;
		double bottom = -fH;
		double top = fH;

		VSMatrix result(1);
		result.frustum((float)left, (float)right, (float)bottom, (float)top, (float)zNear, (float)zFar);

		float m[16];
		VR_GetVRProjection(getEye(), zNear, zFar, m);
		result.loadMatrix(m);

		return result;
	}
}

/* virtual */
DVector3 VREyeInfo::GetViewShift(FRotator viewAngles) const
{
	if (mShiftFactor == 0)
	{
		// pass-through for Mono view
		return { 0,0,0 };
	}
	else
	{
		vec3_t angles;
		VectorSet(angles, viewAngles.Pitch.Degrees(),  viewAngles.Yaw.Degrees(), viewAngles.Roll.Degrees());

		vec3_t v_forward, v_right, v_up;
		AngleVectors(angles, v_forward, v_right, v_up);

		vec3_t tmp;
		VectorScale(v_right, getShift(), tmp);

		float posforward=0;
		float posside=0;
		float dummy=0;
		VR_GetMove(&dummy, &dummy, &posforward, &posside, &dummy, &dummy, &dummy, &dummy);
		DVector3 eyeOffset(tmp[0], tmp[1], tmp[2]);

		if (vr_positional_tracking)
		{
			eyeOffset[1] += -posforward * vr_hunits_per_meter;
			eyeOffset[0] += posside * vr_hunits_per_meter;
			eyeOffset[2] += getHmdAdjustedHeightInMapUnit();
		}

		return {eyeOffset[1], eyeOffset[0], eyeOffset[2]};
	}
}

