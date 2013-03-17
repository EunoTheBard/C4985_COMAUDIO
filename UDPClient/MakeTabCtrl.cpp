#include "Common.h"
#include "Variables.h"
#include "resource.h"
#include "Func.h"

HWND MakeTabCtrl(HWND hwnd) {
	HINSTANCE hInst; 
	RECT rc; 
	int x, y; 
	HWND tabH;
	 	
	hInst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE); 
	GetClientRect(hwnd, &rc);

	tabH = CreateWindowEx( 0 , WC_TABCONTROL , NULL ,WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE , 0 , 0 , rc.right , rc.bottom , hwnd , (HMENU)ID_TAB ,
			hInst, NULL);//1245 570

	hStatic = CreateWindowEx( 0, "STATIC", NULL, WS_CHILD | WS_VISIBLE , 0, 20, rc.right, rc.bottom, hwnd, (HMENU)ID_STATICWND, hInst, NULL); //1245 550

	tc_item.pszText = TEXT(".wav.mp3.wmv.avi");
	SendMessage(tabH, TCM_INSERTITEM, (WPARAM)0, (LPARAM)&tc_item);
	
	tc_item.pszText = TEXT("playlist");
	SendMessage(tabH, TCM_INSERTITEM, (WPARAM)1, (LPARAM)&tc_item);
	

	Org_StaticProc = (FARPROC)GetWindowLong(hwnd, GWL_WNDPROC);
//	SetWindowLong(hStatic, GWL_WNDPROC, (LONG)StaticProc);		

	return tabH;
}

