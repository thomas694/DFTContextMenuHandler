// CmdLinePromptDlg.cpp : Implementation of CCmdLinePromptDlg
#include "stdafx.h"
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
		if (::LoadString(_Module.m_hInst, IDS_ABOUTTEXT, strAboutText, 256) > 0) {
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
