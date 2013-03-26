#include "Common.h"
#include "resource.h"
#include "Variables.h"

// This is to get data for multicast
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
			
			return TRUE;
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
