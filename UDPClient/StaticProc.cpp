#include "Common.h"
#include "Variables.h"
#include "resource.h"
#include "Func.h"


LRESULT CALLBACK StaticProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp){
	 
	RECT rc, rc2; 
	int x, y, i;
	
	switch (msg) { 
		case ID_STATIC:
			// background bmp
			lpPixel = bmptoDIB("480127.bmp", &bmpInfo);
			iWidth = bmpInfo.bmiHeader.biWidth;
			iHeight = bmpInfo.bmiHeader.biHeight;

			// get Window's device context handle
			hdc = GetDC( hwnd );
			hBmp = CreateCompatibleBitmap(hdc, iWidth, iHeight);
			// make memory device context
			hdc_mem = CreateCompatibleDC(hdc);
			// select bitmap which is loaded 
			SelectObject( hdc_mem, hBmp );

			DeleteObject(hBmp);
		
			ReleaseDC(hwnd, hdc);

			// create pen
			hPenL = CreatePen(PS_SOLID, 1, RGB(255,255,0)); // 
			hPenR = CreatePen(PS_SOLID, 1, RGB(0,255,255)); // 

			// initialize a coordinate for wave
			for(i = 0; i< sizeof(ptWaveL)/sizeof(ptWaveL[0]); i++)
			{
				ptWaveL[i].x = 10+i;
				ptWaveL[i].y = -10;

				ptWaveR[i].x = 10+i;
				ptWaveR[i].y = -10;
			}

			// change background mode
			SetBkMode(hdc_mem, TRANSPARENT);

			hFont[0] = CreateFont(18, 0, 0, 0, FW_BOLD, TRUE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_DONTCARE, "Times New Roman");
			hFont[1] = CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_DONTCARE, "Arial Black");
			hFont[2] = CreateFont(12, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_DONTCARE, "Times New Roman");
			
			// initialize MMTIME
			mmTime.wType = TIME_BYTES;

			// store an initial value for volume and show the volume
			waveOutGetVolume(hWaveOut, &dwVolStart);
			wVolNow = LOWORD(dwVolStart);
			wsprintf(strVol, "%03d/128", (wVolNow +1)/512);

			// show state
			wsprintf(strState, "NoData");


			break;
		case WM_PAINT:
	
			nPage = TabCtrl_GetCurSel(hTab); 
			GetClientRect(hwnd, &rc);
			
			switch (nPage){ 
				case 0: 
					// SetBkMode(hdc, TRANSPARENT);
					hdc = BeginPaint(hwnd, &ps); 

					StretchDIBits(hdc_mem, 0, 0, iWidth, iHeight, 0, 0, iWidth, iHeight, lpPixel, &bmpInfo, DIB_RGB_COLORS, SRCCOPY);

					/////////////////////////////////////////
					SelectObject(hdc_mem, hFont[2]);
					SetTextColor(hdc_mem, RGB(255, 255, 0)); 
				
					DrawText(hdc_mem, strFile, (int)strlen(strFile), &rcMove, DT_SINGLELINE | DT_VCENTER);
					DrawText(hdc_mem,strTime, (int)strlen(strTime), &rcInfo[1], DT_SINGLELINE | DT_CENTER | DT_VCENTER);
					DrawText(hdc_mem, strVol, (int)strlen(strVol), &rcInfo[2], DT_SINGLELINE | DT_CENTER | DT_VCENTER);
					DrawText(hdc_mem, strState, (int)strlen(strState), &rcInfo[3], DT_SINGLELINE | DT_CENTER | DT_VCENTER);

					SelectObject(hdc_mem, hPenR);
					Polyline(hdc_mem, ptWaveR, sizeof(ptWaveR)/sizeof(ptWaveR[0]));

					SelectObject(hdc_mem, hPenL);
					Polyline(hdc_mem, ptWaveL, sizeof(ptWaveL)/sizeof(ptWaveL[0]));

					// measure when strFile is over limitation
					StretchDIBits(hdc_mem, 0, 450, 20, 20, 0, 10, 20, 20, lpPixel, &bmpInfo, DIB_RGB_COLORS, SRCCOPY);
					//////////////////////////////////////////////////////


					BitBlt(hdc, 0, 2, iWidth, iHeight, hdc_mem, 0, 0, SRCCOPY);
				
				    EndPaint(hwnd, &ps); 
				
					break; 
				case 1: 
					SetBkMode(hdc, TRANSPARENT); 
				
					SetTextColor(hdc, RGB(255, 0, 0)); 
				//	EndPaint(hwnd, &ps); 
					break; 
			} 
			
			break; 
		case WM_SIZE: 
			
			break; 
		case WM_COMMAND: 
			switch (LOWORD(wp)) { 









				
			} 
			break; 
		default: 
			break; 
	} 
	SetWindowLong(hStatic, GWL_WNDPROC, (LONG)Org_StaticProc);
	return (CallWindowProc((WNDPROC)Org_StaticProc, hwnd, msg, wp, lp));
}