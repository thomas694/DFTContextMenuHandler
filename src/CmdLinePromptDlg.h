// CmdLinePromptDlg.h : Declaration of the CCmdLinePromptDlg

#ifndef __CMDLINEPROMPTDLG_H_
#define __CMDLINEPROMPTDLG_H_

#include "resource.h"       // main symbols
#include <atlhost.h>
#include <string>
#include <list>

// Due to the design of the program, this constant 
// should not be set any higher than 26.
#define		MAX_HISTORY				10

#define		KEY_COMMAND_HISTORY		_T("Software\\Lumenex\\CmdLineExt\\")
#define		KEY_COMMAND_ORDER		_T("Order")

/////////////////////////////////////////////////////////////////////////////
// CCmdLinePromptDlg
class CCmdLinePromptDlg : 
	public CAxDialogImpl<CCmdLinePromptDlg>
{
public:

	typedef std::basic_string<_TCHAR>	string;

	CCmdLinePromptDlg(string &strFileName)
	{
		ATLASSERT(MAX_HISTORY <= 26);
		m_strFileName.assign(strFileName.data());
		m_strOrderString[0] = _T('\0');
		m_nKeyNameMask = NULL;
		m_bDosCursorBlink = FALSE;
	}

	~CCmdLinePromptDlg()
	{
	}

	enum { IDD = IDD_CMDLINEPROMPTDLG };

	string				m_strCmdLine;

private:
	typedef std::list<string>				stringList;
	typedef std::list<string>::iterator 	stringListIterator;
	enum									{ eventBlinkCursor = 2214 };
	enum									{ promptBlinkOff, promptBlinkOn };

	void				LoadCommandHistory();

	string				m_strFileName;
	_TCHAR				m_strOrderString[MAX_HISTORY + 1];
	long				m_nKeyNameMask;
	stringList			m_listCmdLines;

	HICON				m_hDosPromptIcons[2];
	BOOLEAN				m_bDosCursorBlink;

	static BOOLEAN				m_bDisableAutoComplete;
	static WNDPROC				m_pOriginalComboEditProc;
	static LRESULT APIENTRY		AutoCompleteComboBoxProc(HWND hWndEdit, UINT uMsg, WPARAM wParam, LPARAM lParam);

BEGIN_MSG_MAP(CCmdLinePromptDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	COMMAND_HANDLER(IDC_COMBO_CMD_LINE, CBN_EDITCHANGE, OnEditChangeCmdLineCombo)
	MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnEditChangeCmdLineCombo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

#endif //__CMDLINEPROMPTDLG_H_
