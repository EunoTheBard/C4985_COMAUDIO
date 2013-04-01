/*------------------------------------------------------------------------------------------------------------------
--  		FUNCTION:		WndProc
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

void createSocketForMulticast(HWND);
void createSocketForMicrophone(HWND);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc2(HWND, UINT, WPARAM, LPARAM);

static HWND hDlg = NULL;
static HWND hDlg2 = NULL;
static u_short port;
static char szTo[80];
static char szTo2[80];
static u_long lMCAddr;
static char achMCAddr[MAXADDRSTR];


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp){

	//system color
	static const COLORREF newSysColor[] = { RGB(255 , 255, 255) };
	static const int setSysColor[] = { COLOR_BTNFACE  }; 
	static COLORREF defSysColor[1];

	//tab controls
	INITCOMMONCONTROLSEX ic;
	static RECT rcDisp;

	// create a process to simplify
	static PROCESS_INFORMATION prs;
	static STARTUPINFO si;
	TCHAR *strExe = _T("AdvancedPlayer.exe");
	
	//trackbar
	static HWND hTrack = NULL;
	static DWORD pos = 0;
	static int iTimeCursor;

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

	static int iPixel = 0;

	HBITMAP hBmp;
	static HDC hdc_mem;

	// file
	static OPENFILENAME ofn;
	static TCHAR szFile[MAX_PATH];
	static TCHAR szFileTitle[MAX_PATH];

	//wav file
	static HWAVEOUT hWaveOut;
	static WAVEFORMATEX wfe;
	static WAVEHDR whdr;

	const RECT rcWave = {10, 10, 1225, 447};
	const RECT rcBar = {10, 390, 631, 401};
	static RECT rcInfo[] = {{1, 1060, 900, 20}, {2000, 1060, 110, 20}, {2220, 1060, 110, 20}, {2335, 1060, 108, 20}};
	static RECT rcMove = rcInfo[0];

	static MMTIME mmTime;
	static DWORD maxTime;
	DWORD dwOffset;

	// wave
	static POINT ptWaveL[1226], ptWaveR[1226];
	static WORD wHalfMaxAmp;
	static HPEN hPenL, hPenR;
	static int hueL, hueR, hueLR;

	// volume
	static DWORD dwVolStart;
	static WORD wVolNow;
	DWORD dwVolTemp;

	// font
	static HFONT hFont[3];

	//text
	static TCHAR strFile[1024];
	static TCHAR strTime[32];
	static TCHAR strVol[16];
	static TCHAR strState[16];
	static SIZE sizeFile;

	static BOOL open = FALSE;
	static BOOL play = FALSE;
	static BOOL pause = FALSE;
	static BOOL loop = FALSE;

		switch( msg ){
			case WM_CREATE:
				defSysColor[0] = GetSysColor(setSysColor[0]);
				SetSysColors(1 , setSysColor , newSysColor);
				DragAcceptFiles(hwnd, TRUE);
				
				// background bmp
				lpPixel = bmptoDIB(_T("480127.bmp"), &bmpInfo);
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
				ofn.lpstrFilter = _T("WAVE_FORMAT_PCM(*.wav)\0*.wav\0");
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFileTitle = szFileTitle;
				ofn.nMaxFileTitle = MAX_PATH;
				ofn.Flags = OFN_FILEMUSTEXIST;
				ofn.lpstrDefExt = _T("wav");
				ofn.lpstrTitle = _T("Select PCM type of Wav file");

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

				hFont[0] = CreateFont(18, 0, 0, 0, FW_BOLD, TRUE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_DONTCARE, _T("Times New Roman"));
				hFont[1] = CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_DONTCARE, _T("Arial Black"));
				hFont[2] = CreateFont(12, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_DONTCARE, _T("Times New Roman"));
			
				// initialize MMTIME
				mmTime.wType = TIME_BYTES;

				// store an initial value for volume and show the volume
				waveOutGetVolume(hWaveOut, &dwVolStart);
				wVolNow = LOWORD(dwVolStart);
				wsprintf(strVol, _T("%03d/128"), (wVolNow +1)/512);

				// show state
				 wsprintf(strState, _T("NoData"));
			
				//button
				hOpen = CreateWindowEx(0, _T("BUTTON"), _T("Open") , WS_CHILD | WS_VISIBLE , 15, 620 , 70 , 30 , hwnd , (HMENU)ID_OPEN, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hPlay = CreateWindowEx(0, _T("BUTTON"), _T("Play") , WS_CHILD | WS_VISIBLE , 90, 620 , 70 , 30 , hwnd , (HMENU)ID_PLAY, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hPause = CreateWindowEx(0, _T("BUTTON"), _T("Pause") , WS_CHILD | WS_VISIBLE , 165 , 620 , 70 , 30 , hwnd , (HMENU)ID_PAUSE, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hStop = CreateWindowEx(0, _T("BUTTON"), _T("Stop") , WS_CHILD | WS_VISIBLE , 240 , 620 , 70 , 30 , hwnd , (HMENU)ID_STOP, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hLoop = CreateWindowEx(0, _T("BUTTON"), _T("Loop") , WS_CHILD | WS_VISIBLE , 900 , 620 , 70 , 30 , hwnd , (HMENU)ID_LOOP, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hDown = CreateWindowEx(0, _T("BUTTON"), _T("-") , WS_CHILD | WS_VISIBLE , 1000 , 620 , 40 , 30 , hwnd , (HMENU)ID_DOWN, ((LPCREATESTRUCT)(lp))->hInstance, NULL);
				hUp = CreateWindowEx(0, _T("BUTTON"), _T("+") , WS_CHILD | WS_VISIBLE , 1050 , 620 , 40 , 30 , hwnd , (HMENU)ID_UP, ((LPCREATESTRUCT)(lp))->hInstance , NULL);
				hExit = CreateWindowEx(0, _T("BUTTON"), _T("Exit") , WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 1160, 620 , 70 , 30 , hwnd , (HMENU)ID_EXITT, ((LPCREATESTRUCT)(lp))->hInstance, NULL);		

				//track bar
				ic.dwSize = sizeof(INITCOMMONCONTROLSEX); 
				ic.dwICC = ICC_BAR_CLASSES;
				InitCommonControlsEx(&ic);

				hTrack = CreateWindowEx(0, TRACKBAR_CLASS, _T(""), WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS, 10, 580, 1230, 25, hwnd, (HMENU)IDC_TRACKBAR, ((LPCREATESTRUCT)(lp))->hInstance, NULL);
				 SendMessage(hTrack, TBM_SETRANGE, (WPARAM)true,(LPARAM) MAKELONG(0, 1230)); 
				return 0;
			case WM_SIZE:
				return 0;
			case WM_SOCKET_CREATE:
				createSocketForMulticast(hwnd);
				return 0;
			case WM_SOCKET_CREATE2:
				createSocketForMicrophone(hwnd);
				return 0;
			case WM_COMMAND:
				switch(LOWORD(wp)) {
					case ID_TAB:
						break;
					case ID_OPTIONS_DOWNLOADMUSIC:////////   CREATE UDP SOCKET     FROM MENU           !!!!
						hDlg = CreateDialog((HINSTANCE)GetWindowLong(hwnd , GWL_HINSTANCE), MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DlgProc);	
						ShowWindow(hDlg, SW_NORMAL);
						
						return 0;	
					case ID_OPTIONS_MICROPHONE://////      CREATE MICROPHONE FROM MENU  !!!!!!!
						hDlg2 = CreateDialog((HINSTANCE)GetWindowLong(hwnd , GWL_HINSTANCE), MAKEINTRESOURCE(IDD_DIALOG2), hwnd, DlgProc2);	
						ShowWindow(hDlg2, SW_NORMAL);	

						return 0;
					case ID_DIRECTX_MULTIPLAYER:
						GetStartupInfo(&si);
						if (!CreateProcess(strExe , NULL ,NULL , NULL , FALSE ,  CREATE_NEW_CONSOLE , NULL , NULL , &si , &prs))
							MessageBox(hwnd , TEXT("Cannot create a process") , NULL , MB_OK);
						return 0;
					
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
									SendMessage(hTrack, TBM_SETPOS, (WPARAM) TRUE,(LPARAM)iTimeCursor); 
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
									wsprintf(strState, _T("NoData"));
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

								wsprintf(strFile,_T("%s :- %d bits / %s / %d Hz"),
									ofn.lpstrFileTitle,wfe.wBitsPerSample,
									(wfe.nChannels==2)?_T("stereo"):_T("mono"),wfe.nSamplesPerSec);

								SelectObject(hdc_mem,hFont[2]);
								GetTextExtentPoint32(hdc_mem,strFile,(int)_tcslen(strFile),&sizeFile);

								maxTime=100*whdr.dwBufferLength/wfe.nAvgBytesPerSec;

								wsprintf(strTime,_T("00:00:00/%02d:%02d:%02d"),
									(maxTime/100)/60,(maxTime/100)%60,maxTime%100);		//total time

								open=TRUE;
								wsprintf(strState,_T("ready"));
								
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
							wsprintf(strState,_T("playing"));
							InvalidateRect(hwnd,&rcInfo[3],FALSE);
					
							if(pause)
							{	
								pause=FALSE;
								waveOutRestart(hWaveOut);
							}
							else
							{		
								iTimeCursor=0;
								SendMessage(hTrack, TBM_SETPOS, (WPARAM) TRUE,(LPARAM)iTimeCursor); 
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
								wsprintf(strState,_T("playing"));
								waveOutRestart(hWaveOut);
								SetTimer(hwnd,1,50,NULL);
							}
							else
							{
								pause=TRUE;
								play=FALSE;
								wsprintf(strState,_T("pause"));
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
								SendMessage(hTrack, TBM_SETPOS, (WPARAM) TRUE,(LPARAM)iTimeCursor); 
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
									wsprintf(strState,_T("ready"));

									InvalidateRect(hwnd,&rcWave,FALSE);
									InvalidateRect(hwnd,&rcInfo[0],FALSE);
									InvalidateRect(hwnd,&rcInfo[3],FALSE);
								}
								//pushed again after pause
								iTimeCursor=0;
								SendMessage(hTrack, TBM_SETPOS, (WPARAM) TRUE,(LPARAM)iTimeCursor); 
								wsprintf(strTime,_T("00:00:00/%02d:%02d:%02d"), (maxTime/100)/60,(maxTime/100)%60,maxTime%100);			//total time
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

						wsprintf(strVol,_T("%03d/128"),(wVolNow+1)/512);
						InvalidateRect(hwnd,&rcInfo[2],FALSE);
						break;
					case ID_UP:
						waveOutGetVolume(hWaveOut,&dwVolTemp);
						ia=LOWORD(dwVolTemp);		// store in wVolNow after mute cancellation
						ia+=512;
						wVolNow=(ia>0xFFFF)?0xFFFF:ia;
						waveOutSetVolume(hWaveOut,MAKELONG(wVolNow,wVolNow));

						wsprintf(strVol,_T("%03d/128"),(wVolNow+1)/512);
						InvalidateRect(hwnd,&rcInfo[2],FALSE);
						break;
					case ID_EXITT:
					case ID_EXIT_EXIT:
						 KillTimer(hwnd,1);
						
						SetSysColors(1 , setSysColor , defSysColor);
						ExitProcess(-1);
						DestroyWindow(hwnd);
						break;
				}
				return 0;
			case WM_HSCROLL:
				if( (HWND)lp == GetDlgItem( hwnd, IDC_TRACKBAR ) ){
					pos = SendMessage( (HWND)lp, TBM_GETPOS, 0, 0 );
				}
				return 0;
			case WM_KEYDOWN:
				pos = SendMessage(hTrack, TBM_GETPOS, 0, 0);
				iTimeCursor = pos;
				SendMessage(hTrack, TBM_SETSELSTART, (WPARAM)false, (LPARAM)iTimeCursor); 
				waveOutReset(hWaveOut);
				break; 
			case WM_LBUTTONUP: 
				pos = SendMessage(hTrack, TBM_GETPOS, 0, 0); 
				iTimeCursor = pos;
				SendMessage(hTrack, TBM_SETSELEND, (WPARAM)true, (LPARAM)iTimeCursor); 
				break; 
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
			        SendMessage(hTrack, TBM_SETPOS, (WPARAM) TRUE,(LPARAM)iTimeCursor); 

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
					wsprintf(strState, _T("NoData"));

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

				wsprintf(strFile, _T("%s :- %d bits / %s / %d Hz"), PathFindFileName(strFile), wfe.wBitsPerSample, (wfe.nChannels==2)?"stereo":"mono", wfe.nSamplesPerSec);

				SelectObject(hdc_mem, hFont[2]);
				GetTextExtentPoint32(hdc_mem, strFile, (int)_tcslen(strFile), &sizeFile);

				maxTime = 100*whdr.dwBufferLength/wfe.nAvgBytesPerSec;

				wsprintf(strTime,_T("00:00:00/%02d:%02d:%02d"),
					(maxTime/100)/60,(maxTime/100)%60,maxTime%100);		//total time

				open=TRUE;
				wsprintf(strState, _T("ready"));

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
						SendMessage(hTrack, TBM_SETPOS, (WPARAM) TRUE,(LPARAM)0); 
						waveOutWrite(hWaveOut, &whdr, sizeof(WAVEHDR));
						return 0;
					}

					if(play)
					{		//play completed
						iTimeCursor = 1230;
						SendMessage(hTrack, TBM_SETPOS, (WPARAM) TRUE,(LPARAM)iTimeCursor); 
						wsprintf(strTime,_T("%02d:%02d:%02d/%02d:%02d:%02d"),
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
					wsprintf(strState, _T("ready"));
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

					iTimeCursor = (double)dwOffset/whdr.dwBufferLength*1230;		//change Cursor coordinate
					SendMessage(hTrack, TBM_SETPOS, (WPARAM) TRUE,(LPARAM)iTimeCursor);
					for(i = -612 ; i <= 612 ; i++) 
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
						for(i = -612 ; i <= 612 ; i++)
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
					
					wsprintf(strTime, _T("%02d:%02d:%02d/%02d:%02d:%02d"),
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
				
					DrawText(hdc_mem, strFile, (int)_tcslen(strFile), &rcMove, DT_SINGLELINE | DT_VCENTER);
					DrawText(hdc_mem,strTime, (int)_tcslen(strTime), &rcInfo[1], DT_SINGLELINE | DT_CENTER | DT_VCENTER);
					DrawText(hdc_mem, strVol, (int)_tcslen(strVol), &rcInfo[2], DT_SINGLELINE | DT_CENTER | DT_VCENTER);
					DrawText(hdc_mem, strState, (int)_tcslen(strState), &rcInfo[3], DT_SINGLELINE | DT_CENTER | DT_VCENTER);

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
				SetSysColors(1 , setSysColor , defSysColor);
				PostQuitMessage(0);
				return 0;
			default:
				return DefWindowProc(hwnd, msg, wp, lp);
		}

		return 0;
}



void createSocketForMulticast(HWND hwnd)
{
	WSADATA	wsd;
	int nRet;
	char errmsg[512];
	BOOL  fFlag;
	SOCKADDR_IN saddr, stSrcAddr;
	struct ip_mreq stMreq;         /* Multicast interface structure */
	char achInBuf[1024];
	static SOCKET Socket;

	if(WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
		 wsprintf(errmsg,  _T("WSAStartup failed: %d"), WSAGetLastError());
		MessageBox(hwnd, errmsg,  _T("UDP Client"), MB_OK);
        DestroyWindow(hwnd);
    }

	if((Socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		 wsprintf(errmsg,  _T("socket() failed: %d"), WSAGetLastError());
		MessageBox(hwnd, errmsg, _T("UDP Client"), MB_OK);
		WSACleanup();
		DestroyWindow(hwnd);
	}

	fFlag = TRUE;
	if((nRet = setsockopt(Socket, SOL_SOCKET,SO_REUSEADDR,(char *)&fFlag,sizeof(fFlag))) != 0)
	{
		 wsprintf(errmsg, _T("setsockopt SO_REUSEADDR failed: %d"), WSAGetLastError());
		MessageBox(hwnd, errmsg, _T("UDP Client"), MB_OK);
		WSACleanup();
		DestroyWindow(hwnd);
	}

	memset(&saddr, 0, sizeof(SOCKADDR_IN));
	saddr.sin_port = htons(port);
	saddr.sin_family = AF_INET;
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;

	if((nRet = bind(Socket, (SOCKADDR *)&saddr, sizeof(saddr))) == SOCKET_ERROR)
	{
		 wsprintf(errmsg, _T("bind failed: %d"), WSAGetLastError());
		MessageBox(hwnd, errmsg, _T("UDP Client"), MB_OK);
		WSACleanup();
		DestroyWindow(hwnd);
	}

	stMreq.imr_multiaddr.s_addr = inet_addr(achMCAddr);
    stMreq.imr_interface.s_addr = INADDR_ANY;

	if((nRet = setsockopt(Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&stMreq, sizeof(stMreq))) == SOCKET_ERROR)
	{
		 wsprintf(errmsg, _T("setsockopt IP_ADD_MEMBERSHIP address %s failed: %d"), achMCAddr, WSAGetLastError());
		MessageBox(hwnd, errmsg, _T("UDP Client"), MB_OK);
		WSACleanup();
		DestroyWindow(hwnd);
	}

	for(;;) 
	{
		int addr_size = sizeof(struct sockaddr_in);

		if((nRet = recvfrom(Socket, achInBuf, 1024, 0,(struct sockaddr*)&stSrcAddr, &addr_size)) < 0)
		{
			wsprintf(errmsg, _T("recvfrom() failed: %d"), WSAGetLastError());
			MessageBox(hwnd, errmsg, _T("UDP Client"), MB_OK);
			WSACleanup();
			exit(1);
		}
		else
		{
			achInBuf[nRet] = '\0';
			MessageBox(hwnd, achInBuf, _T("UDP Client"), MB_OK);
		}

	}


		 /* Leave the multicast group: With IGMP v1 this is a noop, but 
	   *  with IGMP v2, it may send notification to multicast router.
	   *  Even if it's a noop, it's sanitary to cleanup after one's self.
	   */
	  //stMreq.imr_multiaddr.s_addr = inet_addr(achMCAddr);
	  //stMreq.imr_interface.s_addr = INADDR_ANY;
	  //nRet = setsockopt(Socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&stMreq, sizeof(stMreq));
	  //if (nRet == SOCKET_ERROR) 
	  //{
			//_stprintf(errmsg, _T("setsockopt() IP_DROP_MEMBERSHIP address %s failed: %d"),  achMCAddr, WSAGetLastError());
			//MessageBox(hWnd, errmsg, "UDP Client", MB_OK);
			//WSACleanup();
			//exit(1);
	  //} 

	  ///* Close the socket */
	  //closesocket(Socket);

	  ///* Tell WinSock we're leaving */
	  //WSACleanup();
}

void createSocketForMicrophone(HWND hWnd)
{
		WSADATA	wsd;
		SOCKADDR_IN saddr2;
		HOSTENT *lpHost;
		int nRet;
		char errmsg[512];
		BOOL  fFlag;
		static SOCKET Socket2;
		unsigned int addr;

	

		if(WSAStartup(MAKEWORD(2,2), &wsd) != 0)
		{
			 wsprintf(errmsg, _T("WSAStartup failed: %d"), WSAGetLastError());
			MessageBox(hWnd, errmsg, _T("UDP Client"), MB_OK);
			DestroyWindow(hWnd);
		}

	

		if((Socket2 = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
		{
			 wsprintf(errmsg, _T("socket() failed: %d"), WSAGetLastError());
			MessageBox(hWnd, errmsg, _T("UDP Client"), MB_OK);
			WSACleanup();
			DestroyWindow(hWnd);
		}

		lpHost = gethostbyname(szTo2);
		if(lpHost == NULL) 
		{
			addr = inet_addr(szTo2);
			lpHost = gethostbyaddr((char *)&addr, 4, AF_INET);
		}

		if(lpHost == NULL) 
		{
			MessageBox(hWnd, _T("gethostbyaddr failed"), _T("UDP Client"), MB_OK);
			closesocket(Socket2);
			WSACleanup();
			DestroyWindow(hWnd);
		}

		fFlag = TRUE;
		if((nRet = setsockopt(Socket2, SOL_SOCKET,SO_REUSEADDR,(char *)&fFlag,sizeof(fFlag))) != 0)
		{
			 wsprintf(errmsg, _T("setsockopt SO_REUSEADDR failed: %d"), WSAGetLastError());
			MessageBox(hWnd, errmsg, _T("UDP Client"), MB_OK);
			WSACleanup();
			DestroyWindow(hWnd);
		}

		memset(&saddr2, 0, sizeof(SOCKADDR_IN));
		saddr2.sin_port = htons(port);
		saddr2.sin_family = AF_INET;
		saddr2.sin_addr.S_un.S_addr = *((u_long *)lpHost->h_addr);

		if(connect(Socket2, (SOCKADDR *)&saddr2,sizeof(saddr2)) == SOCKET_ERROR) 
		{
			if(WSAGetLastError() != WSAEWOULDBLOCK) 
			{
				MessageBox(hWnd, _T("connect Error"), _T("UDP Client"), MB_OK | MB_ICONEXCLAMATION);
				closesocket(Socket2);
				WSACleanup();
				DestroyWindow(hWnd);
			}
		}

	
    
		MessageBox(hWnd, _T("Waiting for client's connection"), _T("UDP Client"), MB_OK);
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  	static HWND hParent;
	BOOL bSuccess;
	static HWND hSendto;
	TCHAR errmsg[512];

	hParent = GetParent(hwnd);
	
	switch(uMsg)
	{
		case WM_INITDIALOG:
			hSendto = GetDlgItem(hDlg, IDC_EDIT1);
			SetDlgItemText(hwnd, IDC_EDIT1, TIMECAST_ADDR);
			SetDlgItemInt(hwnd, IDC_EDIT2, TIMECAST_PORT, FALSE);

		r	eturn TRUE;
		case WM_COMMAND:

		switch(LOWORD(wParam))
		{	
			case IDOK:
				GetWindowText(hSendto, szTo, (int)sizeof(szTo));
				if(strcmp(szTo, "") == 0)
				{
					MessageBox(hDlg, _T("no server address"), _T("UDP Client"), MB_OK);
					SetFocus(GetDlgItem(hwnd, IDC_EDIT1));
					return FALSE;
				}
				else
				{
					/* Evaluate multicast address provided */
					lMCAddr = inet_addr(szTo);
					if (!((lMCAddr>=0xe0000000) || (lMCAddr<=0xefffffff)))
					{
						strcpy(achMCAddr, TIMECAST_ADDR);
						_stprintf(errmsg, _T("Invalid address %s provided by user.\nusing default address: %s"), szTo, achMCAddr);
						MessageBox(hwnd, errmsg, "UDP Client", MB_OK);
					}
					else
					{	
						strcpy (achMCAddr, szTo);
					}
				}

				if((port = (u_short)GetDlgItemInt(hwnd, IDC_EDIT2, &bSuccess, FALSE)) == 0)
				{
					MessageBox(hwnd, "Enter port number!", "UDP Client", MB_OK);
					SetFocus(GetDlgItem(hwnd, IDC_EDIT2));
					return FALSE;
				}

				PostMessage(hParent, WM_SOCKET_CREATE, 0, 0);
				DestroyWindow(hwnd);
				return TRUE;
			case IDCANCEL:
				DestroyWindow(hwnd);
				return TRUE;
		}
			return FALSE;
		case WM_DESTROY:
			hDlg = NULL;
			return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK DlgProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hParent2;
	BOOL bSuccess;
	static HWND hSendto2;

	hParent2 = GetParent(hwnd);
	
	switch(uMsg)
	{
		case WM_INITDIALOG:
			hSendto2 = GetDlgItem(hDlg, IDC_EDIT3);
			SetDlgItemInt(hwnd, IDC_EDIT4, DEFAULT_PORT, FALSE);

			return TRUE;
		case WM_COMMAND:

			switch(LOWORD(wParam))
			{	
				case IDOK:
					GetWindowText(hSendto2, szTo2, (int)sizeof(szTo2));
					if(strcmp(szTo2, "") == 0)
					{
						MessageBox(hDlg, _T("no server address"), _T("UDP Client"), MB_OK);
						SetFocus(GetDlgItem(hwnd, IDC_EDIT3));
						return FALSE;
					}
					
					if((port = (u_short)GetDlgItemInt(hwnd, IDC_EDIT4, &bSuccess, FALSE)) == 0)
					{	
						MessageBox(hwnd, _T("Enter port number!"), _T("UDP Client"), MB_OK);
						SetFocus(GetDlgItem(hwnd, IDC_EDIT4));
						return FALSE;
					}

				PostMessage(hParent2, WM_SOCKET_CREATE2, 0, 0);
				DestroyWindow(hwnd);
				return TRUE;
				case IDCANCEL:
					DestroyWindow(hwnd);
					return TRUE;
			}
			return FALSE;
		case WM_DESTROY:
			hDlg2 = NULL;
			return TRUE;
	}
	return FALSE;
}


