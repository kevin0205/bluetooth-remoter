// bt_server.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "bt_server.h"
#include "bt_serverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cbt_serverApp

BEGIN_MESSAGE_MAP(Cbt_serverApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// Cbt_serverApp construction

Cbt_serverApp::Cbt_serverApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

Cbt_serverApp::~Cbt_serverApp()
{
	if (mutex) CloseHandle(mutex);
}


// The one and only Cbt_serverApp object

Cbt_serverApp theApp;


// Cbt_serverApp initialization

BOOL Cbt_serverApp::InitInstance()
{
	mutex = NULL;
	mutex = ::CreateMutex(NULL, TRUE, L"Global\\BrmBTGlobalMutex");

	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(L"Brm BT Remote already running!\nYou will find the icon in the system tray.");
		return FALSE;
	}
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	AfxInitRichEdit2();

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	Cbt_serverDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
