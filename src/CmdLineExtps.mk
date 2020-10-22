
CmdLineExtps.dll: dlldata.obj CmdLineExt_p.obj CmdLineExt_i.obj
	link /dll /out:CmdLineExtps.dll /def:CmdLineExtps.def /entry:DllMain dlldata.obj CmdLineExt_p.obj CmdLineExt_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del CmdLineExtps.dll
	@del CmdLineExtps.lib
	@del CmdLineExtps.exp
	@del dlldata.obj
	@del CmdLineExt_p.obj
	@del CmdLineExt_i.obj
