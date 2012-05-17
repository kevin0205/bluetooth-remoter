// Foobar2000.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "../PluginBase.h"
#include <shellapi.h>
#pragma comment(lib,"shell32.lib")


class Foobar2000 : public PluginBase
{
protected:
wchar_t fpath[256];
public:
	Foobar2000()
	{
		GetEnvironmentVariable(L"programfiles", fpath, 200);
		wcscat(fpath, L"\\foobar2000\\foobar2000.exe");

		actions.insert(std::pair<CString, executableAction>(L"Foobar2000: Prev. track", executableAction(false, L"", this, 1)) );
		actions.insert(std::pair<CString, executableAction>(L"Foobar2000: Next track", executableAction(false, L"", this, 2)) );
		actions.insert(std::pair<CString, executableAction>(L"Foobar2000: Play/pause", executableAction(false, L"", this, 3)) );
		actions.insert(std::pair<CString, executableAction>(L"Foobar2000: Rewind", executableAction(false, L"", this, 4)) );
		actions.insert(std::pair<CString, executableAction>(L"Foobar2000: Fast forward", executableAction(false, L"", this, 5)) );
		actions.insert(std::pair<CString, executableAction>(L"Foobar2000: Raise volume", executableAction(false, L"", this, 6)) );
		actions.insert(std::pair<CString, executableAction>(L"Foobar2000: Lower volume", executableAction(false, L"", this, 7)) );
		actions.insert(std::pair<CString, executableAction>(L"Foobar2000: Stop", executableAction(false, L"", this, 8)) );

		infoCollectors.insert(std::pair<CString, executableAction>(L"Foobar Now playing", executableAction(false, L"You can change the text in Foobar preferences (CTRL+P) > Display > Default User Interface under the label \"Window Title\"", this, 1)) );
	}

	~Foobar2000()
	{
	}

	void callAction(int actionId, CString params, bool keyDown)
	{
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

		switch(actionId)
		{
		case 1: // previous track
			if (keyDown) ShellExecute(NULL, NULL, fpath, L"/prev", NULL, NULL);
			break;
		case 2: // next track
			if (keyDown) ShellExecute(NULL, NULL, fpath, L"/next", NULL, NULL);
			break;
		case 3: // play/pause
			if (keyDown) ShellExecute(NULL, NULL, fpath, L"/playpause", NULL, NULL);
			break;
		case 4: // rewind
			if (keyDown) ShellExecute(NULL, NULL, fpath, L"/command:\"Seek back by 5 seconds\"", NULL, NULL);
			break;
		case 5: // fast forward
			if (keyDown) ShellExecute(NULL, NULL, fpath, L"/command:\"Seek ahead by 5 seconds\"", NULL, NULL);
			break;
		case 6: // raise volume
			if (keyDown) ShellExecute(NULL, NULL, fpath, L"/command:\"Volume Up\"", NULL, NULL);
			break;
		case 7: // lower volume by 5%
			if (keyDown) ShellExecute(NULL, NULL, fpath, L"/command:\"Volume Down\"", NULL, NULL);
			break;
		case 8: // next track
			if (keyDown) ShellExecute(NULL, NULL, fpath, L"/stop", NULL, NULL);
			break;
		}
	}

	CString getFoobarNowPlaying()
	{
		HWND hwndFoo = FindWindow(L"{97E27FAA-C0B3-4b8e-A693-ED7881E99FC1}",NULL);
		if (!hwndFoo) return L"";

		wchar_t title[256];
		GetWindowText (hwndFoo, title, 255);

		// chop off the trailing "[foobar2000 vX.X.X.X]"
		int i;
		for (i = wcslen(title)-1; i>0; i--) if (title[i] == '[') break;
		if (i>2) title[i-1] = 0;

		return title;
	}

	CString callInfoCollector(int infoCollectorId, CString params)
	{
		switch(infoCollectorId)
		{
		case 1:
			return getFoobarNowPlaying();
		}
		return L"";
	}
};

extern "C" __declspec(dllexport) PluginBase * loadPlugin()
{
	return new Foobar2000();
}