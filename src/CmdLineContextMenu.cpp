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
// Version 1.0.4  (c) 2020  thomas694
//     release version
// Version 1.5.0  (c) 2022  thomas694
//     copied logic code into a VS2019 ATL template,
//     using CAtlDllModuleT instead of CComModule
// Version 1.6.0  (c) 2022  thomas694
//     added long path support
//     added option for editing filenames (long paths)
// Version 1.7.0  (c) 2022  thomas694
//     added direct implementations for extra functionality except slideshow
// Version 1.7.1  (c) 2022  thomas694
//     fixed SetDateTime for folders
// Version 1.8.0  (c) 2023  thomas694
//     added drop handler for copy/move items to target folder
// Version 1.8.1  (c) 2023  thomas694
//     fixed deleting empty subfolders
// Version 1.8.2  (c) 2024  thomas694
//     fixed copy/move files progress dialog
// Version 1.8.3  (c) 2024  thomas694
//     fixed problem moving items across volumes
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
#include "pch.h"
#include "CmdLineExt_i.h"
#include "CmdLineContextMenu.h"
#include "CmdLinePromptDlg.h"
#include "dllmain.h"
#include <windows.h>
#include <sys/utime.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <time.h>
#include <iostream>
#include <ShObjIdl_core.h>
#include <future>

#pragma warning(disable : 4996)

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CCmdLineContextMenu

CCmdLineContextMenu::CCmdLineContextMenu()
{
	_tccpy(m_szFolderDroppedIn, _T(""));
	m_idCmdFirst = 0;
	m_idCmdLast = 0;
}

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

		if (_tcslen(m_szFolderDroppedIn) == 0) {

			InsertMenu(hPopup, 0, MF_BYPOSITION, uID++, _T("Convert dots to spaces"));
			InsertMenu(hPopup, 1, MF_BYPOSITION, uID++, _T("Convert spaces to dots"));
			InsertMenu(hPopup, 2, MF_BYPOSITION | MF_SEPARATOR, NULL, NULL);
			InsertMenu(hPopup, 3, MF_BYPOSITION, uID++, _T("Convert underscores to spaces"));
			InsertMenu(hPopup, 4, MF_BYPOSITION, uID++, _T("Convert spaces to underscores"));
			InsertMenu(hPopup, 5, MF_BYPOSITION | MF_SEPARATOR, NULL, NULL);
			InsertMenu(hPopup, 6, MF_BYPOSITION, uID++, _T("Remove group names (...-name.mp3)"));
			InsertMenu(hPopup, 7, MF_BYPOSITION, uID++, _T("Rename extension..."));
			InsertMenu(hPopup, 8, MF_BYPOSITION, uID++, _T("Append extension (.mp -> .mp3)..."));
			InsertMenu(hPopup, 9, MF_BYPOSITION, uID++, _T("Remove n characters from filename..."));
			InsertMenu(hPopup, 10, MF_BYPOSITION, uID++, _T("Set file date/time..."));
			InsertMenu(hPopup, 11, MF_BYPOSITION, uID++, _T("Append to filename..."));
			InsertMenu(hPopup, 12, MF_BYPOSITION, uID++, _T("Insert before filename..."));
			InsertMenu(hPopup, 13, MF_BYPOSITION, uID++, _T("Edit filename..."));
			InsertMenu(hPopup, 14, MF_BYPOSITION, uID++, _T("Empty file(s)..."));
			InsertMenu(hPopup, 15, MF_BYPOSITION | MF_SEPARATOR, NULL, NULL);
			InsertMenu(hPopup, 16, MF_BYPOSITION, uID++, _T("Flatten folders (del empty ones)..."));
			InsertMenu(hPopup, 17, MF_BYPOSITION, uID++, _T("Flatten folders2 (+: folder _ file.ext)..."));
			InsertMenu(hPopup, 18, MF_BYPOSITION, uID++, _T("Delete empty subfolders..."));
			InsertMenu(hPopup, 19, MF_BYPOSITION, uID++, _T("SlideShow..."));
		}
		else
		{
			uID += 30;
			InsertMenu(hPopup, 0, MF_BYPOSITION, uID++, _T("Copy items here"));
			InsertMenu(hPopup, 1, MF_BYPOSITION, uID++, _T("Move items here"));
		}

		m_idCmdLast = uID - 1;

		//HMENU hPopup;
		//hPopup = LoadMenu(_AtlModule.GetResourceInstance(), MAKEINTRESOURCE(IDR_MENU1));

		//HBITMAP hIcon;
		//hIcon = LoadBitmap(_AtlModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1));
		//hIcon = (HBITMAP)LoadIcon(_AtlModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1));
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

        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, m_idCmdLast - m_idCmdFirst + 1);
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

	if (LOWORD(lpici->lpVerb) < 1 || LOWORD(lpici->lpVerb) > (m_idCmdLast - m_idCmdFirst))
		return E_INVALIDARG;

	UINT what = LOWORD(lpici->lpVerb);

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
		EditFilename();
		break;
	case 13:
		EmptyFiles();
		break;
	case 14:
		FlattenTree();
		break;
	case 15:
		FlattenTree2();
		break;
	case 16:
		DeleteEmptySubfolders();
		break;
	case 17:
		SlideShow();
		break;

	case 31:
		StartCopyFilesHere();
		break;
	case 32:
		StartMoveFilesHere();
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

	if (idCmd < 0 || idCmd > m_idCmdLast - m_idCmdFirst)
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
					wcsncpy((LPWSTR)pszName, OLESTR("Convert dots to spaces"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 2:
					wcsncpy((LPWSTR)pszName, OLESTR("Convert spaces to dots"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 3:
					wcsncpy((LPWSTR)pszName, OLESTR("Convert underscores to spaces"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 4:
					wcsncpy((LPWSTR)pszName, OLESTR("Convert spaces to underscores"), cchMax);
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
					wcsncpy((LPWSTR)pszName, OLESTR("Remove n characters from filename"), cchMax);
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
					wcsncpy((LPWSTR)pszName, OLESTR("Edit filename"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 13:
					wcsncpy((LPWSTR)pszName, OLESTR("Empty file(s) (0 KB)"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 14:
					wcsncpy((LPWSTR)pszName, OLESTR("Flatten folders (del empty ones)"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 15:
					wcsncpy((LPWSTR)pszName, OLESTR("Flatten folders2 (+: folder _ file.ext)"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 16:
					wcsncpy((LPWSTR)pszName, OLESTR("Delete empty subfolders"), cchMax);
					((LPWSTR)pszName)[cchMax - 1] = OLESTR('\0');
					hr = S_OK;
					break;
				case 17:
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
					strncpy((LPSTR)pszName, "Convert dots to spaces", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 2:
					strncpy((LPSTR)pszName, "Convert spaces to dots", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 3:
					strncpy((LPSTR)pszName, "Convert underscores to spaces", cchMax);
					((LPSTR)pszName)[cchMax - 1] = '\0';
					hr = S_OK;
					break;
				case 4:
					strncpy((LPSTR)pszName, "Convert spaces to underscores", cchMax);
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
					strncpy((LPSTR)pszName, "Remove n characters from filename", cchMax);
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
//* Last Modified   : 20.9.22
//* Function name	: CCmdLineContextMenu::Initialize
//* Description	    : 
//***************************************
//
STDMETHODIMP CCmdLineContextMenu::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT lpdobj, HKEY hkeyProgID)
{
	HRESULT	hres = E_FAIL;
	_TCHAR	strFilePath[MAX_PATH_EX];

	// No data object
	if (lpdobj == NULL)
		return E_FAIL;

	IShellItemArray* items;
	HRESULT hr = SHCreateShellItemArrayFromDataObject(lpdobj, IID_IShellItemArray, (void**)&items);

	if (FAILED(hr))
		return E_FAIL;

	DWORD count_sia, n;
	hr = items->GetCount(&count_sia);

	if (FAILED(hr))
		return E_FAIL;

	m_strFilenames.resize(count_sia);

	int i;
	LPWSTR* filePath = (LPWSTR*)&strFilePath;
	for (i = 0; i < count_sia; i++) {
		IShellItem* psi;
		hr = items->GetItemAt(i, &psi);
		if (FAILED(hr))
			return E_FAIL;

		hr = psi->GetDisplayName(SIGDN_FILESYSPATH, filePath);
		if (FAILED(hr))
			return E_FAIL;

		m_strFilenames[i] = *filePath;
	}

	hres = S_OK;

	// load a bitmap for our menu item
	//COleDataObject dataobj;
	
	//m_bitmap.LoadBitmap ( IDB_LINKBITMAP );
    //dataobj.Attach ( pDataObj, FALSE );

	// get the directory where the items were dropped
	if (!SHGetPathFromIDListEx(pidlFolder, m_szFolderDroppedIn, MAX_PATH_EX, 0))
	{
		//return E_FAIL;
	}

	return hres;
}

int CCmdLineContextMenu::ConvertDots2Spaces() {

	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[MAX_PATH_EX];
		TCHAR sName[MAX_PATH_EX];
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
		_tcscpy(sName, sNewName);

		//concat components
		TCHAR sPath[MAX_PATH_EX];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		if (_trename(m_strFilenames[i].data(), sPath) != 0) {}
	}

	return 1;
}

int CCmdLineContextMenu::ConvertSpaces2Dots() {

	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[MAX_PATH_EX];
		TCHAR sName[MAX_PATH_EX];
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
		TCHAR sPath[MAX_PATH_EX];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		if (_trename(m_strFilenames[i].data(), sPath) != 0) {}
	}

	return 1;
}

int CCmdLineContextMenu::ConvertSpaces2Underscores() {

	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[MAX_PATH_EX];
		TCHAR sName[MAX_PATH_EX];
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
		TCHAR sPath[MAX_PATH_EX];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		if (_trename(m_strFilenames[i].data(), sPath) != 0) {}
	}

	return 1;
}

int CCmdLineContextMenu::ConvertUnderscores2Spaces() {

	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[MAX_PATH_EX];
		TCHAR sName[MAX_PATH_EX];
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
		_tcscpy(sName, sNewName);

		//concat components
		TCHAR sPath[MAX_PATH_EX];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		if (_trename(m_strFilenames[i].data(), sPath) != 0) {}
	}

	return 1;
}

int CCmdLineContextMenu::RemoveGroupNames()
{
	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[MAX_PATH_EX];
		TCHAR sName[MAX_PATH_EX];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		//replace something
		TCHAR *pDest;
		pDest = _tcsrchr(sName, (TCHAR)'-');
		if (pDest != NULL) {
			*pDest = '\0';
		}

		//concat components
		TCHAR sPath[MAX_PATH_EX];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		if (_trename(m_strFilenames[i].data(), sPath) != 0) {}
	}

	return 1;
}

int CCmdLineContextMenu::RenameExtension()
{
	bool bAsked = false;
	TCHAR sNewExt[MAX_PATH];
	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[MAX_PATH_EX];
		TCHAR sName[MAX_PATH_EX];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		if (!bAsked) {
			bAsked = true;
			//get new extension
			string sText = sExt;
			string sTitle = _T("Enter new extension");
			CCmdLinePromptDlg dlg(sText, sTitle, false);
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
		TCHAR sPath[MAX_PATH_EX];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		if (_trename(m_strFilenames[i].data(), sPath) != 0) {}
	}

	return 1;
}

int CCmdLineContextMenu::EditFilename()
{
	bool bAsked = false;
	TCHAR sNewFilename[MAX_PATH_EX];
	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i = 0;

	if (lFiles > 1)	{
		MessageBox(NULL, _T("More than 1 file selected!"), _T("Edit Filename"), MB_OK);
		return 0;
	}

	//split into components
	TCHAR sDrive[_MAX_DRIVE];
	TCHAR sDir[MAX_PATH_EX];
	TCHAR sName[MAX_PATH_EX];
	TCHAR sExt[_MAX_EXT];
	_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);

	//get new filename
	string sText = sName;
	string sTitle = _T("Enter new filename");
	CCmdLinePromptDlg dlg(sText, sTitle, true);
	if (dlg.DoModal() == IDOK) {
		_tcscpy(sNewFilename, dlg.strExtension.data());
	}
	else {
		return 0;
	}
	_tcscpy(sName, sNewFilename);

	//concat components
	TCHAR sPath[MAX_PATH_EX];
	_tmakepath(sPath, sDrive, sDir, sName, sExt);

	//rename file
	if (_trename(m_strFilenames[i].data(), sPath) != 0) {}

	return 1;
}

int CCmdLineContextMenu::AppendExtension()
{
	bool bAsked = false;
	TCHAR sNewExt[_MAX_EXT];
	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[MAX_PATH_EX];
		TCHAR sName[MAX_PATH_EX];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		if (!bAsked) {
			bAsked = true;
			//get new extension
			string sText = sExt;
			string sTitle = _T("Enter extension to append");
			CCmdLinePromptDlg dlg(sText, sTitle, false);
			if (dlg.DoModal() == IDOK) {
				_tcscpy(sNewExt, dlg.strExtension.data());
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
		TCHAR sPath[MAX_PATH_EX];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		if (_trename(m_strFilenames[i].data(), sPath) != 0) {}
	}

	return 1;
}

int CCmdLineContextMenu::RemoveFromFilename()
{
	bool bAsked = false;
	TCHAR sNewName[_MAX_FNAME];
	TCHAR * pName = NULL;
	long lHowManyChars = 0;
	long lHowManyCharacters = 0;
	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[MAX_PATH_EX];
		TCHAR sName[MAX_PATH_EX];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		if (!bAsked) {
			bAsked = true;
			//get new extension
			string sText = _T("0");
			string sTitle = _T("Remove n characters (-n from start)");
			CCmdLinePromptDlg dlg(sText, sTitle, false);
			if (dlg.DoModal() == IDOK) {
				lHowManyCharacters = _ttol(dlg.strExtension.data());
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
			TCHAR sPath[MAX_PATH_EX];
			_tmakepath(sPath, sDrive, sDir, sNewName, sExt);

			//rename file
			if (_trename(m_strFilenames[i].data(), sPath) != 0) {}
		}
	}

	return 1;
}

void TimetToFileTime(time_t t, LPFILETIME pft)
{
	ULARGE_INTEGER time_value;
	time_value.QuadPart = (t * 10000000LL) + 116444736000000000LL;
	pft->dwLowDateTime = time_value.LowPart;
	pft->dwHighDateTime = time_value.HighPart;
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
	string sText = tmpbuf;
	string sTitle = _T("Set date/time");

	CCmdLinePromptDlg dlg(sText, sTitle, false);
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

	FILETIME modifiedTime;
	TimetToFileTime(time, &modifiedTime);

	//set all files to new date/time
	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {
		if (PathIsDirectory(m_strFilenames[i].data())) {
			HANDLE hDir = CreateFile(m_strFilenames[i].data(), GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
			if (hDir != INVALID_HANDLE_VALUE) {
				#pragma warning(push)
				#pragma warning(disable: 6001)
				SetFileTime(hDir, NULL, NULL, &modifiedTime);
				#pragma warning(pop)
				CloseHandle(hDir);
			}
		}
		else {
			_tutime(m_strFilenames[i].data(), &times);
		}
	}

	return 1;
}

int CCmdLineContextMenu::InsertBeforeFilename()
{
	bool bAsked = false;
	TCHAR s2Insert[_MAX_FNAME];
	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[MAX_PATH_EX];
		TCHAR sName[MAX_PATH_EX];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		if (!bAsked) {
			bAsked = true;
			string sText = _T("");
			string sTitle = _T("Enter string to insert in front");
			CCmdLinePromptDlg dlg(sText, sTitle, false);
			if (dlg.DoModal() == IDOK) {
				_tcscpy(s2Insert, dlg.strExtension.data());
			} else {
				return 0;
			}
		}
		TCHAR sNewName[_MAX_FNAME];
		_tcscpy(sNewName, s2Insert);
		_tcscat(sNewName, sName);

		//concat components
		TCHAR sPath[MAX_PATH_EX];
		_tmakepath(sPath, sDrive, sDir, sNewName, sExt);

		//rename file
		if (_trename(m_strFilenames[i].data(), sPath) != 0) {}
	}

	return 1;
}

int CCmdLineContextMenu::AppendToFilename()
{
	bool bAsked = false;
	TCHAR s2Append[_MAX_FNAME];
	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[MAX_PATH_EX];
		TCHAR sName[MAX_PATH_EX];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);
		
		if (!bAsked) {
			bAsked = true;
			string sText = _T(" (1)");
			string sTitle = _T("Enter string to append");
			CCmdLinePromptDlg dlg(sText, sTitle, false);
			if (dlg.DoModal() == IDOK) {
				_tcscpy(s2Append, dlg.strExtension.data());
			} else {
				return 0;
			}
		}
		_tcscat(sName, s2Append);

		//concat components
		TCHAR sPath[MAX_PATH_EX];
		_tmakepath(sPath, sDrive, sDir, sName, sExt);

		//rename file
		if (_trename(m_strFilenames[i].data(), sPath) != 0) {}
	}

	return 1;
}

int CCmdLineContextMenu::FlattenTree()
{
	bool bAsked = false;
	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	if (lFiles > 1) {

		MessageBox(NULL, _T("select only the base folder"), _T("Info"), MB_OK);
		return 0;

	} else {

		string param = m_strFilenames[0].data();

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
			TCHAR sDir[MAX_PATH_EX];
			TCHAR sName[MAX_PATH_EX];
			TCHAR sExt[_MAX_EXT];
			_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);

			if (!bAsked) {
				bAsked = true;
				string sText = _T("no");
				string sTitle = _T("Flatten folders and delete empty ones?");
				CCmdLinePromptDlg dlg(sText, sTitle, false);
				if (!(dlg.DoModal() == IDOK && _tcscmp(dlg.strExtension.data(), _T("yes")) == 0))
				{
					return 0;
				}
			}

			FlattenTree(param.c_str(), false);
		}

		return 1;
	}
}

int CCmdLineContextMenu::FlattenTree2()
{

	bool bAsked = false;
	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	if (lFiles > 1) {

		MessageBox(NULL, _T("select only the base folder"), _T("Info"), MB_OK);
		return 0;

	} else {

		string param = m_strFilenames[0].data();

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
			TCHAR sDir[MAX_PATH_EX];
			TCHAR sName[MAX_PATH_EX];
			TCHAR sExt[_MAX_EXT];
			_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);

			if (!bAsked) {
				bAsked = true;
				string sText = _T("no");
				string sTitle = _T("Flatten folders (Folder _ file.ext) and delete empty ones?");
				CCmdLinePromptDlg dlg(sText, sTitle, false);
				if (!(dlg.DoModal() == IDOK && _tcscmp(dlg.strExtension.data(), _T("yes")) == 0))
				{
					return 0;
				}
			}

			FlattenTree(param.c_str(), true);
		}

		return 1;
	}
}

int CCmdLineContextMenu::DeleteEmptySubfolders()
{

	bool bAsked = false;
	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	if (lFiles > 1) {

		MessageBox(NULL, _T("select only the base folder"), _T("Info"), MB_OK);
		return 0;

	} else {

		string param = m_strFilenames[0].data();

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
			TCHAR sDir[MAX_PATH_EX];
			TCHAR sName[MAX_PATH_EX];
			TCHAR sExt[_MAX_EXT];
			_tsplitpath(m_strFilenames[i].data(), sDrive, sDir, sName, sExt);

			if (!bAsked) {
				bAsked = true;
				string sText = _T("no");
				string sTitle = _T("Delete empty subfolders?");
				CCmdLinePromptDlg dlg(sText, sTitle, false);
				if (!(dlg.DoModal() == IDOK && _tcscmp(dlg.strExtension.data(), _T("yes")) == 0))
				{
					return 0;
				}
			}

			DeleteEmptySubfolders(param.c_str());
		}

		return 1;
	}
}

bool CCmdLineContextMenu::IsEmptyDirectory(string folder) {
	WIN32_FIND_DATA fd;
	string searchFolder = folder + _T("\\*");
	HANDLE hFind = ::FindFirstFile(searchFolder.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do
		{
			if (_tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0) { continue; }

			::FindClose(hFind);
			return false;
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
		return true;
	}
	return false;
}

void CCmdLineContextMenu::FlattenTree(string baseFolder, bool useFolderNames)
{
	StringArray lstFolders;
	lstFolders.push_back(baseFolder);

	bool bBaseDone = false;
	while (lstFolders.size() > 0) {
		long lIndex = lstFolders.size() - 1;
		string folder = lstFolders[lIndex];

		if (!bBaseDone) lstFolders.pop_back();

		StringArray subFolders = GetSubfolders(folder);
		for (size_t i = 0; i < subFolders.size(); i++)
		{
			lstFolders.push_back(subFolders[i]);
			lIndex = -1;
		}

		if (bBaseDone) {
			// get all files
			WIN32_FIND_DATA fd;
			string searchFolder = folder + _T("\\*");
			HANDLE hFind = ::FindFirstFile(searchFolder.c_str(), &fd);
			if (hFind != INVALID_HANDLE_VALUE) {
				string prepend = _T("");
				if (useFolderNames) {
					size_t pos = folder.find_last_of(_T("\\"));
					prepend = folder.substr(pos + 1) + _T(" _ ");
				}
				do
				{
					if (_tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0 || fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { continue; }

					lIndex = -1;
					int n = 0;
					string fn = baseFolder + _T("\\") + prepend + fd.cFileName;
					struct _stat status;
					if (_tstat(fn.c_str(), &status) == 0) {
						TCHAR sDrive[_MAX_DRIVE];
						TCHAR sDir[MAX_PATH_EX];
						TCHAR sName[MAX_PATH_EX];
						TCHAR sExt[_MAX_EXT];
						_tsplitpath(fn.c_str(), sDrive, sDir, sName, sExt);
						string sBaseName = sName;
						TCHAR sPath[MAX_PATH_EX];
						do
						{
							n++;
							string sNewName = sBaseName + _T("_") + std::to_wstring(n);
							_tmakepath(sPath, sDrive, sDir, sNewName.c_str(), sExt);
						} while (_tstat(sPath, &status) == 0);
						fn = sPath;
					}

					if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
						DWORD attributes = fd.dwFileAttributes ^ FILE_ATTRIBUTE_READONLY;
						SetFileAttributes(fn.c_str(), attributes);
					}

					string oldName = folder + _T("\\") + fd.cFileName;
					int ret = _trename(oldName.c_str(), fn.c_str());
					
				} while (::FindNextFile(hFind, &fd));
				::FindClose(hFind);
			}

			// delete empty folder
			if (lIndex > -1) {
				lstFolders.pop_back();
				if (IsEmptyDirectory(folder)) {
					RemoveDirectory(folder.c_str());
				}
			}
		}
		else {
			bBaseDone = true;
		}
	}
}

CCmdLineContextMenu::StringArray CCmdLineContextMenu::GetSubfolders(CCmdLineContextMenu::string folder)
{
	StringArray lstFolders;

	WIN32_FIND_DATA fd;
	string searchFolder = folder + _T("\\*");
	StringArray lstSubfolders;
	HANDLE hFind = ::FindFirstFile(searchFolder.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do
		{
			if (_tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0) { continue; }

			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				string filename = folder + _T("\\") + fd.cFileName;
				lstSubfolders.push_back(filename);
			}

		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);

		for (size_t i = 0; i < lstSubfolders.size(); i++)
		{
			lstFolders.push_back(lstSubfolders[i].c_str());
			StringArray folders = GetSubfolders(lstSubfolders[i].c_str());
			for (size_t i = 0; i < folders.size(); i++)
			{
				lstFolders.push_back(folders[i].c_str());
			}
		}
	}

	return lstFolders;
}

void CCmdLineContextMenu::DeleteEmptySubfolders(string baseFolder)
{
	m_strBaseFolder = baseFolder;

	CheckSubfolders(baseFolder);
}

bool CCmdLineContextMenu::CheckSubfolders(string folder)
{
	bool ret = true;

	StringArray lstSubfolders;
	string ending = _T("\\thumbs.db");

	long lCount = 0;
	bool bFound = false;
	WIN32_FIND_DATA fd;
	string searchFolder = folder + _T("\\*");
	HANDLE hFind = ::FindFirstFile(searchFolder.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do
		{
			if (_tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0) { continue; }

			lCount++;
			string filename = folder + _T("\\") + fd.cFileName;

			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				lstSubfolders.push_back(filename);
			}
			else {
				if (m_strBaseFolder.compare(filename) != 0) {
					if (filename.compare(filename.length() - ending.length(), ending.length(), ending) == 0) {
						bFound = true;
					}
					else {
						ret = false;	// something other than folder was found
					}
				}
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);

		if (lCount == 1 && bFound) {
			string fn = folder + _T("\\Thumbs.db");
			SetFileAttributes(fn.c_str(), FILE_ATTRIBUTE_NORMAL);
			_tremove(fn.c_str());
		}
		long i = 0;
		for (size_t i = 0; i < lstSubfolders.size(); i++)
		{
			bool isEmpty = CheckSubfolders(lstSubfolders[i].c_str());
			if (isEmpty) {
				DWORD attributes = GetFileAttributes(lstSubfolders[i].c_str());
				if ((attributes & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) != 0) {
					attributes = attributes & ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
					SetFileAttributes(lstSubfolders[i].c_str(), attributes);
				}
				RemoveDirectory(lstSubfolders[i].c_str());
			}
			else if (m_strBaseFolder.compare(folder) != 0) {
				ret = false;
			}
		}
	}
	else {
		ret = false;
	}

	return ret;
}

int CCmdLineContextMenu::SlideShow()
{
	TCHAR szAppPath[MAX_PATH_EX] = _T("");
	string strAppDirectory;
	GetModuleFileName(_AtlModule.hInstance, szAppPath, MAX_PATH_EX);
	// Extract directory
	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.substr(0, strAppDirectory.rfind(_T("\\")));
	strAppDirectory = strAppDirectory.append(_T("\\..\\..\\SlideShow\\bin\\SlideShow.exe"));

	string param;
	size_t lFiles;
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
	size_t lFiles;
	lFiles = m_strFilenames.size();
	int i;
	for (i=0; i<lFiles; i++) {

		if (!bAsked) {
			bAsked = true;
			string sText = _T("no");
			string sTitle = _T("Empty file(s)?");
			CCmdLinePromptDlg dlg(sText, sTitle, false);
			if (!(dlg.DoModal() == IDOK && _tcscmp(dlg.strExtension.data(), _T("yes")) == 0))
			{
				return 0;
			}
		}

		//empty file
		HANDLE h = CreateFile(m_strFilenames[i].data(), GENERIC_WRITE, 0, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		#pragma warning(push)
		#pragma warning(disable: 6001)
		if (h != INVALID_HANDLE_VALUE) CloseHandle(h);
		#pragma warning(pop)
	}

	return 1;
}

int CCmdLineContextMenu::StartCopyFilesHere()
{
	std::packaged_task<int()> task([&]() {
		string szFolderDroppedIn = string(m_szFolderDroppedIn);
		return CopyFilesHere(m_strFilenames, szFolderDroppedIn);
		});
	std::future<int> res = task.get_future();
	std::thread(std::move(task)).detach();
	res.wait_for(1s);
	return 1;
}

int CCmdLineContextMenu::StartMoveFilesHere()
{
	std::packaged_task<int()> task([&]() {
		string szFolderDroppedIn = string(m_szFolderDroppedIn);
		return MoveFilesHere(m_strFilenames, szFolderDroppedIn);
		});
	std::future<int> res = task.get_future();
	std::thread(std::move(task)).detach();
	res.wait_for(1s);
	return 1;
}

void DoWork()
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, 1)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

int CCmdLineContextMenu::CopyFilesHere(StringArray strFilenames, string szFolderDroppedIn)
{
	size_t lTotalItems, lFiles;
	lTotalItems = lFiles = strFilenames.size();
	size_t lDoneItems = 0;

	IProgressDialog* pProgressDlg;
	HRESULT hr = CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pProgressDlg));
	if (FAILED(hr)) pProgressDlg = NULL;
	if (pProgressDlg != NULL) {
		pProgressDlg->SetTitle(_T("Copying items..."));
		pProgressDlg->StartProgressDialog(NULL, NULL, PROGDLG_AUTOTIME, NULL);
		pProgressDlg->SetProgress(lDoneItems, lTotalItems);
		DoWork();
	}

	int i;
	for (i = 0; i < lFiles; i++) {
		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[MAX_PATH_EX];
		TCHAR sName[_MAX_FNAME];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(strFilenames[i].data(), sDrive, sDir, sName, sExt);

		if (pProgressDlg != NULL) {
			if (pProgressDlg->HasUserCancelled())
				break;
			lDoneItems++;
			pProgressDlg->SetProgress(lDoneItems, lTotalItems);
			DoWork();
			string sItem = _T("");
			sItem.append(sName).append(sExt);
			pProgressDlg->SetLine(2, sItem.data(), true, NULL);
		}

		string sNewFilename = _T("");
		sNewFilename.append(szFolderDroppedIn).append(_T("\\")).append(sName).append(sExt);
		if (sNewFilename.rfind(_T("\\\\?\\"), 0) != 0)
			sNewFilename = _T("\\\\?\\") + sNewFilename;

		if (PathIsDirectory(strFilenames[i].data())) {
			CopyDirectory(strFilenames[i].data(), sNewFilename, pProgressDlg, &lDoneItems, &lTotalItems);

		} else {
			bool ret = CopyFile(strFilenames[i].data(), sNewFilename.c_str(), true);
		}
	}

	if (pProgressDlg != NULL) {
		pProgressDlg->StopProgressDialog();
		pProgressDlg->Release();
	}
	
	return 1;
}

int CCmdLineContextMenu::CopyDirectory(string sourceDir, string destDir, IProgressDialog* pProgressDlg, size_t* currentItems, size_t* totalItems)
{
	string strSource;
	string strDest;
	HANDLE hFind;
	WIN32_FIND_DATA fd;

	// Create destination directory
	if (::CreateDirectoryW(destDir.c_str(), 0) == FALSE)
		return ::GetLastError();

	string searchFolder = sourceDir + _T("\\*");

	if (pProgressDlg != NULL) {
		hFind = ::FindFirstFile(searchFolder.c_str(), &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (_tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0) { continue; }
				(*totalItems)++;
			} while (!pProgressDlg->HasUserCancelled() && ::FindNextFile(hFind, &fd) == TRUE);
			::FindClose(hFind);
			hFind = NULL;
		}
		if (pProgressDlg->HasUserCancelled())
			return -1;
	}

	hFind = ::FindFirstFile(searchFolder.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (_tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0) { continue; }

			strSource.erase();
			strDest.erase();
			strSource = sourceDir + _T("\\") + fd.cFileName;
			if (strSource.rfind(_T("\\\\?\\"), 0) != 0)
				strSource = _T("\\\\?\\") + strSource;
			strDest = destDir + _T("\\") + fd.cFileName;
			if (strDest.rfind(_T("\\\\?\\"), 0) != 0)
				strDest = _T("\\\\?\\") + strDest;

			(*currentItems)++;
			if (pProgressDlg != NULL) {
				if (pProgressDlg->HasUserCancelled())
					break;
				pProgressDlg->SetProgress(*currentItems, *totalItems);
				DoWork();
			}

			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// Copy subdirectory
				int ret = CopyDirectory(strSource, strDest, pProgressDlg, currentItems, totalItems);
				if (ret < 0)
					return ret;
			}
			else
			{
				// Copy file
				if (::CopyFile(strSource.c_str(), strDest.c_str(), TRUE) == FALSE)
					return ::GetLastError();
			}
		} while (::FindNextFile(hFind, &fd) == TRUE);

		::FindClose(hFind);

		if (pProgressDlg != NULL && pProgressDlg->HasUserCancelled())
			return -1;

		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
			return dwError;
	}

	return 0;
}

BOOL CCmdLineContextMenu::RemoveDirectory(string strDirectory)
{
	WIN32_FIND_DATA fdFile = {};

	string strSearchItems = strDirectory + _T("\\*.*");
	HANDLE hFind = ::FindFirstFile(strSearchItems.c_str(), &fdFile);
	if (hFind == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	do {
		string strDelete = strDirectory + _T("\\") + fdFile.cFileName;

		if ((fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
			if (_tcscmp(fdFile.cFileName, _T(".")) == 0 || _tcscmp(fdFile.cFileName, _T("..")) == 0)
				continue;
			CCmdLineContextMenu::RemoveDirectory(strDelete);
		}
		else {
			::DeleteFile(strDelete.c_str());
		}
	} while (::FindNextFile(hFind, &fdFile) == TRUE);

	::FindClose(hFind);
	::RemoveDirectory(strDirectory.c_str());

	return TRUE;
}

int CCmdLineContextMenu::MoveFilesHere(StringArray strFilenames, string szFolderDroppedIn)
{
	size_t lFiles;
	lFiles = strFilenames.size();

	IProgressDialog* pProgressDlg;
	HRESULT hr = CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pProgressDlg));
	if (FAILED(hr)) pProgressDlg = NULL;
	if (pProgressDlg != NULL) {
		pProgressDlg->SetTitle(_T("Moving items..."));
		pProgressDlg->StartProgressDialog(NULL, NULL, PROGDLG_AUTOTIME, NULL);
		pProgressDlg->SetProgress(0, lFiles);
		DoWork();
	}

	bool isSameVolume = true;
	if (lFiles > 0) {
		HANDLE hFile = CreateFile(strFilenames[0].c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		BY_HANDLE_FILE_INFORMATION fileInfo = {};
		BOOL ret = GetFileInformationByHandle(hFile, &fileInfo);
		HANDLE hFile2 = CreateFile(szFolderDroppedIn.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		BY_HANDLE_FILE_INFORMATION fileInfo2 = {};
		ret = GetFileInformationByHandle(hFile2, &fileInfo2);
		isSameVolume = fileInfo.dwVolumeSerialNumber == fileInfo2.dwVolumeSerialNumber;
		CloseHandle(hFile);
		CloseHandle(hFile2);
	}

	int i;
	for (i = 0; i < lFiles; i++) {
		//split into components
		TCHAR sDrive[_MAX_DRIVE];
		TCHAR sDir[MAX_PATH_EX];
		TCHAR sName[_MAX_FNAME];
		TCHAR sExt[_MAX_EXT];
		_tsplitpath(strFilenames[i].data(), sDrive, sDir, sName, sExt);

		if (pProgressDlg != NULL) {
			if (pProgressDlg->HasUserCancelled())
				break;
			pProgressDlg->SetProgress(i + 1, lFiles);
			DoWork();
			string sItem = _T("");
			sItem.append(sName).append(sExt);
			pProgressDlg->SetLine(2, sItem.data(), true, NULL);
		}

		string sOldFilename = strFilenames[i].data();
		if (sOldFilename.rfind(_T("\\\\?\\"), 0) != 0)
			sOldFilename = _T("\\\\?\\") + sOldFilename;
		string sNewFilename = _T("");
		sNewFilename.append(szFolderDroppedIn).append(_T("\\")).append(sName).append(sExt);
		if (sNewFilename.rfind(_T("\\\\?\\"), 0) != 0)
			sNewFilename = _T("\\\\?\\") + sNewFilename;

		bool ret;
		if (isSameVolume) {
			ret = MoveFile(sOldFilename.c_str(), sNewFilename.c_str());
			if (!ret)
				break;
			/*
			if (ret == false) {
				DWORD error = ::GetLastError();
				std::string sMessage = std::system_category().message(error);
				string message = string(sMessage.begin(), sMessage.end());
				MessageBox(NULL, message.c_str(), _T("Error"), 0);
			}
			*/
		}
		else {
			size_t dummy;
			int result = CopyDirectory(sOldFilename, sNewFilename, NULL, &dummy, &dummy);
			if (result == 0)
				CCmdLineContextMenu::RemoveDirectory(sOldFilename.c_str());
			else
				break;
		}
	}

	if (pProgressDlg != NULL) {
		pProgressDlg->StopProgressDialog();
		pProgressDlg->Release();
	}

	return 1;
}
