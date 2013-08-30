#define  INCL_WIN
#define  INCL_DOS
#include <os2.h>

#ifdef __EMX__
#define INLINE inline
#else
#define INLINE
#endif

HAB	habDLL;
HMODULE hMod;
PFN	pfnInput;

HWND hwndWarpCenter=0;
SWP  swpWCWindow;
LONG ScreenTop;

char WarpCenterTitle[]="WarpCenter";

void ShowOrHideWC(LONG y);
//ULONG GetWarpCenterInfo();

void InitHookDLL()
    {
    SWP  swpDesktop;

    habDLL = 0;
    DosLoadModule(NULL, 0, "SCMOUSE", &hMod);
    DosQueryProcAddr(hMod, 0, "InputProc", &pfnInput);

    WinQueryWindowPos(HWND_DESKTOP, &swpDesktop); 
    ScreenTop = swpDesktop.cy - 1;
    }

void StartInputHook(void)
    {
    WinSetHook(habDLL, NULLHANDLE, HK_INPUT, pfnInput, hMod);
    }

BOOL StopInputHook(void)
    {
    int rc = WinReleaseHook(habDLL, NULLHANDLE, HK_INPUT, pfnInput, hMod);
    if(rc)
	DosFreeModule(hMod);
    return rc;
    }

BOOL InputProc(HAB hab, PQMSG pqMsg, ULONG fs)
    {
    if (pqMsg->msg == WM_MOUSEMOVE)
	ShowOrHideWC(pqMsg->ptl.y);

    return FALSE;
    }

/******************************/

ULONG GetWarpCenterInfo() 
    {
    SWBLOCK *pSwBlock;
    ULONG SwBlSize;
    ULONG SwBlLen;
    HWND hwnd;
    static HSWITCH hswitchPrev=0;
    int i;

    if(hwndWarpCenter && hswitchPrev==WinQuerySwitchHandle(hwndWarpCenter, 0))
	{
	WinQueryWindowPos(hwndWarpCenter, &swpWCWindow);
	return 0;
	}
/* hwndWarpCenter is not changed since previous call */

    SwBlLen=WinQuerySwitchList(0L, NULL, 0);
/* Get the number of entries in switch list */

    SwBlSize=sizeof(ULONG)+sizeof(SWENTRY)*SwBlLen;
    if(DosAllocMem((PPVOID)&pSwBlock, SwBlSize, PAG_WRITE|PAG_COMMIT))
	return 1;
    WinQuerySwitchList(0L, pSwBlock, SwBlSize);
/* Get the switch list */

    hwndWarpCenter=0;
    for(i = SwBlLen-1; i>0; i--) 
	if(strcmp(pSwBlock->aswentry[i].swctl.szSwtitle, WarpCenterTitle)==0)  
	    {
	    hwnd=pSwBlock->aswentry[i].swctl.hwnd;
	    WinQueryWindowPos(hwnd, &swpWCWindow);
	    if(swpWCWindow.x!=0) continue;
/* Warp Center - Properties has the same title in list*/
	    hwndWarpCenter=hwnd;
	    hswitchPrev=pSwBlock->aswentry[i].hswitch;
	    break;
	    }
/* First started processes are last in switch list */

    DosFreeMem(pSwBlock);
    return (hwndWarpCenter==0);
    }

INLINE
void ShowWarpCenter() 
    {
    if(hwndWarpCenter)
	WinSetWindowPos(hwndWarpCenter, HWND_TOP, 0,0,0,0, SWP_ZORDER);
    }

INLINE
void HideWarpCenter() 
    {
    if(hwndWarpCenter)
	WinSetWindowPos(hwndWarpCenter, HWND_BOTTOM, 0,0,0,0, SWP_ZORDER);
    }

ULONG IsWarpCenterProperties(HWND hwnd)
    {
    HSWITCH hswitch;
    SWCNTRL SwitchData;
    static HWND hwndPrev = 0;

    if(hwnd == hwndPrev) return 1;
    hwndPrev = 0; 
    if(!(hswitch = WinQuerySwitchHandle(hwnd, 0))) return 0;

    WinQuerySwitchEntry(hswitch, &SwitchData);

    if(strcmp(SwitchData.szSwtitle, WarpCenterTitle) == 0)
	{ hwndPrev = hwnd; return 1; }

    return 0;
    }

void ShowOrHideWC(LONG y)
    {
    static BOOL WasShown = FALSE;
    static BOOL MouseOnEdge = FALSE;
    static HWND hwndActive;

    static int CheckingCoord, HideTop, HideBottom;

    if(y == 0 || y == ScreenTop)
	{
	if(MouseOnEdge) return;
	MouseOnEdge = TRUE;
	if(GetWarpCenterInfo()) return;
	if(swpWCWindow.y != 0)		/* Top position */
	    {
	    CheckingCoord = ScreenTop;
	    HideBottom = ScreenTop - (swpWCWindow.cy<<1);
	    HideTop = ScreenTop;
	    }
	else				/* Bottom position */
	    {
	    CheckingCoord = 0;
	    HideBottom = 0;
	    HideTop = (swpWCWindow.cy<<1);
	    }

	if(y == CheckingCoord && WinQueryActiveWindow(HWND_DESKTOP))
	    {
	    ShowWarpCenter();
	    WasShown = TRUE;
	    }
	 return;
	}

    MouseOnEdge = FALSE;

    if(WasShown && (y < HideBottom || y > HideTop))
	{ 
	hwndActive=WinQueryActiveWindow(HWND_DESKTOP);
	if(hwndActive && !IsWarpCenterProperties(hwndActive))
	    {
	    HideWarpCenter();
	    WasShown=FALSE; 
	    }
	}
/* Hide if WarpCenter is not active */
    }
