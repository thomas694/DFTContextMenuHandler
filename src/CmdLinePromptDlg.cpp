// CmdLinePromptDlg.cpp : Implementation of CCmdLinePromptDlg
#include "stdafx.h"
#include "CmdLinePromptDlg.h"
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////
// CCmdLinePromptDlg

//***************************************
//
//	The commands are stored in the registry like so:
// 
//	Key Name				Value
//
//	"Order"					"adbce"
//	  "a"					"/regserver"
//	  "b"					"/service"
//	  "c"					"/unregserver"
//	  "d"					"-install"
//	  "e"					"-flytothemoon"
//
//	The strings will appear in the drop-down combobox
//	in the order specified by the "Order" key.  In this
//	example, "a" followed by "d" followed by "b", etc.
//
//	When the user types a parameter not in the list, the
//	value is saved to the next available key (in this case "f")
//	and its name prepended to the order string (e.g. "fadbce").
//
//	When the user selects one of the items already in the list,
//	the key name is moved to the head of the order string.  So,
//	if the user selects "/unregserver", the order string would
//	become "cadbe".
//
//***************************************

WNDPROC	CCmdLinePromptDlg::m_pOriginalComboEditProc = NULL;
BOOLEAN CCmdLinePromptDlg::m_bDisableAutoComplete = FALSE;

//***************************************
//* Date            : 6.2.99
//* Last Modified   : 2.10.2000
//* Function name	: OnInitDialog
//* Description	    : 
//***************************************
//
LRESULT CCmdLinePromptDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC			hDC = NULL;
	_TCHAR		cStrTitlePrefix[50] = _T("Command Line for ");
	_TCHAR		cTitleString[MAX_PATH + 50];
	SIZE		sizePrefix;
	RECT		rectWindow, rectPath;
	CRegKey		key;
	DWORD		dwXPos, dwYPos;
	HWND		hWndCombo = ::GetDlgItem(m_hWnd, IDC_COMBO_CMD_LINE);
	HWND		hWndComboEdit = NULL;

	::SetTimer(m_hWnd, eventBlinkCursor, 250, NULL);

	// Load the DOS prompt icons.
	m_hDosPromptIcons[promptBlinkOff] = (HICON)::LoadImage(_Module.m_hInst, MAKEINTRESOURCE(IDI_DOS_PROMPT), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	m_hDosPromptIcons[promptBlinkOn] = (HICON)::LoadImage(_Module.m_hInst, MAKEINTRESOURCE(IDI_DOS_PROMPT_BLINK), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);

	if (key.Open(HKEY_CURRENT_USER, KEY_COMMAND_HISTORY) == ERROR_SUCCESS) {
		// Try to load the last position of the window.
		if (key.QueryValue(dwXPos, _T("XPos")) == ERROR_SUCCESS && key.QueryValue(dwYPos, _T("YPos")) == ERROR_SUCCESS) {
			RECT rectScreen, rectWindow;
			::GetWindowRect(m_hWnd, &rectWindow);
			// Make sure that this falls within the screen area.
			if (::SystemParametersInfo(SPI_GETWORKAREA, NULL, (void*)&rectScreen, NULL)) {
				if (dwXPos < rectScreen.right && dwYPos < rectScreen.bottom && (dwXPos + (rectWindow.right - rectWindow.left) > 0) && (dwYPos + (rectWindow.bottom - rectWindow.top) > 0))
					::SetWindowPos(m_hWnd, NULL, dwXPos, dwYPos, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}

		}
		key.Close();
	}
	
	LoadCommandHistory();
	
	// Create the title text, consisiting of a prefix string, plus a compressed
	// version of the file name.
	hDC = ::GetDC(m_hWnd);
	if (hDC != NULL) {
		::GetTextExtentPoint(hDC, cStrTitlePrefix, _tcslen(cStrTitlePrefix), &sizePrefix);
		::GetWindowRect(m_hWnd, &rectWindow);
		// Set the rect to the width of the window (minus the close and help buttons) and
		// the height of the title bar.
		::SetRect(&rectPath, rectWindow.left + sizePrefix.cx, rectWindow.top, rectWindow.right - (2 * ::GetSystemMetrics(SM_CXSIZE)), rectWindow.top + ::GetSystemMetrics(SM_CYSIZE));
		// This doesn't actually draw the text, just determines how much
		// can fit in the given space.
		::DrawTextEx(hDC, (LPTSTR)m_strFileName.data(), m_strFileName.length(), &rectPath, DT_PATH_ELLIPSIS | DT_MODIFYSTRING, NULL);
		::ReleaseDC(m_hWnd, hDC);
		_stprintf(cTitleString, _T("%s%s"), cStrTitlePrefix, m_strFileName.data());
		::SetWindowText(m_hWnd, cTitleString);
	}

	// Hijack the window procedure of the combobox's edit field so we can
	// detect when the user backspaces or deletes part of the string.
	// Kudos to Caroline Price's Codeguru article for tipping me off on the control
	// ID of the combobox's edit control.
	hWndComboEdit = ::GetDlgItem(hWndCombo, 1001);
	if (hWndComboEdit != NULL) {
		m_pOriginalComboEditProc = (WNDPROC)::GetWindowLong(hWndComboEdit, GWL_WNDPROC);
		if (m_pOriginalComboEditProc != NULL) {
			::SetWindowLong(hWndComboEdit,GWL_WNDPROC, (LONG)AutoCompleteComboBoxProc);
		}
	}

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
	_TCHAR				cCmdLineBuff[MAX_PATH];
	_TCHAR				cNewOrder[MAX_HISTORY + 1];
	HWND				hWndCombo = NULL;
	CRegKey				key;
	_TCHAR				strKeyName[2] = {_T('\0'), _T('\0')};
	stringListIterator	posFound;

	::memset((void*)cNewOrder, 0, (MAX_HISTORY + 1) * sizeof(_TCHAR));
			
	// Open the registry key.
	if (key.Create(HKEY_CURRENT_USER, KEY_COMMAND_HISTORY) == ERROR_SUCCESS) {
		// Save the text to the m_strCmdLine variable and the registry.
		if ((hWndCombo = ::GetDlgItem(m_hWnd, IDC_COMBO_CMD_LINE)) != NULL) {
			if (::GetWindowText(hWndCombo, cCmdLineBuff, MAX_PATH)) {
				m_strCmdLine = cCmdLineBuff;

				// If this command string is new, i.e. not previously saved in the registry,
				// we need to save it.
				if ((posFound = std::find(m_listCmdLines.begin(), m_listCmdLines.end(), cCmdLineBuff)) == m_listCmdLines.end()) {
					// If we haven't exceeded our (self-imposed) limit on the number of
					// commands to save . . .
					if (_tcslen(m_strOrderString) < MAX_HISTORY) {
						// Find the first letter of the alphabet that is available by
						// checking the m_nKeyNameMask for an unset bit.
						for (int i = 0; i < 32; i++) {
							if (! (m_nKeyNameMask & (1 << i))) {
								strKeyName[0] = _T('a') + i;
								break;
							}
						}

						// Save the command string to the registry.
						if (key.SetValue(cCmdLineBuff, strKeyName) == ERROR_SUCCESS) {
							// The new order string is simply the old order string with the new
							// key prepended on the front.
							_stprintf(cNewOrder, _T("%s%s"), strKeyName, m_strOrderString);
							key.SetValue(cNewOrder, KEY_COMMAND_ORDER);
						}
					}
					else {
						// We take the last command string in the order as our own.
						// e.g. if the string was "abcde", now it will become "eabcd" and
						// the new command string will be saved to the "e" value.
						strKeyName[0] = m_strOrderString[MAX_HISTORY - 1];
						if (key.SetValue(cCmdLineBuff, strKeyName) == ERROR_SUCCESS) {
							m_strOrderString[MAX_HISTORY - 1] = _T('\0');
							_stprintf(cNewOrder, _T("%s%s"), strKeyName, m_strOrderString);
							key.SetValue(cNewOrder, KEY_COMMAND_ORDER);
						}
					}
				}
				// We have already saved this string, but now we need to move it to the
				// head of the class.
				else {					
					stringListIterator posBegin = m_listCmdLines.begin();
					int nFoundIndex = 0, nOrderStrLen = _tcslen(m_strOrderString);
					int i, nCurrIndex = 0;
					
					// Find the index of this string by backtracking from the found
					// iterator position.  This is a bit hackish, but it works.
					while (posFound != posBegin) {
						nFoundIndex++;
						posFound--;
					}
					
					// The new order begins with the found item, then the rest of the old order
					// string is copied in (minus the one we moved to the front, of course).
					cNewOrder[0] = m_strOrderString[nFoundIndex];
					for (i = 0; i < nOrderStrLen; i++) {
						if (i != nFoundIndex) {
							nCurrIndex++;
							cNewOrder[nCurrIndex] = m_strOrderString[i];
						}
					}
					cNewOrder[nOrderStrLen] = _T('\0');
					key.SetValue(cNewOrder, KEY_COMMAND_ORDER);
				}
			}
		}		
		key.Close();
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
//* Date            : 6.29.99
//* Last Modified   : 6.29.99
//* Function name	: CCmdLinePromptDlg::LoadCommandHistory
//* Description	    : Commands are saved to the registry as values named "a" through
//*					  "z".  The "Order" string saves the order in which these strings
//*					  are to be displayed (e.g. "abcde").
//***************************************
//
void CCmdLinePromptDlg::LoadCommandHistory()
{
	HWND		hWndCombo = NULL;
	CRegKey		key;
	_TCHAR		*pCmdLineBuff = NULL, strValueName[2] = {_T('\0'), _T('\0')};
	DWORD		dwBytesRead = 0, dwMaxValueLength = 0;
	int			i, nOrderLen = 0;
	LONG		lResult;

	m_nKeyNameMask = NULL;
	m_listCmdLines.clear();

	// Open the registry key.
	if (key.Open(HKEY_CURRENT_USER, KEY_COMMAND_HISTORY) == ERROR_SUCCESS) {
		// Now we are going to load up the saved command strings in the correct order.
		if ((hWndCombo = ::GetDlgItem(m_hWnd, IDC_COMBO_CMD_LINE)) != NULL) {
			dwBytesRead = (MAX_HISTORY + 1) * sizeof(_TCHAR);
			// Open the order string.  If it doesn't exist, then we assume that no command strings
			// have been saved.
			if ((lResult = key.QueryValue(m_strOrderString, KEY_COMMAND_ORDER, &dwBytesRead)) == ERROR_SUCCESS) {
				// Get the length of the longest value in the key so we can
				// be sure to allocate enough memory for it.
				if (::RegQueryInfoKey(key, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &dwMaxValueLength, NULL, NULL) == ERROR_SUCCESS) {
					nOrderLen = _tcslen(m_strOrderString);
					pCmdLineBuff = new _TCHAR[dwMaxValueLength + 1];

					// Loop through the order string (e.g. "adb" means value "a", "d", then "b").
					for (i = 0; i < nOrderLen; i++) {
						ATLASSERT(m_strOrderString[i] >= _T('a') && m_strOrderString[i] <= _T('z'));
						// Make a string of length 1 of the character.
						strValueName[0] = m_strOrderString[i];
						dwBytesRead = dwMaxValueLength;
						// Try to read the value named m_strOrderString[i], making sure
						// that this letter hasn't already been used.
						if (! (m_nKeyNameMask & (1 << (m_strOrderString[i] - _T('a')))) && key.QueryValue(pCmdLineBuff, strValueName, &dwBytesRead) == ERROR_SUCCESS) {
							// Set the mask bit saying that this letter (m_strOrderString[i] has been used)
							m_nKeyNameMask |= (1 << (m_strOrderString[i] - _T('a')));
							m_listCmdLines.push_back(pCmdLineBuff);
							::SendMessage(hWndCombo, CB_ADDSTRING, 0, (LPARAM)pCmdLineBuff);
						}
						// If we can't read the value, or the letter has already been used, remove this item from the order
						// string.
						else {
							for (int j = i; j < nOrderLen - 1; j++) {
								m_strOrderString[j] = m_strOrderString[j+1];
							}
							i--;
							nOrderLen--;
							m_strOrderString[nOrderLen] = _T('\0');
						}
					}

					delete pCmdLineBuff;
				}
			}
			else if (lResult == ERROR_MORE_DATA) {
				// If the order string is too long, someone has likely been screwing with it.
				// Just delete the darned thing.
				key.DeleteValue(KEY_COMMAND_ORDER);
			}
			// Set the combo box so that the first item is selected and highlighted.
			::SendMessage(hWndCombo, CB_SETCURSEL, 0, NULL);
			::SendMessage(hWndCombo, CB_SETEDITSEL, 0, MAKELPARAM((-1), (-1)));
		}

		key.Close();
	}
}

//***************************************
//* Date            : 6.30.99
//* Last Modified   : 2.9.2000
//* Function name	: CCmdLinePromptDlg::OnDestroy
//* Description	    : 
//***************************************
//
LRESULT CCmdLinePromptDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT	rectWindow;
	CRegKey	key;

	if (key.Create(HKEY_CURRENT_USER, KEY_COMMAND_HISTORY) == ERROR_SUCCESS) {
		// Save the window position.
		::GetWindowRect(m_hWnd, &rectWindow);
		key.SetValue(rectWindow.left, _T("XPos"));
		key.SetValue(rectWindow.top, _T("YPos"));
		key.Close();
	}
	
	::DestroyIcon(m_hDosPromptIcons[promptBlinkOff]);
	::DestroyIcon(m_hDosPromptIcons[promptBlinkOn]);

	return 1;
}

//***************************************
//* Date            : 2.8.2000
//* Last Modified   : 2.10.2000
//* Function name	: OnEditChangeCmdLineCombo
//* Description	    : Does autocompletion, similar to the address boxes of
//*						Netscape and Internet Explorer.
//***************************************
//
LRESULT CCmdLinePromptDlg::OnEditChangeCmdLineCombo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int			nTextLen = 0, nMatchedTextLen = 0;
	HWND		hWndCombo = ::GetDlgItem(m_hWnd, IDC_COMBO_CMD_LINE);
	_TCHAR		*pStrMatchedText = NULL, *pEnteredText = NULL;
	DWORD		dwStartSel = 0, dwEndSel = 0;
	
	// Get the text length from the combobox, then copy it into a newly allocated buffer.
	nTextLen = ::SendMessage(hWndCombo, WM_GETTEXTLENGTH, NULL, NULL);
	pEnteredText = new _TCHAR[nTextLen + 1];
	::SendMessage(hWndCombo, WM_GETTEXT, (WPARAM)nTextLen + 1, (LPARAM)pEnteredText);
	::SendMessage(hWndCombo, CB_GETEDITSEL, (WPARAM)&dwStartSel, (LPARAM)&dwEndSel);

	// Check to make sure autocompletion isn't disabled due to a backspace or delete
	// (see AutoCompleteComboBoxProc). Also, the user must be typing at the end of the string,
	// not somewhere in the middle.
	if (! m_bDisableAutoComplete && (dwStartSel == dwEndSel) && (dwStartSel == nTextLen)) {
		// Try and find a string that matches the typed substring.  If one is found,
		// set the text of the combobox to that string and set the selection to mask off
		// the end of the matched string.
		int nMatch = ::SendMessage(hWndCombo, CB_FINDSTRING, (WPARAM)-1, (LPARAM)pEnteredText);
		if (nMatch != CB_ERR) {
			nMatchedTextLen = ::SendMessage(hWndCombo, CB_GETLBTEXTLEN, (WPARAM)nMatch, 0);
			if (nMatchedTextLen != CB_ERR) {
				// Since the user may be typing in the same string, but with different case (e.g. "/port --> /PORT")
				// we copy whatever the user has already typed into the beginning of the matched string,
				// then copy the whole shebang into the combobox.  We then set the selection to mask off
				// the inferred portion.
				pStrMatchedText = new _TCHAR[nMatchedTextLen + 1];
				::SendMessage(hWndCombo, CB_GETLBTEXT, (WPARAM)nMatch, (LPARAM)pStrMatchedText);				
				memcpy((void*)pStrMatchedText, (void*)pEnteredText, nTextLen * sizeof(_TCHAR));
				::SendMessage(hWndCombo, WM_SETTEXT, 0, (WPARAM)pStrMatchedText);
				::SendMessage(hWndCombo, CB_SETEDITSEL, 0, MAKELPARAM(nTextLen, -1));
				delete pStrMatchedText;			
			}
		}
	}

	delete pEnteredText;
	bHandled = TRUE;	
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

	default:
		bHandled = FALSE;
		
	}
	return 0;
}

//***************************************
//* Date            : 2.9.2000
//* Last Modified   : 2.9.2000
//* Function name	: CCmdLinePromptDlg::OnTimer
//* Description	    : 
//***************************************
//
LRESULT CCmdLinePromptDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND		hWndDosPrompt = NULL;

	// Flips the DOS prompt icon back and forth between the blink and no-blink states.
	switch (wParam) {
	case (eventBlinkCursor):
		m_bDosCursorBlink = ! m_bDosCursorBlink;
		hWndDosPrompt = ::GetDlgItem(m_hWnd, IDC_STATIC_DOS_PROMPT);
		if (hWndDosPrompt != NULL) {
			::SendMessage(hWndDosPrompt, STM_SETICON, (WPARAM)m_hDosPromptIcons[m_bDosCursorBlink ? promptBlinkOn : promptBlinkOff], NULL);
		}		
		break;
	}
	return 0;
}

//***************************************
//* Date            : 2.10.2000
//* Last Modified   : 2.10.2000
//* Function name	: CCmdLinePromptDlg::AutoCompleteComboBoxProc
//* Description	    : 
//***************************************
//
LRESULT APIENTRY CCmdLinePromptDlg::AutoCompleteComboBoxProc(HWND hWndEdit, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// If the user presses the backspace or delete key, disable the autocompletion
	// until another valid character is entered.  Then pass this and any other message
	// on to the original window procedure.
	switch (uMsg) {
	case WM_CHAR:
		if (wParam == VK_BACK) {
			m_bDisableAutoComplete = TRUE;
		}
		else {
			m_bDisableAutoComplete = FALSE;
		}
		break;
	case WM_KEYDOWN:
		if (wParam == VK_DELETE) {
			m_bDisableAutoComplete = TRUE;;
		}
		break;
	}
	
	return CallWindowProc(m_pOriginalComboEditProc, hWndEdit, uMsg, wParam, lParam);
}
