#include "stdafx.h"
#include "winamp_headers.h"

/*
 * Gets advanced metadata of the file being played and some other useful info
 * The advanced metadata can be normally retrieved ONLY by Winamp plugins, so this class has to do some pretty weird shit :) such as:
 * reading, writing, allocating, freeing and copying memory of a different process (e.g. Winamp)
 * Also, it should should be able to deal with Winamp being closed and started during lifetime of this class
 */
class WinampInfo
{
protected:
	HWND hwndWinamp;
	DWORD processId;
	HANDLE process;

	char *remoteBuf, *whichBuf, *structBuf;
	extendedFileInfoStruct xstruct;

	bool initialize ()
	{
		initialized = true;

		hwndWinamp = FindWindowA("Winamp v1.x",NULL);
		if (!hwndWinamp) { initialized = false; return false; }

		GetWindowThreadProcessId(hwndWinamp, &processId);
		if (!processId) { initialized = false; return false; }

		process = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
		if (!process) { initialized = false; return false; }

		// allocate the memory in winamp's space
		remoteBuf = (char *)allocWinamp(64);
		whichBuf = (char *)allocWinamp(16);
		structBuf = (char *)allocWinamp(sizeof(xstruct));

		return true;
	}

	void *allocWinamp(unsigned long bufsize) // allocate chunk of memory in winamp's address space
	{
		void *remoteBuf;
		remoteBuf = VirtualAllocEx(process, NULL, bufsize, MEM_COMMIT, PAGE_READWRITE);
		return remoteBuf;
	}

	bool freeWinamp(void *remoteBuf, unsigned long bufsize) // free the memory in winamp's space
	{	
		int isError;
		isError = VirtualFreeEx(process, remoteBuf, bufsize, MEM_DECOMMIT);
		if (isError) return false;

		// release it
		isError = VirtualFreeEx(process, remoteBuf, 0, MEM_RELEASE);
		if (isError) return false;

		return true;
	}

	unsigned long readWinampToLocal(void *remoteBuf, void *localBuf, unsigned long bufsize) // returns how many bytes were copied to local buffer
	{
		int isError;
		isError = ReadProcessMemory(process, remoteBuf, localBuf, bufsize, NULL);

		if (!isError) return 0; else return bufsize;
	}

	unsigned long writeLocalToWinamp(void *localBuf, void *remoteBuf, unsigned long bufsize) // returns how many bytes were written to winamp's space
	{
		int isError;
		isError = WriteProcessMemory(process, remoteBuf, localBuf, bufsize, NULL);

		if (!isError) return 0; else return bufsize;
	}

public:
	bool initialized;
	char artist[64];
	char title[64];
	char album[64];
	char track[8];
	char year[8];
	char genre[64];

	WinampInfo()
	{
		initialize();
	}

	~WinampInfo()
	{
		if (!initialized) return;

		// free up the memory we allocated in winamp's space
		freeWinamp(remoteBuf, 256);
		freeWinamp(whichBuf, sizeof(whichBuf));
		freeWinamp(structBuf, sizeof(xstruct));

		CloseHandle(process);
	}
		
	int getTrackNumber()
	{
		if (!initialized && !initialize()) return -1;
		return SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_GETLISTPOS);
	}

	int getTotalTracks()
	{
		if (!initialized && !initialize()) return -1;
		return SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_GETLISTLENGTH);
	}

	int getTrackLength()
	{
		if (!initialized && !initialize()) return -1;
		return SendMessage(hwndWinamp, WM_WA_IPC, 1, IPC_GETOUTPUTTIME);

	}

	int getTrackElapsed()
	{
		if (!initialized && !initialize()) return -1;
		return SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_GETOUTPUTTIME) / 1000;
	}

	bool getMetaInfo()
	{
		artist[0] = title[0] = album[0] = track[0] = year[0] = genre[0] = 0;

		if (!initialized && !initialize()) return false;
		if (!FindWindowA("Winamp v1.x",NULL))
		{
			initialized = false;
			return false;
		}

		// fill in ext struct
		xstruct.filename = (char *)SendMessage(hwndWinamp, WM_WA_IPC, getTrackNumber(), IPC_GETPLAYLISTFILE);
		xstruct.metadata = whichBuf;
		xstruct.ret = remoteBuf;
		xstruct.retlen = 64;

		writeLocalToWinamp(&xstruct, structBuf, sizeof(xstruct));

		// query for all the metadata
		writeLocalToWinamp("ARTIST", whichBuf, 7);
		if(!SendMessage(hwndWinamp,WM_WA_IPC,(int)structBuf, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE)) artist[0] = 0;
		else readWinampToLocal(remoteBuf, artist, 64);

		writeLocalToWinamp("TITLE", whichBuf, 6);
		if(!SendMessage(hwndWinamp,WM_WA_IPC,(int)structBuf, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE)) title[0] = 0;
		else readWinampToLocal(remoteBuf, title, 64);

		writeLocalToWinamp("ALBUM", whichBuf, 6);
		if(!SendMessage(hwndWinamp,WM_WA_IPC,(int)structBuf, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE)) album[0] = 0;
		else readWinampToLocal(remoteBuf, album, 64);

		writeLocalToWinamp("TRACK", whichBuf, 6);
		if(!SendMessage(hwndWinamp,WM_WA_IPC,(int)structBuf, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE)) track[0] = 0;
		else readWinampToLocal(remoteBuf, track, 8);

		writeLocalToWinamp("YEAR", whichBuf, 5);
		if(!SendMessage(hwndWinamp,WM_WA_IPC,(int)structBuf, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE)) year[0] = 0;
		else readWinampToLocal(remoteBuf, year, 8);

		writeLocalToWinamp("GENRE", whichBuf, 6);
		if(!SendMessage(hwndWinamp,WM_WA_IPC,(int)structBuf, IPC_GET_EXTENDED_FILE_INFO_HOOKABLE)) genre[0] = 0;
		else readWinampToLocal(remoteBuf, genre, 64);

		return true;
	}
};