#include "stdafx.h"
#include "bt_comm.h"

void BtComm::cleanUp ()
{
	_log(L"Closing sockets");
	if ( INVALID_SOCKET != listenerSocket ) closesocket(listenerSocket);
	if ( NULL != addrInfo ) HeapFree(GetProcessHeap(), 0, addrInfo);
	// WSACleanup(); // called automatically by MFC
}

void BtComm::alert(wchar_t * message, int error)
{
	CString tmp;
	if (error) tmp.Format(message, error);
	else tmp = message;

	_log(tmp+L"\n");
	AfxMessageBox(tmp, MB_ICONERROR);
}

bool BtComm::startServer ()
{
	int result;
	int ret = 0;

	// open a bluetooth socket using RFCOMM protocol

	listenerSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if ( INVALID_SOCKET == listenerSocket )
	{
		int e = WSAGetLastError();
		if (e == 10050) alert(L"ERROR: No Bluetooth device installed or Bluetooth radio turned off (error %d)", e);
		else if (e == 10047) alert(L"ERROR: Windows sockets are incompatible with your BT driver. Only Microsoft Bluetooth stack works :/ (error %d)", e);
		else alert(L"ERROR: Unable to open socket (error %d)", e);

		cleanUp();  return false;
	}

	_log(L"Socket open...\n");

	// bind socket

	SOCKADDR_BTH sockAddr = {0};
	sockAddr.addressFamily = AF_BTH;
	sockAddr.port = BT_PORT_ANY;
	
	int	addrLen = sizeof(SOCKADDR_BTH);

	if ( SOCKET_ERROR == bind(listenerSocket, (struct sockaddr *) &sockAddr, sizeof(SOCKADDR_BTH) ) )
	{
		int e = WSAGetLastError();
		if (e == 10050) alert(L"ERROR: No Bluetooth device installed or Bluetooth radio turned off (error %d)", e);
		else alert(L"ERROR: Unable to bind local socket (error %d)", e);
		
		cleanUp();  return false;
	}

	_log(L"Socket bound...\n");

	// computer name for BT service broadcast

	wchar_t computerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD computerNameLen;
	if ( !GetComputerName(computerName, &computerNameLen) )
	{
		alert(L"ERROR: unable to get computer name (error %d)", WSAGetLastError());  cleanUp();  return false;
	}

	// start preparing stuff for BT service advertising

	result = getsockname (listenerSocket, (struct sockaddr *)&sockAddr, &addrLen);
	if ( SOCKET_ERROR == result )
	{
		alert(L"ERROR: Unable to get socket name (error %d)", WSAGetLastError());  cleanUp();  return false;
	}

	LPCSADDR_INFO addrInfo = (LPCSADDR_INFO) HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CSADDR_INFO) );
	if ( NULL == addrInfo ) 
	{ 
		alert(L"!Memory allocation error\n");  cleanUp();  return false;
	}

	addrInfo[0].LocalAddr.iSockaddrLength = sizeof( SOCKADDR_BTH );
	addrInfo[0].LocalAddr.lpSockaddr = (LPSOCKADDR)&sockAddr;
	addrInfo[0].RemoteAddr.iSockaddrLength = sizeof( SOCKADDR_BTH );
	addrInfo[0].RemoteAddr.lpSockaddr = (LPSOCKADDR)&sockAddr;
	addrInfo[0].iSocketType = SOCK_STREAM;
	addrInfo[0].iProtocol = BTHPROTO_RFCOMM;

	WSAQUERYSET	wsaQuerySet = {0};

	ZeroMemory(&wsaQuerySet, sizeof(WSAQUERYSET));
	wsaQuerySet.dwSize = sizeof(WSAQUERYSET);
	wsaQuerySet.lpServiceClassId = (LPGUID) &serviceUUID;

	wsaQuerySet.lpszServiceInstanceName = L"Brm Bluetooth Server";
	wsaQuerySet.lpszComment = L"so yeah...";
	wsaQuerySet.dwNameSpace = NS_BTH;
	wsaQuerySet.dwNumberOfCsAddrs = 1;
	wsaQuerySet.lpcsaBuffer = addrInfo;

	// alright, let's advertise our bluetooth service already!

	if ( SOCKET_ERROR == WSASetService(&wsaQuerySet, RNRSERVICE_REGISTER, 0) )
	{
		int e = WSAGetLastError();
		alert(L"ERROR: Unable to advertise the Brm Bluetooth Remote service (error %d)", WSAGetLastError());  cleanUp();  return false; // such a lenghty preparation and yet we fail!
	}

	if (! listenerASocket.Attach(listenerSocket) )
	{
		alert(L"ERROR: Unable to attach Socket to the MFC CAsyncSocket wrapper\n"); cleanUp();  return false;
	}

	if (! listenerASocket.AsyncSelect())
	{
		int e = WSAGetLastError();
		alert(L"ERROR: ASyncSelect() failed: "); _log(e);  cleanUp();  return false;
	}

	if (! listenerASocket.Listen())
	{
		alert(L"ERROR: Unable to listen on a created socket (error %d)", WSAGetLastError());  cleanUp();  return false;
	}

	_log(L"Listening...\n");

	return true;
}

void BtListenerSocket::OnAccept(int nErrorCode)
{
	if (! commSocket.isActive())
	{
		if (Accept(commSocket)) _log(L"Incoming connection accepted\n");
		else _log(L"ERROR: Cannot accept incoming connection!\n");
	}
	else
	{
		CAsyncSocket tmp;
		if (Accept(tmp)) tmp.Close();
		_log(L"ERROR: Cannot accept connection, a different connection is already in progress!\n");
	}
}

void BtListenerSocket::stopComm()
{
	commSocket.ShutDown();
}

bool BtCommSocket::isActive()
{
	return (m_hSocket != INVALID_SOCKET);
}

void BtCommSocket::OnReceive(int nErrorCode)
{
	static char buf [257];
	static wchar_t buf_w [257];
	static int length;
	static size_t tmp;
	
	length = Receive(buf, 256);
	buf[length] = 0;
	mbstowcs_s(&tmp, buf_w, length+1, buf, _TRUNCATE);
	Cbt_serverDlg::instance()->receiveBTMessage(buf_w);
}

void BtCommSocket::OnClose(int nErrorCode)
{
	Close();
	Cbt_serverDlg::instance()->BTDisconnected();
	_log(L"Connection closed\n");
}

void BtComm::stopServer()
{
	listenerASocket.stopComm();
	listenerASocket.ShutDown();
	cleanUp();
}

bool BtComm::sendMessage(wchar_t * msg)
{
	return listenerASocket.sendMessage(msg);
}

bool BtListenerSocket::sendMessage(wchar_t * msg)
{
	char * tmp;
	int length = wcslen(msg);

	int size_needed = WideCharToMultiByte (CP_UTF8, 0, msg, wcslen(msg), NULL, 0, NULL, NULL);
    tmp = new char[size_needed+4];
	memcpy(tmp, &size_needed, 4); // put size to the very beginning of the data, so that the client will know what size to expect (this is necerrasy as some phones are buggy as hell)

    WideCharToMultiByte (CP_UTF8, 0, msg, wcslen(msg), tmp+4, size_needed, NULL, NULL);

	size_needed += 4;

	int sent = 0, ret;
	while (sent < size_needed) // send as multiple chunks, if the data is larger than 256b
	{
		int chunk_size = size_needed - sent > 256 ? 256 : size_needed - sent;
		
		ret = commSocket.Send(tmp+sent, chunk_size);
		if (SOCKET_ERROR == ret) break;

		sent += chunk_size;
	}

	if (SOCKET_ERROR != ret) { _log(L"Data sent: "); _log(size_needed); }
	else _log(L"Unable to send data\n");
	//_log(msg);
	//_log(L"\n");
	delete(tmp);

	return ( SOCKET_ERROR != ret );
	//return ( SOCKET_ERROR != commSocket.Send(msg, strlen(msg)) );
}