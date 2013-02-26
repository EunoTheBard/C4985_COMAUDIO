/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Main.cpp
--
-- PROGRAM: 4985 Assignment Two
--
-- FUNCTIONS:
-- LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
-- VOID CALLBACK TimerProc (HWND, UINT, UINT, DWORD);
-- DWORD WINAPI resolve ( LPVOID ); 
-- void stopTimer( void );
-- int host_to_IP ( char*, struct hostent*& );
-- int IP_to_host ( char*, struct hostent*& );
-- int Service_to_Port (  char*, char*, struct servent *&  );
-- int Port_to_Service (  char*, char*, struct servent *&  );
--
-- DATE: January 15, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Christopher Porter
--
-- PROGRAMMER: Christopher Porter
--
----------------------------------------------------------------------------------------------------------------------*/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <stdio.h>
#include <string.h>
#include <cstring>

#include "resource.h"
#include "header.h"


#pragma comment(lib, "Ws2_32.lib")

HWND  hwnd, hEdit, hReturn, hButton;
int		bServer = 1, bConnected = 0;
SOCKET	AcceptSocket;
struct	sockaddr_in server, client;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WinMain
--
-- DATE: January 15, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Christopher Porter
--
-- PROGRAMMER: Christopher Porter
--
-- INTERFACE: int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
--
-- RETURNS: int.
--
----------------------------------------------------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {
	static char szAppName[] = "UDP Server";

	MSG         msg;
	WNDCLASSEX  wndclass;

	wndclass.cbSize				= sizeof(wndclass);
	wndclass.style					= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc		= WndProc;
	wndclass.cbClsExtra			= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance			= hInstance;
	wndclass.hIcon					= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm				= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor				= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.lpszClassName	= szAppName;
	wndclass.lpszMenuName   = MAKEINTRESOURCE(IDR_MENU1);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindow(szAppName, szAppName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
		NULL, NULL, hInstance, NULL);

	hEdit	= CreateWindowEx(0, "EDIT", "localhost",
		ES_CENTER | WS_CHILD  | WS_TABSTOP | WS_BORDER | ES_LEFT,
		0, 0, 0, 0,
		hwnd, (HMENU)(123),
		(HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE), NULL);

	hReturn	= CreateWindowEx(0, "EDIT", "",
		ES_CENTER| WS_CHILD  | WS_BORDER | ES_LEFT | ES_MULTILINE | WS_VSCROLL,
		0, 0, 0, 0,
		hwnd, (HMENU)(124),
		(HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE), NULL);

	hButton = CreateWindowEx(0, "BUTTON", "Connect",
		WS_TABSTOP| WS_CHILD|BS_DEFPUSHBUTTON,
		0, 0, 0, 0,
		hwnd, (HMENU)(ID_BUTTON_CONNECT),
		(HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE), NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while ( GetMessage(&msg, NULL, 0, 0) ) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} 

	return msg.wParam;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WndProc
--
-- DATE: January 15, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Christopher Porter
--
-- PROGRAMMER: Christopher Porter
--
-- INTERFACE: LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
--
-- RETURNS: LRESULT.
--
----------------------------------------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC         hdc;
	WSADATA		wsaData; 
	MINMAXINFO	*pMin;
	WORD		wVersionRequested;  
	RECT		client;
	int			height;
	static int prevMode = ID_CLIENT_SERVER, prevType = ID_PROTOCOL_UDP, prevNum = ID_NUMBEROFPACKETS_10, prevSize = ID_SIZEOFPACKETS_8192;

	switch ( iMsg ) {
	case WM_CREATE:
		wVersionRequested = MAKEWORD(2,2);
		WSAStartup(wVersionRequested, &wsaData);
		return 0;

	case WM_CTLCOLOREDIT:  
		hdc = (HDC)wParam;
		SetTextColor(hdc, RGB(230, 0, 0));	
		return 0;

	case WM_GETMINMAXINFO: 
		pMin = (MINMAXINFO*)lParam;
		pMin->ptMinTrackSize.x = 520;
		pMin->ptMinTrackSize.y = 320;
		return 0;

	case WM_SIZE:
		GetClientRect(hwnd, &client);
		height = client.bottom / 5;
		if( !bServer ) {
			SetWindowPos( hEdit, HWND_TOP, GAPSIZE, GAPSIZE, client.right - GAPSIZE * 2, height - GAPSIZE * 2,  bConnected ? SWP_HIDEWINDOW : SWP_SHOWWINDOW );
			SetWindowPos( hReturn, HWND_TOP, GAPSIZE, bConnected ? GAPSIZE : height, client.right - GAPSIZE * 2, bConnected ? height *  5 -  (GAPSIZE * 2 ) : height * 3 - GAPSIZE,  SWP_SHOWWINDOW );
			SetWindowPos( hButton, HWND_TOP, GAPSIZE, height * 4, client.right - GAPSIZE * 2, height - GAPSIZE,  bConnected ? SWP_HIDEWINDOW : SWP_SHOWWINDOW  ); 
		} else {
			SetWindowPos( hEdit, HWND_TOP, GAPSIZE, GAPSIZE, client.right - GAPSIZE * 2, height - GAPSIZE * 2,  SWP_HIDEWINDOW );
			SetWindowPos( hReturn, HWND_TOP, GAPSIZE, GAPSIZE, client.right - GAPSIZE * 2, bConnected ? ( height * 5 ) - GAPSIZE * 2 : (height * 4)- GAPSIZE *2 ,  SWP_SHOWWINDOW );
			SetWindowPos( hButton, HWND_TOP, GAPSIZE, height * 4, client.right - GAPSIZE * 2, height - GAPSIZE,  bConnected ? SWP_HIDEWINDOW : SWP_SHOWWINDOW );
		}
		return 0;

	case WM_COMMAND:
		switch( wParam ) {
		case ID_FILE_EXIT:
			exit(1);

		case ID_BUTTON_CONNECT:
			CreateThread( NULL, 0, Connect, 0, 0, NULL );
			bConnected = 1;
			SendMessage( hwnd, WM_SIZE, 0, 0 );
			break;
			
		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &client);
		FillRect(hdc, &client, CreateSolidBrush( RGB( 40, 40, 40 ) ) );
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		WSACleanup();
		PostQuitMessage(0);
		return 0;
	}   
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: TimerProc
--
-- DATE: January 15, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Christopher Porter
--
-- PROGRAMMER: Christopher Porter
--
-- INTERFACE: VOID CALLBACK TimerProc (HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
--
-- RETURNS: void.
--
-- NOTES:
-- A simple timer that will update the RETURN EDIT BOX to show the user that their search is in progress. 
----------------------------------------------------------------------------------------------------------------------*/
VOID CALLBACK TimerProc (HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime) {
	static int i = 0;
	int j;
	char result[BUFFSIZE];
	strcpy(result, bServer ? "Thinking" : "Sending" );
	for( j = 0; j < i; j++ ) {
		strcat(result, ".");
	}
	i = ++i % 4;
	SetWindowText(hReturn, result);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Connect
--
-- DATE: January 15, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Christopher Porter
--
-- PROGRAMMER: Christopher Porter
--
-- INTERFACE: DWORD WINAPI Connect ( LPVOID lpParam )
--
-- RETURNS: DWORD.
--
-- NOTES:
-- Called when the CONNECT button is pressed. This will determine which function to call to correctly convert user
-- input to the output.
----------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI Connect ( LPVOID lpParam ) {
	char	user_input[BUFFSIZE];
	char	*host;
	int		port;
	int		client_len;
	int		bTest = 0;

	struct	hostent	*hp;

	SOCKET		sd;

	WSADATA		wsaData;
	WSAEVENT	AcceptEvent;

	SetTimer( hwnd, ID_TIMER, 300, TimerProc );
	GetWindowText( hEdit, user_input, BUFFSIZE );

	host = strtok(user_input, " ");
	port = TCP_DEFAULT_PORT; /* Using the default port -- 6942. */

	/* Create the socket */
	if( (sd = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED) ) == INVALID_SOCKET ) {
		fatal ("TCP:Cannot Create Socket.");
		return -1;
	}


	memset((char *)&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);


	//-- CLIENT SIDE
	if( bServer == 0 ) {
		char buffer[BUFFSIZE], *first, *second;
		LPSOCKET_INFORMATION SI;
		int n;
		HANDLE hFile;
		DWORD SendBytes, bytesWritten;

		struct linger so_linger;
		so_linger.l_onoff = TRUE;
		so_linger.l_linger = 3000;

		if( ( hp = gethostbyname(host) ) == NULL ) {
			fatal ( "Unknown server address." );
			return -1;
		}

		memcpy((char *)&server.sin_addr, hp->h_addr, hp->h_length);

		if (connect (sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
			fatal( "Cannot connect to server." );
			return -1;
		}

		//------------------------------------//


		GetWindowText(hReturn, buffer, BUFFSIZE);
		SetWindowText( hReturn, "" );		

		first = strtok( buffer, " " );
		if( first == NULL ) {
			fatal( "Invalid input. Usage <COMMAND> <PARAM> Eg. send text.txt -- Send for the file 'text.txt'" );
			return -1;
		}

		if( strcmp( first, "send" ) == 0 ) {
			second = strtok( NULL, " " );
			if( second == NULL ) {
				fatal( "Invalid input. Usage send filename" );
				return -1;
			}
		} else if ( strcmp( first, "test" ) == 0 ) {
			bTest = 1;
		} else {
			fatal( "Invalid input. Usage <COMMAND> <PARAM>" );
			return -2;
		}

			if( setsockopt( sd, SOL_SOCKET, SO_LINGER,(const char *) &so_linger, sizeof( so_linger ) ) == -1 ) {
				fatal( "Failure on setsockopt." );
				return -3;
			}


		if( ( SI = ( LPSOCKET_INFORMATION ) GlobalAlloc( GPTR, sizeof( SOCKET_INFORMATION ) ) ) == NULL ) {
			fatal ( "WSAWaitForMultipleEvents failed." );
			return -1;
		} 

		SI->Socket			= sd;
		ZeroMemory( &( SI->Overlapped ), sizeof( WSAOVERLAPPED ) );  
		SI->BytesSEND		= 0;
		SI->BytesRECV		= 0;
		SI->DataBuf.len		= sizeofPackets;
		SI->DataBuf.buf		= SI->Buffer;

		/*Sending a file. */
		if( bTest == 0 ) {
			hFile = CreateFile( second, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL );

			if( hFile == INVALID_HANDLE_VALUE ) {
				fatal( "Cannot open file. Please make sure your file name is correct." );
				return -2;
			}


			DWORD dwHigh, dwLow, bytesRead;
			dwLow = GetFileSize( hFile, &dwHigh );


			while( dwLow > 0 )  {

				memset((char *)&SI->Buffer, 0, sizeof(SI->Buffer));

				/* Read DATA_BUFSIZE -1 into our Buffer. */
				ReadFile( hFile, SI->Buffer, sizeofPackets - 1, &bytesRead, NULL );

				if (WSASend(SI->Socket, &(SI->DataBuf), 1, &SendBytes, 0, &(SI->Overlapped), NULL) == SOCKET_ERROR) {
					if (WSAGetLastError() != WSA_IO_PENDING) {
						non_fatal( "UDP:WSASend failed." );
						CloseHandle( hFile );
						return -2;
					}
				}

				/* If we are at the last packet, break. */
				if( dwLow < ( sizeofPackets - 1 ) ) {
					non_fatal( "Done sending." );
					break;
				}

				dwLow -= ( sizeofPackets - 1 );
			}
			CloseHandle( hFile );
		} else {

			for(int i = 0; i < numberOfPackets; i++ ) {
				memset((char *)&SI->Buffer, 'A' + i % 26, sizeofPackets);
				if( i == numberOfPackets - 1 ) {
					SI->Buffer[ sizeofPackets - 2 ] = 0;
				}
				if (WSASend(SI->Socket, &(SI->DataBuf), 1, &SendBytes, 0, &(SI->Overlapped), NULL) == SOCKET_ERROR) {
					if (WSAGetLastError() != WSA_IO_PENDING) {
						non_fatal( "TCP:WSASend failed." );
						CloseHandle( hFile );
						return -2;
					}
				}
			}

			non_fatal( "Done sending." );
		}


		stopTimer();
		closesocket (sd);
		fatal( "Done sending." );


		//-- CLIENT SIDE --- ///
	} else {

		// --------------------------------- SERVER SIDE -- //
		server.sin_addr.s_addr = htonl(INADDR_ANY);
		if( bind(sd, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR ) {
			fatal ( "Cannot bind name to socket." );
			return -1;
		}

		if( bTCP )
			listen( sd, 5 );

		stopTimer();

		if( ( AcceptEvent = WSACreateEvent() ) == WSA_INVALID_EVENT ) {
			fatal ( "WSACreateEvent." );
			return -1;
		}

		/*  Creating a Accept Thread*/
		if( CreateThread( NULL, 0, AcceptThread, (LPVOID) AcceptEvent, 0, 0 ) == NULL ) {
			fatal ( "Create Accept Event Thread." );
			return -1;
		}


		SetWindowText(hReturn, "Waiting for Connections.");
		while( true ) {
			AcceptSocket = accept( sd, NULL, NULL );
			if( WSASetEvent( AcceptEvent ) == false ) {
				fatal ( "WSASetEvent failed." );
				return -1;
			}
		}
	}
	return 1;
}

DWORD WINAPI AcceptThread( LPVOID lp ) {

	DWORD			Flags;
	WSAEVENT		EventArray[1];
	DWORD			Index;
	DWORD			RecvBytes;

	LPSOCKET_INFORMATION SocketInfo;

	EventArray[0] = (WSAEVENT) lp;

	while( true ) {

		while( true ) {
			Index = WSAWaitForMultipleEvents(1, EventArray, false, WSA_INFINITE, true );

			if( Index == WSA_WAIT_FAILED ) {
				fatal ( "WSAWaitForMultipleEvents failed." );
				return -1;
			}
			if( Index != WAIT_IO_COMPLETION )
				break;
		}
		WSAResetEvent( EventArray[Index - WSA_WAIT_EVENT_0] );

		if( ( SocketInfo = ( LPSOCKET_INFORMATION ) GlobalAlloc( GPTR, sizeof( SOCKET_INFORMATION ) ) ) == NULL ) {
			fatal ( "WSAWaitForMultipleEvents failed." );
			return -1;
		} 
		SocketInfo->Socket = AcceptSocket;
		ZeroMemory( &( SocketInfo->Overlapped ), sizeof( WSAOVERLAPPED ) );  
		SocketInfo->BytesSEND		= 0;
		SocketInfo->BytesRECV		= 0;
		SocketInfo->DataBuf.len		= BUFFSIZE;
		SocketInfo->DataBuf.buf		= SocketInfo->Buffer;

		Flags = 0;
		/*  WSARecv with a WorkerRoutine. */
		if( WSARecv( SocketInfo->Socket, &( SocketInfo->DataBuf ), 1, &RecvBytes, &Flags, &( SocketInfo->Overlapped ), TCPRoutine ) == SOCKET_ERROR ) {
			if( WSAGetLastError() != WSA_IO_PENDING ) {
				fatal ( "TCP:WSARecv failed." );
				return -1;
			}
		}
	}
}

void CALLBACK TCPRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags) {
	DWORD SendBytes, RecvBytes, Flags = 0, BytesWritten = 0;
	static DWORD startTime, endTime;
	static bool OpenFile = true;
	static HANDLE hFile;
	static int index = 0;
	LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;

	if (Error != 0 || BytesTransferred == 0) {
		closesocket(SI->Socket);
		GlobalFree(SI);
		return;
	}

	SI->BytesRECV = 0;
	ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
	SI->DataBuf.buf = SI->Buffer;
	SI->DataBuf.len = BUFFSIZE;

	SetFilePointer( hFile, 0, NULL, FILE_END);
	WriteFile( hFile, SI->Buffer, strlen( SI->Buffer ), &BytesWritten, NULL );
	index++;

	if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags, &(SI->Overlapped), TCPRoutine) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING ) {
			int x = WSAGetLastError();
			printf("WSARecv() failed with error %d\n");
			return;
		}
	}
}

void fatal ( char *error ) {
	stopTimer();
	SetWindowText(hReturn,  error);
	bConnected = 0;
	SendMessage( hwnd, WM_SIZE, 0, 0 );
}

void non_fatal( char *message ) {
	SetWindowText( hReturn, message );
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: STOPTIMER
--
-- DATE: January 15, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Christopher Porter
--
-- PROGRAMMER: Christopher Porter
--
-- INTERFACE: void stopTimer (void)
--
-- RETURNS: void.
--
-- NOTES:
-- Kills the Timer that is running to refresh the text. 
----------------------------------------------------------------------------------------------------------------------*/
void stopTimer( void ) {
	KillTimer(hwnd, ID_TIMER);
}
