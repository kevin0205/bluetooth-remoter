#pragma once
//#include "afxcmn.h"
#include "bt_serverDlg.h"

#include <initguid.h>
#include <winsock2.h>
#include <ws2bth.h>
#pragma comment(lib, "ws2_32.lib")

DEFINE_GUID(serviceUUID, 0x81403000, 0x13df, 0xb000, 0x7c, 0xf4, 0x35, 0x0b, 0x4a, 0x21, 0x10, 0xee); // unique id for the Brm BT remote service | must be global


// socket used for communicating with an open connection
class BtCommSocket : public CAsyncSocket
{
public:
	virtual bool isActive();
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};



// socket used for listening to incoming connections
class BtListenerSocket : public CAsyncSocket
{
protected:
	BtCommSocket commSocket;
public:
	virtual void OnAccept(int nErrorCode);
	virtual void stopComm();
	virtual bool sendMessage(wchar_t *msg);
};



// main class which manages the bluetooth connections and uses the classes above internally
class BtComm
{
protected:
	SOCKET listenerSocket;
	BtListenerSocket listenerASocket;
	LPCSADDR_INFO addrInfo;

	virtual void cleanUp();
	virtual void alert(wchar_t * message, int error = 0);
public:
	BtComm()
	{
		listenerSocket = INVALID_SOCKET;
		addrInfo = NULL;
	}

	virtual bool startServer();
	virtual void stopServer();
	virtual bool sendMessage(wchar_t * msg);
};