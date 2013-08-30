all: SCMouse.exe SCMouse.dll

SCMouse.dll: scmoudll.c scmoudll.def
	gcc -s -Zomf -Zdll -Zno-rte -oSCMouse.dll scmoudll.c scmoudll.def

scmoudll.lib: scmoudll.def 
	emximp -o scmoudll.lib scmoudll.def 

scmouse.res: scmouse.rc
	rc -r scmouse.rc

SCMouse.exe: scmouse.c scmouse.def scmouse.res scmoudll.lib
	gcc -s -Zomf -Zcrtdll -oSCMouse.exe scmouse.c scmouse.def scmouse.res scmoudll.lib
