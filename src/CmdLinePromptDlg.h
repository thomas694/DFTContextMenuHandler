// CmdLinePromptDlg.h : Declaration of the CCmdLinePromptDlg

#ifndef __CMDLINEPROMPTDLG_H_
#define __CMDLINEPROMPTDLG_H_

#include "resource.h"       // main symbols
#include <atlhost.h>
#include <string>
#include <list>

/////////////////////////////////////////////////////////////////////////////
// CCmdLinePromptDlg
class CCmdLinePromptDlg : 
	public CAxDialogImpl<CCmdLinePromptDlg>
{
public:
	typedef std::basic_string<_TCHAR>	string;

	CCmdLinePromptDlg(string &strExtension, string &strDialogTitle)
	{
		m_strExtension.assign(strExtension.data());
		m_strDialogTitle.assign(strDialogTitle.data());
	}

	~CCmdLinePromptDlg()
	{
	}

	enum { IDD = IDD_CMDLINEPROMPTDLG };

	string				strExtension;

private:
	string				m_strExtension;
	string				m_strDialogTitle;

BEGIN_MSG_MAP(CCmdLinePromptDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
END_MSG_MAP()
// Handler prototypes:
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};

#endif //__CMDLINEPROMPTDLG_H_
