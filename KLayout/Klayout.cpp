/*----------------------------------------- 
KLayout -- simple keyboard layout indicator
(c) Ryzhehvost, 2014 
-----------------------------------------*/ 


#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x501
#endif

#include <windows.h> 
#include <stdio.h>
#include <Strsafe.h>
#include <Commctrl.h>
//#include <Shellapi.h>

#include <Winbase.h>
#include <exception>

#define DECIMATE 10
#define WM_ShellNote (WM_APP + 1)
#define ID_TIMER 1 
#define IDM_PROPERTYS 1
#define IDM_LANG 128

#ifdef _DEBUG
#define ICONBGCOLOR RGB(0,128,128)
#else
#define ICONBGCOLOR RGB(0,0,128)
#endif

typedef struct {
    UINT cbSize;
    DWORD dwMask;
    int idCommand;
    int iImage;
    BYTE fsState;
    BYTE fsStyle;
    WORD cx;
    unsigned __int64 lParam;
    unsigned __int64 pszText;
    int cchText;
} TBBUTTONINFO64, *LPTBBUTTONINFO64;

typedef struct _TNPRIVICON
{
HWND hWnd;
UINT uID;
UINT uCallbackMessage;
DWORD dwState;
UINT uVersion;
HICON hIcon;
} TNPRIVICON, *PTNPRIVICON;

BOOL is64bit=FALSE;
BOOL DoesWin32MethodExist(PCWSTR pszModuleName, PCSTR pszMethodName)
{
    HMODULE hModule = GetModuleHandle(pszModuleName);
    if (hModule == NULL)
    {
        return FALSE;
    }
    return (GetProcAddress(hModule, pszMethodName) != NULL);
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 
HMENU Serv_Menu;
HWND myhwnd;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) 
{ 
is64bit=((DoesWin32MethodExist(L"kernel32.dll", "IsWow64Process") && IsWow64Process(GetCurrentProcess(), &is64bit)) && is64bit);
static  LPCWSTR szAppName = L"KLayout"; 
HWND hwnd; 
MSG msg; 
LPMENUITEMINFO MenuItem;
WNDCLASSEX wndclass; 
wndclass.cbSize = sizeof(wndclass); 
wndclass.style = CS_HREDRAW | CS_VREDRAW; 
wndclass.lpfnWndProc = WndProc; 
wndclass.cbClsExtra = 0; 
wndclass.cbWndExtra = 0; 
wndclass.hInstance = hInstance; 
wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); 
wndclass.hbrBackground =(HBRUSH) GetStockObject(WHITE_BRUSH); 
wndclass.lpszMenuName = NULL; 
wndclass.lpszClassName = szAppName; 
wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION); 
RegisterClassEx(&wndclass); 
hwnd = CreateWindow(szAppName, L"Keyboard Layout Indicator", 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL);
myhwnd=hwnd;
while(!SetTimer(hwnd, ID_TIMER, 300, NULL)) 
	if(IDCANCEL == MessageBox(hwnd, L"Too many clocks or timers!", szAppName, MB_ICONEXCLAMATION | MB_RETRYCANCEL)) 
		return FALSE; 
//ShowWindow(hwnd, iCmdShow); 
//UpdateWindow(hwnd);
Serv_Menu=CreatePopupMenu();
MenuItem=new MENUITEMINFO;
MenuItem->cbSize=sizeof(MENUITEMINFO);
MenuItem->fMask=MIIM_DATA | MIIM_FTYPE | MIIM_STATE | MIIM_STRING | MIIM_ID;
MenuItem->fType=MFT_STRING;
MenuItem->fState=MFS_ENABLED;
MenuItem->wID=IDM_PROPERTYS;
MenuItem->hSubMenu=NULL;
MenuItem->hbmpChecked=NULL;
MenuItem->hbmpUnchecked=NULL;
MenuItem->dwItemData=NULL;
MenuItem->dwTypeData=L"Ñâîéñòâà";
MenuItem->cch=(int)wcslen(MenuItem->dwTypeData);
MenuItem->hbmpItem=NULL;
InsertMenuItem(Serv_Menu,0,true,MenuItem);
delete MenuItem;
while(GetMessage(&msg, NULL, 0, 0)) 
	{ 
	TranslateMessage(&msg); 
	DispatchMessage(&msg); 
	} 
return (int)msg.wParam; 
} 


HBITMAP CreateLangBitmap(HKL Lang,LPWSTR LangName,int NameBuf,BOOL CapsOn)
	//_In  Lang - Keybord layout
	//_Out LangName - buffer for Language Name
	//_In  NameBuf - size of LangName buffer
	//_In  CapsOn - Show both letters uppercase
	//Result - bitmap with two-character lang name

{
wchar_t LangShort [10];
wchar_t LangLong [250];
LOGFONT lf;
HFONT hf;
HDC bdc;
static HDC mdc=0;
HBITMAP LangBitmap;
HBRUSH hBrush; 
RECT rc; 

		GetLocaleInfo(
			MAKELCID(Lang, SORT_DEFAULT),
			LOCALE_SISO639LANGNAME,
			LangShort,
			250
		);
		GetLocaleInfo(
			MAKELCID(Lang, SORT_DEFAULT),
			LOCALE_SNATIVELANGNAME,
			LangLong,
			250
		);
		CharUpperBuff(LangLong,1);

		if (CapsOn)
			CharUpperBuff(LangShort,2);
		else
			CharUpperBuff(LangShort,1);

		if (!mdc) 				 
			mdc=GetDC(NULL);//Ïîëó÷àåì îäèí ðàç ïðè çàïóñêå. Áîëüøå íå îòïóñêàåì.
							//Ýòî ïî-èäåå äîëæíî ðåøèòü ïðîáëåìû ïðè çàïóùåíûõ èãðàõ 
							//ñ õèòðûì ðåæèìîì ýêðàíà
							//è ñîçäàòü íîâûå, èíòåðåñíûå ïðîáëåìû!
							//(íàïðèìåð, ïðè ïåðåêëþ÷åíèè ðåæèìà âðó÷íóþ)
		bdc=CreateCompatibleDC(mdc);
		//ReleaseDC(NULL,mdc);
		lf.lfHeight = -MulDiv(7, GetDeviceCaps(bdc, LOGPIXELSY), 72);
		lf.lfWidth = 0;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
		lf.lfWeight = 0; //maybe 400?
		lf.lfItalic = FALSE;
		lf.lfUnderline = FALSE;
		lf.lfStrikeOut = FALSE;
		lf.lfCharSet = DEFAULT_CHARSET;
 		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
  		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  		lf.lfQuality = NONANTIALIASED_QUALITY;
  		lf.lfPitchAndFamily = FF_DONTCARE | DEFAULT_PITCH;

		StringCchCopyW(lf.lfFaceName,sizeof(lf.lfFaceName)/sizeof(lf.lfFaceName[0]),L"MS Sans Serif");  		//lstrcpy(lf.lfFaceName, L"MS Sans Serif");
		hf = CreateFontIndirect(&lf);
		SelectObject(bdc,hf);

		//LangBitmap=CreateCompatibleBitmap(bdc,16,16);
		LangBitmap=CreateBitmap(16,16,1,32,NULL);
		SelectObject(bdc,LangBitmap);
		hBrush = CreateSolidBrush(ICONBGCOLOR); 
		rc.top=0;
		rc.left=0;
		rc.bottom=16;
		rc.right=16;
		FillRect(bdc, &rc, hBrush); 
		//TextOut(bdc, 0, 0, LangShort, wcslen(LangShort));
		SetTextColor(bdc,RGB(255,255,255));
		SetBkColor(bdc,ICONBGCOLOR);
		rc.top=2;
		DrawText(bdc,LangShort,(int)wcslen(LangShort),&rc,0);
		DeleteDC(bdc);
		DeleteObject(hBrush);
		DeleteObject(hf);
		StringCchCopyW(LangName,NameBuf,LangLong);
		return LangBitmap;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) 
{ 
static BOOL fFlipFlop = FALSE; 
static int prev_pos=0;
static int framecount=0;
int pos;
//HBRUSH hBrush; 
//HDC bdc;
//RECT rc; 
HKL Layout;
//HKL* Layouts;
//int nbuf,curbuf;
//LPMENUITEMINFO MenuItem;
HWND CurWin;
DWORD procid;
HICON TrayIcon;
ICONINFO LangIcon;
HBITMAP LangBitmap;
static NOTIFYICONDATA nid;
//LOGFONT lf;
//HFONT hf;
//wchar_t LangShort [10];
wchar_t LangLong [250];
POINT pt;
//HMENU Lang_Menu;
HINSTANCE nResult;
//static HWND fgwindow;
HWND TrayNotifyHandle;
HWND OSDependence;
HWND FSysTrayHandle;
HANDLE hTrayProc;
int count;

TBBUTTONINFO64 mtbbi;
//TBBUTTON mtbb;
//int idCommand;
DWORD dwTrayProcessID;
SIZE_T dwBytesRead = -1;
TNPRIVICON pico;
LPTBBUTTONINFO ptb;
void* lpData;
void* icoData;

switch(iMsg) 
	{ 
	case WM_CREATE :
	case WM_TIMER : 
		
		framecount++;
		if (framecount==DECIMATE) {
		framecount=0;
		if (fFlipFlop) {//Åñëè èêîíêà óæå óñòàíàâëèâàëàñü - ïðîâåðÿåì å¸ ïîëîæåíèå
		TrayNotifyHandle = FindWindow(L"Shell_TrayWnd", NULL);
		if (TrayNotifyHandle) {
			TrayNotifyHandle = FindWindowEx(TrayNotifyHandle, 0, L"TrayNotifyWnd", NULL);
			if (TrayNotifyHandle) {
				OSDependence = FindWindowEx(TrayNotifyHandle, 0, L"SysPager", NULL);
				if (!OSDependence) {
					// â W2K SysPager îòñóòñòâóåò
					OSDependence = TrayNotifyHandle;
				}
				FSysTrayHandle = FindWindowEx(OSDependence, 0, L"ToolbarWindow32", NULL);
			}
		}
		count=(int)SendMessage(FSysTrayHandle, TB_BUTTONCOUNT,0, 0);
		pos=-1;
		//Äàëüøå èä¸ò ãðÿçíûé õàê, ÷òîáû óçíàòü, íà êàêîì ìåñòå íàøà èêîíêà.
		GetWindowThreadProcessId(FSysTrayHandle, &dwTrayProcessID);
		hTrayProc = OpenProcess(PROCESS_ALL_ACCESS, 0, dwTrayProcessID);
		if (hTrayProc){
			if (lpData = VirtualAllocEx(hTrayProc, 0, sizeof(TBBUTTONINFO64), MEM_COMMIT, PAGE_READWRITE)) {
				for (int i=0;i<count;i++) {
				memset(&mtbbi,0,sizeof(mtbbi));
				if (is64bit)
					mtbbi.cbSize=sizeof(TBBUTTONINFO64);
				else 
					mtbbi.cbSize=sizeof(TBBUTTONINFO);
				mtbbi.dwMask=TBIF_BYINDEX|TBIF_LPARAM;	
				if (WriteProcessMemory(hTrayProc,lpData,&mtbbi,sizeof(TBBUTTONINFO64),&dwBytesRead)) {
					if (SendMessage(FSysTrayHandle, TB_GETBUTTONINFO, i, (LPARAM)lpData)>=0) {
					// ×èòàåì äàííûå èç äðóãîãî ïðîöåññà
						if (ReadProcessMemory(hTrayProc, lpData, &mtbbi, sizeof(TBBUTTONINFO64), &dwBytesRead)) {
							icoData=NULL;
							if (is64bit) {
								if (!(int)*(((int*)&mtbbi.lParam)+1))
									icoData=(void*)mtbbi.lParam;
									//icoData=(void*)*((int*)(&mtbbi.lParam));
									
							} else {
								ptb=(LPTBBUTTONINFO)&mtbbi;
								icoData=(void*)((LPTBBUTTONINFO)&mtbbi)->lParam;
							}
							if (icoData && ReadProcessMemory(hTrayProc, icoData/*(LPCVOID)mtbbi.lParam*/,&pico,sizeof(TNPRIVICON),&dwBytesRead)) {
								if ((pico.hWnd!=0)&&(pico.hWnd==myhwnd)) {
									//ýòî íàøå îêíî.
									pos=i;									
									break;
								}
							} //else throw std::exception("ReadMemory2"); //ReadMemory
						} //else throw std::exception("ReadMemory");//ReadMemory
					} //else throw std::exception("SendMessage");//SendMessage
				} //else throw std::exception("WriteProcessMemory");//WriteProcessMemory
				} //for
			VirtualFreeEx(hTrayProc, lpData, 0, MEM_RELEASE);
            CloseHandle(hTrayProc);
			} //else throw std::exception("lpData");//if (lpData)
		} //else throw std::exception("hTrayProc");//if (hTrayProc)
		if ((pos<0)||(pos>prev_pos))	{
			//Ïåðâûå íå ìû - òî÷íî íàäî ïåðåðèñîâàòü.
			fFlipFlop=FALSE;
			Shell_NotifyIcon(NIM_DELETE, &nid);
			}
		if (pos<0) pos=0;
		prev_pos=pos;
		} //if (fFlipFlop)	
		} // if (framecount)

		//InvalidateRect(hwnd, NULL, FALSE); 
		CurWin=GetForegroundWindow();
		procid=GetWindowThreadProcessId(CurWin, NULL);
		Layout=GetKeyboardLayout(procid);
		LangBitmap = CreateLangBitmap(Layout,LangLong,sizeof(LangLong)/sizeof(LangLong[0]),GetKeyState(VK_CAPITAL)&0x01);
		//LangBitmap= CreateBitmap(16,16,1,24,NULL);
		LangIcon.hbmColor=LangBitmap;
		LangIcon.fIcon=TRUE;
		LangIcon.hbmMask= CreateBitmap(16,16,1,24,NULL);
		LangIcon.xHotspot=0;
		LangIcon.yHotspot=0;
		TrayIcon=CreateIconIndirect(&LangIcon);
		DeleteObject(LangIcon.hbmMask);
		DeleteObject(LangBitmap);
		memset(&nid, 0, sizeof(nid)); 
		nid.cbSize = sizeof(nid);
		nid.hWnd = hwnd; // õýíäë èìåþùåãîñÿ îêíà 
		nid.uID = 1234; // íåêîòîðûé íîìåð (èêîíêè, îòíîñÿùèåñÿ ê äàííîìó îêíó, äîëæíû ðàçëè÷àòüñÿ ïî íîìåðàì)
		nid.hIcon=TrayIcon;
		nid.uCallbackMessage = WM_ShellNote;
		StringCchCopyW(nid.szTip,sizeof(nid.szTip)/sizeof(nid.szTip[0]),LangLong);
		//lstrcpyn(nid.szTip, LangLong, sizeof(nid.szTip)/sizeof(nid.szTip[0]));
		nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;

		if (fFlipFlop)
			Shell_NotifyIcon(NIM_MODIFY, &nid);
		else {
			Shell_NotifyIcon(NIM_ADD, &nid);
			fFlipFlop=TRUE;
			}
		DestroyIcon(TrayIcon);
		return 0;
   case WM_ShellNote:
//	    if (GetForegroundWindow()!=hwnd)	fgwindow=GetForegroundWindow(); //save foreground window handle
	    switch((UINT)lParam) {
          case WM_RBUTTONUP:
				SetForegroundWindow(hwnd);
				GetCursorPos(&pt);
				TrackPopupMenu(Serv_Menu,TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON,pt.x,pt.y,0,hwnd,NULL);
				break;
/*		  case WM_LBUTTONUP:
				Lang_Menu=CreatePopupMenu();
				MenuItem=new MENUITEMINFO;
				MenuItem->cbSize=sizeof(MENUITEMINFO);
				MenuItem->fMask=MIIM_DATA | MIIM_FTYPE | MIIM_STATE | MIIM_STRING | MIIM_ID | MIIM_BITMAP;
				MenuItem->fType=MFT_STRING;
				MenuItem->fState=MFS_ENABLED;
				MenuItem->hSubMenu=NULL;
				MenuItem->hbmpChecked=NULL;
				MenuItem->hbmpUnchecked=NULL;
				MenuItem->dwItemData=NULL;
				MenuItem->hbmpItem=NULL;
				Layouts = NULL;
				nbuf=GetKeyboardLayoutList(0,Layouts);
				Layouts = new HKL[nbuf];
				GetKeyboardLayoutList(nbuf,Layouts);
				for (curbuf=0;curbuf<nbuf;curbuf++) {
					MenuItem->wID=IDM_LANG+curbuf;
					MenuItem->hbmpItem=CreateLangBitmap(Layouts[curbuf],LangLong,sizeof(LangLong)/sizeof(LangLong[0]),false);				
					MenuItem->dwTypeData = LangLong;
					MenuItem->cch=wcslen(MenuItem->dwTypeData);
					InsertMenuItem(Lang_Menu,curbuf,true,MenuItem);
				}
				SetForegroundWindow(hwnd);
				GetCursorPos(&pt);
				TrackPopupMenu(Lang_Menu,TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON,pt.x,pt.y,0,hwnd,NULL);
				break;
*/
        }
	    return 0;
	case WM_COMMAND:	
		switch (LOWORD(wParam)) {
			case IDM_PROPERTYS:
				nResult = ShellExecute(NULL, NULL, L"rundll32.exe", L"shell32.dll,Control_RunDLL input.dll",NULL,SW_SHOW);
				//Unsure, if below part needed at all. Let it be.
#pragma warning(disable:4311)
				if ((int)nResult == SE_ERR_ACCESSDENIED) { // Trick for requesting elevation, this is a verb not listed in the documentation above.
					ShellExecute(NULL,L"runas", L"rundll32.exe", L"shell32.dll,Control_RunDLL input.dll",NULL,SW_SHOW);
				}
#pragma warning(default:4311)
				break;
/*				default:
				SetForegroundWindow(fgwindow);
				Layouts = NULL;
				nbuf=GetKeyboardLayoutList(0,Layouts);
				Layouts = new HKL[nbuf];
				GetKeyboardLayoutList(nbuf,Layouts);
				Layout= Layouts[LOWORD(wParam)-IDM_LANG];
				PostMessage(fgwindow,WM_ACTIVATE,WA_CLICKACTIVE,NULL);
				PostMessage(fgwindow, WM_INPUTLANGCHANGEREQUEST,INPUTLANGCHANGE_BACKWARD,(LPARAM)Layout);
				//ActivateKeyboardLayout(Layout,0);
				break;
*/
		}

		return 0;
	case WM_DESTROY : 
		KillTimer(hwnd, ID_TIMER);
		Shell_NotifyIcon(NIM_DELETE, &nid);
		DestroyMenu(Serv_Menu);
		PostQuitMessage(0); 		
		return 0; 
	} 
return DefWindowProc(hwnd, iMsg, wParam, lParam); 
} 
