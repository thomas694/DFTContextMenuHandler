//--------------------------------------------------------------------------
// DFTContextMenuHandler - a windows shell extension
// 
// This is a shell extension for windows that adds a file tools submenu to 
// the windows explorer context menu for folders and files in general.
//
// Version 1.01
// Copyright (c) 2004-2008  thomas694 (@GH 0CFD61744DA1A21C)
//     first version (for windows 32bit)
// Version 1.5.0  (c) 2022  thomas694
//     copied logic code into a VS2019 ATL template,
//     using CAtlDllModuleT instead of CComModule
//
// This file is part of DFTContextMenuHandler.
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
// CmdLinePromptDlg.cpp : Implementation of CCmdLinePromptDlg
#include "pch.h"
#include "dllmain.h"
#include "CmdLinePromptDlg.h"
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////
// CCmdLinePromptDlg

//***************************************
//* Date            : 6.2.99
//* Last Modified   : 2.10.2000
//* Function name	: OnInitDialog
//* Description	    : 
//***************************************
//

LRESULT CCmdLinePromptDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	HWND	hWndTextbox = NULL;

	if ((hWndTextbox = ::GetDlgItem(m_hWnd, IDC_EDIT1)) != NULL) {
		::SetWindowText(hWndTextbox, m_strExtension.data());
	}
	CWindow win(m_hWnd);
	win.SetWindowText(m_strDialogTitle.data());
	win.CenterWindow();

	::SendMessage(hWndTextbox, WM_SETFOCUS, reinterpret_cast<WPARAM>(hWndTextbox), 0);

	return 1;  // Let the system set the focus
}


//***************************************
//* Date            : 6.2.99
//* Last Modified   : 6.30.99
//* Function name	: OnOK
//* Description	    : 
//***************************************
//
LRESULT CCmdLinePromptDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{

	HWND	hWndTextbox = NULL;
	_TCHAR	cExtension[_MAX_FNAME];

	if ((hWndTextbox = ::GetDlgItem(m_hWnd, IDC_EDIT1)) != NULL) {
		if (::GetWindowText(hWndTextbox, cExtension, _MAX_FNAME)) {
			strExtension = cExtension;
		}
	}
	
	EndDialog(wID);
	return 0;
}

//***************************************
//* Date            : 6.2.99
//* Last Modified   : 6.2.99
//* Function name	: OnCancel
//* Description	    : 
//***************************************
//
LRESULT CCmdLinePromptDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

//***************************************
//* Date            : 2.9.2000
//* Last Modified   : 2.9.2000
//* Function name	: CCmdLinePromptDlg::OnSysCommand
//* Description	    : 
//***************************************
//
LRESULT CCmdLinePromptDlg::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	_TCHAR		strAboutText[256];

	// If the user clicks on the context-help button on the title bar,
	// intercept the message, show the help dialog, and tell the system
	// that the message has been handled.
	switch (wParam) {
	case SC_CONTEXTHELP:
		if (::LoadString(_AtlModule.hInstance, IDS_ABOUTTEXT, strAboutText, 256) > 0) {
			::MessageBox(m_hWnd, strAboutText, _T("About"), MB_OK | MB_ICONQUESTION);
		}
		bHandled = TRUE;
		break;

	case SC_CLOSE:
		EndDialog(IDCLOSE);
		bHandled = TRUE;
		break;

	default:
		bHandled = FALSE;
		
	}
	return 0;
}
