#include <commctrl.h>
#ifndef VARIABLES_H_
#define VARIABLES_H_

WNDPROC OrgTrackProc;
static TCHAR szClassName[] = TEXT("UDP Client");
static HWND hTab = NULL;
static HWND hStatic = NULL;
static HWND hPage0 = NULL;
static HWND hPage1 = NULL;
static TC_ITEM tc_item;
static FARPROC Org_StaticProc;
static int nPage; 
static BITMAPINFO bmpInfo;
static int iWidth, iHeight;
static HDC hdc_mem;
static LPDWORD lpPixel;
static HBITMAP hBmp;
static PAINTSTRUCT ps; 
static HDC hdc;
const RECT rcWave = {10, 20, 1235, 550};
static RECT rcInfo[] = {{1, 660, 900, 20}, {905, 660, 110, 20}, {2220, 1060, 110, 20}, {2335, 1060, 108, 20}};
static RECT rcMove = rcInfo[0];
static char szFile[MAX_PATH];
static POINT ptWaveL[621], ptWaveR[621]; //1224
static HPEN hPenL, hPenR;
static HFONT hFont[3];
static char strFile[1024];
static char strTime[32];
static char strVol[16];
static char strState[16];
static HWAVEOUT hWaveOut;
static DWORD dwVolStart;
static WORD wVolNow;
static MMTIME mmTime;

#endif
