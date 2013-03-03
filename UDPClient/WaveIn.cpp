/*------------------------------------------------------------------------------------------------------------------
--			FUNCTION:		waveIn
--
--			DATE:			FEB 28, 2013
--
--			REVISIONS:		Mar, 2013
--
--			DESIGNER:		Nicholas Raposo	(Team Leader)
--			    &			Christopher Porter
--			PROGRAMMER:	    Cody Srossiter
--							Aiko Rose	
--
--			INTERFACE:		int waveIn(char *fileName, LPHWAVEOUT lphWave,LPWAVEFORMATEX lpwfe,LPWAVEHDR lpwhdr)
--
--			RETURNS:		
--
--			NOTES:			
							 
--
----------------------------------------------------------------------------------------------------------------------*/


#include <windows.h>

int waveIn(char *fileName, LPHWAVEOUT lphWave,LPWAVEFORMATEX lpwfe,LPWAVEHDR lpwhdr)
{
	HANDLE fhWave;
	int iFileSize;
	LPBYTE lpBuf;
	DWORD dwReadSize;

	char formType[5];

	LPBYTE lpWave;

	fhWave=CreateFile(fileName,GENERIC_READ,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(fhWave==INVALID_HANDLE_VALUE){
		CloseHandle(fhWave);
		MessageBox(NULL,"cannot open file",fileName,MB_OK);
		return 1;
	}
	iFileSize=GetFileSize(fhWave,NULL);
	lpBuf=(LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,iFileSize);
	ReadFile(fhWave,lpBuf,iFileSize,&dwReadSize,NULL);
	CloseHandle(fhWave);

	formType[0]=lpBuf[8];
	formType[1]=lpBuf[9];
	formType[2]=lpBuf[10];
	formType[3]=lpBuf[11];
	formType[4]='\0';
	if(strcmp(formType,"WAVE")){
		HeapFree(GetProcessHeap(),0,lpBuf);
		MessageBox(NULL,"Specify wav file",fileName,MB_OK);
		return 2;
	}

	CopyMemory(lpwfe,lpBuf+20,16);
	if(lpwfe->wFormatTag != WAVE_FORMAT_PCM){
		HeapFree(GetProcessHeap(),0,lpBuf);
		MessageBox(NULL,"Spefify PCM type of wav file ",fileName,MB_OK);
		return 3;
	}

	lpWave=(LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,iFileSize-44);
	CopyMemory(lpWave,lpBuf+44,iFileSize-44);

	HeapFree(GetProcessHeap(),0,lpBuf);

	lpwhdr->lpData=(LPSTR)lpWave;
	lpwhdr->dwBufferLength=iFileSize-44;
	lpwhdr->dwFlags=WHDR_BEGINLOOP | WHDR_ENDLOOP;
	lpwhdr->dwLoops=1;

	return 0;
}
