// PowerPoint.h : main header file for the PowerPoint DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "..\\PluginBase.h"


// CPowerPointApp
// See PowerPoint.cpp for the implementation of this class
//

class CPowerPointApp : public CWinApp
{
public:
	CPowerPointApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
