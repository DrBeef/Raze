/*
** i_input.cpp
**
**---------------------------------------------------------------------------
** Copyright 2005-2016 Randy Heit
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
#include "m_argv.h"
#include "v_video.h"

#include "d_eventbase.h"
#include "d_gui.h"
#include "c_buttons.h"
#include "c_console.h"
#include "c_dispatch.h"
#include "dikeys.h"
#include "utf8.h"
#include "keydef.h"
#include "i_interface.h"
#include "engineerrors.h"
#include "i_interface.h"


static void I_CheckGUICapture ();
static void I_CheckNativeMouse ();

bool GUICapture;
static bool NativeMouse = true;

CVAR (Bool,  use_mouse,				true, CVAR_ARCHIVE|CVAR_GLOBALCONFIG)


extern int WaitingForKey, chatmodeon;
extern uint8_t ConsoleState;

static void I_CheckGUICapture ()
{
}

void I_SetMouseCapture()
{
}

void I_ReleaseMouseCapture()
{
}

static void I_CheckNativeMouse ()
{
}

void I_GetEvent ()
{
}

void I_StartTic ()
{
	I_CheckGUICapture ();
	I_CheckNativeMouse ();
	I_GetEvent ();
}

void TBXR_FrameSetup();
void I_StartFrame ()
{
	TBXR_FrameSetup();
}
