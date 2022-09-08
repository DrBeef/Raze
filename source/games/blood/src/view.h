//-------------------------------------------------------------------------
/*
Copyright (C) 2010-2019 EDuke32 developers and contributors
Copyright (C) 2019 Nuke.YKT

This file is part of NBlood.

NBlood is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License version 2
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
//-------------------------------------------------------------------------
#pragma once
#include "build.h"
#include "palette.h"
#include "common_game.h"
#include "messages.h"
#include "player.h"
#include "interpolate.h"
#include "bloodactor.h"

BEGIN_BLD_NS

struct VIEW {
	int bobPhase;
	int Kills;
	int bobHeight; // bob height
	int bobWidth; // bob width
	int swayPhase;
	int swayAmp;
	int shakeBobY; // bob sway y
	int shakeBobX; // bob sway x
	fixedhoriz horiz; // horiz
	fixedhoriz horizoff; // horizoff
	int slope;
	DAngle angle; // angle
	int weaponZ; // weapon z
	int viewz; // view z
	int zViewVel;
	int zWeapon;
	int zWeaponVel;
	int posture; // posture
	double spin; // spin
	DVector3 pos;
	int xvel; //xvel
	int yvel; //yvel
	int zvel; //zvel
	int sectnum; // sectnum
	unsigned int floordist; // floordist
	uint8_t lookCenter; // look center
	uint8_t cantJump;
	uint8_t isRunning; // run
	uint8_t jump; // jump
	uint8_t isUnderwater; // underwater
	int16_t sprflags; // sprite flags
	SPRITEHIT hit;
	DAngle look_ang;
	DAngle rotscrnang;
};

extern VIEW gPrevView[kMaxPlayers];

extern VIEW predict, predictOld;
extern bool gPrediction;

enum VIEW_EFFECT {
	kViewEffectShadow = 0,
	kViewEffectFlareHalo,
	kViewEffectCeilGlow,
	kViewEffectFloorGlow,
	kViewEffectTorchHigh,
	kViewEffectTorchLow,
	kViewEffectSmokeHigh,
	kViewEffectSmokeLow,
	kViewEffectFlame,
	kViewEffectSpear,
	kViewEffectTrail,
	kViewEffectPhase,
	kViewEffectShowWeapon,
	kViewEffectReflectiveBall,
	kViewEffectShoot,
	kViewEffectTesla,
	kViewEffectFlag,
	kViewEffectBigFlag,
	kViewEffectAtom,
	kViewEffectSpotProgress,
};

enum VIEWPOS {
	VIEWPOS_0 = 0,
	VIEWPOS_1
};

enum
{
	kBackTile = 253,

	kCrosshairTile = 2319,
	kLoadScreen = 2049,
	kLoadScreenWideBack = 9216,
	kLoadScreenWideLeft = 9217,
	kLoadScreenWideRight = 9218,
	kLoadScreenWideMiddle = 9219,

	kSBarNumberHealth = 9220,
	kSBarNumberAmmo = 9230,
	kSBarNumberInv = 9240,
	kSBarNumberArmor1 = 9250,
	kSBarNumberArmor2 = 9260,
	kSBarNumberArmor3 = 9270,
};

enum { kFontNum = 5 };

extern FFont* gFont[kFontNum];
extern VIEWPOS gViewPos;
extern int gViewIndex;
extern int deliriumTilt, deliriumPitch;
extern int deliriumPitchO;
extern DAngle deliriumTurnO, deliriumTurn;
extern DAngle gScreenTiltO, gScreenTilt;
extern int gShowFrameRate;
extern int gLastPal;

void hudDraw(PLAYER* gView, sectortype* pSector, double bobx, double boby, double zDelta, int basepal, double interpfrac);
void viewInitializePrediction(void);
void viewUpdatePrediction(InputPacket* pInput);
void viewCorrectPrediction(void);
void viewBackupView(int nPlayer);
void viewCorrectViewOffsets(int nPlayer, vec3_t const* oldpos);
void InitStatusBar(void);
void UpdateStatusBar();
void viewInit(void);
void viewprocessSprites(tspriteArray& tsprites, int32_t cX, int32_t cY, int32_t cZ, int32_t cA, int32_t smooth);
void viewSetMessage(const char* pMessage, const char* color = nullptr, const MESSAGE_PRIORITY priority = MESSAGE_PRIORITY_NORMAL);


void viewSetErrorMessage(const char* pMessage);
void DoLensEffect(void);
void UpdateDacs(int nPalette, bool bNoTint = false);
void viewDrawScreen(bool sceneonly = false);
void viewUpdateDelirium(void);
void viewSetSystemMessage(const char* pMessage, ...);

inline void viewInterpolateSector(sectortype* pSector)
{
	StartInterpolation(pSector, Interp_Sect_Floorz);
	StartInterpolation(pSector, Interp_Sect_Ceilingz);
	StartInterpolation(pSector, Interp_Sect_Floorheinum);
}

inline void viewInterpolateWall(walltype* pWall)
{
	StartInterpolation(pWall, Interp_Wall_X);
	StartInterpolation(pWall, Interp_Wall_Y);
}

inline void viewBackupSpriteLoc(DBloodActor* actor)
{
	if (!actor->interpolated)
	{
		actor->backuploc();
		actor->interpolated = true;
	}
}


END_BLD_NS
