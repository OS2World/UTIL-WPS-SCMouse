#define INCL_DOSPROCESS
#include <os2.h>

int main()
    {
    BOOL WasStarted;
    ULONG actual;

    InitHookDLL();
    WasStarted = StopInputHook();
    if(!WasStarted)
	{
	StartInputHook();
	DosWrite(1, "ON\n", 3, &actual);
	DosSleep(300);
	}
    else
	DosWrite(1, "OFF\n", 4, &actual);
    return 0;
    }
