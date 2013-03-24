#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <stdio.h>
#include <string.h>
#include <cstring>

#include "Define.h"
#include "Prototype.h"
#include "Socket_Information.h"

#pragma comment(lib, "Ws2_32.lib")

SOCKET AcceptSocket;

int main() {
	ServerTCPSetup();
	getchar();
	return 0;
}

/*
 * ServerTCPSetup will setup the server waiting for TCP connections, call the AcceptThread and start a thread per 
 * client using the worker thread.   
 */
int ServerTCPSetup() {
	int					port = DEFAULT_PORT; /* Using the default port -- 6942. */
	struct	sockaddr_in	server;
	SOCKET				sd;
	WSAEVENT			AcceptEvent;

	WSADATA wsaData;

	/* Setting up the version and calling WSAStartup. */
	WORD wVersionRequested = MAKEWORD(2,2);
	WSAStartup(wVersionRequested, &wsaData);

	/* Creating the socket to accept incoming connections. */
	if( (sd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED) ) == INVALID_SOCKET ) {
		printf ("Cannot Create Socket.\n");
		return WSAGetLastError();
	}

	memset((char *)&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);


	/* Binding the socket. */
	if( bind(sd, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR ) {
		printf ( "Cannot bind name to socket.\n" );
		return WSAGetLastError();
	}

	/* Listening for upto 5 clients at the same time. */
	listen( sd, 5 );

	/* Create an AcceptEvent. */
	if( ( AcceptEvent = WSACreateEvent() ) == WSA_INVALID_EVENT ) {
		printf ( "WSACreateEvent.\n" );
		return WSAGetLastError();
	}
	/* Create a thread to handle the AcceptEvent. */
	if( CreateThread( NULL, 0, AcceptThread, (LPVOID) AcceptEvent, 0, 0 ) == NULL ) {
		printf ( "Create Accept Event Thread.\n" );
		return WSAGetLastError();
	}

	/* We are now waiting for connections. */
	printf( "Waiting for Connections.\n");

	/* While running, accept all clients, and set the AcceptEvent. */
	while( true ) {
		AcceptSocket = accept( sd, NULL, NULL );
		if( WSASetEvent( AcceptEvent ) == false ) {
			printf ( "WSASetEvent failed.\n" );
			return WSAGetLastError();
		}
	}
	return 1;
}


/*
 * AcceptThread will wait for AcceptEvents to be triggered and then start to recv data.
 */
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
				printf ( "WSAWaitForMultipleEvents failed.\n" );
				return WSAGetLastError();
			}
			if( Index != WAIT_IO_COMPLETION )
				break;
		}
		WSAResetEvent( EventArray[Index - WSA_WAIT_EVENT_0] );

		if( ( SocketInfo = ( LPSOCKET_INFORMATION ) GlobalAlloc( GPTR, sizeof( SOCKET_INFORMATION ) ) ) == NULL ) {
			printf ( "WSAWaitForMultipleEvents failed.\n" );
			return WSAGetLastError();
		} 
		SocketInfo->Socket = AcceptSocket;
		ZeroMemory( &( SocketInfo->Overlapped ), sizeof( WSAOVERLAPPED ) );  
		SocketInfo->BytesSEND		= 0;
		SocketInfo->BytesRECV		= 0;
		SocketInfo->DataBuf.len		= BUFFSIZE;
		SocketInfo->DataBuf.buf		= SocketInfo->Buffer;

		Flags = 0;
		/*  WSARecv with a WorkerRoutine. */
		printf("New connection.\n");

		/* Attempt to read from the client. Assuming they send us something in the beginning. 
		 * Otherwise, WSASend should be used to send them the song list. 
		 */
		if( WSARecv( SocketInfo->Socket, &( SocketInfo->DataBuf ), 1, &RecvBytes, &Flags, &( SocketInfo->Overlapped ), WorkerRoutine ) == SOCKET_ERROR ) {
			if( WSAGetLastError() != WSA_IO_PENDING ) {
				printf ( "WSARecv() failed.\n" );
				return WSAGetLastError();
			}
			
		}
	}
}

/*
 * WorkerRoutine for TCP/UDP. Moreso designed for TCP Recv, may need a bit of tweaking but seems to work. 
 */
void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags) {
   DWORD SendBytes, RecvBytes, Flags = 0, BytesWritten = 0;
   LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;

   /* Check if there is an error, or BytesTranferred is ZERO.
	* If BytesTransferred is ZERO, the TCP connection is closed. 
	*/
	if (Error != 0 || BytesTransferred == 0) {
		printf("Connection closed.\n");
		closesocket(SI->Socket);
		GlobalFree(SI);
		return;
   }

	/* Otherwise we have recieved data. */
	printf("Recieved: %d bytes.\n", BytesTransferred );

	SI->BytesRECV = 0;
	ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
	SI->DataBuf.buf = SI->Buffer;
	SI->DataBuf.len = BUFFSIZE;

	/** This is for Recv. Just in case we need it.
	if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags, &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING ) {
			printf("WSARecv() failed with error %d.\n", WSAGetLastError());
			return;
		}
	}
	*/
	/* Copy some data into the buffer. */
	strcpy( SI->Buffer, "Some data.\n");

	/* Send the data. */
	if (WSASendTo(SI->Socket, &(SI->DataBuf), 1, &SendBytes, 0, 0, 0,  &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			printf( "UDP:WSASend failed." );
			return;
		}
	}
}