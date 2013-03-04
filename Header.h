#ifndef HEADER_H
#define HEADER_H

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
#include "Client.h"
#include "Server.h"


#define BUFFSIZE 8192
#define GAPSIZE 15
#define TCP_DEFAULT_PORT 6942
#define ID_TIMER 49999






LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

VOID CALLBACK TimerProc (HWND, UINT, UINT, DWORD);
DWORD WINAPI Connect ( LPVOID );  
DWORD WINAPI TCP_SERVER ( LPVOID );
DWORD WINAPI AcceptThread ( LPVOID );
void CALLBACK TCPRoutine( DWORD, DWORD, LPWSAOVERLAPPED, DWORD );

void stopTimer( void );
void fatal ( char* );
void non_fatal( char* );

typedef struct _SOCKET_INFORMATION {
   OVERLAPPED Overlapped;
   SOCKET Socket;
   CHAR Buffer[8192];
   WSABUF DataBuf;
   DWORD BytesSEND;
   DWORD BytesRECV;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;


#endif
