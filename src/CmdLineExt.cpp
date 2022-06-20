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
// CmdLineExt.cpp : Implementation of DLL Exports.

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "CmdLineExt_i.h"
#include "dllmain.h"

using namespace ATL;

// Used to determine whether the DLL can be unloaded by OLE.
_Use_decl_annotations_
STDAPI DllCanUnloadNow(void)
{
	return _AtlModule.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type.
_Use_decl_annotations_
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID* ppv)
{
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry.
_Use_decl_annotations_
STDAPI DllRegisterServer(void)
{
	_TCHAR		strCLSID[50];
	OLECHAR		strWideCLSID[50];
	CRegKey		key;
	HRESULT		hr;
	USES_CONVERSION;

	// registers object, typelib and all interfaces in typelib
	hr = _AtlModule.DllRegisterServer();

	if (SUCCEEDED(hr)) {
		if ((::StringFromGUID2(CLSID_CmdLineContextMenu, strWideCLSID, 50) > 0)) {
			_tcscpy_s(strCLSID, OLE2CT(strWideCLSID));
			hr = key.SetValue(HKEY_CLASSES_ROOT, _T("*\\shellex\\ContextMenuHandlers\\DFTContextMenuHandler\\"), strCLSID);
			hr = key.SetValue(HKEY_CLASSES_ROOT, _T("Directory\\shellex\\ContextMenuHandlers\\DFTContextMenuHandler\\"), strCLSID);
			hr = key.SetValue(HKEY_CLASSES_ROOT, _T("Directory\\shellex\\DragDropHandlers\\DFTContextMenuHandler\\"), strCLSID);
		}
	}

	return hr;
}

// DllUnregisterServer - Removes entries from the system registry.
_Use_decl_annotations_
STDAPI DllUnregisterServer(void)
{
	CRegKey		key;
	HRESULT		hr;
		
	hr = _AtlModule.DllUnregisterServer();

	if (SUCCEEDED(hr)) {
		if (key.Open(HKEY_CLASSES_ROOT, _T("*\\shellex\\ContextMenuHandlers\\")) == ERROR_SUCCESS) {
			hr = key.DeleteValue(NULL);
			if (hr != ERROR_SUCCESS && hr != ERROR_FILE_NOT_FOUND)
				return hr;
			hr = key.DeleteSubKey(_T("DFTContextMenuHandler"));
			if (hr != ERROR_SUCCESS && hr != ERROR_FILE_NOT_FOUND)
				return hr;
		}
		if (key.Open(HKEY_CLASSES_ROOT, _T("Directory\\shellex\\ContextMenuHandlers\\")) == ERROR_SUCCESS) {
			hr = key.DeleteValue(NULL);
			if (hr != ERROR_SUCCESS && hr != ERROR_FILE_NOT_FOUND)
				return hr;
			hr = key.DeleteSubKey(_T("DFTContextMenuHandler"));
			if (hr != ERROR_SUCCESS && hr != ERROR_FILE_NOT_FOUND)
				return hr;
		}
		if (key.Open(HKEY_CLASSES_ROOT, _T("Directory\\shellex\\DragDropHandlers\\")) == ERROR_SUCCESS) {
			hr = key.DeleteValue(NULL);
			if (hr != ERROR_SUCCESS && hr != ERROR_FILE_NOT_FOUND)
				return hr;
			hr = key.DeleteSubKey(_T("DFTContextMenuHandler"));
			if (hr != ERROR_SUCCESS && hr != ERROR_FILE_NOT_FOUND)
				return hr;
		}
	}

	return hr;
}

// DllInstall - Adds/Removes entries to the system registry per user per machine.
STDAPI DllInstall(BOOL bInstall, _In_opt_  LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";

	if (pszCmdLine != nullptr)
	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
		{
			ATL::AtlSetPerUserRegistration(true);
		}
	}

	if (bInstall)
	{
		hr = DllRegisterServer();
		if (FAILED(hr))
		{
			DllUnregisterServer();
		}
	}
	else
	{
		hr = DllUnregisterServer();
	}

	return hr;
}
