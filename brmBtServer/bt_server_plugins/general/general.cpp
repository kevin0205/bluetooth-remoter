// general.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "..\\PluginBase.h"
#include <atlstr.h>
#include <map>
#include <set>

class Mouse
{
public:
	static bool moveLeft, moveRight, moveUp, moveDown, quitThread;
	static HANDLE thread;

	static void movementThread (void * nothing)
	{
		unsigned int timeStarted = GetTickCount();
		POINT mousePos;

		while (!quitThread)
		{
			int speed = (int) (1 + (GetTickCount() - timeStarted) / 1000.0f * 20.0f);
			if (speed > 80) speed = 80;

			GetCursorPos(&mousePos);
			if (moveLeft) mousePos.x -= speed;
			if (moveRight) mousePos.x += speed;
			if (moveUp) mousePos.y -= speed;
			if (moveDown) mousePos.y += speed;
			SetCursorPos(mousePos.x, mousePos.y);
			Sleep(50);
		}
		thread = NULL;
	}

	static void updateMovement()
	{
		if (!moveLeft && !moveRight && !moveUp && !moveDown) quitThread = true;
		else
		{
			quitThread = false;
			if (!thread) thread = (HANDLE) _beginthread(&Mouse::movementThread, 0, NULL);
		}
	}

	static void left(bool keyDown)
	{
		moveLeft = keyDown;
		updateMovement();
	}

	static void right(bool keyDown)
	{
		moveRight = keyDown;
		updateMovement();
	}

	static void up(bool keyDown)
	{
		moveUp = keyDown;
		updateMovement();
	}

	static void down(bool keyDown)
	{
		moveDown = keyDown;
		updateMovement();
	}

	static void terminateAll()
	{
		quitThread = true;
	}

	static void click(bool keyDown, wchar_t param)
	{
		switch (param)
		{
			case 'L': mouse_event(keyDown ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL); break;
			case 'R': mouse_event(keyDown ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL); break;
			case 'M': mouse_event(keyDown ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP, 0, 0, 0, NULL); break;
			case '4': mouse_event(keyDown ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP, 0, 0, XBUTTON1, NULL); break;
			case '5': mouse_event(keyDown ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP, 0, 0, XBUTTON2, NULL); break;
		}
	}
};

bool Mouse::moveLeft = false, Mouse::moveRight = false, Mouse::moveUp = false, Mouse::moveDown = false, Mouse::quitThread = false;
HANDLE Mouse::thread = NULL;




class General : public PluginBase
{
protected:
	std::map<CString, int> keynames;

	void addAlias(wchar_t * name, int vkey)
	{
		keynames.insert(std::pair<CString, int>(name, vkey));
	}

	void initKeys()
	{
		addAlias(L"mouse l", VK_LBUTTON);
		addAlias(L"mouse r", VK_RBUTTON);
		addAlias(L"mouse m", VK_MBUTTON);
		addAlias(L"mouse back", VK_XBUTTON1);
		addAlias(L"mouse forward", VK_XBUTTON2);

		addAlias(L"backspace", VK_BACK);
		addAlias(L"tab", VK_TAB);
		addAlias(L"enter", VK_RETURN);
		addAlias(L"shift", VK_SHIFT);
		addAlias(L"ctrl", VK_CONTROL);
		addAlias(L"alt", VK_MENU);
		addAlias(L"pause", VK_PAUSE);
		addAlias(L"caps lock", VK_CAPITAL);

		addAlias(L"escape", VK_ESCAPE);
		addAlias(L"space", VK_SPACE);
		addAlias(L"page up", VK_PRIOR);
		addAlias(L"page down", VK_NEXT);
		addAlias(L"end", VK_END);
		addAlias(L"home", VK_HOME);
		addAlias(L"left", VK_LEFT);
		addAlias(L"up", VK_UP);
		addAlias(L"right", VK_RIGHT);
		addAlias(L"down", VK_DOWN);
		addAlias(L"printscreen", VK_SNAPSHOT);
		addAlias(L"insert", VK_INSERT);
		addAlias(L"delete", VK_DELETE);

		addAlias(L"win", VK_LWIN);
		addAlias(L"context menu", VK_APPS);
		addAlias(L"sleep", VK_SLEEP);

		addAlias(L"num 0", VK_NUMPAD0);
		addAlias(L"num 1", VK_NUMPAD1);
		addAlias(L"num 2", VK_NUMPAD2);
		addAlias(L"num 3", VK_NUMPAD3);
		addAlias(L"num 4", VK_NUMPAD4);
		addAlias(L"num 5", VK_NUMPAD5);
		addAlias(L"num 6", VK_NUMPAD6);
		addAlias(L"num 7", VK_NUMPAD7);
		addAlias(L"num 8", VK_NUMPAD8);
		addAlias(L"num 9", VK_NUMPAD9);
		addAlias(L"num asterisk", VK_MULTIPLY);
		addAlias(L"num plus", VK_ADD);
		addAlias(L"num minus", VK_SUBTRACT);
		addAlias(L"num decimal", VK_DECIMAL);
		addAlias(L"num divide", VK_DIVIDE);

		addAlias(L"f1", VK_F1);
		addAlias(L"f2", VK_F2);
		addAlias(L"f3", VK_F3);
		addAlias(L"f4", VK_F4);
		addAlias(L"f5", VK_F5);
		addAlias(L"f6", VK_F6);
		addAlias(L"f7", VK_F7);
		addAlias(L"f8", VK_F8);
		addAlias(L"f9", VK_F9);
		addAlias(L"f10", VK_F10);
		addAlias(L"f11", VK_F11);
		addAlias(L"f12", VK_F12);
		addAlias(L"f13", VK_F13);
		addAlias(L"f14", VK_F14);
		addAlias(L"f15", VK_F15);
		addAlias(L"f16", VK_F16);
		addAlias(L"f17", VK_F17);
		addAlias(L"f18", VK_F18);
		addAlias(L"f19", VK_F19);
		addAlias(L"f20", VK_F20);
		addAlias(L"f21", VK_F21);
		addAlias(L"f22", VK_F22);
		addAlias(L"f23", VK_F23);
		addAlias(L"f24", VK_F24);

		addAlias(L"num lock", VK_NUMLOCK);
		addAlias(L"scroll lock", VK_SCROLL);

		addAlias(L"browser back", VK_BROWSER_BACK);
		addAlias(L"browser forward", VK_BROWSER_FORWARD);
		addAlias(L"browser refresh", VK_BROWSER_REFRESH);
		addAlias(L"browser stop", VK_BROWSER_STOP);
		addAlias(L"browser search", VK_BROWSER_SEARCH);
		addAlias(L"browser bookmarks", VK_BROWSER_FAVORITES);
		addAlias(L"browser homepage", VK_BROWSER_HOME);

		addAlias(L"volume mute", VK_VOLUME_MUTE);
		addAlias(L"volume down", VK_VOLUME_DOWN);
		addAlias(L"volume up", VK_VOLUME_UP);
		addAlias(L"media next", VK_MEDIA_NEXT_TRACK);
		addAlias(L"media previous", VK_MEDIA_PREV_TRACK);
		addAlias(L"media stop", VK_MEDIA_STOP);
		addAlias(L"media play pause", VK_MEDIA_PLAY_PAUSE);
		addAlias(L"launch mail", VK_LAUNCH_MAIL);
		addAlias(L"launch media", VK_LAUNCH_MEDIA_SELECT);
		addAlias(L"launch app1", VK_LAUNCH_APP1);
		addAlias(L"launch app2", VK_LAUNCH_APP2);

		addAlias(L"tilde", 0xC0);
		addAlias(L"dash", 0xBD);
		addAlias(L"equals", 0xBB);
		addAlias(L"lbracket", 0xDB);
		addAlias(L"rbracket", 0xDD);
		addAlias(L"backslash", 0xDC);
		addAlias(L"semicolon", 0xBA);
		addAlias(L"apostrophe", 0xDE);
		addAlias(L"comma", 0xBC);
		addAlias(L"point", 0xBE);
		addAlias(L"slash", 0xBF);
		addAlias(L"pipe", 0xE2);

		wchar_t tmp[2] = L"x";
		for (int i = '0'; i <= '9'; i++) { tmp[0] = i;  addAlias(tmp, i); }
		for (int i = 'a'; i <= 'z'; i++) { tmp[0] = i;  addAlias(tmp, i - ('a' - 'A')); }
	}

	void sendKey(int virtualKey, bool keyDown)
	{
		if (!virtualKey) return;

		int flags = 0;
		if (!keyDown) flags |= KEYEVENTF_KEYUP;

		switch (virtualKey)
		{
			case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN: case VK_PRIOR: case VK_NEXT: 
			case VK_END: case VK_HOME: case VK_INSERT: case VK_DELETE: case VK_DIVIDE:
				flags |= KEYEVENTF_EXTENDEDKEY; break;
			default: break;
		}

		int scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
		//if (!keyDown) scanCode |= 80;

		keybd_event(virtualKey, scanCode, flags, 10);
	}

	int getKeyCode(CString name)
	{
		std::map<CString, int>::iterator it;
		name.MakeLower();

		it = keynames.find(name);
		if (it == keynames.end()) return 0;
		else return (*it).second;
	}

public:
	General()
	{
		actions.insert(std::pair<CString, executableAction>(L"Volume Up", executableAction(true, L"Number of % the volume should be increased by.\nValid values: 1 to 100", this, 1)) );
		actions.insert(std::pair<CString, executableAction>(L"Volume Down", executableAction(true, L"Number of % the volume should be decreased by.\nValid values: 1 to 100", this, 2)) );
		actions.insert(std::pair<CString, executableAction>(L"Keystroke", executableAction(true, L"Name of the key you'd like to have virtually pressed. See the key list for full reference (right click on the tray icon).\nExamples:\nm\nbackspace\nmedia play pause", this, 3)) );
		actions.insert(std::pair<CString, executableAction>(L"Mouse Up", executableAction(false, L"", this, 4)) );
		actions.insert(std::pair<CString, executableAction>(L"Mouse Down", executableAction(false, L"", this, 5)) );
		actions.insert(std::pair<CString, executableAction>(L"Mouse Left", executableAction(false, L"", this, 6)) );
		actions.insert(std::pair<CString, executableAction>(L"Mouse Right", executableAction(false, L"", this, 7)) );
		actions.insert(std::pair<CString, executableAction>(L"Mouse Click", executableAction(true, L"L for left click, R for right click, M for the middle button\n4 for the back button 5 for the forward button", this, 8)) );
		actions.insert(std::pair<CString, executableAction>(L"Key Combo", executableAction(true, L"List of keys you'd like to have virtually pressed separated with \"+\". See the key list for full reference (right click on the tray icon).\nExample:\nalt+enter", this, 9)) );

		infoCollectors.insert(std::pair<CString, executableAction>(L"Active window title", executableAction(false, L"No parameters to configure...", this, 1)) );

		initKeys();
	}

	virtual ~General() {}

	void volumeUp(CString & params, bool keyDown)
	{
		sendKey(VK_VOLUME_UP, keyDown);
	}

	void volumeDown(CString & params, bool keyDown)
	{
		sendKey(VK_VOLUME_DOWN, keyDown);
	}

	void keystroke(CString & params, bool keyDown)
	{
		sendKey(getKeyCode(params), keyDown);
	}

	void keyCombo(CString & params, bool keyDown)
	{
		wchar_t * tmp = params.GetBuffer();
		wchar_t * keys[5];
		CString tmpcs;

		keys[0] = wcstok(tmp, L"+");
		for (int i = 1; i < 5; i++) keys[i] = wcstok(NULL, L"+");

		if (keyDown)
		{
			for (int i = 0; i < 5; i++)
			{
				if (keys[i]) { tmpcs = keys[i];  keystroke(tmpcs, true); }
			}
		}
		else
		{
			for (int i = 4; i >= 0; i--)
			{
				if (keys[i]) { tmpcs = keys[i];  keystroke(tmpcs, false); }
			}
		}
	}
	
	void callAction(int actionId, CString params, bool keyDown)
	{
		switch(actionId)
		{
		case 1: volumeUp(params, keyDown); break;
		case 2: volumeDown(params, keyDown); break;
		case 3: keystroke(params, keyDown); break;
		case 4: Mouse::up(keyDown); break;
		case 5: Mouse::down(keyDown); break;
		case 6: Mouse::left(keyDown); break;
		case 7: Mouse::right(keyDown); break;
		case 8: Mouse::click(keyDown, params[0]); break;
		case 9: keyCombo(params, keyDown); break;
		}
	}

	CString callInfoCollector(int infoCollectorId, CString params)
	{
		switch(infoCollectorId)
		{
		case 1:
			HWND h = GetForegroundWindow();
			if (!h) return L"";
			wchar_t tmp[256];
			GetWindowText(h, tmp, 255);
			return tmp;
		}
		return L"";
	}


	void onProfileClosed()
	{
		Mouse::terminateAll();
	}
};

extern "C" __declspec(dllexport) PluginBase * loadPlugin()
{
	return new General();
}