#include <windows.h>
#include <commctrl.h>
#include "Variables.h"

LPDWORD bmptoDIB(TCHAR *fileName, LPBITMAPINFO lpbiInfo)
{
  HANDLE fhBMP;
	int iFileSize;
	LPBYTE lpBMP;
	DWORD dwReadSize;

	LPBITMAPFILEHEADER lpbmpfh;
	LPBITMAPINFO lpbiBMPInfo;

	LPBYTE lpBMPPixel;
	LPDWORD dwColors;
	int iwidth,iheight;

	LPDWORD lpPixel;
	int bitCount;
	int iLength,x,y;
	TCHAR err[20];

	fhBMP = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(fhBMP == INVALID_HANDLE_VALUE)
	{
		CloseHandle(fhBMP);
		wsprintf(err, TEXT("cannot open a file %d"), GetLastError());
		MessageBox(NULL, err, fileName, MB_OK);
		return NULL;
	}

	iFileSize=GetFileSize(fhBMP,NULL);
	lpBMP=(LPBYTE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,iFileSize);
	ReadFile(fhBMP,lpBMP,iFileSize,&dwReadSize,NULL);
	CloseHandle(fhBMP);

	lpbmpfh=(LPBITMAPFILEHEADER)(lpBMP);
	lpbiBMPInfo=(LPBITMAPINFO)(lpBMP+sizeof(BITMAPFILEHEADER));
	bitCount = lpbiBMPInfo->bmiHeader.biBitCount;

	if(lpbmpfh->bfType!=('M'<<8)+'B' || bitCount != 8 && bitCount != 24)
	{
		HeapFree(GetProcessHeap(),0,lpBMP);
   		MessageBox(NULL,TEXT("Can read only 8 or 24 bit files"),fileName,MB_OK);
		return NULL;
	}

	lpBMPPixel=lpBMP+(lpbmpfh->bfOffBits);
	dwColors=(LPDWORD)(lpbiBMPInfo->bmiColors);
	iwidth=lpbiBMPInfo->bmiHeader.biWidth;
	iheight=lpbiBMPInfo->bmiHeader.biHeight;

	lpPixel=(LPDWORD)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,iwidth*iheight*4);

	lpbiInfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	lpbiInfo->bmiHeader.biWidth=iwidth;
	lpbiInfo->bmiHeader.biHeight=iheight;
	lpbiInfo->bmiHeader.biPlanes=1;
	lpbiInfo->bmiHeader.biBitCount=32;
	lpbiInfo->bmiHeader.biCompression=BI_RGB;

	if(iwidth*(bitCount/8)%4) iLength=iwidth*(bitCount/8)+(4-iwidth*(bitCount/8)%4);
	else iLength=iwidth*(bitCount/8);

	switch(bitCount)
	{
		case 8:
			for(y=0;y<iheight;y++)
				for(x=0;x<iwidth;x++)
					CopyMemory(lpPixel+x+y*iwidth,dwColors+lpBMPPixel[x+y*iLength],3);
		case 24:
			for(y=0;y<iheight;y++)
				for(x=0;x<iwidth;x++)
					 CopyMemory(lpPixel+x+y*iwidth,lpBMPPixel+x*3+y*iLength,3);
			break;
	}
	

	HeapFree(GetProcessHeap(),0,lpBMP);

	return lpPixel;
}
