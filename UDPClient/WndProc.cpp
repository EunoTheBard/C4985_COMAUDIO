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

	//tab
	INITCOMMONCONTROLSEX ic;
	static HWND hTab = NULL;
	static TCITEM tc_item;
	static HDC hdc;
	static HDC hdc_mem;
	PAINTSTRUCT ps;

	//trackbar
	static HWND hTrack = NULL;

	// buttons and edit(read-only)
	static HWND hOpen = NULL;  
	static HWND hPlay = NULL;
	static HWND hPause = NULL;
	static HWND hStop = NULL;
	static HWND hLoop = NULL;
	static HWND hDown = NULL;
	static HWND hUp = NULL;
	static HWND hMicroPhone = NULL;
	static HWND hExit = NULL;
	static HWND hInfo1 = NULL;
	static HWND hInfo2 = NULL;
	static HWND hInfo3 = NULL;
	static HWND hInfo4 = NULL;
	
	// file
	static OPENFILENAME ofn;
	static char szFile[MAX_PATH];
	static char szFileTitle[MAX_PATH];

	//wav file
	static HWAVEOUT hWaveOut;
	static WAVEFORMATEX wfe;
	static WAVEHDR whdr;

	//inside tab0
	const RECT rcWave = {10, 10, 631, 381};
	const RECT rcBar = {10, 390, 631, 401};
	static RECT rcInfo[] = {{1, 660, 900, 20}, {905, 660, 110, 20}, {1020, 660, 110, 20}, {1135, 660, 108, 20}};
	static RECT rcMove = rcInfo[0];

	static MMTIME mmTime;
	static DWORD maxTime;
	DWORD dwOffset;

	// wave
	static POINT ptWaveL[621], ptWaveR[621];
	static WORD wHalfMaxAmp;
	static HPEN hPenL, hPenR;
	static int hueL, hueR, hueLR;

	// volume
	static DWORD dwVolStart;
	static WORD wVolNow;
	DWORD dwVolTemp;

	// font
	static HFONT hFont[3];

	//color RGB values
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

	int i, pos, ia;
	DWORD dw;
	short s;

	static LPDWORD lpPixelCursor;
	static BITMAPINFO bmpInfoCursor;
	static int iWidthCursor, iHeightCursor, iTimeCursor;
	
		switch( msg ){
			case WM_CREATE:
				ic.dwSize = sizeof(INITCOMMONCONTROLSEX); 
				ic.dwICC = ICC_BAR_CLASSES;
				InitCommonControlsEx(&ic);
				
				// tab
				hTab = CreateWindowEx( 0 , WC_TABCONTROL , NULL ,WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE ,0 , 0 , 1245 , 570 , hwnd , (HMENU)ID_TAB ,
			((LPCREATESTRUCT)(lp))->hInstance, NULL);

				tc_item.mask = TCIF_TEXT;
				tc_item.pszText = TEXT(".wav");
				TabCtrl_InsertItem(hTab , 0 , &tc_item);

				tc_item.pszText = TEXT(".wav.mp3.wmv.avi");
				TabCtrl_InsertItem(hTab , 1 , &tc_item);
				tc_item.pszText = TEXT("playlist");
				TabCtrl_InsertItem(hTab , 2 , &tc_item);

				//button
				hOpen = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Open") , WS_CHILD | WS_VISIBLE , 15, 620 , 70 , 30 , hwnd , (HMENU)ID_OPEN, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hPlay = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Play") , WS_CHILD | WS_VISIBLE , 90, 620 , 70 , 30 , hwnd , (HMENU)ID_PLAY, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hPause = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Pause") , WS_CHILD | WS_VISIBLE , 165 , 620 , 70 , 30 , hwnd , (HMENU)ID_PAUSE, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hStop = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Stop") , WS_CHILD | WS_VISIBLE , 240 , 620 , 70 , 30 , hwnd , (HMENU)ID_STOP, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hLoop = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Loop") , WS_CHILD | WS_VISIBLE , 800 , 620 , 70 , 30 , hwnd , (HMENU)ID_LOOP, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hDown = CreateWindowEx(0, TEXT("BUTTON"), TEXT("-") , WS_CHILD | WS_VISIBLE , 900 , 620 , 40 , 30 , hwnd , (HMENU)ID_DOWN, ((LPCREATESTRUCT)(lp))->hInstance, NULL);
				hUp = CreateWindowEx(0, TEXT("BUTTON"), TEXT("+") , WS_CHILD | WS_VISIBLE , 950 , 620 , 40 , 30 , hwnd , (HMENU)ID_UP, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hMicroPhone = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Microphone") , WS_CHILD | WS_VISIBLE , 1050 , 620 , 90 , 30 , hwnd , (HMENU)ID_MICROPHONE, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hExit = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Exit") , WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 1160, 620 , 70 , 30 , hwnd , (HMENU)ID_EXITT, (HINSTANCE)GetWindowLong(hwnd , GWL_HINSTANCE) , NULL);
				/*hInfo1 = CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_BORDER | ES_LEFT | WS_VISIBLE, 1, 660, 900, 20, hwnd, (HMENU)ID_INFO1, ((LPCREATESTRUCT)(lp))->hInstance, NULL);
				hInfo2 = CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_BORDER | ES_LEFT | WS_VISIBLE, 905, 660, 110, 20, hwnd, (HMENU)ID_INFO2, ((LPCREATESTRUCT)(lp))->hInstance, NULL);
				hInfo3 = CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_BORDER | ES_LEFT | WS_VISIBLE, 1020, 660, 110, 20, hwnd, (HMENU)ID_INFO3, ((LPCREATESTRUCT)(lp))->hInstance, NULL);
				hInfo4 = CreateWindowEx(0, TEXT("EDIT"),TEXT(""), WS_CHILD | WS_BORDER | ES_LEFT | WS_VISIBLE, 1135, 660, 108, 20, hwnd, (HMENU)ID_INFO4, ((LPCREATESTRUCT)(lp))->hInstance, NULL);
				*/
				//track bar
				hTrack = CreateWindowEx(0, TRACKBAR_CLASS, "", WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS, 10, 580, 1230, 25, hwnd, (HMENU)IDC_TRACKBAR, ((LPCREATESTRUCT)(lp))->hInstance, NULL);
				// OrgTrackProc = (WNDPROC)GetWindowLong(hTrack, GWL_WNDPROC); 
				 //SetWindowLong(hTrack, GWL_WNDPROC, (LONG)TrackProc); 
				// SendMessage(hTrack, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELPARAM(0, 100));


				DragAcceptFiles(hwnd, TRUE);

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

				hFont[0] = CreateFont(18, 0, 0, 0, FW_BOLD, TRUE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_DONTCARE, "Times New Roman");
				hFont[1] = CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_DONTCARE, "Arial Black");
				hFont[2] = CreateFont(12, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_DONTCARE, "Times New Roman");

				// get Window's device context handle
				hdc = GetDC( hwnd );
				ReleaseDC(hwnd, hdc);

				// initialize MMTIME
				mmTime.wType = TIME_BYTES;

				// store an initial value for volume and show the volume
				waveOutGetVolume(hWaveOut, &dwVolStart);
				wVolNow = LOWORD(dwVolStart);
				wsprintf(strVol, "%03d/128", (wVolNow +1)/512);

				// show state
				wsprintf(strState, "NoData");

				return 0;
			//case WM_SIZE:
			//	MoveWindow(hTab , 0 , 0 , LOWORD(lp) , 30 , TRUE);
			//	return 0;
			case WM_COMMAND:
				switch(LOWORD(wp)) {
					case ID_TAB:
						break;
					case ID_OPTIONS_DOWNLOADMUSIC:


						// create UDP socket


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
									InvalidateRect(hwnd, &rcBar, FALSE);
									 
									waveOutReset(hWaveOut);
									waveOutUnprepareHeader(hWaveOut, &whdr, sizeof(WAVEHDR));
									waveOutClose(hWaveOut);
									HeapFree(GetProcessHeap(), 0, whdr.lpData);
								}

								if(waveIn(ofn.lpstrFile, &hWaveOut, &wfe, &whdr))
								{
									
									for(i=0;i<sizeof(ptWaveL)/sizeof(ptWaveL[0]);i++)
									{
										ptWaveL[i].y = -10;		//èdefault value
										ptWaveR[i].y = -10;		//èdefault value
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

								wHalfMaxAmp = pow(2, wfe.wBitsPerSample-1);	//çhalf of the max amplitud
								
								for(i=0;i<sizeof(ptWaveL)/sizeof(ptWaveL[0]);i++)
									ptWaveL[i].y=195;		//screen center for wave

								if(wfe.nChannels == 2)
								{
                                    for(i = 0 ; i < sizeof(ptWaveR)/sizeof(ptWaveR[0]) ; i++)
										ptWaveR[i].y = 195;	//screen center for wave
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
						//	changeColortoYellow(&r, &g, &b);
							if(pause)
							{	
								//changeColortoCyan(&rr, &gg, &bb); //change color for pause button to cyan
								//InvalidateRect(hwnd,&rcButton[2],FALSE);
								pause=FALSE;
								waveOutRestart(hWaveOut);
							}
							else
							{		
							//	changeColortoCyan(&rs, &gs, &bs); // change color for stop button to cyan
								//InvalidateRect(hwnd,&rcButton[3],FALSE);
								iTimeCursor=0;
								InvalidateRect(hwnd,&rcBar,FALSE);
                            	waveOutWrite(hWaveOut,&whdr,sizeof(WAVEHDR));
							}
							SetTimer(hwnd,1,50,NULL);
							break;


						
					case ID_PAUSE: // when "Pause" button is pushed

						if(!play && !pause) return 0;

							//çplaying or pause
							if(pause)
							{
								//changeColortoYellow(&r, &g, &b); // change color for play button to yellow
								//InvalidateRect(hWnd,&rcButton[1],FALSE);
								//changeColortoCyan(&rr, &gg, &bb);
								pause=FALSE;
								play=TRUE;
								wsprintf(strState,"playing");
								waveOutRestart(hWaveOut);
								SetTimer(hwnd,1,50,NULL);
							}
							else
							{
								//changeColortoCyan(&r, &g, &b); //change color for play button to cyan
								//InvalidateRect(hwnd,&rcButton[1],FALSE);
								//changeColortoYellow(&rr, &gg, &bb);
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
							{	//çpushed during playing		
								//changeColortoYellow(&rs, &gs, &bs);
								play=FALSE;
								pause=FALSE;
								iTimeCursor=0;
								InvalidateRect(hwnd,&rcBar,FALSE);
								waveOutReset(hWaveOut);		//MM_WOM_DONE message is sent
							//	changeColortoCyan(&r, &g, &b); //change color for play button to cyan
							//	InvalidateRect(hWnd,&rcButton[1],FALSE);
							}
							else
							{
								for(i=0;i<sizeof(ptWaveL)/sizeof(ptWaveL[0]);i++)
									ptWaveL[i].y=195;		//center for wave drawn

								if(wfe.nChannels==2)
									for(i=0;i<sizeof(ptWaveR)/sizeof(ptWaveR[0]);i++)
										ptWaveR[i].y=195;	//center for wave drawn

								if(pause)
								{		//pushed during pause
									//changeColortoCyan(&rr, &gg, &bb); //change color for pause button to cyan
									//InvalidateRect(hWnd,&rcButton[2],FALSE);
									//changeColortoYellow(&rs, &gs, &bs);
								//	InvalidateRect(hWnd,&rcButton[3],FALSE);
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
								InvalidateRect(hwnd,&rcBar,FALSE);
								InvalidateRect(hwnd,&rcInfo[1],FALSE);
							}

						break;
					case ID_LOOP:
						if(loop)
						{
							loop=FALSE;
							//changeColortoCyan(&rLoop, &gLoop, &bLoop);
						}
						else
						{
							loop=TRUE;
							//changeColortoYellow(&rLoop, &gLoop, &bLoop);
						}
						//InvalidateRect(hwnd,&rcButton[4],FALSE);
						break;
					case ID_DOWN://-
						//changeColortoYellow(&vr1, &vg1, &vb1); // change color for volume-down button to yellow
						//changeColortoCyan(&vr2, &vg2, &vb2); // change color for volume-up button to cyan
						//InvalidateRect(hwnd,&rcButton[6],FALSE);
						waveOutGetVolume(hWaveOut,&dwVolTemp);
						ia=LOWORD(dwVolTemp);		// store in wVolNow after mute cancellation
						ia-=512;
						wVolNow=(ia<0)?0:ia;
						waveOutSetVolume(hWaveOut,MAKELONG(wVolNow,wVolNow));

						//mute=FALSE;
						wsprintf(strVol,"%03d/128",(wVolNow+1)/512);
						InvalidateRect(hwnd,&rcInfo[2],FALSE);
						break;
					case ID_UP:
						//changeColortoYellow(&vr2, &vg2, &vb2); // change color for volume-up button to yellow
						//changeColortoCyan(&vr1, &vg1, &vb1); // change color for volume-down button to cyan
						//InvalidateRect(hwnd,&rcButton[5],FALSE);
						waveOutGetVolume(hWaveOut,&dwVolTemp);
						ia=LOWORD(dwVolTemp);		// store in wVolNow after mute cancellation
						ia+=512;
						wVolNow=(ia>0xFFFF)?0xFFFF:ia;
						waveOutSetVolume(hWaveOut,MAKELONG(wVolNow,wVolNow));

						wsprintf(strVol,"%03d/128",(wVolNow+1)/512);
						InvalidateRect(hwnd,&rcInfo[2],FALSE);
						break;
					case ID_MICROPHONE:
						break;
					case ID_EXITT:
					case ID_EXIT_EXIT:
						 KillTimer(hwnd,1);
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
					InvalidateRect(hwnd, &rcBar, FALSE);

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
						ptWaveL[i].y = -10;		//èdefault value
						ptWaveR[i].y = -10;		//èdefault value
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

				wHalfMaxAmp=pow(2, wfe.wBitsPerSample - 1);	//çhalf of the max amplitud
			
				for(i=0;i<sizeof(ptWaveL)/sizeof(ptWaveL[0]);i++)
					ptWaveL[i].y=195;		//screen center for wave

				if(wfe.nChannels == 2)
				{
					for(i = 0 ; i < sizeof(ptWaveR)/sizeof(ptWaveR[0]) ; i++)
						ptWaveR[i].y = 195;	//screen center for wave 
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
				/*case WM_NOTIFY:
					switch (((NMHDR *)lp)->code){
						case TCN_SELCHANGE:
							if (TabCtrl_GetCurSel(hTab) == 0){

							}else if(TabCtrl_GetCurSel(hTab) == 1){

							}else{

							}
							break;
					}
					InvalidateRect(hwnd , NULL , TRUE);
					return 0;*/
				case MM_WOM_DONE:		
					pause = FALSE;
					waveOutReset(hWaveOut);

					if(loop && play)
					{		
						iTimeCursor = 0;
						InvalidateRect(hwnd, &rcBar, FALSE);
						waveOutWrite(hWaveOut, &whdr, sizeof(WAVEHDR));
						return 0;
					}

					if(play)
					{		//çplay completed
						iTimeCursor = 600;
						wsprintf(strTime,"%02d:%02d:%02d/%02d:%02d:%02d",
							(maxTime/100)/60,(maxTime/100)%60,maxTime%100,			//current time
							(maxTime/100)/60,(maxTime/100)%60,maxTime%100);			//total time
						/*changeColortoCyan(&r, &g, &b);
						InvalidateRect(hwnd, &rcButton[1], FALSE);*/
					}

					for(i = 0 ; i < sizeof(ptWaveL)/sizeof(ptWaveL[0]) ; i++)
						ptWaveL[i].y=195;		//center for wave drawn

					if(wfe.nChannels == 2)
						for(i=0 ; i<sizeof(ptWaveR)/sizeof(ptWaveR[0]) ; i++)
							ptWaveR[i].y=195;	//center for wave drawn

					play = FALSE;
					rcMove.left = rcInfo[0].left;
					wsprintf(strState, "ready");
					KillTimer(hwnd, 1);

					InvalidateRect(hwnd, &rcWave, FALSE);
					InvalidateRect(hwnd, &rcBar, FALSE);
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

					for(i = -310 ; i <= 310 ; i++)
					{			//çleft
						s = 0;
						dw = dwOffset + i*wfe.nBlockAlign;
						if(0 <= dw && dw < whdr.dwBufferLength)
						{
							CopyMemory(&s, whdr.lpData+dw, wfe.wBitsPerSample/8);
							if(wfe.wBitsPerSample == 8) s-= 128;			// shift no volume(128) to 0
						}
						ptWaveL[i+310].y = 185*(-s)/wHalfMaxAmp + 195;
					}
					if(wfe.nChannels == 2)
					{			//stereo
						for(i = -310 ; i <= 310 ; i++)
						{		//right
							s = 0;
							dw = dwOffset + i*wfe.nBlockAlign + wfe.wBitsPerSample/8;
							if(0 <= dw && dw < whdr.dwBufferLength)
							{
								CopyMemory(&s, whdr.lpData + dw, wfe.wBitsPerSample/8);
								if(wfe.wBitsPerSample == 8) s-=128;		//shift no volume(128) to 0
							}
							ptWaveR[i+310].y = 185*(-s)/wHalfMaxAmp + 195;
						}
					}

					dw = (double)dwOffset/wfe.nAvgBytesPerSec*100;

					wsprintf(strTime, "%02d:%02d:%02d/%02d:%02d:%02d",
						(dw/100)/60,(dw/100)%60,dw%100,						//current time
						(maxTime/100)/60,(maxTime/100)%60,maxTime%100);		//total time
					InvalidateRect(hwnd, &rcWave, FALSE);
					InvalidateRect(hwnd, &rcBar, FALSE);
					InvalidateRect(hwnd, &rcInfo[0], FALSE);
					InvalidateRect(hwnd, &rcInfo[1], FALSE);
					return 0;
			case WM_PAINT:
				hdc = BeginPaint(hwnd , &ps);

				SelectObject(hdc_mem, hFont[2]);
				SetTextColor(hdc_mem, RGB(255, 255, 0)); 

				DrawText(hdc_mem, strFile, (int)strlen(strFile), &rcMove, DT_SINGLELINE | DT_VCENTER);
				DrawText(hdc_mem, strTime, (int)strlen(strTime), &rcInfo[1], DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				DrawText(hdc_mem, strVol, (int)strlen(strVol), &rcInfo[2], DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				DrawText(hdc_mem, strState, (int)strlen(strState), &rcInfo[3], DT_SINGLELINE | DT_CENTER | DT_VCENTER);

				SelectObject(hdc_mem, hPenR);
				Polyline(hdc_mem, ptWaveR, sizeof(ptWaveR)/sizeof(ptWaveR[0]));

				SelectObject(hdc_mem, hPenL);
				Polyline(hdc_mem, ptWaveL, sizeof(ptWaveL)/sizeof(ptWaveL[0]));

				// measure when strFile is over limitation
				/*StretchDIBits(hdc_mem, 0, 450, 20, 20, 0, 10, 20, 20, lpPixel, &bmpInfo, DIB_RGB_COLORS, SRCCOPY);

				BitBlt(hdc, 0, 0, iWidth, iHeight, hdc_mem, 0, 0, SRCCOPY);*/
				
				EndPaint(hwnd , &ps);
				return 0;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
		}

		return DefWindowProc(hwnd, msg, wp, lp);
}