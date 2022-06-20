//--------------------------------------------------------------------------
// DFTContextMenuHandler - a windows shell extension
// 
// This is a shell extension for windows that adds a file tools submenu to 
// the windows explorer context menu for folders and files in general.
//
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
// dllmain.cpp : Implementation of DllMain.

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "CmdLineExt_i.h"
#include "dllmain.h"

CCmdLineExtModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	_AtlModule.hInstance = hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved);
}
