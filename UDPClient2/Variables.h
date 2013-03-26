#include <commctrl.h>
#ifndef VARIABLES_H_
#define VARIABLES_H_

#define WM_SOCKET_CREATE  (WM_USER+10)
#define WM_SOCKET_CREATE2	(WM_USER+11)
#define MAXADDRSTR  16
#define TIMECAST_ADDR "234.5.6.10"
#define TIMECAST_PORT 8920
#define DEFAULT_PORT 7000


static HWND hTab = NULL;
static HWND hDlg = NULL;
static HWND hDlg2 = NULL;
static u_short port;
static char szTo[80];
static char szTo2[80];
static u_long lMCAddr;
static char achMCAddr[MAXADDRSTR];

#endif
