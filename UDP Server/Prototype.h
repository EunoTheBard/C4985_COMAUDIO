#ifndef PROTOTYPE_H
#define PROTOTYPE_H

DWORD WINAPI AcceptThread( LPVOID lp );
int ServerTCPSetup() ;
void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);

#endif