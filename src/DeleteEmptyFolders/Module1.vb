'--------------------------------------------------------------------------
' DFTContextMenuHandler - a windows shell extension
' 
' This is a shell extension for windows that adds a file tools submenu to 
' the windows explorer context menu for folders and files in general.
'
' Version 1.01
' Copyright (c) 2004-2008  thomas694 (@GH 0CFD61744DA1A21C)
'     first version (for windows 32bit)
' Version 1.02
' Copyright (c) 2015  thomas694
'     changed to 64bit
' Version 1.03
' Copyright (c) 2017  thomas694
'     added option for emptying files
'     fixed menu ids for newer windows version
' Version 1.0.4  (c) 2020  thomas694
'     release version
' Version 1.5.0  (c) 2022  thomas694
'     copied logic code into a VS2019 ATL template,
'     using CAtlDllModuleT instead of CComModule
'
' This project/file is part of DFTContextMenuHandler.
'
' DFTContextMenuHandler is free software: you can redistribute it and/or modify
' it under the terms of the GNU General Public License as published by
' the Free Software Foundation, either version 3 of the License, or
' (at your option) any later version.
'
' This program is distributed in the hope that it will be useful,
' but WITHOUT ANY WARRANTY; without even the implied warranty of
' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
' GNU General Public License for more details.
'
' You should have received a copy of the GNU General Public License
' along with this program.  If not, see <http://www.gnu.org/licenses/>.
'--------------------------------------------------------------------------
Imports System.IO

Module Module1

    Private m_lstFolders As New List(Of String)
    Private m_strBaseFolder As String

    Sub Main()

        Dim sParam As String = Environment.CommandLine()

        sParam = sParam.Substring(sParam.IndexOf("""", 3) + 1)

        'If sParam.EndsWith("""") Then
        '    sParam = sParam.Remove(sParam.Length - 1)
        '    sParam = sParam.Substring(sParam.LastIndexOf("""") + 1)
        'Else
        '    sParam = sParam.Substring(sParam.LastIndexOf(" ") + 1)
        'End If

        If (System.IO.File.GetAttributes(sParam) And IO.FileAttributes.Directory) <> IO.FileAttributes.Directory Then
            MsgBox("no folder selected")
            Exit Sub
        End If

        DeleteEmptySubFolders(sParam)

    End Sub


    Private Function GetSubFolders(ByVal sFolder As String) As Boolean

        GetSubFolders = True

        Dim listSubFolders As New List(Of String)

        Dim files() As String = Directory.GetFileSystemEntries(sFolder)
        Dim lCount As Long
        Dim bFound As Boolean
        For Each file As String In files
            lCount += 1

            If (System.IO.File.GetAttributes(file) And IO.FileAttributes.Directory) = IO.FileAttributes.Directory Then
                listSubFolders.Add(file)
            Else
                If m_strBaseFolder <> sFolder Then
                    If file.ToLower.EndsWith("\thumbs.db") Then
                        bFound = True
                    Else
                        'something other than folder was found
                        GetSubFolders = False
                        Exit Function
                    End If
                End If
            End If

        Next
        If lCount = 1 And bFound Then
            File.SetAttributes(sFolder & "\Thumbs.db", FileAttributes.Normal)
            File.Delete(sFolder & "\Thumbs.db")
        End If
        Dim i As Long
        For i = 0 To listSubFolders.Count - 1
            Dim bRet As Boolean = GetSubFolders(listSubFolders.Item(i))
            If bRet Then
                m_lstFolders.Add(listSubFolders.Item(i))
            Else
                If m_strBaseFolder <> sFolder Then
                    'bubble upwards
                    GetSubFolders = False
                End If
            End If
        Next

    End Function

    Private Sub DeleteEmptySubFolders(ByVal sBaseFolder As String)

        m_strBaseFolder = sBaseFolder

        GetSubFolders(sBaseFolder)

        Dim i As Long
        For i = 0 To m_lstFolders.Count - 1
            System.IO.Directory.Delete(m_lstFolders.Item(i))
        Next

    End Sub

End Module
