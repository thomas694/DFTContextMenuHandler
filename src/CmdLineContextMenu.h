// CmdLineContextMenu.h : Declaration of the CCmdLineContextMenu

#ifndef __CMDLINECONTEXTMENU_H_
#define __CMDLINECONTEXTMENU_H_

#include "resource.h"       // main symbols
#include <shlguid.h>
#include <comdef.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include <shlwapi.h>

#define ID_MENU_ITEM 0

#define MAX_PATH_EX 32767

/////////////////////////////////////////////////////////////////////////////
// CCmdLineContextMenu
class ATL_NO_VTABLE CCmdLineContextMenu : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCmdLineContextMenu, &CLSID_CmdLineContextMenu>,
	public IContextMenu3,
	public IShellExtInit
{
public:
	CCmdLineContextMenu();

	// IContextMenu interface
    STDMETHOD(QueryContextMenu)(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO lpici);
    STDMETHOD(GetCommandString)(UINT_PTR idCmd, UINT  uType, UINT *pwReserved, LPSTR pszName, UINT cchMax);   
    
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

protected:
	TCHAR m_szFolderDroppedIn[MAX_PATH_EX];
	//CBitmap     m_bitmap;

private:
	int ConvertDots2Spaces();
	int ConvertSpaces2Dots();
	int ConvertUnderscores2Spaces();
	int ConvertSpaces2Underscores();
	int RemoveGroupNames();
	int RenameExtension();
	int AppendExtension();
	int RemoveFromFilename();
	int SetDateTime();
	int InsertBeforeFilename();
	int AppendToFilename();
	int FlattenTree();
	int FlattenTree2();
	int DeleteEmptySubfolders();
	int SlideShow();
	int EmptyFiles();
	int EditFilename();
	typedef std::basic_string<_TCHAR> string;
	typedef std::vector<string> StringArray;
	int CopyFilesHere(StringArray strFilenames, string szFolderDroppedIn);
	int MoveFilesHere(StringArray strFilenames, string szFolderDroppedIn);
	int StartCopyFilesHere();
	int StartMoveFilesHere();

	string m_strFileName;
	UINT m_idCmdFirst;
	UINT m_idCmdLast;
	StringArray m_strFilenames;

	void DeleteEmptySubfolders(string baseFolder);
	bool CheckSubfolders(string folder);
	void FlattenTree(string baseFolder, bool useFolderNames);
	StringArray GetSubfolders(string folder);
	bool IsEmptyDirectory(string folder);
	int CopyDirectory(string sourceDir, string destDir, IProgressDialog* pProgressDlg, size_t* currentFiles, size_t* totalFiles);
	BOOL RemoveDirectory(string strDir);

	string m_strBaseFolder;
	StringArray m_lstFolders;
};

OBJECT_ENTRY_AUTO(__uuidof(CmdLineContextMenu), CCmdLineContextMenu)

#endif //__CMDLINECONTEXTMENU_H_
