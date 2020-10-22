// CmdLineExt.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f CmdLineExtps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "CmdLineExt.h"

#include "CmdLineExt_i.c"
#include "CmdLineContextMenu.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_CmdLineContextMenu, CCmdLineContextMenu)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_CMDLINEEXTLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

//***************************************
//* Date            : 6.2.99
//* Last Modified   : 6.2.99
//* Function name	: DllRegisterServer
//* Description	    : 
//***************************************
//
STDAPI DllRegisterServer(void)
{
	_TCHAR		strCLSID[50];
	OLECHAR		strWideCLSID[50];
	CRegKey		key;
	HRESULT		hr;
	USES_CONVERSION;

	hr = _Module.RegisterServer(TRUE);

	if (SUCCEEDED(hr)) {
		if (::StringFromGUID2(CLSID_CmdLineContextMenu, strWideCLSID, 50) > 0) {
			_tcscpy(strCLSID, OLE2CT(strWideCLSID));
			hr = key.SetValue(HKEY_CLASSES_ROOT, _T("exefile\\shellex\\ContextMenuHandlers\\CmdLineExt\\"), strCLSID);
		}
	}
	return hr;
}

//***************************************
//* Date            : 6.2.99
//* Last Modified   : 6.2.99
//* Function name	: DllUnregisterServer
//* Description	    : 
//***************************************
//
STDAPI DllUnregisterServer(void)
{
	CRegKey		key;
	HRESULT		hr;

    hr = _Module.UnregisterServer(TRUE);

	if (SUCCEEDED(hr)) {
		if (key.Open(HKEY_CLASSES_ROOT, _T("exefile\\shellex\\ContextMenuHandlers\\")) == ERROR_SUCCESS) {
			hr = key.DeleteValue(NULL);
			if (hr != ERROR_SUCCESS && hr != ERROR_FILE_NOT_FOUND)
				return hr;
			hr = key.DeleteSubKey(_T("CmdLineExt"));
			if (hr != ERROR_SUCCESS && hr != ERROR_FILE_NOT_FOUND)
				return hr;
		}
	}

	return hr;
}