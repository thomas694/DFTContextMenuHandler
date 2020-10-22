// CmdLineContextMenu.cpp : Implementation of CCmdLineContextMenu
#include "stdafx.h"
#include "CmdLineExt.h"
#include "CmdLineContextMenu.h"
#include "CmdLinePromptDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CCmdLineContextMenu

//***************************************
//* Date            : 6.2.99
//* Last Modified   : 2.10.2000
//* Function name	: CCmdLineContextMenu::QueryContextMenu
//* Description	    : 
//***************************************
//
STDMETHODIMP CCmdLineContextMenu::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	::InsertMenu(hmenu, indexMenu++, MF_BYPOSITION | MF_STRING, idCmdFirst + ID_RUN_WITH_CMD_LINE_PARAMS, _T("&Run with Parameters"));
	return ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_NULL, (USHORT)(ID_RUN_WITH_CMD_LINE_PARAMS + 1)));
}

//***************************************
//* Date            : 6.2.99
//* Last Modified   : 6.2.99
//* Function name	: CCmdLineContextMenu::InvokeCommand
//* Description	    : 
//***************************************
//
STDMETHODIMP CCmdLineContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO lpici)
{
	HINSTANCE		hInst = NULL;

    switch (LOWORD(lpici->lpVerb)) {
	case ID_RUN_WITH_CMD_LINE_PARAMS:
		CCmdLinePromptDlg		dlg(m_strFileName);
		if (dlg.DoModal() == IDOK) {
			hInst = ::ShellExecute(NULL, NULL, m_strFileName.data(), dlg.m_strCmdLine.data(), NULL, SW_SHOW);
			if ((int)hInst <= 32) {
				LPVOID lpMsgBuf;
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
				(int)hInst, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &lpMsgBuf, 0, NULL );
				::MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION );
				LocalFree( lpMsgBuf );
			}
		}
		break;
	}

	return S_OK;
}
 
//***************************************
//* Date            : 6.2.99
//* Last Modified   : 1.21.2000
//* Function name	: CCmdLineContextMenu::GetCommandString
//* Description	    : 
//***************************************
//
STDMETHODIMP CCmdLineContextMenu::GetCommandString(UINT idCmd, UINT  uType, UINT *pwReserved, LPSTR pszName, UINT cchMax)
{
	HRESULT				hr = E_INVALIDARG;

	switch (uType) {
	case GCS_HELPTEXTA:
	case GCS_HELPTEXTW:
		switch (idCmd) {
		case ID_RUN_WITH_CMD_LINE_PARAMS:
			if ((uType & GCS_HELPTEXTW) == GCS_HELPTEXTW) {
				wcsncpy((LPWSTR)pszName, OLESTR("Run this executable, specifiying command-line parameters."), cchMax);
				((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
			}
			else {
				strncpy((LPSTR)pszName, "Run this executable, specifiying command-line parameters.", cchMax);
				((LPSTR)pszName)[cchMax - 1] = '\0';
			}
			hr = S_OK;
			break;
		}
		break;

	case GCS_VALIDATE:
		hr = S_OK;
		break;
	}

	return hr;
}
    
//***************************************
//* Date            : 6.2.99
//* Last Modified   : 6.2.99
//* Function name	: CCmdLineContextMenu::HandleMenuMsg
//* Description	    : 
//***************************************
//
STDMETHODIMP CCmdLineContextMenu::HandleMenuMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	return S_OK;
}
   
//***************************************
//* Date            : 6.2.99
//* Last Modified   : 6.2.99
//* Function name	: CCmdLineContextMenu::HandleMenuMsg2
//* Description	    : 
//***************************************
//
STDMETHODIMP CCmdLineContextMenu::HandleMenuMsg2(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult)
{

	return S_OK;
}
    
//***************************************
//* Date            : 6.2.99
//* Last Modified   : 6.2.99
//* Function name	: CCmdLineContextMenu::Initialize
//* Description	    : 
//***************************************
//
STDMETHODIMP CCmdLineContextMenu::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT lpdobj, HKEY hkeyProgID)
{
	HRESULT				hres = E_FAIL;
	FORMATETC			fmte = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	STGMEDIUM			medium;
	_TCHAR				strFilePath[MAX_PATH];

	// No data object
	if (lpdobj == NULL)
		return E_FAIL;

	// Use the given IDataObject to get a list of filenames (CF_HDROP).
	hres = lpdobj->GetData(&fmte, &medium);

	if (FAILED(hres))
		return E_FAIL;

	// Make sure HDROP has only one file.
	if (DragQueryFile((HDROP)medium.hGlobal, (UINT)(-1), NULL, 0) == 1)
	{
		DragQueryFile((HDROP)medium.hGlobal, 0, strFilePath, MAX_PATH);
		m_strFileName = strFilePath;
		hres = S_OK;
	}
	else
		hres = E_FAIL;

	// Release the data.
	ReleaseStgMedium (&medium);

	return hres;
}