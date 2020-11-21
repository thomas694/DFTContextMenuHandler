//--------------------------------------------------------------------------
// DFTContextMenuHandler - a windows shell extension
// 
// This is a shell extension for windows that adds a file tools submenu to 
// the windows explorer context menu for folders and files in general.
//
// Version 1.01
// Copyright (c) 2004-2008  thomas694 (@GH 0CFD61744DA1A21C)
//     first version (for windows 32bit)
// Version 1.02
// Copyright (c) 2015  thomas694
//     changed to 64bit
// Version 1.03
// Copyright (c) 2017  thomas694
//     added option for emptying files
//     fixed menu ids for newer windows version
//
// DFTContextMenuHandler is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//--------------------------------------------------------------------------
// CmdLineContextMenu.cpp : Implementation of CCmdLineContextMenu
#include "stdafx.h"
#include "CmdLineExt.h"
#include "CmdLineContextMenu.h"
#include "CmdLinePromptDlg.h"
#include <windows.h>
#include <sys/utime.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <time.h>
#include <iostream>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CCmdLineContextMenu

//***************************************
//* Date            : 6.2.99
//* Last Modified   : 2.10.2000
//* Function name	: CCmdLineContextMenu::QueryContextMenu
//* Description	    : 
//***************************************
//
STDMETHODIMP CCmdLineContextMenu::QueryContextMenu(HMENU hmenu, 
												   UINT indexMenu, 
												   UINT idCmdFirst, 
												   UINT idCmdLast, 
												   UINT uFlags)
{

	//HRESULT hr;
	
    if(!(CMF_DEFAULTONLY & uFlags))
    {

		//seperator before "Send to"
		UINT seperator_before_sendto = 31485;
	//	32761
	//	32758
	//	32763

		int items = GetMenuItemCount(hmenu);
		UINT data;
		for (int i=0; i<items; i++) {
			data = GetMenuItemID(hmenu, i);
			if (data == 32763) seperator_before_sendto = 32763;
		}

		/*
		int items = GetMenuItemCount(hmenu);
		UINT data;

		for (int i=5; i<30; i++) {
			data = GetMenuItemID(hmenu, items - i -1);
			char ids[20];
			itoa(data, ids, 10);
			char buf[100];
			GetMenuString(hmenu, data, buf, 100, 0);
			char cmp[125];
			strcpy(cmp, ids);
			strcat(cmp, ": ");
			strcat(cmp, buf);
			MessageBox(NULL, cmp, _T("Caption"), MB_OK);
			if (data == 193) { break; }
		}
		*/

		m_idCmdFirst = idCmdFirst;

		// First, create and populate a submenu.
		HMENU hPopup = CreatePopupMenu();
		UINT uID = idCmdFirst + 1;

		InsertMenu ( hPopup, 0, MF_BYPOSITION, uID++, _T("Convert dots 2 spaces") );
		InsertMenu ( hPopup, 1, MF_BYPOSITION, uID++, _T("Convert spaces 2 dots") );
		InsertMenu ( hPopup, 2, MF_BYPOSITION | MF_SEPARATOR, NULL, NULL);
		InsertMenu ( hPopup, 3, MF_BYPOSITION, uID++, _T("Convert underscores 2 spaces") );
		InsertMenu ( hPopup, 4, MF_BYPOSITION, uID++, _T("Convert spaces 2 underscores") );
		InsertMenu ( hPopup, 5, MF_BYPOSITION | MF_SEPARATOR, NULL, NULL);
		InsertMenu ( hPopup, 6, MF_BYPOSITION, uID++, _T("Remove group names (...-name.mp3)") );
		InsertMenu ( hPopup, 7, MF_BYPOSITION, uID++, _T("Rename extension...") );
		InsertMenu ( hPopup, 8, MF_BYPOSITION, uID++, _T("Append extension (.mp -> .mp3)...") );
		InsertMenu ( hPopup, 9, MF_BYPOSITION, uID++, _T("Remove n chars from filename...") );
		InsertMenu ( hPopup,10, MF_BYPOSITION, uID++, _T("Set file date/time...") );
		InsertMenu ( hPopup,11, MF_BYPOSITION, uID++, _T("Append to filename...") );
		InsertMenu ( hPopup,12, MF_BYPOSITION, uID++, _T("Insert before filename...") );
		InsertMenu ( hPopup,13, MF_BYPOSITION, uID++, _T("Empty file(s)...") );
		InsertMenu ( hPopup,14, MF_BYPOSITION | MF_SEPARATOR, NULL, NULL);
		InsertMenu ( hPopup,15, MF_BYPOSITION, uID++, _T("Flatten folders (del empty ones)...") );
		InsertMenu ( hPopup,16, MF_BYPOSITION, uID++, _T("Flatten folders2 (+: folder _ file.ext)...") );
		InsertMenu ( hPopup,17, MF_BYPOSITION, uID++, _T("Delete empty subfolders...") );
		InsertMenu ( hPopup,18, MF_BYPOSITION, uID++, _T("SlideShow...") );

		m_idCmdLast = uID - 1;

		//HMENU hPopup;
		//hPopup = LoadMenu(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MENU1));

		//HBITMAP hIcon;
		//hIcon = LoadBitmap(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1));
		//hIcon = (HBITMAP)LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1));
		//if (hIcon == NULL) { MessageBox(NULL, _T("icon null"), _T("C"), MB_OK); }

		MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
		//mii.cbSize = sizeof(MENUITEMINFO);
		//mii.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_STRING | MIIM_BITMAP;
		mii.fMask = MIIM_ID | MIIM_SUBMENU | 64 | 128; //| MIIM_CHECKMARKS;
		mii.wID = idCmdFirst;
		mii.hSubMenu = hPopup;
		//mii.hbmpItem = hIcon;
		//mii.hbmpChecked = hIcon;
		//mii.hbmpUnchecked = hIcon;
		mii.dwTypeData = _T("My File Tools");
		//mii.cch = strlen("My File Tools");

		BOOL ret;
		ret=InsertMenuItem(hmenu,
					   seperator_before_sendto,
					   false,
					   &mii);
		//if (ret == 0) { MessageBox(NULL, _T("insert menu item failed"), _T("C"), MB_OK); }

		//SetMenuItemBitmaps(hmenu, idCmdFirst, MF_BYCOMMAND, hIcon, hIcon);
		/*
		if ( NULL != m_bitmap.GetSafeHandle() )
        {
			SetMenuItemBitmaps ( hmenu, idCmdFirst, MF_BYPOSITION, (HBITMAP) m_bitmap.GetSafeHandle(), NULL );
        }
		*/

/*
		InsertMenuItem(hmenu,
					   indexMenu+1,
					   true,
					   &mii);
*/
        // TODO: Add error handling to verify HRESULT return values.

//TESTING
/*
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_DATA|MIIM_TYPE|MIIM_ID;
		TCHAR acMenuText[80];
		mii.dwTypeData = acMenuText;
		mii.cch = sizeof(acMenuText);
		ret=GetMenuItemInfo(hmenu, 20, true, &mii);
		cout << "Name: " << acMenuText << endl;
		MessageBox(NULL, acMenuText, NULL, MB_OK);

		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_DATA|MIIM_TYPE|MIIM_ID;
		mii.dwTypeData = acMenuText;
		mii.cch = sizeof(acMenuText);
		ret=GetMenuItemInfo(hmenu, 21, true, &mii);
		cout << "Name: " << acMenuText << endl;
//		_stprintf( acMenuText, TEXT( "%d" ), mii.wID ) ;
		MessageBox(NULL, acMenuText, NULL, MB_OK);

		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_DATA|MIIM_TYPE|MIIM_ID;
		mii.dwTypeData = acMenuText;
		mii.cch = sizeof(acMenuText);
		ret=GetMenuItemInfo(hmenu, 22, true, &mii);
		cout << "Name: " << acMenuText << endl;
		MessageBox(NULL, acMenuText, NULL, MB_OK);

		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_DATA|MIIM_TYPE|MIIM_ID;
		mii.dwTypeData = acMenuText;
		mii.cch = sizeof(acMenuText);
		ret=GetMenuItemInfo(hmenu, 23, true, &mii);
		cout << "Name: " << acMenuText << endl;
		MessageBox(NULL, acMenuText, NULL, MB_OK);
*/

        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 17);
    }

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));

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


	//char buffer[20];
	//ltoa( LOWORD(lpici->lpVerb), buffer, 10 );
	//MessageBox(NULL, _T("Hello"), _T("Caption"), MB_OK);

	// If lpVerb really points to a string, ignore this
	// call and bail out.
	if ( 0 != HIWORD(lpici->lpVerb) )
		return E_INVALIDARG;

	if (LOWORD(lpici->lpVerb) < 1 || LOWORD(lpici->lpVerb) > (m_idCmdLast-m_idCmdFirst))
		return E_INVALIDARG;

    switch (LOWORD(lpici->lpVerb)) {
	case 1:
		ConvertDots2Spaces();
		break;
	case 2:
		ConvertSpaces2Dots();
		break;
	case 3:
		ConvertUnderscores2Spaces();
		break;
	case 4:
		ConvertSpaces2Underscores();
		break;
	case 5:
		RemoveGroupNames();
		break;
	case 6:
		RenameExtension();
		break;
	case 7:
		AppendExtension();
		break;
	case 8:
		RemoveFromFilename();
		break;
	case 9:
		SetDateTime();
		break;
	case 10:
		AppendToFilename();
		break;
	case 11:
		InsertBeforeFilename();
		break;
	case 12:
		EmptyFiles();
		break;
	case 13:
		FlattenTree();
		break;
	case 14:
		FlattenTree2();
		break;
	case 15:
		DeleteEmptySubfolders();
		break;
	case 16:
		SlideShow();
		break;

	/*
	case ID_MENU_ITEM:
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
	*/
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
STDMETHODIMP CCmdLineContextMenu::GetCommandString(UINT_PTR idCmd, 
												   UINT uType, 
												   UINT *pwReserved, 
												   LPSTR pszName, 
												   UINT cchMax)
{
	HRESULT hr = E_INVALIDARG;

	if (idCmd < 0 || idCmd > m_idCmdLast-m_idCmdFirst)
		return hr;

	switch (uType) {

		case GCS_HELPTEXTW:
			switch (idCmd) {
				case 0:
					wcsncpy((LPWSTR)pszName, OLESTR("Some useful file tools"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 1:
					wcsncpy((LPWSTR)pszName, OLESTR("Convert dots 2 spaces"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 2:
					wcsncpy((LPWSTR)pszName, OLESTR("Convert spaces 2 dots"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 3:
					wcsncpy((LPWSTR)pszName, OLESTR("Convert underscores 2 spaces"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 4:
					wcsncpy((LPWSTR)pszName, OLESTR("Convert spaces 2 underscores"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 5:
					wcsncpy((LPWSTR)pszName, OLESTR("Remove group names (...-name.mp3)"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 6:
					wcsncpy((LPWSTR)pszName, OLESTR("Rename extension"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 7:
					wcsncpy((LPWSTR)pszName, OLESTR("Append extension (.mp -> .mp3)"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 8:
					wcsncpy((LPWSTR)pszName, OLESTR("Remove n chars from filename"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 9:
					wcsncpy((LPWSTR)pszName, OLESTR("Set file date/time"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 10:
					wcsncpy((LPWSTR)pszName, OLESTR("Append to filename"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 11:
					wcsncpy((LPWSTR)pszName, OLESTR("Insert before filename"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 12:
					wcsncpy((LPWSTR)pszName, OLESTR("Flatten folders (del empty ones)"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 13:
					wcsncpy((LPWSTR)pszName, OLESTR("Empty file(s) (0 KB)"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 14:
					wcsncpy((LPWSTR)pszName, OLESTR("Flatten folders2 (+: folder _ file.ext)"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 15:
					wcsncpy((LPWSTR)pszName, OLESTR("Delete empty subfolders"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 16:
					wcsncpy((LPWSTR)pszName, OLESTR("SlideShow"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;

			}
			break;

		case GCS_HELPTEXTA:
			switch (idCmd) {
				case 0:
					strncpy((LPSTR)pszName, "Some useful file tools", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 1:
					strncpy((LPSTR)pszName, "Convert dots 2 spaces", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 2:
					strncpy((LPSTR)pszName, "Convert spaces 2 dots", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 3:
					strncpy((LPSTR)pszName, "Convert underscores 2 spaces", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 4:
					strncpy((LPSTR)pszName, "Convert spaces 2 underscores", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 5:
					strncpy((LPSTR)pszName, "Remove group names (...-name.mp3)", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 6:
					strncpy((LPSTR)pszName, "Rename extension", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 7:
					strncpy((LPSTR)pszName, "Append extension (.mp -> .mp3)", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 8:
					strncpy((LPSTR)pszName, "Remove n chars from filename", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 9:
					strncpy((LPSTR)pszName, "Set date/time", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 10:
					strncpy((LPSTR)pszName, "Append to filename", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 11:
					strncpy((LPSTR)pszName, "Insert before filename", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 12:
					strncpy((LPSTR)pszName, "Flatten folders (del empty ones)", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 13:
					strncpy((LPSTR)pszName, "Empty file(s) (0 KB)", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 14:
					strncpy((LPSTR)pszName, "Flatten folders2 (+: folder _ file.ext)", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 15:
					strncpy((LPSTR)pszName, "Delete empty subfolders", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 16:
					strncpy((LPSTR)pszName, "SlideShow", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
			}
			break;

		case GCS_VERBA:
			if (idCmd == ID_MENU_ITEM) {
				strncpy((LPSTR)pszName, "Mymenu", cchMax);
				((LPSTR)pszName)[cchMax - 1] = '\0';
				hr = S_OK;
			}
			break;

		case GCS_VERBW:
			if (idCmd == ID_MENU_ITEM) {
				wcsncpy((LPWSTR)pszName, OLESTR("Mymenu"), cchMax);
				((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
				hr = S_OK;
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


	int lFiles;
	lFiles = DragQueryFile((HDROP)medium.hGlobal, (UINT)(-1), NULL, 0);

	m_strFilenames.resize(lFiles);

	int i;
	for (i=0; i<lFiles; i++) {
		DragQueryFile((HDROP)medium.hGlobal, i, strFilePath, MAX_PATH);
		m_strFilenames[i] = strFilePath;
	}

	hres = S_OK;

/*
	// Make sure HDROP has only one file.
	if (DragQueryFile((HDROP)medium.hGlobal, (UINT)(-1), NULL, 0) == 1)
	{
		DragQueryFile((HDROP)medium.hGlobal, 0, strFilePath, MAX_PATH);
		m_strFileName = strFilePath;
		hres = S_OK;
	}
	else
		hres = E_FAIL;
*/

	// Release the data.
	ReleaseStgMedium (&medium);

	// load a bitmap for our menu item
	//COleDataObject dataobj;
	
	//m_bitmap.LoadBitmap ( IDB_LINKBITMAP );
    //dataobj.Attach ( pDataObj, FALSE );

	// get the directory where the items were dropped
	if ( !SHGetPathFromIDList ( pidlFolder, m_szFolderDroppedIn ))
    {
		//return E_FAIL;
    }

	return hres;
}

int CCmdLineContextMenu::ConvertDots2Spaces() {

	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[_MAX_DIR];
		TCHAR sName[_MAX_FNAME];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);

		BOOL bIsDir;
		bIsDir = PathIsDirectory(m_strFilenames[i].data());
		if (bIsDir) {
			_tcscat(sName, sExt);
			_tccpy(sExt, _T(""));
		}

		TCHAR sNewName[_MAX_FNAME] = _T("");
		TCHAR *pPos;
		pPos = sName;
		int n;
		for (n=1; n<=_tcslen(sName); n++) {
			if (*pPos == '.') {
				if (*(pPos+1) == '.') {
					n++;
					pPos++;
					_tcscat(sNewName, _T("."));
				} else {
					_tcscat(sNewName, _T(" "));
				}
			} else {
				_tcsncat(sNewName, pPos, 1);
			}
			pPos++;
		}
		//_tcsncat(sNewName, pPos, 1);
		_tcscpy(sName, sNewName);

		/*
		//replace dots with spaces
		TCHAR *pDest;
		do {
			pDest = _tcschr(sName, (TCHAR)'.');
			if (pDest != NULL) {
				*pDest = ' ';
			}
		} while (pDest != NULL);
		*/

		//concat components
		TCHAR sPath[_MAX_PATH];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		_trename(m_strFilenames[i].data(), sPath);
	}

	return 1;
}



int CCmdLineContextMenu::ConvertSpaces2Dots() {

	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[_MAX_DIR];
		TCHAR sName[_MAX_FNAME];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		//encode real dots
		TCHAR sNewName[_MAX_FNAME] = _T("");
		TCHAR *pPos;
		pPos = sName;
		int n;
		for (n=1; n<=_tcslen(sName); n++) {
			if (*pPos == '.') {
				_tcscat(sNewName, _T(".."));
			} else {
				_tcsncat(sNewName, pPos, 1);
			}
			pPos++;
		}
		//_tcsncat(sNewName, pPos, 1);
		_tcscpy(sName, sNewName);

		//replace spaces with dots
		TCHAR *pDest;
		do {
			pDest = _tcschr(sName, (TCHAR)' ');
			if (pDest != NULL) {
				*pDest = '.';
			}
		} while (pDest != NULL);

		//concat components
		TCHAR sPath[_MAX_PATH];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		_trename(m_strFilenames[i].data(), sPath);
	}

	return 1;
}

int CCmdLineContextMenu::ConvertSpaces2Underscores() {

	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[_MAX_DIR];
		TCHAR sName[_MAX_FNAME];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		//encode real underscores
		TCHAR sNewName[_MAX_FNAME] = _T("");
		TCHAR *pPos;
		pPos = sName;
		int n;
		for (n=1; n<=_tcslen(sName); n++) {
			if (*pPos == '_') {
				_tcscat(sNewName, _T("__"));
			} else {
				_tcsncat(sNewName, pPos, 1);
			}
			pPos++;
		}
		//_tcsncat(sNewName, pPos, 1);
		_tcscpy(sName, sNewName);

		//replace spaces with underscores
		TCHAR *pDest;
		do {
			pDest = _tcschr(sName, (TCHAR)' ');
			if (pDest != NULL) {
				*pDest = '_';
			}
		} while (pDest != NULL);

		//concat components
		TCHAR sPath[_MAX_PATH];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		_trename(m_strFilenames[i].data(), sPath);
	}

	return 1;
}

int CCmdLineContextMenu::ConvertUnderscores2Spaces() {

	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[_MAX_DIR];
		TCHAR sName[_MAX_FNAME];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		TCHAR sNewName[_MAX_FNAME] = _T("");
		TCHAR *pPos;
		pPos = sName;
		int n;
		for (n=1; n<=_tcslen(sName); n++) {
			if (*pPos == '_') {
				if (*(pPos+1) == '_') {
					n++;
					pPos++;
					_tcscat(sNewName, _T("_"));
				} else {
					_tcscat(sNewName, _T(" "));
				}
			} else {
				_tcsncat(sNewName, pPos, 1);
			}
			pPos++;
		}
		//_tcsncat(sNewName, pPos, 1);
		_tcscpy(sName, sNewName);

		/*
		//replace underscores with spaces
		TCHAR *pDest;
		do {
			pDest = _tcschr(sName, (TCHAR)'_');
			if (pDest != NULL) {
				*pDest = ' ';
			}
		} while (pDest != NULL);
		*/

		//concat components
		TCHAR sPath[_MAX_PATH];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		_trename(m_strFilenames[i].data(), sPath);
	}

	return 1;
}

int CCmdLineContextMenu::RemoveGroupNames() {

	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[_MAX_DIR];
		TCHAR sName[_MAX_FNAME];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		//replace something
		TCHAR *pDest;
		pDest = _tcsrchr(sName, (TCHAR)'-');
		if (pDest != NULL) {
			*pDest = '\0';
		}

		//concat components
		TCHAR sPath[_MAX_PATH];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		_trename(m_strFilenames[i].data(), sPath);

		//MessageBox(NULL, sPath, _T("RemoveGroupNames"), MB_OK);
	}

	return 1;
}

int CCmdLineContextMenu::RenameExtension() {

	bool bAsked = false;
	TCHAR sNewExt[_MAX_EXT];
	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[_MAX_DIR];
		TCHAR sName[_MAX_FNAME];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		if (!bAsked) {
			bAsked = true;
			//get new extension
			string abc;
			abc.assign(sExt);
			string sTitle;
			sTitle.assign(_T("Enter new extension"));
			CCmdLinePromptDlg		dlg(abc, sTitle);
			if (dlg.DoModal() == IDOK) {
				/*
				// point is added automatically
				if (dlg.strExtension[0] != '.') {
					MessageBox(NULL, _T("No point"), _T("RenameExtension"), MB_OK);
				}
				*/
				_tcscpy(sNewExt, dlg.strExtension.data());
				//MessageBox(NULL, sExt, _T("RenameExtension"), MB_OK);
			} else {
				return 0;
			}
		}
		_tcscpy(sExt, sNewExt);

		//concat components
		TCHAR sPath[_MAX_PATH];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		_trename(m_strFilenames[i].data(), sPath);

		//MessageBox(NULL, sPath, _T("RemoveGroupNames"), MB_OK);
	}

	return 1;
}

int CCmdLineContextMenu::AppendExtension() {

	bool bAsked = false;
	TCHAR sNewExt[_MAX_EXT];
	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[_MAX_DIR];
		TCHAR sName[_MAX_FNAME];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		if (!bAsked) {
			bAsked = true;
			//get new extension
			string abc;
			abc.assign(sExt);
			string sTitle;
			sTitle.assign(_T("Enter extension to append"));
			CCmdLinePromptDlg		dlg(abc, sTitle);
			if (dlg.DoModal() == IDOK) {
				/*
				// point is added automatically
				if (dlg.strExtension[0] != '.') {
					MessageBox(NULL, _T("No point"), _T("RenameExtension"), MB_OK);
				}
				*/
				_tcscpy(sNewExt, dlg.strExtension.data());
				//MessageBox(NULL, sExt, _T("RenameExtension"), MB_OK);
			} else {
				return 0;
			}
		}
		if (_tcslen(sExt) == 0) {
			_tcscat(sName, sNewExt);
		} else {
			_tcscat(sExt, sNewExt);
		}

		//concat components
		TCHAR sPath[_MAX_PATH];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		_trename(m_strFilenames[i].data(), sPath);

		//MessageBox(NULL, sPath, _T("RemoveGroupNames"), MB_OK);
	}

	return 1;
}

int CCmdLineContextMenu::RemoveFromFilename() {

	bool bAsked = false;
	TCHAR sNewName[_MAX_FNAME];
	TCHAR * pName = NULL;
	long lHowManyChars = 0;
	long lHowManyCharacters = 0;
	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[_MAX_DIR];
		TCHAR sName[_MAX_FNAME];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		if (!bAsked) {
			bAsked = true;
			//get new extension
			string abc;
			abc.assign(_T("0"));
			string sTitle;
			sTitle.assign(_T("Remove n chars (-n from start)"));
			CCmdLinePromptDlg		dlg(abc, sTitle);
			if (dlg.DoModal() == IDOK) {
				/*
				// point is added automatically
				if (dlg.strExtension[0] != '.') {
					MessageBox(NULL, _T("No point"), _T("RenameExtension"), MB_OK);
				}
				*/
				lHowManyCharacters = _ttol(dlg.strExtension.data());
				//MessageBox(NULL, sExt, _T("RenameExtension"), MB_OK);
			} else {
				return 0;
			}
		}
		lHowManyChars = lHowManyCharacters;
		if (_tcslen(sName) > abs(lHowManyChars)) {
			if (lHowManyChars < 0) {
				lHowManyChars = abs(lHowManyChars);
				pName = sName;
				pName = _tcsninc(pName, lHowManyChars);
				_tcsncpy(sNewName, pName, _tcslen(sName) - lHowManyChars);
				sNewName[_tcslen(sName) - lHowManyChars] = '\0';
			} else {
				_tcsncpy(sNewName, sName, _tcslen(sName) - lHowManyChars);
				sNewName[_tcslen(sName) - lHowManyChars] = '\0';
			}

			//concat components
			TCHAR sPath[_MAX_PATH];
			_tmakepath(sPath, sDrive, sDir, sNewName, sExt);

			//rename file
			_trename(m_strFilenames[i].data(), sPath);
		}

		//MessageBox(NULL, sPath, _T("RemoveGroupNames"), MB_OK);
	}

	return 1;
}

int CCmdLineContextMenu::SetDateTime() {

	//get new date/time
	tm *tm;

	struct _stat status;
	int nRet = _tstat(m_strFilenames[0].data(), &status);
	time_t mtime = status.st_mtime;
	tm = localtime(&mtime);
	
	TCHAR tmpbuf[128];
	_tcsftime(tmpbuf, 128, _T("%d.%m.%Y %H:%M:%S"), tm);
	string abc;
	abc.assign(tmpbuf);
	string sTitle;
	sTitle.assign(_T("Set date/time"));

	CCmdLinePromptDlg		dlg(abc, sTitle);
	if (dlg.DoModal() == IDOK) {
		long lRet;
		lRet = _stscanf(dlg.strExtension.data(), _T("%2u.%2u.%4u %2u:%2u:%2u"), 
												&tm->tm_mday, &tm->tm_mon, &tm->tm_year, 
												&tm->tm_hour, &tm->tm_min, &tm->tm_sec);
		if (lRet != 6) {
			return 0;
		}
		tm->tm_mon -= 1;
		tm->tm_year -= 1900;
	} else {
		return 0;
	}

	time_t time;
	time = mktime(tm);
	
	_utimbuf times;
	times.actime = time;
	times.modtime = time;

	//set all files to new date/time
	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {
		_tutime(m_strFilenames[i].data(), &times);
	}

	return 1;
}

int CCmdLineContextMenu::InsertBeforeFilename()
{
	bool bAsked = false;
	TCHAR s2Insert[_MAX_FNAME];
	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[_MAX_DIR];
		TCHAR sName[_MAX_FNAME];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		if (!bAsked) {
			bAsked = true;
			//get string to append
			string abc;
			abc.assign(_T(""));
			string sTitle;
			sTitle.assign(_T("Enter string to insert in front"));
			CCmdLinePromptDlg		dlg(abc, sTitle);
			if (dlg.DoModal() == IDOK) {
				_tcscpy(s2Insert, dlg.strExtension.data());
				//MessageBox(NULL, sExt, _T("RenameExtension"), MB_OK);
			} else {
				return 0;
			}
		}
		TCHAR sNewName[_MAX_FNAME];
		_tcscpy(sNewName, s2Insert);
		_tcscat(sNewName, sName);

		//concat components
		TCHAR sPath[_MAX_PATH];
		_tmakepath(sPath, sDrive, sDir, sNewName, sExt);

		//rename file
		_trename(m_strFilenames[i].data(), sPath);

		//MessageBox(NULL, sPath, _T("RemoveGroupNames"), MB_OK);
	}

	return 1;
}

int CCmdLineContextMenu::AppendToFilename()
{
	bool bAsked = false;
	TCHAR s2Append[_MAX_FNAME];
	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[_MAX_DIR];
		TCHAR sName[_MAX_FNAME];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		if (!bAsked) {
			bAsked = true;
			//get string to append
			string abc;
			abc.assign(_T(" (1)"));
			string sTitle;
			sTitle.assign(_T("Enter string to append"));
			CCmdLinePromptDlg		dlg(abc, sTitle);
			if (dlg.DoModal() == IDOK) {
				_tcscpy(s2Append, dlg.strExtension.data());
				//MessageBox(NULL, sExt, _T("RenameExtension"), MB_OK);
			} else {
				return 0;
			}
		}
		_tcscat(sName, s2Append);

		//concat components
		TCHAR sPath[_MAX_PATH];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		_trename(m_strFilenames[i].data(), sPath);

		//MessageBox(NULL, sPath, _T("RemoveGroupNames"), MB_OK);
	}

	return 1;
}

int CCmdLineContextMenu::FlattenTree()
{

	bool bAsked = false;
	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	if (lFiles > 1) {

		MessageBox(NULL, _T("select only the base folder"), _T("Info"), MB_OK);
		return 0;

	} else {

		string param;
		param.append(m_strFilenames[0].data());

		struct _stat buffer;
		int nResult;
		nResult = _tstat(param.c_str(), &buffer);

		if ((buffer.st_mode & _S_IFDIR) != _S_IFDIR) {

			MessageBox(NULL, _T("no folder selected"), _T("Info"), MB_OK);
			return 0;

		}
		
		for (i=0; i<lFiles; i++) {
		
			//split into components
			TCHAR sDrive[_MAX_DRIVE];
			TCHAR sDir[_MAX_DIR];
			TCHAR sName[_MAX_FNAME];
			TCHAR sExt[_MAX_EXT];
			_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);

			if (!bAsked) {
				bAsked = true;
				//get string to append
				string abc;
				abc.assign(_T("no"));
				string sTitle;
				sTitle.assign(_T("Flatten folders and delete empty ones?"));
				CCmdLinePromptDlg dlg(abc, sTitle);
				if (dlg.DoModal() == IDOK &&  _tcscmp(dlg.strExtension.data(), _T("yes")) == 0)
				{
				} else {
					//cancel command
					return 0;
				}
			}

			/*
			string param;
			for (i=0; i<lFiles; i++) {
				param.append(m_strFilenames[i].data());
				param.append("|");
			}
			*/

			
			TCHAR szAppPath[MAX_PATH] = _T("");
			string strAppDirectory;
			GetModuleFileName(_Module.GetModuleInstance(), szAppPath, MAX_PATH);
			// Extract directory
			strAppDirectory.assign(szAppPath);
			strAppDirectory = strAppDirectory.substr(0, strAppDirectory.rfind(_T("\\")));
			strAppDirectory = strAppDirectory.append(_T("\\..\\..\\flatten\\bin\\Debug\\flatten.exe"));
			ShellExecute(NULL, NULL, strAppDirectory.c_str(), param.c_str(), NULL, SW_SHOWNORMAL);

		}

		return 1;
	}
}

int CCmdLineContextMenu::FlattenTree2()
{

	bool bAsked = false;
	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	if (lFiles > 1) {

		MessageBox(NULL, _T("select only the base folder"), _T("Info"), MB_OK);
		return 0;

	} else {

		string param;
		param.append(m_strFilenames[0].data());

		struct _stat buffer;
		int nResult;
		nResult = _tstat(param.c_str(), &buffer);

		if ((buffer.st_mode & _S_IFDIR) != _S_IFDIR) {

			MessageBox(NULL, _T("no folder selected"), _T("Info"), MB_OK);
			return 0;

		}
		
		for (i=0; i<lFiles; i++) {
		
			//split into components
			TCHAR sDrive[_MAX_DRIVE];
			TCHAR sDir[_MAX_DIR];
			TCHAR sName[_MAX_FNAME];
			TCHAR sExt[_MAX_EXT];
			_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);

			if (!bAsked) {
				bAsked = true;
				//get string to append
				string abc;
				abc.assign(_T("no"));
				string sTitle;
				sTitle.assign(_T("Flatten folders (Folder _ file.ext) and delete empty ones?"));
				CCmdLinePromptDlg dlg(abc, sTitle);
				if (dlg.DoModal() == IDOK &&  _tcscmp(dlg.strExtension.data(), _T("yes")) == 0)
				{
				} else {
					//cancel command
					return 0;
				}
			}

			/*
			string param;
			for (i=0; i<lFiles; i++) {
				param.append(m_strFilenames[i].data());
				param.append("|");
			}
			*/

			
			TCHAR szAppPath[MAX_PATH] = _T("");
			string strAppDirectory;
			GetModuleFileName(_Module.GetModuleInstance(), szAppPath, MAX_PATH);
			// Extract directory
			strAppDirectory.assign(szAppPath);
			strAppDirectory = strAppDirectory.substr(0, strAppDirectory.rfind(_T("\\")));
			strAppDirectory = strAppDirectory.append(_T("\\..\\..\\flatten2\\bin\\Debug\\flatten2.exe"));
			ShellExecute(NULL, NULL, strAppDirectory.c_str(), param.c_str(), NULL, SW_SHOWNORMAL);

		}

		return 1;
	}
}

int CCmdLineContextMenu::DeleteEmptySubfolders()
{

	bool bAsked = false;
	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	if (lFiles > 1) {

		MessageBox(NULL, _T("select only the base folder"), _T("Info"), MB_OK);
		return 0;

	} else {

		string param;
		param.append(m_strFilenames[0].data());

		struct _stat buffer;
		int nResult;
		nResult = _tstat(param.c_str(), &buffer);

		if ((buffer.st_mode & _S_IFDIR) != _S_IFDIR) {

			MessageBox(NULL, _T("no folder selected"), _T("Info"), MB_OK);
			return 0;

		}
		
		for (i=0; i<lFiles; i++) {
		
			//split into components
			TCHAR sDrive[_MAX_DRIVE];
			TCHAR sDir[_MAX_DIR];
			TCHAR sName[_MAX_FNAME];
			TCHAR sExt[_MAX_EXT];
			_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);

			if (!bAsked) {
				bAsked = true;
				//get string to append
				string abc;
				abc.assign(_T("no"));
				string sTitle;
				sTitle.assign(_T("Delete empty subfolders?"));
				CCmdLinePromptDlg dlg(abc, sTitle);
				if (dlg.DoModal() == IDOK &&  _tcscmp(dlg.strExtension.data(), _T("yes")) == 0)
				{
				} else {
					//cancel command
					return 0;
				}
			}

			/*
			string param;
			for (i=0; i<lFiles; i++) {
				param.append(m_strFilenames[i].data());
				param.append("|");
			}
			*/

			
			TCHAR szAppPath[MAX_PATH] = _T("");
			string strAppDirectory;
			GetModuleFileName(_Module.GetModuleInstance(), szAppPath, MAX_PATH);
			// Extract directory
			strAppDirectory.assign(szAppPath);
			strAppDirectory = strAppDirectory.substr(0, strAppDirectory.rfind(_T("\\")));
			strAppDirectory = strAppDirectory.append(_T("\\..\\..\\DeleteEmptyFolders\\bin\\Debug\\DeleteEmptyFolders.exe"));
			ShellExecute(NULL, NULL, strAppDirectory.c_str(), param.c_str(), NULL, SW_SHOWNORMAL);

		}

		return 1;
	}
}

int CCmdLineContextMenu::SlideShow()
{
	TCHAR szAppPath[MAX_PATH] = _T("");
	string strAppDirectory;
	GetModuleFileName(_Module.GetModuleInstance(), szAppPath, MAX_PATH);
	// Extract directory
	strAppDirectory.assign(szAppPath);
	strAppDirectory = strAppDirectory.substr(0, strAppDirectory.rfind(_T("\\")));
	strAppDirectory = strAppDirectory.append(_T("\\..\\..\\SlideShow\\bin\\SlideShow.exe"));

	string param;
	int lFiles;
	lFiles = m_strFilenames.size();

	if (lFiles == 0)
		return 0;

	int i;
	for (i=0; i<lFiles; i++) {
		param.append(_T("|"));
		param.append(m_strFilenames[i].data());
	}
	//remove first pipe
	param.erase(0, 1);
	//enclose in quotation marks
	//param = "\"" + param + "\"";
	param.append(_T("\""));
	param.insert(0, _T("\""));

	//string param;
	//param.append("\"");
	//param.append(m_strFilenames[0].data());
	//param.append("\"");
	//MessageBox(0, m_strFilenames[0].data(), "", MB_OK);

	ShellExecute(NULL, NULL, strAppDirectory.c_str(), param.c_str(), NULL, SW_SHOWNORMAL);

	return 1;
}

int CCmdLineContextMenu::EmptyFiles()
{
	bool bAsked = false;
	TCHAR s2Append[_MAX_FNAME];
	int lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		if (!bAsked) {
			bAsked = true;
			//get string to append
			string abc;
			abc.assign(_T("no"));
			string sTitle;
			sTitle.assign(_T("Empty file(s)?"));
			CCmdLinePromptDlg dlg(abc, sTitle);
			if (dlg.DoModal() == IDOK &&  _tcscmp(dlg.strExtension.data(), _T("yes")) == 0)
			{
			} else {
				//cancel command
				return 0;
			}
		}

		//empty file
		HANDLE h = CreateFile(m_strFilenames[i].data(), GENERIC_WRITE, 0, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(h);

		//MessageBox(NULL, sPath, _T("RemoveGroupNames"), MB_OK);
	}

	return 1;
}
