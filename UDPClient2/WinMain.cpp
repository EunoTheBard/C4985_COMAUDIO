/*------------------------------------------------------------------------------------------------------------------
--  		SOURCE FILE:	WinMain.cpp		application entry point
--		
--			PROGRAM:		UDP Client
--
--							
--							
--
--			FUNCTIONS:		int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow)
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
--
--			NOTES:			This creates a specific size of window with a menu. 
--			Import Library: Shlwapi.lib, winmm.lib, ComCtl32.lib & WS2_32.lib
--
-- 
----------------------------------------------------------------------------------------------------------------------*/
#include <tchar.h>
#include <windows.h>
#include "Variables.h"
#include "WndProc.h"
#include "resource.h"


#define WINDOW_WIDTH (1260)
#define WINDOW_HEIGHT (740)
#define WINDOW_X ((GetSystemMetrics( SM_CXSCREEN ) - WINDOW_WIDTH ) / 2)
#define WINDOW_Y ((GetSystemMetrics( SM_CYSCREEN ) - WINDOW_HEIGHT ) / 2)

static TCHAR szClassName[] = TEXT("UDP Client");

/*------------------------------------------------------------------------------------------------------------------
--			FUNCTION:		WinMain
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
--			INTERFACE:		int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow)
--
--			RETURNS:		int 
--
--			NOTES:			Window is created with a specific width, height and position. 
							This is a main function having message loop.  
--
----------------------------------------------------------------------------------------------------------------------*/

#ifdef UNICODE
int WINAPI wWinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInst, LPWSTR lpCmdLine, int nShowCmd)
#else
int WINAPI WinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd)
#endif
{
	static HWND hWnd;
	MSG msg;
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszClassName = szClassName;
	
	if (!RegisterClass(&wc)) return -1;

	hWnd = CreateWindow(szClassName, szClassName, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	if (hWnd == NULL) return -1;

	ShowWindow(hWnd , SW_SHOW);
	UpdateWindow(hWnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
} 
