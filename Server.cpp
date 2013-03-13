#include "Server.h"

	void ServerSetup() {

	}

	void SendFile( char* fileName, LPSOCKET_INFORMATION SI) {
		DWORD SendBytes;

		HANDLE hFile = CreateFile( fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL );

		if( hFile == INVALID_HANDLE_VALUE ) {
			return;
		}


		DWORD dwHigh, dwLow, bytesRead;
		dwLow = GetFileSize( hFile, &dwHigh );


		while( dwLow > 0 )  {

			memset((char *)&SI->Buffer, 0, sizeof(SI->Buffer));

			/* Read DATA_BUFSIZE -1 into our Buffer. */
			ReadFile( hFile, SI->Buffer, BUFFSIZE - 1, &bytesRead, NULL );

			if (WSASend(SI->Socket, &(SI->DataBuf), 1, &SendBytes, 0, &(SI->Overlapped), NULL) == SOCKET_ERROR) {
				if (WSAGetLastError() != WSA_IO_PENDING) {
					CloseHandle( hFile );
					return;
				}
			}

			/* If we are at the last packet, break. */
			if( dwLow < ( BUFFSIZE - 1 ) ) {
				break;
			}

			dwLow -= ( BUFFSIZE - 1 );
		}
		CloseHandle( hFile );
	}