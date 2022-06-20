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

        Flatten(sParam)

    End Sub

    Private Sub Flatten(ByVal sBaseFolder As String)

        Dim lstFolders As New List(Of String)
        lstFolders.Add(sBaseFolder)

        Dim bBaseDone As Boolean
        While lstFolders.Count > 0
            Dim lIndex As Long = lstFolders.Count - 1
            Dim sFolder As String = lstFolders.Item(lIndex)

            If Not bBaseDone Then lstFolders.RemoveAt(lIndex)

            Dim subFolders() As String = Directory.GetDirectories(sFolder)
            For Each subFolder As String In subFolders
                '  If (System.IO.File.GetAttributes(subFolder) And IO.FileAttributes.Directory) = IO.FileAttributes.Directory Then
                lstFolders.Add(subFolder)
                lIndex = -1
                ' End If
            Next
            Threading.Thread.Sleep(0)
            If bBaseDone Then
                'get all files
                Dim files() As String = Directory.GetFiles(sFolder)
                For Each item As String In files
                    lIndex = -1
                    Dim n As Integer = 0
                    Dim sFolderName As String = GetFolderName(sFolder)
                    If File.Exists(Path.Combine(sBaseFolder, sFolderName & " _ " & Path.GetFileName(item))) Then
                        n = 1
                        While File.Exists(Path.Combine(sBaseFolder, sFolderName & " _ " & Path.GetFileNameWithoutExtension(item) & "_" & n & Path.GetExtension(item)))
                            n += 1
                        End While
                    End If

                    Dim fa As FileAttributes = File.GetAttributes(item)
                    If (fa And FileAttributes.ReadOnly) = FileAttributes.ReadOnly Then
                        fa = fa Xor FileAttributes.ReadOnly
                        File.SetAttributes(item, fa)
                    End If

                    If n > 0 Then
                        File.Move(item, Path.Combine(sBaseFolder, sFolderName & " _ " & Path.GetFileNameWithoutExtension(item) & "_" & n & Path.GetExtension(item)))
                    Else
                        File.Move(item, Path.Combine(sBaseFolder, sFolderName & " _ " & Path.GetFileName(item)))
                    End If
                Next

                'delete empty folder
                If lIndex > -1 Then
                    lstFolders.RemoveAt(lIndex)
                    If Directory.GetFileSystemEntries(sFolder).Length = 0 Then Directory.Delete(sFolder)
                End If
            Else
                bBaseDone = True
            End If
        End While

    End Sub

    Private Function GetFolderName(ByVal sFolder As String) As String

        Dim lIndex As Long = sFolder.LastIndexOf("\")
        If lIndex > -1 Then
            GetFolderName = sFolder.Substring(lIndex + 1)
        Else
            GetFolderName = sFolder
        End If

    End Function

End Module
