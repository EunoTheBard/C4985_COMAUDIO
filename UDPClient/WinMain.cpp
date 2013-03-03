/*------------------------------------------------------------------------------------------------------------------
--			SOURCE FILE:	WinMain.cpp		application entry point
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
-- 
--
-- 
----------------------------------------------------------------------------------------------------------------------*/

#include <windows.h>
#include "WndProc.h"
#include "resource.h"

#define WINDOW_WIDTH (1260)
#define WINDOW_HEIGHT (740)
#define WINDOW_X ((GetSystemMetrics( SM_CXSCREEN ) - WINDOW_WIDTH ) / 2)
#define WINDOW_Y ((GetSystemMetrics( SM_CYSCREEN ) - WINDOW_HEIGHT ) / 2)

TCHAR szClassName[] = TEXT("UDP Client");

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

int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hCurInst;
	wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszClassName = szClassName;
	
	if (!RegisterClass(&wc)) return -1;

	hWnd = CreateWindow(szClassName, szClassName, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hCurInst, NULL);

	if (hWnd == NULL) return -1;

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
} 