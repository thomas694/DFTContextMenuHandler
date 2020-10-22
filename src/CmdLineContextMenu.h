// CmdLineContextMenu.h : Declaration of the CCmdLineContextMenu

#ifndef __CMDLINECONTEXTMENU_H_
#define __CMDLINECONTEXTMENU_H_

#include "resource.h"       // main symbols
#include <shlguid.h>
#include <comdef.h>
#include <shlobj.h>
#include <string>

#define ID_RUN_WITH_CMD_LINE_PARAMS		0

/////////////////////////////////////////////////////////////////////////////
// CCmdLineContextMenu
class ATL_NO_VTABLE CCmdLineContextMenu : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCmdLineContextMenu, &CLSID_CmdLineContextMenu>,
	public IContextMenu3,
	public IShellExtInit
{
public:
	CCmdLineContextMenu()
	{
	}

	// IContextMenu interface
    STDMETHOD(QueryContextMenu)(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO lpici);
    STDMETHOD(GetCommandString)(UINT idCmd, UINT  uType, UINT *pwReserved, LPSTR pszName, UINT cchMax);   
    
	// IContextMenu2 interface
	STDMETHOD(HandleMenuMsg)(UINT uMsg, WPARAM wParam, LPARAM lParam);    
    
	// IContextMenu3 interface
	STDMETHOD(HandleMenuMsg2)(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);
    
	// IShellExtInit interface
	STDMETHOD(Initialize)(LPCITEMIDLIST pidlFolder, LPDATAOBJECT lpdobj, HKEY hkeyProgID);

DECLARE_REGISTRY_RESOURCEID(IDR_CMDLINECONTEXTMENU)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCmdLineContextMenu)
	COM_INTERFACE_ENTRY(IContextMenu3)
	COM_INTERFACE_ENTRY(IContextMenu2)
	COM_INTERFACE_ENTRY(IContextMenu)
	COM_INTERFACE_ENTRY(IShellExtInit)
END_COM_MAP()

private:
	typedef std::basic_string<_TCHAR>	string;
	string							m_strFileName;

};

#endif //__CMDLINECONTEXTMENU_H_
