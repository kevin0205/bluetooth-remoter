// iTunes.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "iTunes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CiTunesApp

BEGIN_MESSAGE_MAP(CiTunesApp, CWinApp)
END_MESSAGE_MAP()


// CiTunesApp construction

CiTunesApp::CiTunesApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CiTunesApp object

CiTunesApp theApp;


// CiTunesApp initialization

BOOL CiTunesApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}




class iTunes : public PluginBase
{
protected:
	bool initialized;
	CiTunes app;

	bool initIT()
	{
		try
		{
			if (!FindWindowW(L"iTunes", NULL))
			{
				// AfxMessageBox(L"iTunes not running");
				
				if (initialized) app.ReleaseDispatch();

				initialized = false;
				return false;
			}
		}
		catch(COleException * e) // in case we couldn't properly release the dispatch
		{
			return false;
		}

		if (!initialized)
		{
			COleException e;
			if(!app.CreateDispatch(L"iTunes.Application", &e)) {
			   /*CString str;
			   str.Format(L"CreateDispatch() failed w/err 0x%08lx", e.m_sc);
			   AfxMessageBox(str, MB_SETFOREGROUND);*/
			   return false;
			}
			initialized = true;
		}

		return true;
	}

public:
	iTunes()
	{
		initialized = false;

		actions.insert(std::pair<CString, executableAction>(L"iTunes: Prev. track", executableAction(false, L"", this, 1)) );
		actions.insert(std::pair<CString, executableAction>(L"iTunes: Next track", executableAction(false, L"", this, 2)) );
		actions.insert(std::pair<CString, executableAction>(L"iTunes: Play/pause", executableAction(false, L"", this, 3)) );
		actions.insert(std::pair<CString, executableAction>(L"iTunes: Rewind", executableAction(false, L"", this, 4)) );
		actions.insert(std::pair<CString, executableAction>(L"iTunes: Fast forward", executableAction(false, L"", this, 5)) );
		actions.insert(std::pair<CString, executableAction>(L"iTunes: Raise volume by 5%", executableAction(false, L"", this, 6)) );
		actions.insert(std::pair<CString, executableAction>(L"iTunes: Lower volume by 5%", executableAction(false, L"", this, 7)) );
		actions.insert(std::pair<CString, executableAction>(L"iTunes: Stop", executableAction(false, L"", this, 8)) );

		infoCollectors.insert(std::pair<CString, executableAction>(L"ITunes Info", executableAction(true, L"Available tokens:\n%title %artist %album %isplaying %genre %year\n%track %elapsed %remaining %length\nExample:\n%isplaying%artist - %title", this, 1)) );

	}

	~iTunes()
	{
		if (initialized) app.ReleaseDispatch();
	}
	
	void callAction(int actionId, CString params, bool keyDown)
	{
		::CoInitializeEx(NULL, COINIT_MULTITHREADED); // make OLE multrithread-aware

		if (!initIT()) return;

		switch(actionId)
		{
		case 1: if (keyDown) app.BackTrack(); break;
		case 2: if (keyDown) app.NextTrack(); break;
		case 3: if (keyDown) app.PlayPause(); break;
		case 4: if (keyDown) app.Rewind(); else app.Resume(); break;
		case 5: if (keyDown) app.FastForward(); else app.Resume(); break;
		case 6:
			if (keyDown) {
				long vol = app.get_SoundVolume();
				vol += 5;  if (vol > 100) vol = 100;
				app.put_SoundVolume(vol);
				break;
			}
		case 7:
			if (keyDown) {
				long vol = app.get_SoundVolume();
				vol -= 5;  if (vol < 0) vol = 0;
				app.put_SoundVolume(vol);
				break;
			}
		case 8: if (keyDown) app.Stop(); break;
		}
	}

	CString getIsPlayingString()
	{
		int state = app.get_PlayerState();
		switch (state)
		{
		case 0: return L"(stopped) ";
		case 2: return L"[ >> ] ";
		case 3: return L"[ << ] ";
		default: return L"";
		}
	}

	CString getInfo(CString params)
	{
		try
		{
			CString tmp;

			int elapsed = app.get_PlayerPosition(); tmp.Format(L"%d:%.2d", elapsed / 60, elapsed % 60); // this throws an exception when no track is selected (e.g. after iTunes starts); there is (probably) no other way to determine this
			params.Replace(L"%elapsed", tmp);

			CITTrack track = app.get_CurrentTrack();
			
			params.Replace(L"%album", track.get_Album());
			params.Replace(L"%artist", track.get_Artist());
			params.Replace(L"%genre", track.get_Genre());
			params.Replace(L"%title", track.get_Name());
			params.Replace(L"%isplaying", getIsPlayingString());

			int year = track.get_Year(); tmp.Format(L"%d", year);
			params.Replace(L"%year", tmp);

			int trackNo = track.get_TrackNumber(); tmp.Format(L"%d", trackNo);
			params.Replace(L"%track", tmp);

			int total = track.get_Duration(); tmp.Format(L"%d:%.2d", total / 60, total % 60);
			params.Replace(L"%length", tmp);

			int rem = total-elapsed; tmp.Format(L"%d:%.2d", rem / 60, rem % 60);
			params.Replace(L"%remaining", tmp);

			track.ReleaseDispatch();

			return params;
		}
		catch(COleException * e)
		{
			return L"iTunes not responding";
		}
		catch(COleDispatchException * e)
		{
			return L"No track selected";
		}
	}

	CString callInfoCollector(int infoCollectorId, CString params)
	{
		::CoInitializeEx(NULL, COINIT_MULTITHREADED); // make OLE multrithread-aware
		if (!initIT()) return L"iTunes not running";

		if (infoCollectorId == 1) return getInfo(params);

		return L"";
	}

	void onProfileClosed()
	{
		if (initialized)
		{
			try
			{
				app.Resume(); // in case the connection was closed while rewinding/fast-forwarding :)
				app.ReleaseDispatch();
			}
			catch(COleException * e)
			{
				// iTunes not responding
			}
		}
		initialized = false;
	}
};




extern "C" __declspec(dllexport) PluginBase * loadPlugin()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new iTunes();
}