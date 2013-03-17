#include "Common.h"
#include "Variables.h"
#include "resource.h"
#include "Func.h"
//#include "Shared.h"
//#include "Player.h"

//CPlayer     *g_pPlayer = NULL;                  // Global player object.
////
////  Description: Handles WM_PAINT messages.  This has special behavior in order to handle cases where the
//// video is paused and resized.
////
//void OnPaint(HWND hwnd)
//{
//    PAINTSTRUCT ps;
//    HDC hdc = BeginPaint(hwnd, &ps);
//
//    if (g_pPlayer && g_pPlayer->HasVideo())
//    {
//        // We have a player with an active topology and a video renderer that can paint the
//        // window surface - ask the videor renderer (through the player) to redraw the surface.
//        g_pPlayer->Repaint();
//    }
//    else
//    {
//        // The player topology hasn't been activated, which means there is no video renderer that 
//        // repaint the surface.  This means we must do it ourselves.
//        RECT rc;
//        GetClientRect(hwnd, &rc);
//        FillRect(hdc, &rc, (HBRUSH) COLOR_WINDOW);
//    }
//    EndPaint(hwnd, &ps);
//}
//
//
//void OnTimer(void)
//{
//    if (g_pPlayer->GetState() == PlayerState_Started)
//    {
//        g_pPlayer->DrawSeekbar();
//    }
//}
//
//void OnOpenFile(HWND parent, bool render)
//{
//    OPENFILENAME ofn;       // common dialog box structure
//    char  szFile[260];       // buffer for file name
//
//    // Initialize OPENFILENAME
//    ZeroMemory(&ofn, sizeof(ofn));
//    ofn.lStructSize = sizeof(ofn);
//    ofn.hwndOwner = parent;
//    ofn.lpstrFile = szFile;
//    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
//    // use the contents of szFile to initialize itself.
//    ofn.lpstrFile[0] = '\0';
//    ofn.nMaxFile = sizeof(szFile);
//    ofn.lpstrFilter = "All\0*.*\0MP3\0*.mp3\0WMV\0*.wmv\0ASF\0*.asf\0AVI\0*.avi\0";
//    ofn.nFilterIndex = 1;
//    ofn.lpstrFileTitle = NULL;
//    ofn.nMaxFileTitle = 0;
//    ofn.lpstrInitialDir = NULL;
//    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
//
//    // Display the Open dialog box. 
//    if (GetOpenFileName(&ofn)==TRUE) 
//    {
//        // call the player to actually open the file and build the topology
//        if(g_pPlayer != NULL)
//        {
//            if(render)
//            {
//                g_pPlayer->OpenURL(ofn.lpstrFile, parent);
//            }
//          
//        }
//    }
//}
//
//LRESULT CALLBACK StaticProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp){
//	 
//	RECT rc, rc2; 
//	int x, y, i;
//
//	static HWND hOpen2 = NULL;  
//	static HWND hPlay2 = NULL;
//	static HWND hPause2 = NULL;
//	static HWND hStop2 = NULL;
//	static HWND hExit2 = NULL;
//	
//	switch (msg) { 
//		case ID_STATIC:
//			g_pPlayer = new (std::nothrow) CPlayer(hwnd);
//
//			//button
//				hOpen2 = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Open") , WS_CHILD | WS_VISIBLE , 15, 620 , 70 , 30 , hwnd , (HMENU)ID_OPEN2, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
//				hPlay2 = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Play") , WS_CHILD | WS_VISIBLE , 90, 620 , 70 , 30 , hwnd , (HMENU)ID_PLAY2, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
//				hPause2 = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Pause") , WS_CHILD | WS_VISIBLE , 165 , 620 , 70 , 30 , hwnd , (HMENU)ID_PAUSE2, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
//				hStop2 = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Stop") , WS_CHILD | WS_VISIBLE , 240 , 620 , 70 , 30 , hwnd , (HMENU)ID_STOP2, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
//				hExit2 = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Exit") , WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 1160, 620 , 70 , 30 , hwnd , (HMENU)ID_EXITT2, ((LPCREATESTRUCT)(lp))->hInstance, NULL);		
//
//			break;
//		case WM_PAINT:
//	
//			nPage = TabCtrl_GetCurSel(hTab); 
//			GetClientRect(hwnd, &rc);
//			
//			switch (nPage){ 
//				case 0: 
//				OnPaint(hwnd);
//					break; 
//				case 1: 
//					
//					
//					break; 
//			} 
//			
//			break; 
//		case WM_TIMER:
//        OnTimer();
//        break;
//
//    case WM_ERASEBKGND:
//        // Suppress window erasing, to reduce flickering while the video is playing.
//        return 1;
//		case WM_MOUSEMOVE:
//        TRACKMOUSEEVENT eventTrack;
//        eventTrack.cbSize = sizeof( TRACKMOUSEEVENT );
//        eventTrack.dwFlags = TME_LEAVE;
//        eventTrack.hwndTrack = hwnd;
//        TrackMouseEvent(&eventTrack);
//
//        g_pPlayer->SetMouseOver(true);
//        break;
//
//    case WM_MOUSELEAVE:
//        g_pPlayer->SetMouseOver(false);
//        break;
//		case WM_SIZE: 
//			
//			break; 
//		case WM_COMMAND: 
//			switch (LOWORD(wp)) { 
//				ID_CANCEL:
//				return (CallWindowProc((WNDPROC)Org_StaticProc, hwnd, msg, wp, lp));
//
//
//
//
//
//
//
//
//				
//			} 
//			break; 
//		default: 
//			break; 
//	} 
//	SetWindowLong(hStatic, GWL_WNDPROC, (LONG)Org_StaticProc);
//	return 0;
//}
