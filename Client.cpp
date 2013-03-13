#include "Client.h"

	void ClientSetup( char* host, struct sockaddr_in server, SOCKET	sd) {
		LPSOCKET_INFORMATION SI;
		struct	hostent	*hp;
		int n;
		HANDLE hFile;
		DWORD SendBytes, bytesWritten, RecvBytes, Flags = 0;

		struct linger so_linger;
		so_linger.l_onoff = TRUE;
		so_linger.l_linger = 3000;

		if( ( hp = gethostbyname(host) ) == NULL ) {
			fatal ( "Unknown server address." );
			return;
		}

		memcpy((char *)&server.sin_addr, hp->h_addr, hp->h_length);

		if (connect (sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
			fatal( "Cannot connect to server." );
			return;
		}

		if( setsockopt( sd, SOL_SOCKET, SO_LINGER,(const char *) &so_linger, sizeof( so_linger ) ) == -1 ) {
			fatal( "Failure on setsockopt." );
			return;
		}


		if( ( SI = ( LPSOCKET_INFORMATION ) GlobalAlloc( GPTR, sizeof( SOCKET_INFORMATION ) ) ) == NULL ) {
			fatal ( "WSAWaitForMultipleEvents failed." );
			return;
		} 

		SI->Socket			= sd;
		ZeroMemory( &( SI->Overlapped ), sizeof( WSAOVERLAPPED ) );  
		SI->BytesSEND		= 0;
		SI->BytesRECV		= 0;
		SI->DataBuf.len		= BUFFSIZE;
		SI->DataBuf.buf		= SI->Buffer;


		/*  WSARecv with a WorkerRoutine. */
		if( WSARecv( SI->Socket, &( SI->DataBuf ), 1, &RecvBytes, &Flags, &( SI->Overlapped ), TCPRoutine ) == SOCKET_ERROR ) {
			if( WSAGetLastError() != WSA_IO_PENDING ) {
				fatal ( "TCP:WSARecv failed." );
				return;
			}
		}
	}

