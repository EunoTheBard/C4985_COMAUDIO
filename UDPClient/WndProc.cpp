/*------------------------------------------------------------------------------------------------------------------
--			FUNCTION:		WndProc
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
--			INTERFACE:		LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
--
--			RETURNS:		
--
--			NOTES:			
							 
--
----------------------------------------------------------------------------------------------------------------------*/

#include "Common.h"
#include "Func.h"
#include "Variables.h"
#include "resource.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp){

	//tab controls
	INITCOMMONCONTROLSEX ic;
	static RECT rcDisp;
	
	//trackbar
	static HWND hTrack = NULL;

	// buttons and edit(read-only) handles
	static HWND hOpen = NULL;  
	static HWND hPlay = NULL;
	static HWND hPause = NULL;
	static HWND hStop = NULL;
	static HWND hLoop = NULL;
	static HWND hDown = NULL;
	static HWND hUp = NULL;
	static HWND hExit = NULL;
	
	static HDC hdc;
	PAINTSTRUCT ps;
	DWORD dw;
	int i, ia;
	short s;

	static LPDWORD lpPixel;
	static BITMAPINFO bmpInfo;
	static int iWidth, iHeight;

	static LPDWORD lpPixelCursor;
	static BITMAPINFO bmpInfoCursor;
	static int iWidthCursor, iHeightCursor, iTimeCursor;

	static int iPixel = 0;

	HBITMAP hBmp;
	static HDC hdc_mem;

	// file
	static OPENFILENAME ofn;
	static char szFile[MAX_PATH];
	static char szFileTitle[MAX_PATH];

	//wav file
	static HWAVEOUT hWaveOut;
	static WAVEFORMATEX wfe;
	static WAVEHDR whdr;

	const RECT rcWave = {10, 10, 1225, 447};//10, 20, 1235, 550 //10, 10, 631, 381
	const RECT rcBar = {10, 390, 631, 401};
	static RECT rcInfo[] = {{1, 1060, 900, 20}, {2000, 1060, 110, 20}, {2220, 1060, 110, 20}, {2335, 1060, 108, 20}};
	static RECT rcMove = rcInfo[0];

	static MMTIME mmTime;
	static DWORD maxTime;
	DWORD dwOffset;

	// wave
	static POINT ptWaveL[1226], ptWaveR[1226];//621
	static WORD wHalfMaxAmp;
	static HPEN hPenL, hPenR;
	static int hueL, hueR, hueLR;

	// volume
	static DWORD dwVolStart;
	static WORD wVolNow;
	DWORD dwVolTemp;

	// font
	static HFONT hFont[3];
	
	static BYTE rr = 0, gg = 128, bb = 128; // open, pause, exit
	static BYTE r = 0, g = 128, b = 128; // play
	static BYTE rs = 0, gs = 128, bs = 128; // stop
	static BYTE vr1 = 0, vg1 = 128, vb1 = 128; // volume-down
	static BYTE vr2 = 0, vg2 = 128, vb2 = 128; // volume-up
	static BYTE rLoop = 0, gLoop = 128, bLoop = 128; // loop

	//text
	static char strFile[1024];
	static char strTime[32];
	static char strVol[16];
	static char strState[16];
	static SIZE sizeFile;

	static BOOL open = FALSE;
	static BOOL play = FALSE;
	static BOOL pause = FALSE;
	static BOOL loop = FALSE;

		switch( msg ){
			case WM_CREATE:
				
				DragAcceptFiles(hwnd, TRUE);
				
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

					// initialize OPENFILENAME
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFilter = "WAVE_FORMAT_PCM(*.wav)\0*.wav\0";
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFileTitle = szFileTitle;
				ofn.nMaxFileTitle = MAX_PATH;
				ofn.Flags = OFN_FILEMUSTEXIST;
				ofn.lpstrDefExt = "wav";
				ofn.lpstrTitle = "Select PCM type of Wav file";

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
			
				//button
				hOpen = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Open") , WS_CHILD | WS_VISIBLE , 15, 620 , 70 , 30 , hwnd , (HMENU)ID_OPEN, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hPlay = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Play") , WS_CHILD | WS_VISIBLE , 90, 620 , 70 , 30 , hwnd , (HMENU)ID_PLAY, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hPause = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Pause") , WS_CHILD | WS_VISIBLE , 165 , 620 , 70 , 30 , hwnd , (HMENU)ID_PAUSE, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hStop = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Stop") , WS_CHILD | WS_VISIBLE , 240 , 620 , 70 , 30 , hwnd , (HMENU)ID_STOP, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hLoop = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Loop") , WS_CHILD | WS_VISIBLE , 900 , 620 , 70 , 30 , hwnd , (HMENU)ID_LOOP, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hDown = CreateWindowEx(0, TEXT("BUTTON"), TEXT("-") , WS_CHILD | WS_VISIBLE , 1000 , 620 , 40 , 30 , hwnd , (HMENU)ID_DOWN, ((LPCREATESTRUCT)(lp))->hInstance, NULL);
				hUp = CreateWindowEx(0, TEXT("BUTTON"), TEXT("+") , WS_CHILD | WS_VISIBLE , 1050 , 620 , 40 , 30 , hwnd , (HMENU)ID_UP, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hExit = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Exit") , WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 1160, 620 , 70 , 30 , hwnd , (HMENU)ID_EXITT, ((LPCREATESTRUCT)(lp))->hInstance, NULL);		

				//track bar
				ic.dwSize = sizeof(INITCOMMONCONTROLSEX); 
				ic.dwICC = ICC_BAR_CLASSES;
				InitCommonControlsEx(&ic);

				hTrack = CreateWindowEx(0, TRACKBAR_CLASS, "", WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS, 10, 580, 1230, 25, hwnd, (HMENU)IDC_TRACKBAR, ((LPCREATESTRUCT)(lp))->hInstance, NULL);
			 //   TrackProc = (WNDPROC)GetWindowLong(hTrack, GWL_WNDPROC); 
				//SetWindowLong(hTrack, GWL_WNDPROC, (LONG)TrackProc); 
			//	SendMessage(hTrack, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELPARAM(0, 100));
				
				return 0;
			case WM_SIZE:
				return 0;
			/*case WM_NOTIFY:
				switch (((NMHDR *)lp)->code){
					case TCN_SELCHANGE:
						if (TabCtrl_GetCurSel(hTab) == 0){
							InvalidateRect(hTab, &rcDisp, FALSE); 
						    InvalidateRect(hStatic, NULL, FALSE);

						}else if(TabCtrl_GetCurSel(hTab) == 1){
							InvalidateRect(hStatic, NULL, FALSE);
						}else{

						}
						 
						break;
					case TCN_SELCHANGING:
						return FALSE;
				}
		
				return 0;
*/
			case WM_COMMAND:
				switch(LOWORD(wp)) {
					case ID_TAB:
						break;
					case ID_OPTIONS_DOWNLOADMUSIC:





						////////////////////////    CREATE UDP SOCKET     FROM MENU           !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


						break;
					case ID_OPTIONS_MICROPHONE:
						





						/////////////////////            CREATE MICROPHONE FROM MENU        !!!!!!!!!!!!!!!!!!!!



						break;
					case ID_DIRECTX_MULTIPLAYER:
						hTab = MakeTabCtrl(hwnd);
					//	PostMessage(hStatic, ID_STATIC, (WPARAM)0, (LPARAM)0);	
						break;
					case ID_DIRECTX_TABCLOSEFORMULTIPLAYER:
						break;
					case ID_OPEN: // when "Open" is pushed
						if(GetOpenFileName(&ofn))
							{
								if(open)
								{		
									play=FALSE;
									pause=FALSE;

									KillTimer(hwnd, 1);
									rcMove.left = rcInfo[0].left;
									iTimeCursor = 0;
									 
									waveOutReset(hWaveOut);
									waveOutUnprepareHeader(hWaveOut, &whdr, sizeof(WAVEHDR));
									waveOutClose(hWaveOut);
									HeapFree(GetProcessHeap(), 0, whdr.lpData);
								}

								if(waveIn(ofn.lpstrFile, &hWaveOut, &wfe, &whdr))
								{
									for(i=0;i<sizeof(ptWaveL)/sizeof(ptWaveL[0]);i++)
									{
										ptWaveL[i].y = -10;		//default value
										ptWaveR[i].y = -10;		//default value
									}
									strFile[0] = NULL;
									strTime[0] = NULL;
									wsprintf(strState, "NoData");
								    InvalidateRect(hwnd, &rcWave, FALSE);
									InvalidateRect(hwnd, &rcInfo[0], FALSE);
									InvalidateRect(hwnd, &rcInfo[1], FALSE);
									InvalidateRect(hwnd, &rcInfo[3], FALSE);
									
									return 0;
								}
								waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfe, (DWORD)hwnd, 0, CALLBACK_WINDOW);
								waveOutPrepareHeader(hWaveOut, &whdr, sizeof(WAVEHDR));

								wHalfMaxAmp = pow(2, wfe.wBitsPerSample-1);	//half of the max amplitud
								
								for(i=0;i<sizeof(ptWaveL)/sizeof(ptWaveL[0]);i++)
									ptWaveL[i].y=250;		//screen center for wave

								if(wfe.nChannels == 2)
								{
                                    for(i = 0 ; i < sizeof(ptWaveR)/sizeof(ptWaveR[0]) ; i++)
										ptWaveR[i].y = 250;	//screen center for wave
								}
								else
								{
                                    for(i = 0 ; i < sizeof(ptWaveR)/sizeof(ptWaveR[0]) ; i++)
										ptWaveR[i].y = -10;	//default value
								}

								wsprintf(strFile,"%s :- %d bits / %s / %d Hz",
									ofn.lpstrFileTitle,wfe.wBitsPerSample,
									(wfe.nChannels==2)?"stereo":"mono",wfe.nSamplesPerSec);

								SelectObject(hdc_mem,hFont[2]);
								GetTextExtentPoint32(hdc_mem,strFile,(int)strlen(strFile),&sizeFile);

								maxTime=100*whdr.dwBufferLength/wfe.nAvgBytesPerSec;

								wsprintf(strTime,"00:00:00/%02d:%02d:%02d",
									(maxTime/100)/60,(maxTime/100)%60,maxTime%100);		//total time

								open=TRUE;
								wsprintf(strState,"ready");
								
								InvalidateRect(hwnd,&rcWave,FALSE);
								InvalidateRect(hwnd,&rcInfo[0],FALSE);
								InvalidateRect(hwnd,&rcInfo[1],FALSE);
								InvalidateRect(hwnd,&rcInfo[3],FALSE);
							}
							return 0;

					case ID_PLAY: // when "Play" is pressed
						if(!open || play) return 0;

							//file is open but not playing
							play=TRUE;
							wsprintf(strState,"playing");
							InvalidateRect(hwnd,&rcInfo[3],FALSE);
					
							if(pause)
							{	
								pause=FALSE;
								waveOutRestart(hWaveOut);
							}
							else
							{		
								iTimeCursor=0;
                            	waveOutWrite(hWaveOut,&whdr,sizeof(WAVEHDR));
							}
							SetTimer(hwnd,1,50,NULL);
							break;
					case ID_PAUSE: // when "Pause" button is pushed
						if(!play && !pause) return 0;

							//playing or pause
							if(pause)
							{
								pause=FALSE;
								play=TRUE;
								wsprintf(strState,"playing");
								waveOutRestart(hWaveOut);
								SetTimer(hwnd,1,50,NULL);
							}
							else
							{
								pause=TRUE;
								play=FALSE;
								wsprintf(strState,"pause");
								waveOutPause(hWaveOut);
								KillTimer(hwnd,1);
							}
							InvalidateRect(hwnd,&rcInfo[3],FALSE);
						break;
					case ID_STOP: // when "Stop" button is pressed
						if(!open) return 0;

							if(play) 
							{	//pushed during playing		
								play=FALSE;
								pause=FALSE;
								iTimeCursor=0;
								waveOutReset(hWaveOut);		//MM_WOM_DONE message is sent
							}
							else
							{
								for(i=0;i<sizeof(ptWaveL)/sizeof(ptWaveL[0]);i++)
									ptWaveL[i].y=250;		//center for wave drawn

								if(wfe.nChannels==2)
									for(i=0;i<sizeof(ptWaveR)/sizeof(ptWaveR[0]);i++)
										ptWaveR[i].y=250;	//center for wave drawn

								if(pause)
								{		//pushed during pause
									play=FALSE;
									pause=FALSE;
									waveOutReset(hWaveOut);

									rcMove.left=rcInfo[0].left;
									wsprintf(strState,"ready");

									InvalidateRect(hwnd,&rcWave,FALSE);
									InvalidateRect(hwnd,&rcInfo[0],FALSE);
									InvalidateRect(hwnd,&rcInfo[3],FALSE);
								}
								//pushed again after pause
								iTimeCursor=0;
								wsprintf(strTime,"00:00:00/%02d:%02d:%02d", (maxTime/100)/60,(maxTime/100)%60,maxTime%100);			//total time
								InvalidateRect(GetTopWindow(hwnd),&rcInfo[1],FALSE);
							}
						break;
					case ID_LOOP:
						if(loop)
						{
							loop=FALSE;
						}
						else
						{
							loop=TRUE;
						}
						break;
					case ID_DOWN://-
						waveOutGetVolume(hWaveOut,&dwVolTemp);
						ia=LOWORD(dwVolTemp);		// store in wVolNow after mute cancellation
						ia-=512;
						wVolNow=(ia<0)?0:ia;
						waveOutSetVolume(hWaveOut,MAKELONG(wVolNow,wVolNow));

						wsprintf(strVol,"%03d/128",(wVolNow+1)/512);
						InvalidateRect(hwnd,&rcInfo[2],FALSE);
						break;
					case ID_UP:
						waveOutGetVolume(hWaveOut,&dwVolTemp);
						ia=LOWORD(dwVolTemp);		// store in wVolNow after mute cancellation
						ia+=512;
						wVolNow=(ia>0xFFFF)?0xFFFF:ia;
						waveOutSetVolume(hWaveOut,MAKELONG(wVolNow,wVolNow));

						wsprintf(strVol,"%03d/128",(wVolNow+1)/512);
						InvalidateRect(hwnd,&rcInfo[2],FALSE);
						break;
					case ID_EXITT:
					case ID_EXIT_EXIT:
						 KillTimer(hwnd,1);
						// SetWindowLong(hTrack, GWL_WNDPROC, (LONG)TrackProc); 
						// DestroyWindow(hTrack);
						ExitProcess(-1);
						DestroyWindow(hwnd);
						break;
				}
				return 0;
			/*case WM_HSCROLL:
				if( (HWND)lp == GetDlgItem( hwnd, IDC_SLIDER ) ){
					pos = SendMessage( (HWND)lp, TBM_GETPOS, 0, 0 );
				}
				return 0;*/
			case WM_DROPFILES:		//drag and drop for file
				DragQueryFile((HDROP)wp, 0, strFile, sizeof(strFile));
				DragFinish((HDROP)wp);

				//when open button is pushed
				if(open)
				{		
					play = FALSE;
					pause = FALSE;

					KillTimer(hwnd, 1);
					rcMove.left = rcInfo[0].left;
					iTimeCursor = 0;
			
					waveOutReset(hWaveOut);
					waveOutUnprepareHeader(hWaveOut, &whdr, sizeof(WAVEHDR));
					waveOutClose(hWaveOut);
					HeapFree(GetProcessHeap(), 0, whdr.lpData);
				}

				if(waveIn(strFile, &hWaveOut, &wfe, &whdr))
				{
					//finished abnormaly
					for(i = 0 ; i < sizeof(ptWaveL)/sizeof(ptWaveL[0]) ; i++)
					{
						ptWaveL[i].y = -10;		//default value
						ptWaveR[i].y = -10;		//default value
					}
					strFile[0] = NULL;
					strTime[0] = NULL;
					wsprintf(strState, "NoData");

					InvalidateRect(hwnd, &rcWave, FALSE);
					InvalidateRect(hwnd, &rcInfo[0], FALSE);
					InvalidateRect(hwnd, &rcInfo[1], FALSE);
					InvalidateRect(hwnd, &rcInfo[3], FALSE);
					return 0;
				}
				waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfe, (DWORD)hwnd, 0, CALLBACK_WINDOW);
				waveOutPrepareHeader(hWaveOut, &whdr, sizeof(WAVEHDR));

				wHalfMaxAmp=pow(2, wfe.wBitsPerSample - 1);	//half of the max amplitud
			
				for(i=0;i<sizeof(ptWaveL)/sizeof(ptWaveL[0]);i++)
					ptWaveL[i].y=250;		//screen center for wave

				if(wfe.nChannels == 2)
				{
					for(i = 0 ; i < sizeof(ptWaveR)/sizeof(ptWaveR[0]) ; i++)
						ptWaveR[i].y = 250;	//screen center for wave 
				}
				else
				{
					for(i = 0 ; i < sizeof(ptWaveR)/sizeof(ptWaveR[0]) ; i++)
						ptWaveR[i].y = -10;	//default value
				}

				wsprintf(strFile, "%s :- %d bits / %s / %d Hz", PathFindFileName(strFile), wfe.wBitsPerSample, (wfe.nChannels==2)?"stereo":"mono", wfe.nSamplesPerSec);

				SelectObject(hdc_mem, hFont[2]);
				GetTextExtentPoint32(hdc_mem, strFile, (int)strlen(strFile), &sizeFile);

				maxTime = 100*whdr.dwBufferLength/wfe.nAvgBytesPerSec;

				wsprintf(strTime,"00:00:00/%02d:%02d:%02d",
					(maxTime/100)/60,(maxTime/100)%60,maxTime%100);		//total time

				open=TRUE;
				wsprintf(strState, "ready");

				InvalidateRect(hwnd, &rcWave, FALSE);
				InvalidateRect(hwnd, &rcInfo[0], FALSE);
				InvalidateRect(hwnd, &rcInfo[1], FALSE);
				InvalidateRect(hwnd, &rcInfo[3], FALSE);
				return 0;
				
				case MM_WOM_DONE:		
					pause = FALSE;
					waveOutReset(hWaveOut);

					if(loop && play)
					{		
						iTimeCursor = 0;
						waveOutWrite(hWaveOut, &whdr, sizeof(WAVEHDR));
						return 0;
					}

					if(play)
					{		//play completed
						iTimeCursor = 600;
						wsprintf(strTime,"%02d:%02d:%02d/%02d:%02d:%02d",
							(maxTime/100)/60,(maxTime/100)%60,maxTime%100,			//current time
							(maxTime/100)/60,(maxTime/100)%60,maxTime%100);			//total time
					}

					for(i = 0 ; i < sizeof(ptWaveL)/sizeof(ptWaveL[0]) ; i++)
						ptWaveL[i].y=250;		//center for wave drawn 195

					if(wfe.nChannels == 2)
						for(i=0 ; i<sizeof(ptWaveR)/sizeof(ptWaveR[0]) ; i++)
							ptWaveR[i].y=250;	//center for wave drawn 195

					play = FALSE;
					rcMove.left = rcInfo[0].left;
					wsprintf(strState, "ready");
					KillTimer(hwnd, 1);

					InvalidateRect(hwnd, &rcWave, FALSE);
					InvalidateRect(hwnd, &rcInfo[0], FALSE);
					InvalidateRect(hwnd, &rcInfo[1], FALSE);
					InvalidateRect(hwnd, &rcInfo[3], FALSE);
					return 0;
				case WM_TIMER:
					rcMove.left-= 1;
					if(rcMove.left<-sizeFile.cx) rcMove.left = rcInfo[0].right;

					waveOutGetPosition(hWaveOut, &mmTime, sizeof(MMTIME));
					dwOffset = mmTime.u.cb;

					iTimeCursor = (double)dwOffset/whdr.dwBufferLength*600;		//change Cursor coordinate

					for(i = -612 ; i <= 612 ; i++) //-310 310
					{			//left
						s = 0;
						dw = dwOffset + i*wfe.nBlockAlign;
						if(0 <= dw && dw < whdr.dwBufferLength)
						{
							CopyMemory(&s, whdr.lpData+dw, wfe.wBitsPerSample/8);
							if(wfe.wBitsPerSample == 8) s-= 128;			// shift no volume(128) to 0
						}
						ptWaveL[i+612].y = 185*(-s)/wHalfMaxAmp + 250; //ptWaveL[i+310].y     + 195;
					}
					if(wfe.nChannels == 2)
					{			//stereo
						for(i = -612 ; i <= 612 ; i++)//-310 310
						{		//right
							s = 0;
							dw = dwOffset + i*wfe.nBlockAlign + wfe.wBitsPerSample/8;
							if(0 <= dw && dw < whdr.dwBufferLength)
							{
								CopyMemory(&s, whdr.lpData + dw, wfe.wBitsPerSample/8);
								if(wfe.wBitsPerSample == 8) s-=128;		//shift no volume(128) to 0
							}
							ptWaveR[i+612].y = 185*(-s)/wHalfMaxAmp + 250; //ptWaveR[i+310].y      +195
						}
					}

					dw = (double)dwOffset/wfe.nAvgBytesPerSec*100;

					wsprintf(strTime, "%02d:%02d:%02d/%02d:%02d:%02d",
						(dw/100)/60,(dw/100)%60,dw%100,						//current time
						(maxTime/100)/60,(maxTime/100)%60,maxTime%100);		//total time
					InvalidateRect(hwnd, &rcWave, FALSE);
					InvalidateRect(hwnd, &rcInfo[0], FALSE);
					InvalidateRect(hwnd, &rcInfo[1], FALSE);
					return 0;
				case WM_PAINT:
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

					BitBlt(hdc, 0, 0, iWidth, iHeight, hdc_mem, 0, 0, SRCCOPY);
				
				    EndPaint(hwnd, &ps);

					return 0;
			case WM_DESTROY:
				SetWindowLong(hTrack, GWL_WNDPROC, (LONG)TrackProc); 
				DestroyWindow(hTrack);
				PostQuitMessage(0);
				return 0;
			default:
				return DefWindowProc(hwnd, msg, wp, lp);
		}

		return 0;
}
