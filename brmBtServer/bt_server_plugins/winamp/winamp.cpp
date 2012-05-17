// winamp.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "../PluginBase.h"
#include "WinampInfo.h"


class Winamp : public PluginBase
{
protected:
	WinampInfo * w; // WinampInfo will be initialized only if it's needed

	bool isStopped()
	{
		if (SendMessage(FindWindow(L"Winamp v1.x", NULL), WM_WA_IPC, 0, IPC_ISPLAYING) == 0) return true;
		return false;
	}

	CString getIsPlayingString()
	{
		HWND h = FindWindow(L"Winamp v1.x", NULL);
		if (!h) return L"(Winamp not running) ";

		int res = SendMessage(h, WM_WA_IPC, 0, IPC_ISPLAYING);
		switch (res)
		{
		case 1: return L"";
		case 3: return L"(paused) ";
		default: return L"(stopped) ";
		}
	}

public:
	Winamp()
	{
		w = NULL;
		actions.insert(std::pair<CString, executableAction>(L"Winamp: Prev. track", executableAction(false, L"", this, 1)) );
		actions.insert(std::pair<CString, executableAction>(L"Winamp: Next track", executableAction(false, L"", this, 2)) );
		actions.insert(std::pair<CString, executableAction>(L"Winamp: Play/pause", executableAction(false, L"", this, 3)) );
		actions.insert(std::pair<CString, executableAction>(L"Winamp: Rewind", executableAction(false, L"", this, 4)) );
		actions.insert(std::pair<CString, executableAction>(L"Winamp: Fast forward", executableAction(false, L"", this, 5)) );
		actions.insert(std::pair<CString, executableAction>(L"Winamp: Raise volume by 5%", executableAction(false, L"", this, 6)) );
		actions.insert(std::pair<CString, executableAction>(L"Winamp: Lower volume by 5%", executableAction(false, L"", this, 7)) );
		actions.insert(std::pair<CString, executableAction>(L"Winamp: Stop", executableAction(false, L"", this, 8)) );

		infoCollectors.insert(std::pair<CString, executableAction>(L"Winamp Info", executableAction(true, L"Available tokens:\n%title %artist %album %isplaying %genre %year\n%track %playlistpos %playlistlength\n%elapsed %remaining %length\nExample:\n%isplaying%artist - %title", this, 1)) );
	}

	~Winamp()
	{
		if (w) delete w;
	}

	void callAction(int actionId, CString params, bool keyDown)
	{
		switch(actionId)
		{
		case 1: // previous track
			if (keyDown) SendMessage(FindWindow(L"Winamp v1.x", NULL), 0x111, 40044, 0);
			break;
		case 2: // next track
			if (keyDown) SendMessage(FindWindow(L"Winamp v1.x", NULL), 0x111, 40048, 0);
			break;
		case 3: // play/pause
			if (keyDown)
			{
				if (isStopped())
					SendMessage(FindWindow(L"Winamp v1.x", NULL), 0x111, 40045, 0); // play
				else
					SendMessage(FindWindow(L"Winamp v1.x", NULL), 0x111, 40046, 0); // pause/play
			}
			break;
		case 4: // rewind
			if (keyDown) SendMessage(FindWindow(L"Winamp v1.x", NULL), 0x111, 40144, 0);
			break;
		case 5: // fast forward
			if (keyDown) SendMessage(FindWindow(L"Winamp v1.x", NULL), 0x111, 40148, 0);
			break;
		case 6: // raise volume by 5%
			if (keyDown)
			{
				HWND h = FindWindow(L"Winamp v1.x", NULL);
				if (!h) break;
				for (int i = 0; i < 5; i++) SendMessage(h, 0x111, 40058, 0);
			}
			break;
		case 7: // lower volume by 5%
			if (keyDown)
			{
				HWND h = FindWindow(L"Winamp v1.x", NULL);
				if (!h) break;
				for (int i = 0; i < 5; i++) SendMessage(h, 0x111, 40059, 0);
			}
			break;
		case 8: // stop
			if (keyDown) SendMessage(FindWindow(L"Winamp v1.x", NULL), 0x111, 40047, 0);
			break;
		}
	}

	CString callInfoCollector(int infoCollectorId, CString params)
	{
		switch(infoCollectorId)
		{
		case 1:
			return getInfo(params);
		}
		return L"";
	}

	CString getInfo(CString params)
	{
		if (!w) w = new WinampInfo();

		static wchar_t buf[64];

		w->getMetaInfo();
		
		mbstowcs(buf, w->album, 64); params.Replace(L"%album", buf);
		mbstowcs(buf, w->artist, 64); params.Replace(L"%artist", buf);
		mbstowcs(buf, w->genre, 64); params.Replace(L"%genre", buf);
		mbstowcs(buf, w->title, 64); params.Replace(L"%title", buf);
		mbstowcs(buf, w->year, 64); params.Replace(L"%year", buf);
		mbstowcs(buf, w->track, 64); params.Replace(L"%track", buf);
		params.Replace(L"%isplaying", getIsPlayingString());
		
		CString tmp;

		int elapsed = w->getTrackElapsed(); tmp.Format(L"%d:%d", elapsed / 60, elapsed % 60);
		params.Replace(L"%elapsed", tmp);

		int total = w->getTrackLength(); tmp.Format(L"%d:%d", total / 60, total % 60);
		params.Replace(L"%length", tmp);

		int rem = total-elapsed; tmp.Format(L"%d:%d", rem / 60, rem % 60);
		params.Replace(L"%remaining", tmp);

		int plPos = w->getTrackNumber()+1; tmp.Format(L"%d", plPos);
		params.Replace(L"%playlistpos", tmp);

		int plLength = w->getTotalTracks(); tmp.Format(L"%d", plLength);
		params.Replace(L"%playlistlength", tmp);

		return params;
	}
};

extern "C" __declspec(dllexport) PluginBase * loadPlugin()
{
	return new Winamp();
}