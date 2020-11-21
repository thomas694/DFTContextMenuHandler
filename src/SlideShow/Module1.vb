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
Imports System.Threading

Module Module1

    Dim alPicFolders As New ArrayList
    Dim alPics As New ArrayList
    Dim Form As Form1

    Public Sub Main(ByVal argv As String())

        AddHandler System.Windows.Forms.Application.ThreadException, AddressOf MyThreadExceptionHandler
        Application.SetUnhandledExceptionMode(UnhandledExceptionMode.CatchException)
        AddHandler System.AppDomain.CurrentDomain.UnhandledException, AddressOf MyUnhandledExceptionHandler

        Form = New Form1
        Form.Show()

        Dim sFolders As String()

        If argv.Length = 1 Then
            Dim sParam As String = argv.GetValue(0)
            'one or more folders separated by colons
            sFolders = sParam.Split("|")

            For Each sFolder As String In sFolders
                Dim fi As System.IO.FileInfo = New System.IO.FileInfo(sFolder)
                If (fi.Attributes And IO.FileAttributes.Directory) = 0 Then
                    MessageBox.Show("Please choose only directories!")
                    Form.Close()
                    Exit Sub
                End If
            Next
        Else
            Form.FolderBrowserDialog1.SelectedPath = Application.StartupPath
            Form.FolderBrowserDialog1.ShowNewFolderButton = False
            Form.FolderBrowserDialog1.ShowDialog()
            sFolders = New String() {Form.FolderBrowserDialog1.SelectedPath}
        End If

        alPicFolders = GetAllSubDirectories(sFolders, False)
        'alPicFolders.Insert(0, sFolders)

        alPics = ReadFiles(alPicFolders, Form.htPathCounts)

        If alPics.Count > 0 Then
            Form.SlideShow(alPics.ToArray(GetType(String)))
        End If

        Form.Hide()
        Form.ShowDialog()

    End Sub

    Sub MyUnhandledExceptionHandler(ByVal sender As Object, ByVal e As UnhandledExceptionEventArgs)
        MessageBox.Show(e.ExceptionObject.ToString())
    End Sub

    Sub MyThreadExceptionHandler(ByVal sender As Object, ByVal e As ThreadExceptionEventArgs)
        MessageBox.Show(e.Exception.ToString())
    End Sub

    Private Function ReadFiles(ByVal alPicFolders As ArrayList, ByVal ht As Hashtable) As ArrayList
        Dim alPics As New ArrayList
        For Each sFolder As String In alPicFolders
            Dim intCount As Integer = 0
            For Each sFile As String In System.IO.Directory.GetFiles(sFolder)
                If sFile.ToLower.EndsWith(".jpg") OrElse _
                   sFile.ToLower.EndsWith(".jpeg") OrElse _
                   sFile.ToLower.EndsWith(".bmp") OrElse _
                   sFile.ToLower.EndsWith(".gif") OrElse _
                   sFile.ToLower.EndsWith(".tif") Then
                    intCount += 1
                    alPics.Add(sFile)
                End If
            Next
            ht.Add(sFolder, intCount)
        Next
        Return alPics
    End Function

    Private Function GetAllSubDirectories(ByVal sBaseFolders As String(), ByVal bRecursive As Boolean) As ArrayList
        Dim alFolders As New ArrayList
        'alFolders.Add(sBaseFolder)
        For Each sBaseFolder As String In sBaseFolders
            If Not bRecursive Then
                alFolders.Add(sBaseFolder)
            End If
            For Each sFolder As String In System.IO.Directory.GetDirectories(sBaseFolder)
                alFolders.Add(sFolder)
                Dim alSubFolders As ArrayList
                alSubFolders = GetAllSubDirectories(New String() {sFolder}, True)
                If alSubFolders.Count > 0 Then
                    alFolders.AddRange(alSubFolders.ToArray)
                End If
            Next
        Next

        Return alFolders
    End Function

End Module
