// iTunes.h : main header file for the iTunes DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "..\\PluginBase.h"

#undef GetFreeSpace // this is some silly macro which clashes with iTunes COM
#include "CiTunes.h"
#include "CITTrack.h"


// CiTunesApp
// See iTunes.cpp for the implementation of this class
//

class CiTunesApp : public CWinApp
{
public:
	CiTunesApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
