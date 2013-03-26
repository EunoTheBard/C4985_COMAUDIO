#include "Common.h"
#include "resource.h"
#include "Variables.h"

// This is to get data for microphone one server, one client
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
