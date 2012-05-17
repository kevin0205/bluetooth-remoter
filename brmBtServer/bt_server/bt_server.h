// bt_server.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cbt_serverApp:
// See bt_server.cpp for the implementation of this class
//

class Cbt_serverApp : public CWinApp
{
private:
	HANDLE mutex;
public:
	Cbt_serverApp();
	~Cbt_serverApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cbt_serverApp theApp;