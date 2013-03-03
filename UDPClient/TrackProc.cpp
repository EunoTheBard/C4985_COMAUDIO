#include "Common.h"

LRESULT CALLBACK TrackProc(HWND hTrack, UINT msg, WPARAM wp, LPARAM lp) {
	static BOOL bRange;
	DWORD dwPos;
	char szRange[8];

	switch (msg) { 
		case WM_KEYDOWN: 
			if (wp == VK_CONTROL) { 
				bRange = TRUE; 
				dwPos = SendMessage(hTrack, TBM_GETPOS, 0, 0); 
				SendMessage(hTrack, TBM_SETSELSTART, (WPARAM)FALSE, (LPARAM)dwPos); 
				wsprintf(szRange, "%d", dwPos); 
			//	SetWindowText(hFrom, szRange); 
				//EnableWindow(hCancel, TRUE); 
				return 0; 
			} 
			break; 
		case WM_LBUTTONUP: 
			if (bRange) { 
				dwPos = SendMessage(hTrack, TBM_GETPOS, 0, 0); 
				SendMessage(hTrack, TBM_SETSELEND, (WPARAM)TRUE, (LPARAM)dwPos); 
				wsprintf(szRange, "%d", dwPos); 
			//	SetWindowText(hTo,szRange); 
			} 
			bRange = FALSE; 
			break; 
	} 

//	return (CallWindowProc(OrgTrackProc, hTrack, msg, wp, lp));
	return TRUE;
}