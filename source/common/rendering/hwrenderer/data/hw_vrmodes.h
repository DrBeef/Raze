#pragma once

#include "matrix.h"

class DFrameBuffer;

enum
{
	VR_MONO = 0,
	VR_GREENMAGENTA = 1,
	VR_REDCYAN = 2,
	VR_SIDEBYSIDEFULL = 3,
	VR_SIDEBYSIDESQUISHED = 4,
	VR_LEFTEYEVIEW = 5,
	VR_RIGHTEYEVIEW = 6,
	VR_QUADSTEREO = 7,
	VR_SIDEBYSIDELETTERBOX = 8,
	VR_AMBERBLUE = 9,
	VR_TOPBOTTOM = 11,
	VR_ROWINTERLEAVED = 12,
	VR_COLUMNINTERLEAVED = 13,
	VR_CHECKERINTERLEAVED = 14,
	VR_OPENXR = 15
};

struct VREyeInfo
{
	float mShiftFactor;
	float mScaleFactor;

	VSMatrix GetCenterProjection(float fov, float aspectRatio, float fovRatio) const;
	VSMatrix GetStereoProjection(float fov, float aspectRatio, float fovRatio) const;
	DVector3 GetViewShift(FRotator angles) const;
	VSMatrix GetHUDProjection(int width, int height) const;
	VSMatrix GetPlayerSpriteProjection(int width, int height) const;

private:
	float getShift() const;
	int getEye() const;

    float getStereoSeparation(double stereoLevel) const;
};

struct VRMode
{
	int mEyeCount;
	float mHorizontalViewportScale;
	float mVerticalViewportScale;
	float mWeaponProjectionScale;
	VREyeInfo mEyes[2];

	static const VRMode *GetVRMode(bool toscreen = true);
	void AdjustViewport(DFrameBuffer *fb) const;
};
