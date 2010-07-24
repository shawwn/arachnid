//========================================================================
//	file:		r_gl2_win32.cpp
//	author:		Shawn Presser 
//	date:		7/1/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "r_gl2_common.h"

// windows headers.
#include "../engine/e_lean_windows.h"
//========================================================================

//========================================================================
// Globals
//========================================================================
HWND		hWnd(NULL);
HDC			hDC(NULL);
//========================================================================

//===================
// GL2_Startup
//===================
bool							GL2_Startup(const wstring& windowTitle)
{
	HWND wnd(FindWindow(NULL, windowTitle.c_str()));
	E_VALIDATE(wnd != NULL,
		"gl2", "could not find window handle",
		return false );

	HDC dc(GetDC(wnd));
	if (dc == NULL)
		return false;

	hWnd = wnd;
	hDC = dc;

	return true;
}


//===================
// GL2_Shutdown
//===================
bool							GL2_Shutdown()
{
	if (hDC != NULL)
	{
		assert(hWnd != NULL);
		if (hWnd != NULL)
			ReleaseDC(hWnd, hDC);
	}
	hDC = NULL;
	hWnd = NULL;
	return true;
}


//===================
// GL2_SwapBuffers
//===================
bool							GL2_SwapBuffers()
{
	if (hDC == NULL)
		return false;

	if (SwapBuffers(hDC) != TRUE)
		return false;

	return true;
}


//===================
// GL2_RefreshWindow
//===================
bool							GL2_RefreshWindow()
{
	InvalidateRect(hWnd, NULL, TRUE);
	return true;
}

