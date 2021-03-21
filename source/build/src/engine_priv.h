// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.
//
// This file has been modified from Ken Silverman's original release
// by Jonathon Fowler (jf@jonof.id.au)
// by the EDuke32 team (development@voidpoint.com)

#pragma once

#include "cmdlib.h"

#ifndef ENGINE_PRIV_H
#define ENGINE_PRIV_H

extern int16_t thesector[MAXWALLSB], thewall[MAXWALLSB];
extern int16_t bunchfirst[MAXWALLSB], bunchlast[MAXWALLSB];
extern int16_t maskwall[MAXWALLSB], maskwallcnt;
extern tspriteptr_t tspriteptr[MAXSPRITESONSCREEN + 1];
extern int32_t xdimen, xdimenscale, xdimscale, ydimen;
extern float fxdimen;
extern int32_t globalposx, globalposy, globalposz;
extern fixed_t qglobalhoriz, qglobalang;
extern float fglobalposx, fglobalposy, fglobalposz;
extern int16_t globalang, globalcursectnum;
extern int32_t globalpal, cosglobalang, singlobalang;
extern int32_t cosviewingrangeglobalang, sinviewingrangeglobalang;
extern int32_t xyaspect;
extern int32_t globalshade;
extern int16_t globalpicnum;

extern int32_t globalorientation;

extern char inpreparemirror;

extern int16_t sectorborder[256];
extern int32_t hitallsprites;

extern int16_t bunchp2[MAXWALLSB];
extern int16_t numscans, numbunches;


// int32_t wallmost(int16_t *mostbuf, int32_t w, int32_t sectnum, char dastat);

void set_globalang(fixed_t const ang);

int32_t animateoffs(int tilenum, int fakevar);

static FORCE_INLINE int32_t bad_tspr(tspriteptr_t tspr)
{
    // NOTE: tspr->owner >= MAXSPRITES (could be model) has to be handled by
    // caller.
    return (tspr->owner < 0 || (unsigned)tspr->picnum >= MAXTILES);
}

inline void set_globalpos(int32_t const x, int32_t const y, int32_t const z)
{
    globalposx = x, fglobalposx = (float)x;
    globalposy = y, fglobalposy = (float)y;
    globalposz = z, fglobalposz = (float)z;
}


inline int widthBits(int num)
{
    return sizeToBits(tileWidth(num));
}

inline int heightBits(int num)
{
    return sizeToBits(tileHeight(num));
}


#endif	/* ENGINE_PRIV_H */
