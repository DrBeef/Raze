//-------------------------------------------------------------------------
/*
Copyright (C) 1997, 2005 - 3D Realms Entertainment

This file is part of Shadow Warrior version 1.2

Shadow Warrior is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

Original Source: 1997 - Frank Maddin and Jim Norwood
Prepared for public release: 03/28/2005 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------

#include "ns.h"
//#define MAIN
#define QUIET
#include "build.h"

#include "names2.h"
#include "panel.h"
#include "game.h"
#include "network.h"

BEGIN_SW_NS

bool PredictionOn = true;
bool Prediction = false;
PLAYER PredictPlayer;
USER PredictUser;
PLAYERp ppp = &PredictPlayer;

typedef struct
{
    int x,y,z;
    binangle ang;
    fixedhoriz horiz;
    short filler;
} PREDICT, *PREDICTp;

PREDICT Predict[/*MOVEFIFOSIZ*/256];
int predictmovefifoplc;

void DoPlayerSectorUpdatePreMove(PLAYERp);
void DoPlayerSectorUpdatePostMove(PLAYERp);


void
InitPrediction(PLAYERp pp)
{
    if (!PredictionOn)
        return;

    // make a copy of player struct and sprite
    *ppp = *pp;
    PredictUser = *User[pp->PlayerSprite];
}

void
DoPrediction(PLAYERp ppp)
{
#if 0
    USERp u;
    SPRITE spr;
    int bakrandomseed;

    // routine called from MoveLoop

    if (!PredictionOn)
        return;

    ppp->input = Player[myconnectindex].inputfifo[predictmovefifoplc & (MOVEFIFOSIZ-1)];

    // get rid of input bits so it doesn't go into other code branches that would
    // get it out of sync
    ppp->input.actions &= ~(SB_WEAPONMASK_BITS|SB_ITEMUSE_BITS|SB_INVNEXT|SB_INVPREV|SB_INVUSE|SB_HOLSTER|SB_CENTERVIEW|SB_FIRE|SB_OPEN);;
    ppp->KeyPressBits |= (SB_WEAPONMASK_BITS|SB_ITEMUSE_BITS|SB_INVNEXT|SB_INVPREV|SB_INVUSE|SB_HOLSTER|SB_CENTERVIEW|SB_FIRE|SB_OPEN);

    // back up things so they won't get stepped on
    bakrandomseed = randomseed;
    spr = sprite[Player[myconnectindex].PlayerSprite];
    sprite[Player[myconnectindex].PlayerSprite].cstat = 0;

    u = User[ppp->PlayerSprite];
    User[ppp->PlayerSprite] = &PredictUser;

    ppp->oposx = ppp->posx;
    ppp->oposy = ppp->posy;
    ppp->oposz = ppp->posz;
    ppp->angle.backup();
    ppp->horizon.backup();

    // go through the player MOVEMENT code only
    Prediction = true;
    DoPlayerSectorUpdatePreMove(ppp);
    (*ppp->DoPlayerAction)(ppp);
    DoPlayerSectorUpdatePostMove(ppp);
    Prediction = false;

    // restore things
    User[ppp->PlayerSprite] = u;
    sprite[Player[myconnectindex].PlayerSprite] = spr;
    randomseed = bakrandomseed;

    Predict[predictmovefifoplc & (MOVEFIFOSIZ-1)].x = ppp->posx;
    Predict[predictmovefifoplc & (MOVEFIFOSIZ-1)].y = ppp->posy;
    Predict[predictmovefifoplc & (MOVEFIFOSIZ-1)].z = ppp->posz;
    Predict[predictmovefifoplc & (MOVEFIFOSIZ-1)].ang = ppp->angle.ang;
    Predict[predictmovefifoplc & (MOVEFIFOSIZ-1)].horiz = ppp->horizon.horiz;
    predictmovefifoplc++;
#endif
}

void
CorrectPrediction(int actualfifoplc)
{
#if 0
    PREDICTp predict = &Predict[actualfifoplc & (MOVEFIFOSIZ-1)];

    if (!PredictionOn)
        return;

    if (!CommEnabled)
        return;

    // see if player position is predicted position
    if (predict->ang == Player[myconnectindex].angle.ang &&
        predict->x == Player[myconnectindex].posx &&
        predict->y == Player[myconnectindex].posy &&
        predict->z == Player[myconnectindex].posz &&
        predict->horiz == Player[myconnectindex].horizon.horiz
        )
    {
        return;
    }

//    //DSPRINTF(ds,"PREDICT ERROR: %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", predict->angle.ang.asbuild(), Player[myconnectindex].angle.ang.asbuild(), predict->x,    Player[myconnectindex].posx, predict->y,    Player[myconnectindex].posy, predict->z,    Player[myconnectindex].posz,  predict->horiz.asbuild(), Player[myconnectindex].horizon.horiz.asbuild()));
//    MONO_PRINT(ds);

    InitPrediction(&Player[myconnectindex]);
    // puts the predicted pos back to actual pos
    predictmovefifoplc = movefifoplc;

    while (predictmovefifoplc < Player[myconnectindex].movefifoend)
    {
        DoPrediction(ppp);
    }
#endif
}

END_SW_NS
